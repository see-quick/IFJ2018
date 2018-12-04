/**
 * Predmet:  IFJ 
 * Projekt:  Implementace prekladace imperativniho jazyka IFJ18
 * Soubor:   parser.h
 *
 * Popis: Syntakticka a semanticka analyza
 *
 * Autori:   Maros Orsak            	xorsak02@stud.fit.vutbr.cz
 *           Polishchuk Kateryna     	xpolis03@stud.fit.vutbr.cz
 *           Igor Ignac                 xignac00@stud.fit.vutbr.cz
 *           Marek Rohel            	xrohel01@stud.fit.vutbr.cz
*/

#ifndef IFJ_SYNTACTICANALYZER_H
#define IFJ_SYNTACTICANALYZER_H

#include "symtable.h"
#include "list.h"
#include "prece.h"
#include <stdbool.h>

GlobalMap* gMap;		     // globalni tabulka symbolu


int error_lex(void);
int error_int(void);

/**
** Pomocna funkce na kotnrolu typu tokenu
** @ int type - typ tokenu
**/
int checkTokenType(int);

int parse(GlobalMap* , tList *);


int check_substr_ord_build_in(int param);
int check_chr_build_in();
int check_length_substr_ord_build_in();


/**
** Pocatecni stav pruchodu
** Dale nasleduje zacatek programu <prog> - <main_p> EOF
**/
int prog(void);
int main_p(void);
int st_list();
int stat();
int move_value(expr_return res);
int sth();
int pm_list();
int pm_list2();
int term_list(bool);
int term_list2(bool);
int func();
void insert_build_in_functions();
void instruction_exit(int);
char * generate_param(char *, unsigned short );

#endif


