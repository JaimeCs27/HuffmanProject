typedef struct Node {
    struct Node *next;
    struct Node *left;
    struct Node *right;
    unsigned char symbol;
    int count;
}Node;

void insertNewSymbol(Node *actual, Node *next, Node *List, unsigned char c);
void insertSymbol(Node *List, Node *element, Node *head, Node *aux);
void sortList(Node *head);
void insertInOrder(Node *head, Node *element);
void freeNode(Node *head);