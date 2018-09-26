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
#include "hashtable.h"

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


// hlavni funkce precedencni analyzy
int parse_expr(void){
	printf("Precedencni analyza...\n");
	return SUCCESS;
}


int sth(void){
	printf("Pravidlo pro <id = sth> \n");

	return SUCCESS;
}

int stat(void){
	int result = SUCCESS;

	switch(token){
		//<STAT> -> id = <STH> 
		case LEX_ID:
			printf("Identifikator pravidlo -> \n");

			//dalsi musi byt '='
			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}

			if(!checkTokenType(LEX_EQUAL)){
				fprintf(stderr, "Ocekavano '=' na radku %d\n", gToken.row);
				resetToken();
				return SYN_ERR;
			}


			//volani sth()
			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}


			result = sth();
			if(result != SUCCESS){
				return result;
			}

			return result;
		break;

		//<STAT> -> if <EXPR> then eol <ST-LIST> else eol <ST-LIST> end if 
		case KW_IF:
			printf("Pravidlo pro if:\n");

			//nacteni a predani do vyrazove SA
			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}

			result = parse_expr();
			if(result != SUCCESS){
				resetToken();
				return result;
			}

			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}


			//token je jiz nacteny, musi = KW_THEN
			if(!checkTokenType(KW_THEN)){
				fprintf(stderr, "Ocekavano 'then' na radku %d\n", gToken.row);
				resetToken();
				return SYN_ERR;
			}

			//dalsi = LEX_EOL
			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}

			if(!checkTokenType(LEX_EOL)){
				fprintf(stderr, "2 Ocekavano 'eol' na radku %d \n", gToken.row);
				resetToken();
				return SYN_ERR;
			}

			//dalsi token -> volani st_list()
			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}

			result = st_list();
			if(result != SUCCESS){
				resetToken();
				return result;
			}


			//token nacteny z st_list() = KW_ELSE
			if(!checkTokenType(KW_ELSE)){
				fprintf(stderr, "Ocekavano 'else' na radku %d \n", gToken.row);
				resetToken();
				return SYN_ERR;
			}

			//dalsi = LEX_EOL
			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}

			if(!checkTokenType(LEX_EOL)){
				fprintf(stderr, "3 Ocekavano 'eol' na radku %d\n", gToken.row);
				resetToken();
				return SYN_ERR;
			}

			//nacteni tokenu a volani st_list()
			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}

			result = st_list();
			if(result != SUCCESS){
				resetToken();
				return result;
			}


			//token nacten z fce st_list() = KW_END
			if(!checkTokenType(KW_END)){
				fprintf(stderr, "Ocekano 'end' na radku %d \n", gToken.row);
				resetToken();
				return SYN_ERR;
			}

			//dalsi = IF
			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}


			if(!checkTokenType(KW_IF)){
				fprintf(stderr, "Ocekavano 'if' na radku %d \n", gToken.row);
				resetToken();
				return SYN_ERR;
			}

			return SUCCESS;
		break;


		//<STAT> -> while <EXPR> do eol <ST-LIST> end
		case KW_WHILE:
			printf("While pravidlo -> \n");

			
			//vyrazova SA
			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}

			result = parse_expr();
			if(result != SUCCESS){
				resetToken();
				return result;
			}

			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}

			if(!checkTokenType(KW_DO)){
				fprintf(stderr, "Ocekavano 'do' na radku %d\n", gToken.row);
				resetToken();
				return SYN_ERR;
			}

			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}


			if(!checkTokenType(LEX_EOL)){
				fprintf(stderr, "4 Ocekavano 'eol' na radku %d\n", gToken.row);
				resetToken();
				return SYN_ERR;
			}

			//volani st_list
			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}

			result = st_list();


			if(result != SUCCESS){
				resetToken();
				return result;
			}

			//dalsi token je zase nacten z fce, musi byt end
			if(!checkTokenType(KW_END)){
				fprintf(stderr, "Ocekavan 'end' na radku %d\n", gToken.row);
				resetToken();
				return SYN_ERR;
			}


			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}

			return result;
		break;
	}

	//cokoliv jineho = chyba!
	fprintf(stderr, "Ocekavano 'while' 'id' 'if' 'print' na radku %d \n", gToken.row); // dopsat
	resetToken();
	return SYN_ERR;
}


