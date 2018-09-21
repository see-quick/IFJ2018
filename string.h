/**
 * Predmet:  IFJ 
 * Projekt:  Implementace prekladace imperativniho jazyka IFJ18
 * Soubor:   string.h
 *
 * Popis: hlavickovy soubor
 *
 *
 * Datum:
 *
 * Autori:   Maros Orsak       vedouci 
 *           Polishchuk Kateryna     <xpolis03@fit.vutbr.cz>           
 *           Igor Ignac           
 *           Marek Rohel       

*/


#ifndef IFJ_STR_H
#define IFJ_STR_H

/*************************/
/*******Struktury*********/
/*************************/
typedef struct{
  char *data;
  unsigned int length;
  unsigned int memory;
} tString;

typedef struct{
  unsigned int row;
  tString data;
} tToken;


/* Implementace struktury tString pro data(atribut) tokenu*/
int strInit(tString *);
/*Pridavani symbolu do data tokenu*/
int strAdd(tString *, char);
/*Uvolneni pameti pro data tokenu*/
int strFree(tString *);
// Vytvoreni pole pro data tokenu
tString strCreate(char *);
// Nastaveni nulove delky pole pro data, a zaplneni tohoto pole nulami
int strClear(tString *);
// Nakopirovani data tokenu do pole char *
int strCopy(tString *, char *);
// Nakopirovani pole do pole
int strCopyStr(tString *, tString *);
int strCopyArr(tString *, char[]);
int strCompare (tString *, tString *);
char *getStr(tString *);


#endif