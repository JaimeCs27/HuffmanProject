#include "Tabla.h"
#include "Nodos.h"
#include <sys/mman.h>
#include <fcntl.h>  
#include <pthread.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>


#define DEBUG printf("Aqui\n");
#define SHARED_MEMORY_SIZE 4096  // 4KB de memoria compartida


typedef struct {
    char* name;
    int count;
} CharactersCount;

typedef struct {
    int nameLength;
    char name[256]; // Asumiendo que el nombre del archivo no excederá los 255 caracteres
    long totalCharacters;
    long compressedBytes;
    unsigned char compressedData[SHARED_MEMORY_SIZE]; // Buffer para los datos comprimidos
} CompressedFileData;


extern Table *table;
long int fileLength = 0;
CharactersCount* characters[97];
int indexC = 0;

void processFile(const char *filePath, Node **list) {
  FILE *file = fopen(filePath, "r");
  if (!file) {
    printf("Error opening the file %s\n", filePath);
    return;
  }

  unsigned char character;
  unsigned int cant = 0;
  do{
    character = fgetc(file);
    if(feof(file))
      break;
    fileLength++; // Incrementa la longitud por cada carácter leído
    cant++;
    CountCharacter(list, character);
  }while (1);
  characters[indexC] = cant;
  indexC++;
  fclose(file);
}

void processDirectory(const char *directoryPath, Node** list) {
    struct dirent *entry;
    DIR *dp = opendir(directoryPath);

    if (dp == NULL) {
        perror("The directory can not be opened");
        return;
    }

    

    while ((entry = readdir(dp))) {
        // Ignorar las entradas "." y ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        char filePath[1024];
        snprintf(filePath, sizeof(filePath), "%s/%s", directoryPath, entry->d_name);

        // Llama a processFile con la ruta completa
        processFile(filePath, list);

        //printf("Process: %s\n", entry->d_name);
    }

    closedir(dp);
}

void CountCharacter(Node **list, unsigned char character) {
  Node *current, *previous, *newNode;
  if (!*list) // If the list is empty, create a new node as the head
  {
    *list = (Node *)malloc(sizeof(Node)); // Create a new node
    (*list)->symbol = character;                  // Assign the character
    (*list)->count = 1; // Initialize the count to 1
    (*list)->next = (*list)->left = (*list)->right = NULL; // Initialize pointers
  } else {
    // Find the correct position in the list for the character
    current = *list;
    previous = NULL;
    while (current && current->symbol < character) {
      previous = current;      // Keep reference to the previous node
      current = current->next; // Move to the next node
    }

    // Check if the character already exists in the list
    if (current && current->symbol == character) {
      current->count++; // If it exists, increment its count
    } else {
      newNode = (Node *)malloc(sizeof(Node));
      newNode->symbol = character;
      newNode->left = newNode->right = NULL;
      newNode-> count = 1;
      newNode->next = current;
      if(previous) previous->next = newNode;
      else *list = newNode;
    }
  }
}

void writeCompressedDataToFile(CompressedFileData *sharedData, int numFiles, FILE *outputFile) {
    for (int i = 0; i < numFiles; i++) {
        fwrite(&sharedData[i].nameLength, sizeof(int), 1, outputFile);
        fwrite(sharedData[i].name, sizeof(char), sharedData[i].nameLength, outputFile);
        fwrite(&sharedData[i].totalCharacters, sizeof(long), 1, outputFile);
        fwrite(&sharedData[i].compressedBytes, sizeof(long), 1, outputFile);
        fwrite(sharedData[i].compressedData, sizeof(char), sharedData[i].compressedBytes, outputFile);
    }
}

void compressFile(const char *filePath, const char *fileName, Node **list, CompressedFileData *sharedData, int fileIndex) {
    FILE *fe = fopen(filePath, "r");
    if (!fe) {
        printf("Error opening the file %s\n", filePath);
        return;
    }

    size_t bufferSize = 1024;
    unsigned char *compressedData = sharedData[fileIndex].compressedData;
    int compressedBytes = 0;
    unsigned char byte = 0;
    int nBits = 0;
    int c;

    sharedData[fileIndex].nameLength = strlen(fileName);
    strncpy(sharedData[fileIndex].name, fileName, 255);

    int cant = 0;

    while ((c = fgetc(fe)) != EOF) {
        cant++;
        Table *node = findSymbol(table, (unsigned char)c);
        if (!node) {
            fprintf(stderr, "Symbol not found in the table: %c\n", c);
            continue;
        }

        for (int i = node->nBits - 1; i >= 0; i--) {
            unsigned char bit = (node->bits >> i) & 1;
            byte = (byte << 1) | bit;
            nBits++;

            if (nBits == 8) {
                compressedData[compressedBytes++] = byte;
                byte = 0;
                nBits = 0;
            }
        }
    }

    if (nBits > 0) {
        byte <<= (8 - nBits);
        compressedData[compressedBytes++] = byte;
    }

    sharedData[fileIndex].compressedBytes = compressedBytes;
    sharedData[fileIndex].totalCharacters = cant;

    fclose(fe);
}

