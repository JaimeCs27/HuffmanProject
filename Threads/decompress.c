#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <pthread.h>

#define DEBUG printf("Aqui\n");

typedef struct tree {
    unsigned char symbol;
    unsigned long int bits;
    char nBits;
    struct tree *left;
    struct tree *right;
} Node;

// Estructura para pasar los parámetros a cada hilo
typedef struct {
    Node *tree;
    FILE *fi;
    char title[256];  // Almacena el título del libro
    unsigned long totalCharacters;  // Cantidad total de caracteres del libro
    unsigned long compressedSize;   // Tamaño comprimido del contenido
    unsigned char *compressedData;  // Puntero a los datos comprimidos
    char *directory;                // Directorio de salida
} ThreadParams;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void createTree(Node *current, Node *newNode, Node *tree, FILE *fi, int elements);
void *decompressFileThread(void* params);
void decompressBook(ThreadParams *threadParams);
Node *rebuildHuffmanTree(FILE *fi, int numElements);
void deleteTree(Node *n);
void decompress(const char *compressedFilePath, const char *outputDirectory);

// Función para eliminar el árbol de Huffman
void deleteTree(Node *n) {
    if (n->left) deleteTree(n->left);
    if (n->right) deleteTree(n->right);
    free(n);
}

// Función que descomprime el contenido del archivo comprimido
void *decompressFileThread(void* params) {
    ThreadParams *threadParams = (ThreadParams*) params;
    decompressBook(threadParams);
    free(threadParams->compressedData);  // Liberar memoria de los datos comprimidos
    pthread_exit(NULL);
}

// Función para descomprimir un libro individual
void decompressBook(ThreadParams *threadParams) {
    // Crear el archivo de salida con el título
    char outputPath[1024];
    snprintf(outputPath, sizeof(outputPath), "%s/%s", threadParams->directory, threadParams->title);
    FILE *outputFile = fopen(outputPath, "w");
    if (!outputFile) {
        perror("Error creating the output file");
        return;
    }

    // Recorrer el contenido comprimido y descomprimir usando el árbol de Huffman
    unsigned char bitBuffer = 0;
    int bitsInBuffer = 0;
    Node *current = threadParams->tree;
    int cantBook = threadParams->totalCharacters;

    for (unsigned long i = 0; i < threadParams->compressedSize; ++i) {
        for (int bit = 7; bit >= 0; --bit) {
            unsigned char currentBit = (threadParams->compressedData[i] >> bit) & 1;
            current = currentBit ? current->right : current->left;
            
            if (!current->left && !current->right) {
                // Es un nodo hoja, escribir el símbolo en el archivo de salida
                fputc(current->symbol, outputFile);
                cantBook--;
                current = threadParams->tree; // Regresar al inicio del árbol
            }
            if(cantBook <= 0)
                break;
        }
        if(cantBook <= 0)
            break;
    }

    fclose(outputFile);
}

// Función principal que coordina la descompresión utilizando hilos
void decompress(const char *compressedFilePath, const char *outputDirectory) {
    printf("%s\n%s\n", compressedFilePath, outputDirectory);
    FILE *fi = fopen(compressedFilePath, "rb");

    if (!fi) {
        perror("Error opening the compressed file");
        return;
    }

    // Leer la cantidad total de caracteres en el archivo comprimido
    long int totalCharacters;
    fread(&totalCharacters, sizeof(long int), 1, fi);

    // Leer el número de elementos en la tabla de Huffman
    int numElements;
    fread(&numElements, sizeof(int), 1, fi);

    // Reconstruir el árbol de Huffman
    Node *huffmanTree = rebuildHuffmanTree(fi, numElements);
    
    // Crear el directorio de salida si no existe
    mkdir(outputDirectory, 0777);

    int numBooks = 97;
    
    pthread_t threads[numBooks];
    ThreadParams threadParams[numBooks];

    for (int i = 0; i < numBooks; i++) {
        unsigned int titleLength;
        fread(&titleLength, sizeof(unsigned int), 1, fi);

        char title[256];
        fread(title, sizeof(char), titleLength, fi);
        title[titleLength] = '\0';  // Terminar la cadena

        unsigned long totalCharactersInBook;
        fread(&totalCharactersInBook, sizeof(unsigned long), 1, fi);

        unsigned long compressedSize;
        fread(&compressedSize, sizeof(unsigned long), 1, fi);

        unsigned char *compressedData = (unsigned char *)malloc(compressedSize);
        fread(compressedData, sizeof(unsigned char), compressedSize, fi);

        // Configurar los parámetros del hilo
        threadParams[i].tree = huffmanTree;
        strcpy(threadParams[i].title, title);
        threadParams[i].totalCharacters = totalCharactersInBook;
        threadParams[i].compressedSize = compressedSize;
        threadParams[i].compressedData = compressedData;
        threadParams[i].directory = (char *)outputDirectory;

        // Crear el hilo
        pthread_create(&threads[i], NULL, decompressFileThread, &threadParams[i]);
    }

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < numBooks; i++) {
        pthread_join(threads[i], NULL);
    }

    fclose(fi);
    deleteTree(huffmanTree); // Liberar el árbol de Huffman
}

// Función para reconstruir el árbol de Huffman desde el archivo comprimido
Node *rebuildHuffmanTree(FILE *fi, int numElements) {
    Node *tree = (Node *)malloc(sizeof(Node));
    memset(tree, 0, sizeof(Node)); // Inicializar el árbol

    for (int i = 0; i < numElements; i++) {
        unsigned char symbol;
        unsigned long int bits;
        char nBits;

        fread(&symbol, sizeof(unsigned char), 1, fi);
        fread(&bits, sizeof(unsigned long int), 1, fi);
        fread(&nBits, sizeof(char), 1, fi);

        // Reconstruir el nodo en el árbol de Huffman
        Node *current = tree;
        for (int bitPos = nBits - 1; bitPos >= 0; bitPos--) {
            unsigned char bit = (bits >> bitPos) & 1;

            if (bit) {
                if (!current->right) {
                    current->right = (Node *)malloc(sizeof(Node));
                    memset(current->right, 0, sizeof(Node));
                }
                current = current->right;
            } else {
                if (!current->left) {
                    current->left = (Node *)malloc(sizeof(Node));
                    memset(current->left, 0, sizeof(Node));
                }
                current = current->left;
            }
        }

        current->symbol = symbol;
    }

    return tree;
}

int main(int argc, char* argv[]) {
    Node *tree;
    long int characters;
    int elements;
    char *fileName;
    char *directory;

    if (argc > 3) {
        printf("Expecting less arguments\n");
        printf("Correct Usage: ./decompress <Compressed File Name> <Directory Name>\n");
        return 1;
    }

    if (argc == 2) {
        printf("Directory argument not given using the default name: 'CompressedFile'\n");
        directory = "CompressedFile";
    } else {
        directory = argv[2];
    }
    if (argc <= 1) {
        printf("Not enough arguments passed\n");
        printf("Correct Usage: ./decompress <Compressed File Name> <Directory Name>\n");
        return 1;
    }
    fileName = argv[1];

    // Crear el directorio
    struct stat st = {0};
    if (stat(directory, &st) == -1) {
        mkdir(directory, 0700);
    }

    clock_t start, end;
    double cpuTimeUsed;
    start = clock();

    decompress(fileName, directory);

    end = clock();
    cpuTimeUsed = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Concurrent huffman compression took:: %f seconds\n", cpuTimeUsed);
    return 0;
}
