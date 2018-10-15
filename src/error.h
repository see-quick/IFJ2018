/**
 * Predmet:  IFJ 
 * Projekt:  Implementace prekladace imperativniho jazyka IFJ18
 * Soubor:   error.h
 *
 * Popis: Chybove stavy programu
 *
 *
 * Datum:
 *
 * Autori:   Maros Orsak       vedouci 
 *           Polishchuk Kateryna     <xpolis03@fit.vutbr.cz>           
 *           Igor Ignac           
 *           Marek Rohel       
*/

#ifndef IFJ_ERR_H
#define IFJ_ERR_H


#define  SUCCESS                   0  
// chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexému)
#define  ERROR_LEX                 1
// chyba v programu v rámci syntaktické analýzy (chybná syntaxe programu)
#define  SYN_ERR                   2
#define  SEM_ERR                   3   // sémantická chyba v programu (...)

// sémantická chyba typové kompatibility v aritmetických, řetězcových a relačních výrazech
#define  ERR_INCOMPATIBLE_TYPE     4
// nesouvisi pocet formalnich paramentru a argumentu pri volani funkce
#define  ERR_PARAMS_COUNT          5
#define  ERR_SEMANTIC              6   // ostatní sémantické chyby 

#define  ERR_DIVISION              9

#define  INT_ERR                   99  // interní chyba překladače



#endif