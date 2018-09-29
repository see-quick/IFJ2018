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

int main(int argc, char** argv) {
   
  int result; 

  GlobalMap* globalMap = NULL;
  global_map_init(globalMap);
  globalMap = (GlobalMap*) malloc ( sizeof(GlobalMap) );

  for ( int i=0; i<MAX_SIZE_OF_HASH_TABLE; (*globalMap)[i++] = undefined_pointer_global );


  // printf ("---------------------------\n");
  // printf ("\nLet's set HTSIZE to 19 ( must be prvocislo )\n");
  // global_map_print(globalMap);

	//  testovani SA          
  result = parse(globalMap); 


  global_map_free(globalMap);

   
  return result;  
}