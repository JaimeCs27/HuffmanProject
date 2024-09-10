# HuffmanProject
Este proyecto implementara el algoritmo de compresion y descompresion de Huffman, utilizando 3 métodos distintos. También se tomará el tiempo de ejecución de cada método utilizado para realizar una comparación de tiempo de ejecución.

## Tabla de Contenidos
 - [Implementacion](#Implementación)
 - [Compilación](#Compilación)
 - [Uso](#Uso)
 - [Créditos](#Créditos)

## Implementación

En este proyecto se implementarón tres enfoques, un algoritmo Serial, uno Paralelo y uno Concurrente.
Para la creación del arbol binario se utilizó la siguiente estructura:
```c
typedef struct Node {
    struct Node *next;
    struct Node *left;
    struct Node *right;
    unsigned char symbol;
    int count;
}Node;
```
