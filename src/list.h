/**
 * Predmet:  IFJ
 * Projekt:  Implementace prekladace imperativniho jazyka IFJ18
 * Soubor:   list.h
 *
 * Popis:  hlavickovy subor
 *
 * Datum: 22.9.2018 23:25
 *
 * Autori:   Maros Orsak       vedouci
 *           Polishchuk Kateryna     <xpolis03@fit.vutbr.cz>
 *           Igor Ignac
 *           Marek Rohel
*/

#include <stdio.h>
#include <stdlib.h>


#ifndef _LIST_H
#define _LIST_H

//INSTRUCTRUCNA SADA
typedef enum
{
    //praca s ramcami, volanie funkcií
            INSTRUCT_MOVE = 600,
            INSTRUCT_CREATEFREAME = 601,
            INSTRUCT_PUSHFRAME = 602,
            INSTRUCT_DEFVAR = 603,
            INSTRUCT_CALL = 604,
            INSTRUCT_RETURN = 605,
    //praca so zasobnikom
            INSTRUCT_PUSHS = 606,
            INSTRUCT_POPS = 607,
            INSTRUCT_CLEARS = 608,
    //aritmeticke, relacne, boolovske a konverzni INSTRUCT.
            INSTRUCT_ADD = 609,
            INSTRUCT_SUB = 610,
            INSTRUCT_MULL = 611,
            INSTRUCT_DIV = 612,
            INSTRUCT_ADDS = 613,
            INSTRUCT_SUBS = 614,
            INSTRUCT_MULS = 615,
            INSTRUCT_DIVS = 616,
            INSTRUCT_LT = 617,
            INSTRUCT_GT = 618,
            INSTRUCT_EQ = 619,
            INSTRUCT_LTS = 620,
            INSTRUCT_GTS = 621,
            INSTRUCT_EQS = 622,
            INSTRUCT_AND = 623,
            INSTRUCT_OR = 624,
            INSTRUCT_NOT = 625,
            INSTRUCT_ANDS = 626,
            INSTRUCT_ORS = 627,
            INSTRUCT_NOTS = 628,
            INSTRUCT_INT2FLOAT = 629,
            INSTRUCT_FLOAT2IN = 630,
            INSTRUCT_FLOAT2R2EINT = 631,
            INSTRUCT_FLOAT2R2OINT = 632,
            INSTRUCT_INT2CHAR = 633,
            INSTRUCT_STRI2INT = 634,
            INSTRUCT_INT2FLOATS = 635,
            INSTRUCT_FLOAT2INTS = 636,
            INSTRUCT_FLOAT2R2EINTS = 637,
            INSTRUCT_FLOAT2R2OINTS = 638,
            INSTRUCT_INT2CHARS = 639,
            INSTRUCT_STRI2INTS = 640,
    //vstupno-vstupne INSTRUCT.
            INSTRUCT_READ = 641,
            INSTRUCT_WRITE = 642,
    //praca s retezcami
            INSTRUCT_CONCAT = 643,
            INSTRUCT_STRLEN = 644,
            INSTRUCT_GETCHAR = 645,
            INSTRUCT_SETCHAR = 646,
    //praca s typy
            INSTRUCT_TYPE = 647,
    //INSTRUCT. pre riadenie toku programu  toku programu
            INSTRUCT_LABEL = 648,
            INSTRUCT_JUMP = 649,
            INSTRUCT_JUMPIFEQ = 650,
            INSTRUCT_JUMPIFNEQ = 651,
            INSTRUCT_JUMPIFEQS = 652,
            INSTRUCT_JUMPIFNEQS = 653,
            INSTRUCT_EXIT = 654,
    //ladiacie INSTRUCT.
            INSTRUCT_BREAK = 655,
            INSTRUCT_DPRINT  = 656,

    //hlavicka .IFJcode18 + instr popframe
            INSTRUCT_HEAD = 657,
            INSTRUCT_POPFRAME = 658
} tInstructionTypes;

/** **/ //pridat nil
typedef enum
{
    GF = 700,    //pre ID
    LF = 701,    //pre ID
    TF = 702,    //pre ID
    I = 703,     // napr:int@10
    F = 704,     // napr:float@2.33
    S = 705,      // string@ahoj
    FCE = 706,     //pre funkcie
    EMPTY = 707 //prazdna adresa
} tDatType;

// zmenil som union na struct
typedef struct value
{
    int i;
    double d;
    char *s;
} tValue;

typedef struct InstD
{
    tDatType type;
    tValue value;
} tInstructionData;

// Struktura pre instrukcie
typedef struct Data
{
    tInstructionTypes type;
    tInstructionData address1;
    tInstructionData address2;
    tInstructionData address3;
} tData;

// Obsahuje instrukcie a ukazatel na dalsiu node v zozname
typedef struct Node
{
    tData data;
    struct Node *next;
} tNode;

// Ukazatele na prvy, aktivny a posledny node v zozname
typedef struct List
{
    tNode *first;
    tNode *last;
    tNode *act;
    //FILE *stream;
} tList;

// Prototypy funkcii
int list_error();
tList* list_init();
void insert_item (tList *instr_list, tInstructionTypes *instr_name , tInstructionData *addr1, tInstructionData *addr2, tInstructionData *addr3);
void dispose_list(tList *instr_list);
tNode* return_instruct(tList *instr_list);
void move_activity(tList *instr_list);
void set_active(tList *instr_list);
void operand_type(char* order, tValue instr_operand, tDatType instr_type);
void print_list_elements(tList *instr_list);

#endif //_LIST_H
