// Main program to compress files
#include "Nodos.h"
#include "Tabla.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct NodeType {
  unsigned char letter;
  int frequency;
  struct NodeType *next, *zero, *one;
} NodeType;

void CountCharacter(NodeType **list, unsigned char character);

// Global variables
Table *Table;

void processFile(const char *filePath, NodeType **list, long *fileLength) {
  FILE *file = fopen(filePath, "r");
  if (!file) {
    printf("Error al abrir el archivo %s\n", filePath);
    return;
  }

  unsigned char character;
  *fileLength = 0; // Inicializa la longitud del archivo

  while ((character = fgetc(file)) != EOF) {
    CountCharacter(list, character);
    (*fileLength)++; // Incrementa la longitud por cada carácter leído
  }

  fclose(file);
}

void processDirectory(const char *directoryPath, NodeType** list) {
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
            long fileLength = 0; 
            processFile(filePath, list, &fileLength);
            printf("Archivo: %s, Longitud: %ld caracteres\n", entry->d_name, fileLength);
        }
    }

    closedir(dp);
}

int main(int argc, char *argv[]) {
  NodeType *List;
  NodeType *Tree;

  processDirectory("Libros", &List);
  sortList(List);
  Tree = createTree(List);
  Table = createTable(Tree);

  

  FILE *fs = fopen(argv[2], "wb");
  if (!fs) {
      perror("Error al crear el archivo comprimido");
      return 1;
  }

  // lenght of file
  fwrite(&fileLength, sizeof(long int), 1, fs);

  // Count elements in the table
  int countElements = 0;
  Table *t = Table;  
  while (t) {
      countElements++;
      t = t->sig;
  }

  // Write the number of elements in the table
  fwrite(&countElements, sizeof(int), 1, fs);

  // Save the table
  t = Table; 
  while (t) { 
      fwrite(&t->letter, sizeof(char), 1, fs);
      fwrite(&t->bits, sizeof(unsigned long int), 1, fs);
      fwrite(&t->nbits, sizeof(char), 1, fs);
      t = t->sig;
  }


  FILE *fe = fopen(argv[1], "r");
  if (!fe) {
      perror("Error al abrir el archivo de entrada");
      fclose(fs);
      return 1;
  }

  unsigned long dWORD = 0;  
  int nBits = 0;  
  unsigned char c;

  do {
      c = fgetc(fe);
      if (feof(fe)) {
          break;
      }

      //look for the symbol
      t = findSymbol(Table, c);
      
      while (nBits + t->nbits > 32) {
          c = dWORD >> (nBits - 8);           
          fwrite(&c, sizeof(char), 1, fs);    
          nBits -= 8;                         
      }
      dWORD <<= t->nbits;  
      dWORD |= t->bits;    
      nBits += t->nbits;   
  } while (1);

  // Extract bits form dWORD
  while (nBits > 0) {
      if (nBits >= 8) c = dWORD >> (nBits - 8);
      else c = dWORD << (8 - nBits);
      fwrite(&c, sizeof(char), 1, fs);
      nBits -= 8;
  }

  fclose(fe);  // Close file
  fclose(fs); //Close file

  

  destroyTree(Tree); // Input: Tree, Output: None, Function: Destroys it to free memory
  destroyTable(Table); // Input: Table, Output: None, Function: Destroys it to
                       // free memory

  return 0;
}

void CountCharacter(NodeType **list, unsigned char character) {
  NodeType *current, *previous, *newNode;

  if (!*list) // If the list is empty, create a new node as the head
  {
    *list = (NodeType *)malloc(sizeof(NodeType)); // Create a new node
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
      // If it doesn't exist, create a new node for the character
      newNode = (NodeType *)malloc(sizeof(NodeType));
      newNode->letter = character;
      newNode->frequency = 1;
      newNode->zero = newNode->one = NULL;
      newNode->next = current; // Link the new node in the list

      if (previous)
        previous->next =
            newNode; // If not the first, link with the previous node
      else
        *list = newNode; // If it's the first, update the list head
    }
  }
}