int st_list(void){
	int result = SUCCESS;

	// -- DEBUG -- 
	printf("----- In <st_list> ----\n");

	//pravidlo <ST-LIST> -> <STAT> eol <ST-LIST>


	switch(token){

		case KW_IF:
		case LEX_ID:
		case KW_WHILE:

			//podle pravidla zavolame stat()
			result = stat();
			if(result != SUCCESS){
				resetToken();
				return result;
			}

			token = getToken();


			if(!checkTokenType(LEX_EOL)){
				fprintf(stderr, "1 Ocekavan 'eol' na radku %d\n", gToken.row);
				resetToken();
				return SYN_ERR;
			}

			//dalsi token a znova volame st_list()
			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}

			return st_list();
		break;

		//<ST-LIST> -> nic
      case KW_ELSE:
      case KW_END:
      	return SUCCESS;
      break;
	}

	return result;
}

int pm_list2(void){
	printf("----- In <pm-list2> --- \n");

	switch(token){
		case LEX_COMMA:
			token = getToken();

			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}

			// SEMANTICKE AKCE pridat
			if(!checkTokenType(LEX_ID)){
				fprintf(stderr, "Ocekavno 'id' na radku %d\n", gToken.row);
				resetToken();
				return ERROR_LEX;
			}

			token = getToken();

			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}

			return pm_list2();
		break;

		case LEX_R_BRACKET:
			return SUCCESS;
		break;
	}

	token = getToken();
	if(!error_lex()){
		return ERROR_LEX;
	} else if(!error_int()){
		return INT_ERR;
	}

	return SUCCESS;
}


int pm_list(void){
	int result = SUCCESS;


	if (token == LEX_ID){
		token = getToken();

		// SEMANTICKE AKCE

		if(!error_lex()){
			return ERROR_LEX;
		} else if (!error_int()){
			return INT_ERR;
		}

		return pm_list2();
	}
	else{
		return SUCCESS;
	}

	return result;
}

int func(void){
	int result;
	//pravidlo <func> -> def ID ( <pm_list> ) eol <ST-LIST> end

	//dalsi token musi byt typu LEX_ID
	token = getToken();
	if(!error_lex()){
		return ERROR_LEX;
	} else if(!error_int()){
		return INT_ERR;
	}

	if(!checkTokenType(LEX_ID)){
		fprintf(stderr, "Ocekavan identifikator na radku %d \n", gToken.row);
		resetToken();
		return SYN_ERR;
	}

	// SEMANTICKA AKCE
	// tmp = global_map_get_item(*globalTS, gToken.data);

 //    if (tmp == NULL) {
 //    	// neni v tabulce -> vlozeni deklarace funkce do globalni TS
 //    	result = global_map_put(globalTS, gToken.data, tmp.globalData);
 //    }


    // else {} Semanticka kontrola - funkce jiz byla deklarovana. 


   	//dalsi token musi byt '('
	token = getToken();
	if(!error_lex()){
		return ERROR_LEX;
	} else if (!error_int()){
		return INT_ERR;
	}

	if(!checkTokenType(LEX_L_BRACKET)){
		fprintf(stderr, "Ocekavana '(' na radku %d\n", gToken.row);
		resetToken();
		return SYN_ERR;
	}

	//po kontrole dalsiho tokenu volane pm_list()

	token = getToken();
	if(!error_lex()){
		return ERROR_LEX;
	} else if (!error_int()){
		return INT_ERR;
	}

	result = pm_list();
	if(result != SUCCESS){
		resetToken();
		return result;
	}

	//dalsi token mame z fce pm_list(), musi byt ')'

	if(!error_lex()){
		return ERROR_LEX;
	} else if (!error_int()){
		return INT_ERR;
	}

	if(!checkTokenType(LEX_R_BRACKET)){
		fprintf(stderr, "Ocekavana ')' na radku %d\n", gToken.row);
		resetToken();
		return SYN_ERR;
	}

	token = getToken();
	if(!error_lex()){
		return ERROR_LEX;
	} else if (!error_int()){
		return INT_ERR;
	}

	if(!checkTokenType(LEX_EOL)){
		fprintf(stderr, "Ocekavan 'eol' na radku %d \n", gToken.row);
		resetToken();
		return SYN_ERR;
	}

	//dalsi token pude do funce ST-LIST
	token = getToken();
	if(!error_lex()){
		return ERROR_LEX;
	} else if (!error_int()){
		return INT_ERR;
	}

	result = st_list();
	if(result != SUCCESS){
		resetToken();
		return result;
	}

	//dalsi token je nacten, musi = KW_END

	if(!error_lex()){
		return ERROR_LEX;
	} else if (!error_int()){
		return INT_ERR;
	}

	if(!(checkTokenType(KW_END))){
		fprintf(stderr,"Ocekavan 'end' na radku %d", gToken.row);
		resetToken();
		return SYN_ERR;
	}

	//nacteni a kontrola dalsiho tokenu
	token = getToken();
	if(!error_lex()){
		return ERROR_LEX;
	} else if (!error_int()){
		return INT_ERR;
	}

	return SUCCESS;

}


