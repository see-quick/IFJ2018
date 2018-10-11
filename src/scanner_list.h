#include <stdio.h>
#include <stdlib.h>

#define FALSE 0
#define TRUE 1

extern int errflg;
extern int solved;
 
typedef struct tDLElem {                 
		char * data;
		int is_important;
        struct tDLElem *lptr;          /* ukazatel na předchozí prvek seznamu */
        struct tDLElem *rptr;        /* ukazatel na následující prvek seznamu */
} *tDLElemPtr;

typedef struct {                                  /* dvousměrně vázaný seznam */
    tDLElemPtr First;                      /* ukazatel na první prvek seznamu */
    tDLElemPtr Act;                     /* ukazatel na aktuální prvek seznamu */
    tDLElemPtr Last;                    /* ukazatel na posledni prvek seznamu */
} tDLList;

                                             /* prototypy jednotlivých funkcí */
void DLInitList (tDLList *);
void DLDisposeList (tDLList *);
void DLInsertFirst (tDLList *, char *);
void DLInsertLast(tDLList *, char *);
void DLFirst (tDLList *);
void DLLast (tDLList *);
char * DLCopyFirst (tDLList *);
void DLCopyLast (tDLList *, char *);
void DLDeleteFirst (tDLList *);
void DLDeleteLast (tDLList *);
void DLPostDelete (tDLList *);
void DLPreDelete (tDLList *);
void DLPostInsert (tDLList *, char *);
void DLPreInsert (tDLList *, char *);
void DLCopy (tDLList *, char *);
void DLActualize (tDLList *, char *);
void DLSucc (tDLList *);
void DLPred (tDLList *);
int DLActive (tDLList *);
void print_elements_of_list(tDLList);
void DLIsImportant(tDLList *);
char * DLFirstImportant(tDLList *L);
char * DLLastImportant(tDLList *L);
void DLNotImportant(tDLList *L);
