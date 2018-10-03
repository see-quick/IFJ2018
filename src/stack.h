/**
 * Predmet:  IFJ
 * Projekt:  Implementace prekladace imperativniho jazyka IFJ18
 * Soubor:   stack.c
 *
 * Popis: hlavickovy subor zasobnika
 *
 *
 * Datum: 27.9.2018 - 22:18
 *
 * Autori:   Maros Orsak       vedouci
 *           Polishchuk Kateryna     <xpolis03@fit.vutbr.cz>
 *           Igor Ignac
 *           Marek Rohel

*/

#ifndef STACK_STACK_H
#define STACK_STACK_H

#define stack_pops(COUNT, STACK) for(int i=0; i < COUNT; i++){stack_pop(STACK);}

#include <stdio.h>
#include "string.h"

typedef struct{
    int top;
    int finderOfParenthesis; // TODO: same as top, but this variable is used for finding first occurence of '<' in Non-terminal
    unsigned memory;
    int *array;
}tStack;

tStack* stack_init(unsigned memory);
int stack_error();
int stack_empty(tStack *stack);
int stack_full(tStack *stack);
int stack_get_size(tStack *stack);
void stack_push ( tStack *stack, int token );
int stack_top(tStack *stack);
int stack_pop ( tStack *stack ); // sem bude vracat Token
void stack_free(tStack *stack);
void stack_print(tStack *stack);
void stack_refresh(tStack *stack);
void stack_search_for_theorem(tStack *stack);

#endif //STACK_STACK_H
