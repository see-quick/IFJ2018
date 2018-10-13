/**
 * Predmet:  IFJ 
 * Projekt:  Implementace prekladace imperativniho jazyka IFJ18
 * Soubor:   parser.h
 *
 * Popis: Syntakticka a semanticka analyza
 *
 *
 * Datum:
 *
 * Autori:   Maros Orsak       vedouci
 *           Polishchuk Kateryna     <xpolis03@fit.vutbr.cz>           
 *           Igor Ignac           
 *           Marek Rohel       

*/

#ifndef IFJ_SYNTACTICANALYZER_H
#define IFJ_SYNTACTICANALYZER_H

#include "symtable.h"
#include "list.h"

GlobalMap* gMap;		     // globalni tabulka symbolu


int error_lex(void);
int error_int(void);

/**
** Pomocna funkce na kotnrolu typu tokenu
** @ int type - typ tokenu
**/
int checkTokenType(int);

int parse(GlobalMap* , tList *);

/**
** Pocatecni stav pruchodu
** Dale nasleduje zacatek programu <prog> - <main_p> EOF
**/
int prog(void);
int main_p(void);
int st_list();
int stat();
int sth(LocalMap*);
int pm_list();
int pm_list2();
int term_list(void);
int term_list2(void);
int func();
int parse_expression(void);
void insert_build_in_functions();

#endif


