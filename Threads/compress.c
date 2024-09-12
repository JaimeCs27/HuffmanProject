#include "Tabla.h"
#include "Nodos.h"
#include <pthread.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define MAX_FILENAME 1024
#define BUFFER_SIZE 8192
#define DEBUG printf("Aqui\n");

// Estructura para almacenar información del archivo
typedef struct {
    char filename[MAX_FILENAME];
    unsigned int filenameLength;
    unsigned long totalCharacters;
    unsigned long totalBytesWritten;
    unsigned char *compressedData;
    
    int compressedSize;
} FileData;

// Estructura para pasar argumentos a los hilos
typedef struct {
    char *filePath;
    FILE *outputFile;
    
    pthread_mutex_t *fileMutex;
    char *fileName;
} ThreadData;

typedef struct {
    const char *filePath;
    Node **list;
} ThreadParams;

typedef struct {
    char* name;
    int count;
} CharactersCount;


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
extern Table *table;
long int fileLength = 0;
CharactersCount* characters[97];
int indexC = 0;
pthread_mutex_t indexMutex;

void compressFileToBuffer(const char* path, unsigned char **compressedData, int *compressedSize);
void CountCharacter(Node **list, unsigned char character);
void processDirectory(const char *directoryPath, Node** list);
//void compressFile(const char* path, FILE *compress, unsigned char *byte, int *nBits);
void compress(const char* directoryPath, FILE *compress);


//PRUEBA

typedef struct {
    const char *filePath;
    Node **list;
    pthread_mutex_t *listMutex;  // Mutex para proteger la lista compartida
    int characterIndex;
} ThreadArgs;

void *compressFile(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    //printf("%s\n", data->filePath);
    FILE *inputFile = fopen(data->filePath, "r");
    if (!inputFile) {
        perror("Error opening the file");
        return NULL;
    }
    
    // Leer el archivo y realizar la compresión
    FileData fileData;
    strncpy(fileData.filename, data->fileName, MAX_FILENAME);
    fileData.filenameLength = strlen(fileData.filename);
    
    // Aquí es donde llamamos a la nueva función que comprime el archivo en un buffer
    compressFileToBuffer(data->filePath, &fileData.compressedData, &fileData.compressedSize);

    // Contar el total de caracteres en el archivo original (útil para estadísticas)
    fseek(inputFile, 0, SEEK_END);
    fileData.totalCharacters = ftell(inputFile);
    fclose(inputFile);

    // Bloquear el acceso al archivo global
    pthread_mutex_lock(data->fileMutex);

    // Escribir los metadatos y datos comprimidos en el archivo de salida
    fwrite(&fileData.filenameLength, sizeof(unsigned int), 1, data->outputFile);
    fwrite(&fileData.filename, sizeof(char), fileData.filenameLength, data->outputFile);
    fwrite(&fileData.totalCharacters, sizeof(unsigned long), 1, data->outputFile);
    fwrite(&fileData.compressedSize, sizeof(size_t), 1, data->outputFile);  // Guardar el tamaño comprimido
    fwrite(fileData.compressedData, sizeof(unsigned char), fileData.compressedSize, data->outputFile); // :p
    pthread_mutex_unlock(data->fileMutex);
    // Liberar el buffer de datos comprimidos
    free(fileData.compressedData);

    return NULL;
}

void compressFileToBuffer(const char* path, unsigned char **compressedData, int *compressedSize) {
    FILE *fe = fopen(path, "r");
    if (!fe) {
        printf("Error compressing the file\n");
        return;
    }
    // Buffer dinámico para almacenar los datos comprimidos
    size_t bufferSize = 1024;  // Tamaño inicial
    *compressedData = (unsigned char *)malloc(bufferSize);
    *compressedSize = 0;
    
    unsigned char byte = 0;
    int nBits = 0;
    int c;

    while ((c = fgetc(fe)) != EOF) {
        // Utilizar la función findSymbol para buscar el símbolo en la tabla de Huffman
        Table *node = findSymbol(table, (unsigned char)c);

        if (node == NULL) {
            fprintf(stderr, "Symbol not found in the table: %c\n", c);
            continue;
        }

        // Agregar los bits al byte actual
        for (int i = node->nBits - 1; i >= 0; i--) {
            unsigned char bit = (node->bits >> i) & 1;
            byte = (byte << 1) | bit;
            nBits++;

            // Si completamos 8 bits, agregamos el byte al buffer
            if (nBits == 8) {
                if (*compressedSize >= bufferSize) {
                    bufferSize *= 2;
                    *compressedData = (unsigned char *)realloc(*compressedData, bufferSize);
                }
                (*compressedData)[(*compressedSize)++] = byte;
                byte = 0;
                nBits = 0;
            }
        }
    }

    // Agregar cualquier byte restante al buffer
    if (nBits > 0) {
        byte <<= (8 - nBits);  // Desplazar los bits restantes a la izquierda
        if (*compressedSize >= bufferSize) {
            bufferSize *= 2;
            *compressedData = (unsigned char *)realloc(*compressedData, bufferSize);
        }
        (*compressedData)[(*compressedSize)++] = byte;
    }

    fclose(fe);
}

void compress(const char* directoryPath, FILE *compress) {
    struct dirent *entry;
    DIR *dp = opendir(directoryPath);
    if (dp == NULL) {
        perror("The directory can not be opened");
        return;
    }

    pthread_mutex_t fileMutex;
    pthread_mutex_init(&fileMutex, NULL);
    int numThreads = 97; // Este número depende de la cantidad de archivos/hilos que quieras manejar
    pthread_t threads[numThreads];
    ThreadData threadData[numThreads];
    int i = 0;
    // Leer el directorio y comprimir cada archivo
    while ((entry = readdir(dp))) {
        unsigned char byte = 0;
        int nBits = 0;
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construir la ruta completa del archivo
        char filePath[1024];
        snprintf(filePath, sizeof(filePath), "%s/%s", directoryPath, entry->d_name);

        // Obtener el nombre del archivo
        const char *fileName = strrchr(filePath, '/') ? strrchr(filePath, '/') + 1 : filePath;

        // Escribir el tamaño del nombre del archivo
        int fileNameLength = strlen(fileName);
        // Obtener el tamaño del archivo original
        threadData[i].filePath = (char *)malloc(strlen(filePath) + 1);  
        strcpy(threadData[i].filePath, filePath);
        threadData[i].fileName = (char *)malloc(strlen(fileName) + 1);
        strcpy(threadData[i].fileName, fileName); 
        threadData[i].outputFile = compress;
        threadData[i].fileMutex = &fileMutex;
        i++; 
    }
    for(int i = 0; i < numThreads; i++){
        pthread_create(&threads[i], NULL, compressFile, &threadData[i]);
    }

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }
    closedir(dp);
}

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
    struct timeval start, end;
    double elapsedTime;

    // Obtener el tiempo de inicio
    gettimeofday(&start, NULL);

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
        perror("Error creating the compressed file");
        return 1;
    }

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
    compress(directory, compressFile);

    fclose(compressFile); //Close file
    
    //printNode(Tree);
    freeNode(Tree); // Input: Tree, Output: None, Function: Destroys it to free memory
    //printTable(table);
    destroyTable(table); // Input: Table, Output: None, Function: Destroys it to

    gettimeofday(&end, NULL);

    // Calcular el tiempo transcurrido en segundos
    elapsedTime = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    printf("Concurrent Huffman compression took: %f seconds\n", elapsedTime);

    return 0;

}
