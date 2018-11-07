/**
 * Predmet:  IFJ
 * Projekt:  Implementace prekladace imperativniho jazyka IFJ18
 * Soubor:   hashtabletest.c
 *
 * Popis: Testovanie hashovacej tabulky
 *
 * Datum: 22.9.2018 23:56
 *
 * Autori:   Maros Orsak       vedouci
 *           Polishchuk Kateryna     <xpolis03@fit.vutbr.cz>
 *           Igor Ignac
 *           Marek Rohel

*/

#include "../src/stack.h"
#include <stdio.h>

void stacktest(){
    tStack *stack = stack_init(6);
    tToken tokenValue;
    tokenValue.data.data = "A\0";

    printf("This is current size of stack -> %d\n",stack_get_size(stack));

    stack_push(stack, tokenValue);
    tokenValue.data.data = "B\0";
    stack_push(stack, tokenValue);
    tokenValue.data.data = "C\0";
    stack_push(stack, tokenValue);
    tokenValue.data.data = "D\0";
    stack_push(stack, tokenValue);
    tokenValue.data.data = "E\0";
    stack_push(stack, tokenValue);
    tokenValue.data.data = "F\0";
    stack_push(stack, tokenValue);

    // printf("This is on the top -> %c", *(stack_top(stack)));
    printf("This is on the top -> %c\n", *(stack_top(stack).data.data));
    stack_print(stack);

    stack_pop(stack);
    stack_pop(stack);
    stack_pop(stack);
    stack_pop(stack);
    stack_pop(stack);

    printf("This is on the top -> %c\n", (stack_top(stack)));
    stack_print(stack);

    stack_refresh(stack);
    stack_print(stack);

    stack_free(stack);
    stack_print(stack);


//    stack_top(stack, charValue);
}