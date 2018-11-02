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
#include "instr_parse.h"
#include "list.h"

extern char * function_name;
extern char * variable_name;
extern tList * while_list;
extern tList * variables_list;


int main(int argc, char** argv) {
   
  int result = SUCCESS;

  tList *list = list_init();

  GlobalMap* globalMap;
  globalMap = global_map_init(MAX_SIZE_OF_HASH_TABLE);

  result = parse(globalMap, list);

  // if(result == SUCCESS) {
      reverse(&(list->first));
      set_active(list);
      parse_instructions(list);
  // }

  global_map_free(globalMap);

  dispose_list(list);
  dispose_list(while_list);
  dispose_list(variables_list);
  free(list);
  free(while_list);
  free(variables_list);


  free(function_name);
  free(variable_name);

  return result;  
}