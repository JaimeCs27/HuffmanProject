// Main program to compress files
#include "Nodos.h"
#include "Tabla.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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

  while ((character = fgetc(file)) != EOF) {
    CountCharacter(list, character);
    fileLength++; // Incrementa la longitud por cada carácter leído
  }

  fclose(file);
}

void compressFile(const char* path, FILE *compress, unsigned long *dWORD, int *nBits){
    FILE *fe = fopen(path, 'r');
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

      //look for the symbol
      t = findSymbol(Table, c);
      
      while (*nBits + t->nbits > 32) {
          c = dWORD >> (*nBits - 8);           
          fwrite(&c, sizeof(char), 1, compress);    
          *nBits -= 8;                         
      }
      *dWORD <<= t->nbits;  
      *dWORD |= t->bits;    
      *nBits += t->nbits;   
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
    unsigned char c;
    while ((entry = readdir(dp))) {
        if (entry->d_type == DT_REG) {  // Procesa todos los archivos regulares
            char filePath[1024];
            snprintf(filePath, sizeof(filePath), "%s/%s", directoryPath, entry->d_name);
            
            compressFile(filePath, list, &dWORD, &nBits);
            //printf("Archivo: %s, Longitud: %ld caracteres\n", entry->d_name, fileLength);
        }
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
        if (entry->d_type == DT_REG) {  // Procesa todos los archivos regulares
            char filePath[1024];
            snprintf(filePath, sizeof(filePath), "%s/%s", directoryPath, entry->d_name);
            
            processFile(filePath, list);
            printf("Archivo: %s, Longitud: %ld caracteres\n", entry->d_name, fileLength);
        }
    }

    closedir(dp);
}

int main(int argc, char *argv[]) {
  Node *List;
  Node *Tree;

  processDirectory("Libros", &List);
  sortList(List);
  Tree = List;
  createTable(Tree, 0, 0);

  

  FILE *compress = fopen("Libros", "wb");
  if (!compress) {
      perror("Error al crear el archivo comprimido");
      return 1;
  }

  // lenght of file
  fwrite(fileLength, sizeof(long int), 1, compress);

  // Count elements in the table
  int countElements = 0;
  Table *t = table;  
  while (t) {
      countElements++;
      t = t->sig;
  }

  // Write the number of elements in the table
  fwrite(countElements, sizeof(int), 1, compress);

  // Save the table
  t = table; 
  while (t) { 
      fwrite(&t->letter, sizeof(char), 1, compress);
      fwrite(&t->bits, sizeof(unsigned long int), 1, compress);
      fwrite(&t->nbits, sizeof(char), 1, compress);
      t = t->sig;
  }


  compress("libros". compress);
  fclose(compress); //Close file

  

  destroyTree(Tree); // Input: Tree, Output: None, Function: Destroys it to free memory
  destroyTable(Table); // Input: Table, Output: None, Function: Destroys it to
                       // free memory

  return 0;
}

void CountCharacter(Node **list, unsigned char character) {
  Node *current, *previous, *newNode;

  if (!*list) // If the list is empty, create a new node as the head
  {
    *list = (Node *)malloc(sizeof(Node)); // Create a new node
    (*list)->letter = character;                  // Assign the character
    (*list)->frequency = 1; // Initialize the frequency to 1
    (*list)->next = (*list)->zero = (*list)->one = NULL; // Initialize pointers
  } else {
    // Find the correct position in the list for the character
    current = *list;
    previous = NULL;
    while (current && current->letter < character) {
      previous = current;      // Keep reference to the previous node
      current = current->next; // Move to the next node
    }

    // Check if the character already exists in the list
    if (current && current->letter == character) {
      current->frequency++; // If it exists, increment its frequency
    } else {
      insertNewSymbol(previous, current, *list, character);
    }
  }
}
