/**
 * Predmet:  IFJ
 * Projekt:  Implementace prekladace imperativniho jazyka IFJ18
 * Soubor:   prece.h
 *
 * Popis: Hlavickovy subor pre precedencnu analyzu
 *
 *
 * Datum:
 *
 * Autori:   Maros Orsak       vedouci
 *           Polishchuk Kateryna     <xpolis03@fit.vutbr.cz>
 *           Igor Ignac
 *           Marek Rohel

*/

#ifndef IFJ_PRECE_H
#define IFJ_PRECE_H

#include "string.h"
#include "error.h"
#include <stdbool.h>
#include "scanner.h"
#include "hashtable.h"
#include "list.h"
#include "stack.h"

#define SIZEOFTABLE 14

/* VSTUPNY TOKEN (identifikator) */
/* e = expresion */
#define ePLUS   	0 // +
#define eMINUS      1 // -
#define eMUL        2 // *
#define eDIV        3 // /
#define eLESS		4  // <
#define eGREAT 		5  // >
#define eLEQUAL		6  // <=
#define	eGEQUAL 	7  // >=
#define	eEQUAL		8  // ==
#define	eNEQUAL 	9  // !=
#define eIDENT		10 // i
#define	eLBAR		11 // (
#define	eRBAR		12 // )
#define eDOLAR      13 // dolar $

#define eSOLVING_RULE   22 // preventujeme kvoli situacii kde ak najde symbol < a pride dalsi < tak nech sa z toho nezblazni
#define E               42  // E - non terminal

#define eSYNTERROR 800 // syntax error

// Enum ktorý, bude selectovať o akú operáciu pojde pri vykonavaní precedenčnej analýzy(využívanie stacku a getToken()
typedef enum{
    EQ = 800,   // = Znamienko, znamenajúce jednoduchý shift v precedenčnej tabuľke
    L = 801,    // < Znamienko, znamenajúce obohatený shifting
    G = 802,    // > Znamienko, znamenajúce obohatený redukcia
    Err = 803   // Error
}prece_states;

typedef struct{
    tString *string;    // chces aby som ti vracal aj ten NonTerminal? :) ci staci iba hodnota
    bool bool_result;;   // pri vyrazoch < > <= >= bude vzdy vysledok true alebo false;
    int result;         // pouzivaju sa navratove hodnoty z "error.h"
}expr_return;

// hlavni funkce precedencni analyzy
expr_return parse_expr(LocalMap*, tList*);
void setEmptyDataIDF(tDataIDF* dataIDF);

// pomocne debugovanie funckie
char* convert_to_char(int token);


#endif //IFJ_PRECE_H
