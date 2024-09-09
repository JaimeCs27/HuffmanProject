#include "Tabla.h"
#include "Nodos.h"
#include <pthread.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DEBUG printf("Aqui\n");


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

void CountCharacter(Node **list, unsigned char character);
void *processFileThread(void *arg);
void processDirectory(const char *directoryPath, Node** list);
void compressFile(const char* path, FILE *compress, unsigned char *byte, int *nBits);
void compress(const char* directoryPath, FILE *compress);


//PRUEBA

typedef struct {
    const char *filePath;
    Node **list;
    pthread_mutex_t *listMutex;  // Mutex para proteger la lista compartida
    int characterIndex;
} ThreadArgs;

void *processFileThread(void *args) {
    ThreadArgs *threadArgs = (ThreadArgs *)args;
    
    FILE *file = fopen(threadArgs->filePath, "r");
    if (!file) {
        printf("Error al abrir el archivo %s\n", threadArgs->filePath);
        pthread_exit(NULL);
    }
    unsigned char character;
    int cant = 0;
    do {
        character = fgetc(file);
        if (feof(file)) break;
        // Proteger el acceso a la lista con el mutex
        pthread_mutex_lock(threadArgs->listMutex);
        cant++;
        fileLength++;
        CountCharacter(threadArgs->list, character); // Actualiza la lista de nodos
        pthread_mutex_unlock(threadArgs->listMutex);
    } while (1);
    characters[threadArgs->characterIndex] = cant; 
    indexC++;
    fclose(file);
    pthread_exit(NULL);
}

void processFilesConcurrently(const char **filePaths, int numFiles, Node **list) {
    pthread_t threads[numFiles];
    ThreadArgs threadArgs[numFiles];
    pthread_mutex_t listMutex;  // Mutex para proteger la lista compartida

    // Inicializar los mutexes
    pthread_mutex_init(&listMutex, NULL);
    pthread_mutex_init(&indexMutex, NULL);  // Mutex para proteger `indexC`

    for (int i = 0; i < numFiles; i++) {
        // Inicializar los argumentos del hilo
        threadArgs[i].filePath = filePaths[i];
        threadArgs[i].list = list;
        threadArgs[i].listMutex = &listMutex;

        // Asignar un índice único para el array `characters`
        pthread_mutex_lock(&indexMutex);
        threadArgs[i].characterIndex = indexC;
        
        pthread_mutex_unlock(&indexMutex);

        // Crear un nuevo hilo para procesar el archivo
        if (pthread_create(&threads[i], NULL, processFileThread, (void *)&threadArgs[i]) != 0) {
            printf("Error al crear el hilo para el archivo %s\n", filePaths[i]);
        }
    }

    // Esperar que todos los hilos terminen
    for (int i = 0; i < numFiles; i++) {
        pthread_join(threads[i], NULL);
    }

    // Destruir los mutexes
    pthread_mutex_destroy(&listMutex);
    pthread_mutex_destroy(&indexMutex);
}

void compressFile(const char* path, FILE *compress, unsigned char *byte, int *nBits){
    //printf("PATH: %s\n", path);
    FILE *fe = fopen(path, "r");
    if(!fe){
      printf("Error al comprimir archivo\n");
      return;
    }
    
    int c;
    while ((c = fgetc(fe)) != EOF) {
        // Utilizar la función findSymbol para buscar el símbolo en la tabla de Huffman
        Table *node = findSymbol(table, (unsigned char)c);

        if (node == NULL) {
            fprintf(stderr, "Símbolo no encontrado en la tabla: %c\n", c);
            continue;
        }

        // Agregar los bits al byte actual
        for (int i = node->nBits - 1; i >= 0; i--) {
            // Extraer el bit en la posición i
            unsigned char bit = (node->bits >> i) & 1;

            // Colocar el bit en la posición correspondiente en el byte
            *byte = (*byte << 1) | bit;
            (*nBits)++;

            // Si hemos completado 8 bits, escribir el byte en el archivo
            if (*nBits == 8) {
                fwrite(byte, sizeof(unsigned char), 1, compress);
                *byte = 0;
                *nBits = 0;
            }
        }
    }

    // Escribir cualquier bit restante en el byte
    if (*nBits > 0) {
        *byte <<= (8 - *nBits);  // Desplazar los bits restantes a la izquierda
        fwrite(byte, sizeof(unsigned char), 1, compress);
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
        fwrite(&fileNameLength, sizeof(int), 1, compress);

        // Escribir el nombre del archivo
        fwrite(fileName, sizeof(char[fileNameLength]), 1, compress);

        // Obtener el tamaño del archivo original
        FILE *inputFile = fopen(filePath, "r");
        if (!inputFile) {
            printf("Error al abrir el archivo %s para compresión\n", filePath);
            continue;
        }

        fseek(inputFile, 0, SEEK_END);
        unsigned int fileSize = ftell(inputFile);
        fseek(inputFile, 0, SEEK_SET);
        fclose(inputFile);

        // Escribir la cantidad de caracteres procesados (tamaño del archivo)
        fwrite(&fileSize, sizeof(unsigned int), 1, compress);

        // Comprimir y escribir los bits correspondientes a este archivo
        compressFile(filePath, compress, &byte, &nBits);
    }

    closedir(dp);
}

//PRUEBA 

void processDirectory(const char *directoryPath, Node **list) {
    struct dirent *entry;
    DIR *dp = opendir(directoryPath);

    if (dp == NULL) {
        perror("The directory can not be opened");
        return;
    }

    // Array dinámico para almacenar las rutas de los archivos
    const char **filePaths = NULL;
    int fileCount = 0;

    // Leer el directorio y almacenar las rutas de los archivos
    while ((entry = readdir(dp))) {
        // Ignorar las entradas "." y ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construir la ruta completa del archivo
        char filePath[1024];
        snprintf(filePath, sizeof(filePath), "%s/%s", directoryPath, entry->d_name);

        // Almacenar la ruta del archivo en el array
        fileCount++;
        filePaths = realloc(filePaths, fileCount * sizeof(char *));
        filePaths[fileCount - 1] = strdup(filePath);  // Duplica la cadena para almacenarla
    }

    // Cerrar el directorio
    closedir(dp);

    // Procesar los archivos concurrentemente
    if (fileCount > 0) {
        processFilesConcurrently(filePaths, fileCount, list);
    }

    // Liberar la memoria del array de rutas de archivos
    for (int i = 0; i < fileCount; i++) {
        free((void *)filePaths[i]);
    }
    free(filePaths);
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
    clock_t ti, a;
    ti = clock();
    cpuTimeUsed = ((double) (ti - start)) / CLOCKS_PER_SEC;
    printf("Process file dura: %f segundos\n", cpuTimeUsed);
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
        perror("Error al crear el archivo comprimido");
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
    a = clock();
    compress(directory, compressFile);

    ti = clock();
    cpuTimeUsed = ((double) (ti - a)) / CLOCKS_PER_SEC;
    printf("Compress dura: %f segundos\n", cpuTimeUsed);


    fclose(compressFile); //Close file
    
    //printNode(Tree);
    freeNode(Tree); // Input: Tree, Output: None, Function: Destroys it to free memory
    //printTable(table);
    destroyTable(table); // Input: Table, Output: None, Function: Destroys it to

    end = clock();
    cpuTimeUsed = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("La compresion de huffman en Serial duro: %f segundos\n", cpuTimeUsed);

    return 0;

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
