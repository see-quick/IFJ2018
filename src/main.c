/**
 * Predmet:  IFJ 
 * Projekt:  Implementace prekladace imperativniho jazyka IFJ18
 * Soubor:   main.c
 *
 * Popis: Hlavni vykonavaci funkce programu
 *
 *
 * Datum:
 *
 * Autori:   Maros Orsak       vedouci 
 *           Polishchuk Kateryna     <xpolis03@fit.vutbr.cz>           
 *           Igor Ignac           
 *           Marek Rohel       

*/

/*Pouzite knihovny */
#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "error.h"
#include "parser.h"
#include "list.h"

int main(int argc, char** argv) {
   
  int result; 

  tList *list = list_init(instr_file);

  GlobalMap* globalMap;
  globalMap = global_map_init(MAX_SIZE_OF_HASH_TABLE);

  result = parse(globalMap, list); 

  global_map_free(globalMap);

  interpret(list);

  dispose_list(list);
  free(list);

   
  return result;  
}