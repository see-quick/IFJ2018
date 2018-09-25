/**
 * Predmet:  IFJ 
 * Projekt:  Implementace prekladace imperativniho jazyka IFJ18
 * Soubor:   parser.c
 *
 * Popis: Syntakticka a semanticka analyza
 *
 *
 * Datum:
 *
 * Autori:   Maros Orsak       vedouci
 *           Polishchuk Kateryna     <xpolis03@fit.vutbr.cz>           
 *           Igor Ignac           
 *           Marek Rohel       

*/

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "parser.h"
#include "scanner.h"
#include "error.h"

int token;        // aktualni token



int error_lex(void){
	if(token == ERROR_LEX){
		fprintf(stderr, "Lexikalni chyba na radku %d\n", gToken.row);
		resetToken();
		return 0;
	}

	return 1;
}

int error_int(void){
	if(token == INT_ERR){
		fprintf(stderr, "Interni chyba\n");
		resetToken();
		return 0;
	}
	return 1;
}

int checkTokenType(int tokenType){ //funkce na kotnrolu typu tokenu
	return (token == tokenType) ? 1 : 0;
}



int main_p(){
	printf("Zacatek programu...\n");

	int result;

	printf("Token : %d\n", token);
}


int prog(){
	int result; //stav funkce

	result = main_p();
	if(result != SUCCESS){
		resetToken();
		return result;
	}

	//token nacten z main_p() = EOL
	if((!checkTokenType(LEX_EOL)) && (!checkTokenType(LEX_EOF))) {
		fprintf(stderr, "Ocekavano 'eof' 'eol' na radku %d \n", gToken.row);
		resetToken();
		return SYN_ERR;
	}
    return SUCCESS;
}




int parse() {

	int result = SUCCESS;


	if(initToken() == INT_ERR){
		fprintf(stderr, "Nepodařilo se inicializovat strukturu pro token \n");
		result = INT_ERR; 
	}

	do {
		if((token = getToken()) == ERROR_LEX) {
			fprintf(stderr, "Nepodařilo se načíst token\n");
			result = INT_ERR;
		} else if (token == INT_ERR) {
			fprintf(stderr, "Interni chyba\n");
		}
	} while(token == LEX_EOL);

	if(result == SUCCESS){
		result = prog();
	}

	resetToken();

	return result;

}