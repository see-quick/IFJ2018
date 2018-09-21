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

int main(int argc, char** argv) {
   
   int result;             
   //testovani LA

   if(initToken() == INT_ERR){
		fprintf(stderr, "Nepodařilo se inicializovat strukturu pro token \n");
		result = INT_ERR; 
	}
	
	do {
		if((token = getToken()) == ERROR_LEX) {
			fprintf(stderr, "Lexikalni chyba \n");
			result = ERROR_LEX;
		} else if (token == INT_ERR) {
			fprintf(stderr, "Interni chyba\n");
		}
		else{
			printf("Token c: %d %s\n", token, gToken.data.data);
			//printf("Radek c: %d\n", gToken.row);
		}
	} while(token != LEX_EOF);
   
  return result;  
}