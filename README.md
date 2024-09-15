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
    struct Node *next;      // Puntero al siguiente nodo en la lista
    struct Node *left;      // Puntero al lado izquierdo del arbol binario, su valor es de 0
    struct Node *right;     // Puntero al lado derecho del arbol binario, su valor es de 1
    unsigned char symbol;   // Caractér almacenado
    int count;              // Frecuencia en que aparece el caractér
}Node;

void insertSymbol(Node *List, Node *element, Node *head, Node *aux);
void sortList(Node **head);
void insertInOrder(Node **head, Node *element);
void freeNode(Node *head);
void printNode(Node **head);
```

Tambien se utilizará un archivo Tabla.h que se utilizará para la creación de la tabla de huffman, contiene lo siguiente:
```c
// Structure for the table
typedef struct Table
{
    unsigned char symbol;   // Stores the character in the table
    unsigned char nBits;    // Number of bits stored
    unsigned long int bits; // Bit encoding
    struct Table *next;     // Pointer to the next element in the table
} Table;

// Function to create a table recursively
void createTable(Node *node, int nBits, int bits);

// Function to insert an element into the table
void insertElement(unsigned char c, int nBits, int bits);

// Function to find a symbol in the table
Table *findSymbol(Table *table, unsigned char symbol);

// Function to destroy the table
void destroyTable(Table *table);
```

Ahora se analizará cada método diferente y como se implementó.

### Serial
El algoritmo serial, sería el siguiente:
1. Procesa el directorio dado y crea una lista enlazada que contiene los símbolos utilizados en todos los libros y la frecuencia de cada símbolo.
2. La lista creada se ordena de menor a mayor conforme a su frecuencia.
3. Se crea el arbol binario de Huffman, donde el lado izquierdo es un 0 y el lado derecho es un 1.
4. Se crea la tabla de Huffman recorriendo el arbol creado.
5. Guardamos en el archivo binario, la cantidad de caracteres totales, cantidad de elementos en la tabla y guardamos la tabla que se generó.
6. Leemos la carpeta correspondiente, y leemos linealmente cada archivo, por cada archivo, guardamos en el binario la cantidad de caracteres del nombre archivo, luego el nombre del archivo, la cantidad de caracteres del archivo.
7. Por archivo, leemos un caracter, lo buscamos en la tabla Huffman y recorremos el arbol binario, si es izquierda se escribe un 0 en el binario, si es derecha se escribe un 1 en el binario.
8. Cerramos el archivo binario.

### Paralelo
Para este método se utilizó el mmap para poder compartir una variable entre los procedimientos.
El algoritmo paralelo, sería el siguiente:
1. Procesa el directorio dado y crea una lista enlazada que contiene los símbolos utilizados en todos los libros y la frecuencia de cada símbolo.
2. La lista creada se ordena de menor a mayor conforme a su frecuencia.
3. Se crea el arbol binario de Huffman, donde el lado izquierdo es un 0 y el lado derecho es un 1.
4. Se crea la tabla de Huffman recorriendo el arbol creado.
5. Guardamos en el archivo binario, la cantidad de caracteres totales, cantidad de elementos en la tabla y guardamos la tabla que se generó.
6. Abrimos el directorio, y aqui por cada archivo, realizamos un fork().
7. Por cada archivo guardamos en una variable compartida por los procesos la informacion de ese archivo.
8. Al finalizar todos los procesos, escribimos todos los datos de los archivos.

### Concurrente
Para este método se utilizó la biblioteca pthread, para el proceso de sincronización de hilos.
El algoritmo concurrente, sería el siguiente:
1. Procesa el directorio dado y crea una lista enlazada que contiene los símbolos utilizados en todos los libros y la frecuencia de cada símbolo.
2. La lista creada se ordena de menor a mayor conforme a su frecuencia.
3. Se crea el arbol binario de Huffman, donde el lado izquierdo es un 0 y el lado derecho es un 1.
4. Se crea la tabla de Huffman recorriendo el arbol creado.
5. Guardamos en el archivo binario, la cantidad de caracteres totales, cantidad de elementos en la tabla y guardamos la tabla que se generó.

## Créditos

- Jaime Cabezas Segura
- Sebastián Lopez Villavicencio
- Natasha Calderón Rojas
