// Main program to compress files
#include "Tabla.h"
#include "Nodos.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define DEBUG printf("Aqui\n");

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
  character = fgetc(file);
  while ( !feof(file)) {
    CountCharacter(list, character);
    fileLength++; // Incrementa la longitud por cada carácter leído
    character = fgetc(file);
  }
  fclose(file);
}

void compressFile(const char* path, FILE *compress, unsigned long int *dWORD, int *nBits){
    
    printf("PATH: %s\n", path);

    FILE *fe = fopen(path, "r");
    
    if(!fe){
      printf("Error al comprimir archivo\n");
      return(0);
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
      if (*nBits >= 8) c = *dWORD >> (*nBits - 8);
      else c = *dWORD << (8 - *nBits);
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
    unsigned long dWORD = 0;  
    int nBits = 0;  
    while ((entry = readdir(dp))) {
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

      char filePath[1024];
      snprintf(filePath, sizeof(filePath), "%s/%s", directoryPath, entry->d_name);
      
      compressFile(filePath, compress, &dWORD, &nBits);
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

        // Imprimir información adicional si es necesario
        printf("Archivo: %s, Longitud: %ld caracteres\n", entry->d_name, fileLength);
    }

    closedir(dp);
}

int main(int argc, char *argv[]) {
  Node *List;
  Node *Tree;

  processDirectory("Libros", &List);
  //printNode(&List);
  sortList(&List);

  


  //printNode(&List);
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
  //printNode(&Tree);
  
  createTable(Tree, 0, 0);
  //printTable(table);

  

  FILE *compressFile = fopen("Libros.bin", "wb");
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


  

  //compress("Prueba", compressFile);

  fclose(compressFile); //Close file

  

  freeNode(Tree); // Input: Tree, Output: None, Function: Destroys it to free memory
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
