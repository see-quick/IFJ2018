/**
 * Predmet:  IFJ 
 * Projekt:  Implementace prekladace imperativniho jazyka IFJ18
 * Soubor:   scanner.h
 *
 * Popis: hlavickovy soubor scanner.h
 *
 *
 * Datum:
 *
 * Autori:   Maros Orsak       vedouci
 *           Polishchuk Kateryna     <xpolis03@fit.vutbr.cz>           
 *           Igor Ignac           
 *           Marek Rohel       

*/


#ifndef IFJ_LEX_H
#define IFJ_LEX_H


/************/
/**Knihovny**/
/************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "string.h"

/* Koncove stavy konecneho automatu Lexilakniho analyzatoru */

/*Klicova slova */
#define    KW_DEF                10
#define    KW_DO                 11
#define    KW_ELSE               12                                          
#define    KW_END                13
#define    KW_IF                 14
#define    KW_NOT                15
#define    KW_NIL                16                                             
#define    KW_THEN               17
#define    KW_WHILE              18   

/*Vestavene funkce */
#define    KW_INPUT_S            19
#define    KW_INPUT_I            20
#define    KW_INPUT_F            21
#define    KW_PRINT              22
#define    KW_LENGTH             23
#define    KW_SUBSTR             24
#define    KW_ORD                25
#define    KW_CHR                26


/* Zakladni tokeny */    
#define    LEX_L_SBRACKET        27
#define    LEX_R_SBRACKET        28
#define    LEX_L_BRACKET         29
#define    LEX_R_BRACKET         30
#define    LEX_ADDITION          31
#define    LEX_SUBSTRACTION      32
#define    LEX_MULTIPLICATION    33
#define    LEX_DIVISION          34
#define    LEX_DOT               35
#define    LEX_EQUAL             36
#define    LEX_UNEQUAL           37
#define    LEX_LESSER            38
#define    LEX_LESSER_EQUAL      39
#define    LEX_GREATER           40
#define    LEX_GREATER_EQUAL     41
#define    LEX_EOL               42
#define    LEX_COMMA             43
#define    LEX_EXCM              44
#define    LEX_COLON             45
#define    LEX_SEMICOLON         46
#define    LEX_EOF               47
#define    LEX_STRING            48
#define    LEX_ID                49
#define    LEX_NUMBER            50
#define    LEX_REAL_NUMBER       51
#define    LEX_DOUBLE_EQUAL      52
/////////////////////////////// 



/* Chybove tokeny */
#define    LEX_ERROR             53




/*Stavy*/
#define    S_START              54
#define    S_LESSER             55
#define    S_GREATER            56
#define    S_AS_EXCM            57
#define    S_COMMENT_ROW        58
#define    S_STRING             59
#define    S_STRING_ESCAPED     60
#define    S_STRING_ASCII       61
#define    S_ID                 62
#define    S_NUMBER             63
#define    S_NUMBER_POINT       64
#define    S_NUMER_DECIMAL      65
#define    S_NUMBER_EXPONENT    66
#define    S_REAL               67
#define    S_EOF                68
#define    S_EQUAL              69
#define    S_EOL                70 

 




tToken gToken;
int token;

#define SIZE            10
#define KEYWORD_COUNT   16
#define SHIFT           10




/*************************************************************/
/**************Popis vsech funkci*****************************/
/*************************************************************/


// Vytvoreni globalniho tokenu
int initToken();
// Pushnuti symbolu do data tokenu, hodnota int/char
void pushToken(int);
// Funkce vypise data tokenu
void returnToken();
// Navrat tokenu do pocatecniho stavu
void resetToken();


int isKeyword(tString *);

// Funkce vraci vyp lexemu, hlavni nacitaci funkce
int getToken();


#endif