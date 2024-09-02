
// Main program to compress files
#include "Tabla.h"
#include "Nodos.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// //Nodos.h
// typedef struct Node {
//     struct Node *next;
//     struct Node *left;
//     struct Node *right;
//     unsigned char symbol;
//     int count;
// }Node;

// void insertNewSymbol(Node *actual, Node *next, Node *List, unsigned char c);
// void insertSymbol(Node *List, Node *element, Node *head, Node *aux);
// void sortList(Node **head);
// void insertInOrder(Node **head, Node *element);
// void freeNode(Node *head);
// void printNode(Node **head);

// /*
//     Function that insert a symbol in a list
//     input
//     -list: Node * type, is the actual position in the list
//     -element: Node * type, is the element we want to insert
//     -head: Node * type, is the head of the list
//     -aux: Node * type, is the next node in the list
//     output
//     none
// */
// void insertSymbol(Node *list, Node *element, Node *head, Node *aux){    
//     element->next = aux;
//     if(!list) head = element;
//     else list->next = element;
// }
// /*
//     Function that inserts a new entry in a list
//     input
//     -actual: Node * type, this is the actual position in the list
//     -next: Node * type, this is the next position in the list
//     -List" Node * type, this is the head of the list
//     -c: unsigned char, this is the symbol that we are going to insert
//     output
//     none
// */
// void insertNewSymbol(Node *actual, Node *next, Node *List, unsigned char c){
//     Node *newNode = (Node *)malloc(sizeof(Node));
//     newNode->symbol = c;
//     newNode->left = newNode->right = NULL;
//     newNode-> count = 1;
//     insertSymbol(actual, newNode, List, next);

    
// }

// /*
//     Function that sort a linklist from smallest to largest
//     input
//     -head: Node * type, this is the head of the list
//     output
//     none
// */

// /*
//     Function that release the memory of the list
//     input
//     -head: Node * type: head of the list
//     output 
//     none
// */
// void freeNode(Node *head){
//     if(head->left) freeNode(head->left);
//     if(head->right) freeNode(head->right);
//     free(head);
// }


// void printNode(Node **head){
//     Node *aux;
//     aux = *head;
//     while(aux){
//         printf("Simbolo: %c cuenta: %i\n", aux->symbol, aux->count);
//         if(aux->left)
//             printf("izquierda Simbolo: %c cuenta: %i\n", aux->left->symbol, aux->left->count);
//         if(aux->right)
//             printf("derecha Simbolo: %c cuenta: %i\n", aux->right->symbol, aux->right->count);
//         aux = aux->next;
//     }
// }





// void sortList(Node **head){
//     Node *listAux;
//     Node *aux;
//     listAux = *head;
//     *head = NULL;
    
//     while(listAux){
//         aux = listAux;
//         listAux = aux->next;
        
//         insertInOrder(head, aux);
//     }
// }
// /*
//     Function that insert a element in the position that belongs
//     input
//     -head: Node * type, this is the head of the list
//     -element: Node * type, this is the element that we are going to add to the list
//     output
//     none
// */
// void insertInOrder(Node **head, Node *element){
//     Node *aux;
//     Node *auxNext;
    
//     if(!*head){
//         *head = element;
//         (*head)->next = NULL;
//     }else{
//         aux = *head;
//         auxNext = NULL;
//         while(aux && aux->count < element->count){
//             auxNext = aux;
//             aux = aux->next;
            
//         }
//         element->next = aux;
//         if(auxNext) auxNext->next = element;
//         else *head = element;
//     }
// }


// //Tabla.h

// typedef struct Table
// {
//     unsigned char symbol;   // Stores the character in the table
//     unsigned char nBits;    // Number of bits stored
//     unsigned long int bits; // Bit encoding
//     struct Table *next;     // Pointer to the next element in the table
// } Table;

// // Function to create a table recursively
// void createTable(Node *node, int nBits, int bits);

// // Function to insert an element into the table
// void insertElement(unsigned char c, int nBits, int bits);

// // Function to find a symbol in the table
// Table *findSymbol(Table *table, unsigned char symbol);

// // Function to destroy the table
// void destroyTable(Table *table);

// void createTable(Node *node, int nBits, int bits)
// {
//     if (node->right){
//         createTable(node->right, nBits + 1, (bits << 1) | 1);
//     }
        
//     if (node->left){
//         createTable(node->left, nBits + 1, bits << 1);
//     }
        
//     if (!node->right && !node->left)
//         insertElement(node->symbol, nBits, bits);
// }

// Table *table;

// // Function to insert an element into the table
// void insertElement(unsigned char c, int nBits, int bits)
// {
//     Table *t, *p, *a;

//     t = (Table *)malloc(sizeof(Table));
//     t->symbol = c;
//     t->bits = bits;
//     t->nBits = nBits;

//     // If the table is empty, initialize it with the new element
//     if (table == NULL)
//     {
//         table = t;
//         table->next = NULL;
//     }
//     else
//     {
//         p = table;
//         a = NULL;
//         // Traverse the table to find the insertion point
//         while (p && p->symbol < t->symbol)
//         {
//             a = p;
//             p = p->next;
//         }
//         t->next = p;
//         if (a)
//         {
//             a->next = t;
//         }
//         else
//         {
//             table = t;
//         }
//     }
// }

// // Function to find a symbol in the table
// Table *findSymbol(Table *table, unsigned char symbol)
// {
//     Table *t = table;
//     while (t && t->symbol != symbol)
//     {
//         t = t->next;
//     }
//     return t;
// }

// // Function to destroy the table
// void destroyTable(Table *table)
// {
//     Table *temp;
//     while (table != NULL)
//     {
//         temp = table;
//         table = table->next;
//         free(temp);
//     }
// }

// void printTable(Table *table)
// {
//     Table *temp;
//     temp = table;
//     while (temp != NULL)
//     {
//         printf("Simbolo: %c, nBits: %i, bits: %li \n", temp->symbol, temp->nBits, temp->bits);
//         temp = temp->next;
//     }
// }










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
<<<<<<< HEAD
      //printf("%i entry\n", cant);
      //printf("compress: %s\n", entry->d_name);
=======
      // printf("%i entry\n", cant);
      // printf("compress: %s\n", entry->d_name);
>>>>>>> sos
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

  processDirectory("Prueba", &List);
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

  printf("elementos: %i\n", countElements);

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


  

  compress("Prueba", compressFile);

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
