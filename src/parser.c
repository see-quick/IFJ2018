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


/*Funkce pro vypsani lexikalni chyby na standardni chybovy vystup*/
int error_lex(void){
	if(token == ERROR_LEX){
		fprintf(stderr, "Lexikalni chyba na radku %d\n", gToken.row);
		resetToken();
		return 0;
	}

	return 1;
}


/*Funkce pro vypsani interni chyby na standardni chybovy vystup*/
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

int term(void){
	int result = SUCCESS;

	switch(token){
		case LEX_ID:
		case LEX_NUMBER:
		case LEX_REAL_NUMBER:
		case LEX_STRING:
			//  semanticka kontrola dalsich parametru 
			// kontrola, zda je dana promenna deklarovana
			// jinak vytvor vnitrni promennou s hodnotou

			// semanticka chyba nesouhlas pocet parametru funkce
			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}
			return SUCCESS;
		break;

		default:
			//jiny token = chyba!
			fprintf(stderr, "Ocekavano 'identifikator' 'konstanta' na radku %d \n", gToken.row);
			resetToken();
			return SYN_ERR;
	}

	return result;
}

int term_list2(void){
	int result;

	switch(token){
		//<PM-LIST2> -> , id/lex_number/lex_real_number/lex_string <PM-LIST2>
		// UDELAT STAV PRO ID/LEX_NUMBER/LEX_REAL_NUMBER/LEX_STRING ????
		case LEX_COMMA:
			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}

			result = term();

			if(result != SUCCESS){
				resetToken();
				return result;
			}

			//nacteno z param()
			return term_list2();
		break;

		case LEX_R_BRACKET:
			// SEMANTICKA KONTROLA souhlas pocet parametru funkce
			return SUCCESS;
		break;

		default:
			//cokoliv jineho = chyba
			fprintf(stderr, "Ocekavano ',' 'identifikator' 'konstanta' ')' na radku %d \n", gToken.row);
			resetToken();
			return SYN_ERR;
	}
}

int term_list(void){
	int result = SUCCESS;

	switch(token){
		case LEX_ID:
		case LEX_NUMBER:
		case LEX_REAL_NUMBER:
		case LEX_STRING:
			// semanticka akce 
			// if LEX_ID -> zkontrolovat zda je promenna definovana 
			// pokud je to LEX_ID, LEX_NUMBER, LEX_REAL, LEX_STRING , podle typu vytvorit vnitrni promennou s hodnotou argumentu

			//nacteni a volani pm_list2()
			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}


			return term_list2();
		break;

		case LEX_R_BRACKET:
			// semanticka akce, nesouhlasi pocet paramtru funkci

			return SUCCESS;
		break;

		default:
			fprintf(stderr, "Ocekavano 'identifikator' 'konstanta' ')' na radku %d\n", gToken.row);
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

}


