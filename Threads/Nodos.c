#include "Nodos.h"
#include <stdlib.h>
#include <stdio.h>

#define DEBUG printf("Aqui\n");

/*
    Function that insert a symbol in a list
    input
    -list: Node * type, is the actual position in the list
    -element: Node * type, is the element we want to insert
    -head: Node * type, is the head of the list
    -aux: Node * type, is the next node in the list
    output
    none
*/
void insertSymbol(Node *list, Node *element, Node *head, Node *aux){    
    element->next = aux;
    if(!list) head = element;
    else list->next = element;
}

/*
    Function that release the memory of the list
    input
    -head: Node * type: head of the list
    output 
    none
*/
void freeNode(Node *head){
    if(head->left) freeNode(head->left);
    if(head->right) freeNode(head->right);
    free(head);
}


void printNode(Node **head){
    Node *aux;
    aux = *head;
    while(aux){
        printf("Simbolo: %c cuenta: %i\n", aux->symbol, aux->count);
        if(aux->left)
            printf("izquierda Simbolo: %c cuenta: %i\n", aux->left->symbol, aux->left->count);
        if(aux->right)
            printf("derecha Simbolo: %c cuenta: %i\n", aux->right->symbol, aux->right->count);
        aux = aux->next;
    }
}

/*
    Function that sort a linklist from smallest to largest
    input
    -head: Node * type, this is the head of the list
    output
    none
*/

void sortList(Node **head){
    Node *listAux;
    Node *aux;
    listAux = *head;
    *head = NULL;
    
    while(listAux){
        aux = listAux;
        listAux = aux->next;
        
        insertInOrder(head, aux);
    }
}
/*
    Function that insert a element in the position that belongs
    input
    -head: Node * type, this is the head of the list
    -element: Node * type, this is the element that we are going to add to the list
    output
    none
*/
void insertInOrder(Node **head, Node *element){
    Node *aux;
    Node *auxNext;
    
    if(!*head){
        *head = element;
        (*head)->next = NULL;
    }else{
        aux = *head;
        auxNext = NULL;
        while(aux && aux->count < element->count){
            auxNext = aux;
            aux = aux->next;
            
        }
        element->next = aux;
        if(auxNext) auxNext->next = element;
        else *head = element;
    }
}



// void sortList(Node **headRef) {
//     Node *sorted = NULL;  // Crear una nueva cabeza para la lista ordenada
//     Node *current = *headRef;  // Comenzar con la lista existente

//     while (current != NULL) {
//         Node *next = current->next;  // Almacenar el siguiente nodo para continuar
//         insertInOrder(&sorted, current);  // Insertar el nodo actual en la lista ordenada
//         current = next;  // Moverse al siguiente nodo
//     }

//     *headRef = sorted;  // Actualizar la cabeza de la lista original para que apunte a la lista ordenada
// }

// void insertInOrder(Node **headRef, Node *newNode) {
//     Node **current = headRef;

//     // Encontrar la posición correcta para insertar el nuevo nodo
//     while (*current != NULL && (*current)->count < newNode->count) {
//         current = &((*current)->next);
//     }

//     // Insertar el nuevo nodo en la posición encontrada
//     newNode->next = *current;
//     *current = newNode;
// }
