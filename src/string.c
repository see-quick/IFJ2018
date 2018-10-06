/**
 * Predmet:  IFJ 
 * Projekt:  Implementace prekladace imperativniho jazyka IFJ18
 * Soubor:   string.c
 *
 * Popis: program pro praci s retezcemi
 *
 *
 * Datum:
 *
 * Autori:   Maros Orsak       vedouci 
 *           Polishchuk Kateryna     <xpolis03@fit.vutbr.cz>           
 *           Igor Ignac           
 *           Marek Rohel       

*/

/*Pouzite knihovny */
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "scanner.h"
#include "error.h"


/**********************************/
/***********Strings****************/
/**********************************/

int strInit (tString *s)
{
  s->length = 0;
  if ((s->str = (char *) calloc(SIZE, sizeof(char))) == NULL)
    return INT_ERR;
  s->memory = SIZE;

  return SUCCESS;
}

int strAdd (tString *s, char c)
{
  if (s->memory == s->length) {
    s->memory += SIZE;
    if ((s->str = (char *) realloc(s->str, s->memory * sizeof(char))  ) == NULL)
      return INT_ERR;
  }

  s->str[s->length++] = c;
  s->str[s->length] = '\0';

  return SUCCESS;
}



int strFree (tString *s)
{
  if (s->str)
    free(s->str);
  s->str = NULL;
  s->memory = 0;
  s->length = 0;

  return SUCCESS;
}


tString strCreate (char *array)
{
  tString s = {NULL, 0, 0};
  if (strInit(&s) == INT_ERR)
    return s;

  int i = 0;
  while (array[i] != '\0')
    if (strAdd(&s, array[i++]) == INT_ERR) {
      strFree(&s);
      return s;
    }

  return s;
}

int strClear (tString *s)
{
   s->str[0] = '\0';
   s->length = 0;

  return SUCCESS;
}

int strCopy (tString *s, char *array)
{
  array = (char *) realloc(array, s->length + 1);
  if (array == NULL)
    return INT_ERR;

  int i = s->length;
  do
    array[i] = s->str[i];
  while (i--);

  return SUCCESS;
}


int strCopyStr(tString *s1, tString *s2){
   int new_length = s2->length;
   if (new_length >= s1->memory){
      if ((s1->str = (char*) realloc(s1->str, new_length + 1)) == NULL)
         return INT_ERR;
      s1->memory = new_length + 1;
   }

   strcpy(s1->str, s2->str);
   s1->length = new_length;
   return SUCCESS;
}

int strCompare(tString *s1, tString *s2){
  return (strcmp(s1->str, s2->str));
}


int strCopyArr(tString *s, char array[]){
   strClear(s);
   int length = strlen(array);
   for(int i = 0; i < length; i++) {
      if(strAdd(s, array[i]) != SUCCESS)
         return INT_ERR;     
   }
   return SUCCESS;
}


char *getStr(tString *s){
  return s->str;
}