int sth(void){
	int result = SUCCESS;
	printf("Pravidlo pro <id = sth> \n");
	switch(token){
		// case LEX_ID_F:
		case LEX_ID:

		printf("function\n");
				// SEMANTICKA AKCE, KONTROLA DEFINICE FUNKCE

    //     	    token = getToken();
				// if(!isErrorLex()){
				// 	return ERROR_LEX;
				// } else if (!isErrorComp()){
				// 	return INT_ERR;
				// }

				// token = getToken();
				// if(!error_lex()){
				// 	return ERROR_LEX;
				// } else if (!error_int()){
				// 	return INT_ERR;
				// }

				// if(!checkTokenType(LEX_L_BRACKET)){
				// 	fprintf(stderr, "Ocekavano '(' na radku %d\n", gToken.row);
				// 	resetToken();
				// 	return SYN_ERR;
				// }


				// token = getToken();
				// if(!error_lex()){
				// 	return ERROR_LEX;
				// } else if (!error_int()){
				// 	return INT_ERR;
				// }


				// //volane term_list()
				// result = term_list();
				// if(result != SUCCESS){
				// 	resetToken();
				// 	return result;
				// }

				// //dalsi token je nacten, musi = ')'

				// if(!checkTokenType(LEX_R_BRACKET)){
				// 	fprintf(stderr, "CHYBA_P: Ocekavana ')' na radku %d \n", gToken.row);
				// 	resetToken();
				// 	return SYN_ERR;
				// }

		break;

		// semanticka chyba prirazeni
		case LEX_EOL:
			fprintf(stderr, "Ocekavan vyraz v prirazeni na radku %d \n", gToken.row);
			return SEM_ERR;
		break;

		//pokud neni token LEX_ID_F, prozenem to precedencni SA
		default:

			printf("prirazeni\n");
			//return parse_expr();
		
	}
	return result;
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


			// nacteni dalsiho tokenu , musi byt identifikator nebo vyraz
			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}

			// volani pravidla sth()
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


			return SUCCESS;
		break;


		//<STAT> -> while <EXPR> do eol <ST-LIST> end
		case KW_WHILE:
			printf("While pravidlo -> \n");

			
			//vyrazova SA, pro precedencni analyzu
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

			//dalsi token je zase nacten z fce, musi byt END
			if(!checkTokenType(KW_END)){
				fprintf(stderr, "Ocekavan 'end' na radku %d\n", gToken.row);
				resetToken();
				return SYN_ERR;
			}

			return result;
		break;

		// case KW_PRINT

		default:
			fprintf(stderr, "Ocekavano 'while' 'id' 'if' 'print' na radku %d \n", gToken.row); // dopsat
			resetToken();
			return SYN_ERR;
	}

	return result;

	
}


int st_list(void){
	int result = SUCCESS;

	// -- DEBUG -- 
	printf("----- In <st_list> ----\n");

	//pravidlo <ST-LIST> -> <STAT> eol <ST-LIST>


	switch(token){

		case KW_IF:
		case KW_WHILE:
		case LEX_ID:

			//podle pravidla zavolame stat()
			result = stat();
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

			if(!checkTokenType(LEX_EOL)){
				fprintf(stderr, "1 Ocekavan 'eol' na radku %d\n", gToken.row);
				resetToken();
				return SYN_ERR;
			}

			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}

			return st_list();

		// case KW_PRINT

		//<ST-LIST> -> nic
      case KW_ELSE:
      case KW_END:
      		return SUCCESS;
      		break;

      case KW_DEF:

			// volani dc_list()
			result = func();
			if(result != SUCCESS){
				resetToken();
				return result;
			}
			// dalsi token je nacteny z func()

			if(!checkTokenType(LEX_EOL)){
				fprintf(stderr, "1 Ocekavan 'eol' na radku %d\n", gToken.row);
				resetToken();
				return SYN_ERR;
			}

			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}

			return st_list();

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



int main_p(void){

	// -- DEBUG --
	printf("----- In <main_p> -----\n");

	int result = SUCCESS;


	//printf("Token : %d\n", token);

	switch(token){
		case KW_IF:
		case LEX_ID:
		case KW_WHILE:
		case KW_PRINT:
		case KW_DEF:
		//case KW_INPUTS:
		// case KW_INPUTI:
		//case KW_INPUTF:
		//case KW_LENGTH:
		//case KW_CHR:
		//case KW_ORD:
		//case KW_SUBSTR:

			//volani st_list()
			result = st_list();
			if(result != SUCCESS){
				resetToken();
				return result;
			}

			//dalsi musi byt EOL / EOF
			token = getToken();

			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}

			return SUCCESS;
		break;


		default:
			fprintf(stderr, "Ocekavano zacatek programu na radku %d\n", gToken.row);
			resetToken();
			return SYN_ERR;
	}

	return result;

}


int prog(){
	int result; //stav funkce

	switch(token){
		case KW_DEF:
		case KW_IF:
		case KW_WHILE:
		case LEX_ID:
		case KW_PRINT:
			result = main_p();
			if(result != SUCCESS){
				resetToken();
				return result;
			}

			//token nacten z main_p() = EOF
			if(!checkTokenType(LEX_EOF)) {
				fprintf(stderr, "Ocekavano 'eof' 'eol' na radku %d \n", gToken.row);
				resetToken();
				return SYN_ERR;
			}

			printf("Konec prog, navratova hodnota je %d\n", result);
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