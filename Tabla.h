#include "Nodos.h"

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