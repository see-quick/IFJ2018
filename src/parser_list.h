typedef struct listItem
{
  char * name;
  struct listItem *nextItem;
} tListItem;
    
typedef struct
{
  struct listItem *first;  // ukazatel na prvni prvek
  struct listItem *last;   // ukazatel na posledni prvek
  struct listItem *active; // ukazatel na aktivni prvek
} tListOfIDF;

void listInit(tListOfIDF *L);
void listFree(tListOfIDF *L);
void listInsertLast(tListOfIDF *L, char *);
void listFirst(tListOfIDF *L);
void listNext(tListOfIDF *L);
char *listGetData(tListOfIDF *L);