int fn_list(void){
	// pravidlo <fn-list> -> <func> eol <fn-list>

	int result;

	if (token == KW_DEF){

		result = func();
			if(result != SUCCESS){
				resetToken();
				return result;
			}

			//dalsi token je nacteny z function()
			if(!checkTokenType(LEX_EOL)){
				fprintf(stderr, "Ocekavan 'eol' na radku %d\n", gToken.row);
				resetToken();
				return SYN_ERR;
			}

			//rekurzivni volani teto fce, podle pravidla
			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}
			return fn_list();
	}

	return SUCCESS;
}


int main_p(void){

	// -- DEBUG --
	printf("----- In <main_p> -----\n");

	int result = SUCCESS;


	//printf("Token : %d\n", token);

	switch(token){
		case KW_IF:
		case LEX_ID:
		case KW_WHILE:
		//case KW_INPUTS:
		// case KW_INPUTI:
		//case KW_INPUTF:
		//case KW_LENGTH:
		// case KW_PRINT:
		//case KW_CHR:
		//case KW_ORD:
		//case KW_SUBSTR:

			//volani st_list()
			result = st_list();
			if(result != SUCCESS){
				resetToken();
				return result;
			}

			//dalsi musi byt EOL
			token = getToken();

			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}
			if((!checkTokenType(LEX_EOL)) && (!checkTokenType(LEX_EOF))) {
				fprintf(stderr, "Ocekavano 'eol' 'eof' na radku %d \n", gToken.row);
				resetToken();
				return SYN_ERR;
			} 

			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}

			printf("Konec main_p, navratova hodnota je %d\n", result);

			return SUCCESS;
		break;

		case KW_DEF:

			// volani dc_list()
			result = fn_list();
			if(result != SUCCESS){
				resetToken();
				return result;
			}

			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}

			printf("def: Konec main_p, navratova hodnota je %d\n", result);

			return SUCCESS;

		break;
	}

	//neco jineho = chyba!
	fprintf(stderr, "Ocekavano zacatek programu na radku %d\n", gToken.row);
	resetToken();
	return SYN_ERR;

}


int prog(){
	int result; //stav funkce

	switch(token){
		case KW_DEF:
		case KW_IF:
		case KW_WHILE:
		case LEX_ID:
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
    	break;
	}

	//neco jineho = chyba!
	fprintf(stderr, "Ocekavano 'zacatek programu' na radku %d\n", gToken.row);
	resetToken();
	return SYN_ERR;
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