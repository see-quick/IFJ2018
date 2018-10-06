#include <stdio.h>
#include <malloc.h>
#include "parser_list.h"

void listInit(tListOfIDF *L)
// funkce inicializuje seznam 
{
  L->first  = NULL;
  L->last   = NULL;
  L->active = NULL;
}
  
void listFree(tListOfIDF *L)
// funkce dealokuje seznam 
{
  tListItem *ptr;
  while (L->first != NULL)
  {
    ptr = L->first;
    L->first = L->first->nextItem;
    // uvolnime celou polozku
    free(ptr);
  }
}

void listInsertLast(tListOfIDF *L, char *name)
// vlozi novou instruci na konec seznamu
{
  tListItem *newItem;
  newItem = malloc(sizeof (tListItem));
  newItem->name = name;

  newItem->nextItem = NULL;

  if (L->first == NULL)
     L->first = newItem;
  else
    L->last->nextItem=newItem;
    L->last=newItem;
}

void listFirst(tListOfIDF *L)
// zaktivuje prvni 
{
  L->active = L->first;
}

void listNext(tListOfIDF *L)
// aktivni instrukci se stane nasledujici instrukce
{
  if (L->active != NULL)
  L->active = L->active->nextItem;
}


char *listGetData(tListOfIDF *L)
// vrati posledni identifikator
{
  if (L->last == NULL)
  {
    return NULL;
  }
  else return L->last->name;
}
