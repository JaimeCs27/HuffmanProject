
// Main program to compress files
#include "Tabla.h"
#include "Nodos.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG printf("Aqui\n");

unsigned int characters[97];
int indexC = 0;
void CountCharacter(Node **list, unsigned char character);

// Global variables
extern Table *table;
long int fileLength = 0;

void processFile(const char *filePath, Node **list) {
  FILE *file = fopen(filePath, "r");
  if (!file) {
    printf("Error al abrir el archivo %s\n", filePath);
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

void compressFile(const char* path, FILE *compress, unsigned long int *dWORD, int *nBits){
    //printf("PATH: %s\n", path);
    FILE *fe = fopen(path, "r");
    if(!fe){
      printf("Error al comprimir archivo\n");
      return;
    }
    unsigned char c;
    
  do {
      c = fgetc(fe);
      if (feof(fe)) {
          break;
      }
      Table *t;
      //look for the symbol
      t = findSymbol(table, c);
      
      while (*nBits + t->nBits > 32) {
          c = *dWORD >> (*nBits - 8);           
          fwrite(&c, sizeof(char), 1, compress);    
          *nBits -= 8;                         
      }
      *dWORD <<= t->nBits;
      *dWORD |= t->bits;
      *nBits += t->nBits;
  } while (1);

  // Extract bits form dWORD
  while (*nBits > 0) {
      if (*nBits >= 8) 
        c = *dWORD >> (*nBits - 8);
      else 
        c = *dWORD << (8 - *nBits);
      fwrite(&c, sizeof(char), 1, compress);
      *nBits -= 8;
  }

  fclose(fe);
}

void compress(const char* directoryPath, FILE *compress){
    
    struct dirent *entry;
    DIR *dp = opendir(directoryPath);
    if (dp == NULL) {
        perror("The directory can not be opened");
        return;
    }
    
    indexC = 0;
    
    while ((entry = readdir(dp))) {
      unsigned long dWORD = 0;  
      int nBits = 0;
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
      int cant = 0;
      char filePath[1024];
      snprintf(filePath, sizeof(filePath), "%s/%s", directoryPath, entry->d_name);
      while(1){
        if(entry->d_name[cant] == '\0')
          break;
        cant++;
      }
      unsigned int a = characters[indexC];
      //printf("%i entry\n", cant);
      //printf("compress: %s\n", entry->d_name);
      fwrite(&cant, sizeof(int), 1, compress);
      fwrite(&entry->d_name, sizeof(char[cant]), 1, compress);
      fwrite(&a, sizeof(unsigned int), 1, compress);
      compressFile(filePath, compress, &dWORD, &nBits);
      indexC++;
      //printf("Archivo: %s, Longitud: %ld caracteres\n", entry->d_name, fileLength);
    }
    closedir(dp);
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

int main(int argc, char *argv[]) {
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
      perror("Error al crear el archivo comprimido");
      return 1;
  }

    

  // lenght of file
  fwrite(&fileLength, sizeof(long int), 1, compressFile);

  

  // Count elements in the table
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
                       // free memory

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
      //insertNewSymbol(previous, current, *list, character);
    }
  }
}
