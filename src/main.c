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
extern char * call_name;
extern tList * while_list;
extern tList * variables_list;
extern tList * function_statements_list;
tList * pom_list;

extern tInstructionTypes instr_type;
extern tInstructionData instr1;
extern tInstructionData instr2;
extern tInstructionData instr3;


int main(int argc, char** argv) {
   
  int result = SUCCESS;

  tList *list = list_init();
  pom_list = list_init();

  GlobalMap* globalMap;
  globalMap = global_map_init(MAX_SIZE_OF_HASH_TABLE);

  instr_type = INSTRUCT_HEAD;
  insert_item(pom_list, &instr_type, &instr1, &instr2, &instr3);

  reverse(&(pom_list->first));
  set_active(pom_list);
  parse_instructions(pom_list);      


  result = parse(globalMap, list);

  // if(result == SUCCESS) {

    reverse(&(function_statements_list->first));
      set_active(function_statements_list);
      parse_instructions(function_statements_list);      

      reverse(&(list->first));
      set_active(list);
      parse_instructions(list);
  // }

  global_map_free(globalMap);

  dispose_list(list);
  dispose_list(while_list);
  dispose_list(variables_list);
  dispose_list(function_statements_list);

  free(list);
  free(while_list);
  free(variables_list);
  free(function_statements_list);


  free(function_name);
  free(call_name);
  free(variable_name);

  return result;  
}