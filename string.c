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

int strInit (tString *str)
{
  str->length = 0;
  if ((str->data = (char *) calloc(SIZE, sizeof(char))) == NULL)
    return INT_ERR;
  str->memory = SIZE;

  return SUCCESS;
}

int strAdd (tString *str, char c)
{
  if (str->memory == str->length) {
    str->memory += SIZE;
    if ((str->data = (char *) realloc(str->data, str->memory * sizeof(char)) ) == NULL)
      return INT_ERR;
  }

  str->data[str->length++] = c;
  str->data[str->length] = '\0';

  return SUCCESS;
}

int strFree (tString *str)
{
  if (str->data)
    free(str->data);
  str->data = NULL;
  str->memory = 0;
  str->length = 0;

  return SUCCESS;
}


tString strCreate (char *array)
{
  tString str = {NULL, 0, 0};
  if (strInit(&str) == INT_ERR)
    return str;

  int i = 0;
  while (array[i] != '\0')
    if (strAdd(&str, array[i++]) == INT_ERR) {
      strFree(&str);
      return str;
    }

  return str;
}

int strClear (tString *str)
{
   str->data[0] = '\0';
   str->length = 0;

  return SUCCESS;
}

int strCopy (tString *str, char *array)
{
  array = (char *) calloc(str->length + 1, sizeof(char));
  if (array == NULL)
    return INT_ERR;

  int i = str->length;
  do
    array[i] = str->data[i];
  while (i--);

  return SUCCESS;
}


int strCopyStr(tString *s1, tString *s2){
   int new_length = s2->length;
   if (new_length >= s1->memory){
      if ((s1->data = (char*) realloc(s1->data, new_length + 1)) == NULL)
         return INT_ERR;
      s1->memory = new_length + 1;
   }

   strcpy(s1->data, s2->data);
   s1->length = new_length;
   return SUCCESS;
}

int strCompare(tString *str1, tString *str2){
  return (strcmp(str1->data, str2->data));
}


int strCopyArr(tString *str, char array[]){
   strClear(str);
   int length = strlen(array);
   for(int i = 0; i < length; i++) {
      if(strAdd(str, array[i]) != SUCCESS)
         return INT_ERR;     
   }
   return SUCCESS;
}


char *getStr(tString *s){
  return s->data;
}

