# HuffmanProject
Este proyecto implementara el algoritmo de compresion y descompresion de Huffman, utilizando 3 métodos distintos. También se tomará el tiempo de ejecución de cada método utilizado para realizar una comparación de tiempo de ejecución.

## Tabla de Contenidos
 - [Implementacion](#Implementación)
 - [Compilación](#Compilación)
 - [Uso](#Uso)
 - [Créditos](#Créditos)

## Implementación

En este proyecto se implementarón tres enfoques, un algoritmo Serial, uno Paralelo y uno Concurrente.
Para la creación del arbol binario se creo un archivo Nodo.h que contiene lo siguiente:
```c
typedef struct Node {
    struct Node *next; // Puntero al siguiente nodo en la lista
    struct Node *left; // Puntero al lado izquierdo del arbol binario, su valor es de 0
    struct Node *right; // Puntero al lado derecho del arbol binario, su valor es de 1
    unsigned char symbol; // Caractér almacenado
    int count; // Frecuencia en que aparece el caractér
}Node;

void insertNewSymbol(Node *actual, Node *next, Node *List, unsigned char c);
void insertSymbol(Node *List, Node *element, Node *head, Node *aux);
void sortList(Node **head);
void insertInOrder(Node **head, Node *element);
void freeNode(Node *head);
void printNode(Node **head);
```


