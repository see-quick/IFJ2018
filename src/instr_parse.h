/**
 * Predmet:  IFJ
 * Projekt:  Implementace prekladace imperativniho jazyka IFJ18
 * Soubor:   instr_parse.h
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
#include "list.h"

#ifndef _INSTR_PARSE_H
#define _INSTR_PARSE_H

//  Prototypy funkcii
char* instruct_type(tDatType instruction);
void print_symb(tInstructionData instr_operand);
void parse_instructions(tList *instr_list);


#endif //_INSTR_PARSE_H
