/**
 * Predmet:  IFJ
 * Projekt:  Implementace prekladace imperativniho jazyka IFJ18
 * Soubor:   stack.c
 *
 * Popis: zdrojove subory zasobnika
 *
 *
 * Datum: 27.9.2018 - 22:18
 *
 * Autori:   Maros Orsak       vedouci
 *           Polishchuk Kateryna     <xpolis03@fit.vutbr.cz>
 *           Igor Ignac
 *           Marek Rohel

*/
#include "stack.h"
#include <stdbool.h>
#include <stdlib.h>

/* Zakladne funckie so Stackom */

/**
 * Funkcia, ktora kontroluje pracu so stackom, ak bude nejaka chyba pri operaciach, automaticky nas nato upozorni
 * @return -1 -> like error
 */
int stack_error(){
    printf("Something went wrong with using stack\n");
    return -1;
}

/**
 * Inicializuje stack na velkost ktoru si zvolime
 * @param memory velkost zasobnika
 * @return vracia zasobnik
 */
tStack* stack_init(unsigned memory){
    tStack* stack = (tStack*) malloc(sizeof(tStack));
    if(stack == NULL){
        stack_error(stack);
    }
    stack->top = -1;
    stack->finderOfParenthesis = -1;
    stack->memory = memory;
    stack->array = (tToken*) malloc(stack->memory * sizeof(tToken));
    return stack;
}

/**
 * Funkcia kontroluje zda je zasobnik prazdny
 * @param stack konkretny zasobnik
 * @return -1 v pripade ze je prazdny
 */
int stack_empty(tStack *stack){
    return stack->top == -1;
}

/**
 * Funkcia, ktora bude kontrolovat ked bude zasobnik plny
 * @param stack
 * @return v pripade ze je plny tak vracia 1
 */
int stack_full(tStack *stack){
    return stack->top == stack->memory - 1;
}

/**
 * Funckia, ktora vracia Token, ktory je na vrchole zasobnika
 * @param stack konkretny zasobnik
 * @return token
 */
tToken stack_top(tStack *stack){
    if(stack_empty(stack)){
        stack_error(stack);
    } else{
        return stack->array[stack->top];
    }
}

/**
 * Funckia, ktora pushuje zvoleny token do zasobnika
 * @param stack konkretny token
 * @param token pushovany token
 */
void stack_push ( tStack *stack, tToken token){
    if(stack_full(stack)){
        if(stack_error(stack) == -1){
            return;
        }
    }
    stack->top++;
    stack->finderOfParenthesis++;
    stack->array[stack->top] =  token;
    printf("%c <-- Pushed to stack\n", *(token.data.data));
}

/**
 * Funckia, ktora bude popovat dany token von zo zasobniku
 * @param stack konkrenty token
 * @return vrati token na vrchole zasobnika
 */
tToken stack_pop(tStack *stack){
    // ak zasobnik nie je prazdny
    if(!(stack_empty(stack))){
        tToken item = stack->array[stack->top];
        stack->top--;
        stack->finderOfParenthesis--;
        return item;
    }
    printf("Popujes prazdny zasobnik\n");;
}

/**
 * Funkcia, ktora nam vrati hodnotu celeho stacku
 * @param stack konkretny stack
 * @return hodnota stacku
 */
int stack_get_size(tStack *stack){
    return stack->memory;
}

/**
 * Pomocna debugovacia funkcia
 * @param stack konkretny stack
 */
void stack_print(tStack *stack){
    printf("[------------> STACK BEGIN <-------------]\n");
    for(int i = 0; i < stack_get_size(stack); i++){
        printf("Current stack item is -> %c\n", *(stack->array[i].data.data));
    }
    printf("[-------------> STACK END <--------------]\n");
}

/**
 * Vzdy pouzi tuto funckiu pred funkciou stack_free(), sluzi iniciliazivanie vsetkych poli na NULL
 * @param stack konkretny stack
 */
void stack_refresh(tStack *stack){
    for(int i = 0; i < stack_get_size(stack); i++){
        stack->array[i].data.data = NULL;
    }
    stack->top = -1;
    stack->finderOfParenthesis = -1;
}

/**
 * Funkcia, ktora uvolni cely stack
 * @param stack konkretny stack
 */
void stack_free(tStack *stack)
{
    stack->memory = 0;
    free(stack->array);
    free(stack);
}

/**
 * Funckia, pomocou ktorej sa bude hladat znamienko <, na redukovanie pravidiel E -> E + E a podobne
 * @param stack konkretny stack
 */
void stack_search_for_theorem(tStack *stack) {
    /* this will be testing code */
    /*
     for(int i = 0; stack->array[stack->findOfParenthesis].typeOfToken != "<"; i++) { //solving rule
        stack->findOfParenthesis--;
     */
}