void compress(const char *directoryPath, CompressedFileData *sharedData) {
    Node *list = NULL;
    struct dirent *entry;
    DIR *dp = opendir(directoryPath);
    if (dp == NULL) {
        perror("The directory cannot be opened");
        return;
    }

    int fileIndex = 0;

    while ((entry = readdir(dp))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char filePath[1024];
        snprintf(filePath, sizeof(filePath), "%s/%s", directoryPath, entry->d_name);

        pid_t pid = fork();
        if (pid == 0) {
            compressFile(filePath, entry->d_name, &list, sharedData, fileIndex);
            exit(0);
        } else if (pid > 0) {
            wait(NULL); // Esperar a que termine el proceso hijo
        } else {
            perror("Error al crear el proceso hijo");
            return;
        }
        fileIndex++;
    }

    closedir(dp);
}


int main(int argc, char *argv[]){
    Node *List;
    Node *Tree;
    char *fileName;
    char *directory;

    if(argc > 3){
        printf("Expecting less arguments\n");
        printf("Correct Usage: ./compress <Directory Path> <Compressed File Name>\n");
        return 1;
    }

    if(argc == 2){
        printf("Compressed File Name argument not given using the default name: 'CompressedFile.bin'\n");
        fileName = "CompressedFile.bin";
    }else{
        fileName = argv[2];
    }
    if(argc <= 1){
        printf("Not enought arguments passed\n");
        printf("Correct Usage: ./compress <Directory Path> <Compressed File Name>\n");
        return 1;
    }
    directory = argv[1];
    clock_t start, end;
    double cpuTimeUsed;
    
    start = clock();
    processDirectory(directory, &List);

    sortList(&List);

  
    Tree = List;
    while(Tree && Tree->next){
        Node *newNode = (Node*)malloc(sizeof(Node));
        newNode->symbol = ';';
        newNode->right = Tree;
        Tree = Tree->next;
        newNode->left = Tree;
        Tree = Tree->next;
        newNode->count = newNode->left->count + newNode->right->count;
        insertInOrder(&Tree, newNode);
    }

    createTable(Tree, 0, 0);

    FILE *compressFile = fopen(fileName, "wb");
    if (!compressFile) {
        perror("Error creating compressed file");
        return 1;
    }

    int numFiles = 4;

    CompressedFileData *sharedData = mmap(NULL, sizeof(CompressedFileData) * numFiles,
                                      PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (sharedData == MAP_FAILED) {
        perror("Error creating shared memory");
        exit(EXIT_FAILURE);
    }
    
    compress(directory, sharedData);

    fwrite(&fileLength, sizeof(long int), 1, compressFile);
    int countElements = 0;
    Table *t = table;  
    while (t) {
        countElements++;
        t = t->next;
    }

    // Write the number of elements in the table
    fwrite(&countElements, sizeof(int), 1, compressFile);

    // Save the table
    t = table; 
    while (t) { 
        fwrite(&t->symbol, sizeof(char), 1, compressFile);
        fwrite(&t->bits, sizeof(unsigned long int), 1, compressFile);
        fwrite(&t->nBits, sizeof(char), 1, compressFile);
        t = t->next;
    }
    
    //AQUI VA EL COMPRESS
    
    writeCompressedDataToFile(sharedData, numFiles, compressFile);


    fclose(compressFile); //Close file

    freeNode(Tree); // Input: Tree, Output: None, Function: Destroys it to free memory
    //printTable(table);
    destroyTable(table); // Input: Table, Output: None, Function: Destroys it to

    end = clock();
    cpuTimeUsed = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Parallel Huffman compression took: %f seconds\n", cpuTimeUsed);

    return 0;
}