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
#include "prece.h"

int token;        	         // aktualni token
GlobalMap* gMap;		     // globalni tabulka symbolu
tDataFunction *gDataptr;	 // ukazatel na uzel globalni tabulky symbolu
tDataFunction gData;
tInstructionTypes instr_type;
LocalMap* localMap;
tDataIDF lData;

char * str;
char * tmp_str;
char * tmp_str2;
char * tmp_str3;


tList * ilist;

int paramCount = 0;          // pocet parametru funkce
int argCount = 0;            // pocet argumentu pri volani funkce

tInstructionData instr1;
tInstructionData instr2;
tInstructionData instr3;


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


			// +1 argument
			argCount++;

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

	char s;

	switch(token){
		case LEX_ID:
		case LEX_NUMBER:
		case LEX_REAL_NUMBER:
		case LEX_STRING:

			// semanticka akce
			// if LEX_ID -> zkontrolovat zda je promenna definovana
			// pokud je to LEX_ID, LEX_NUMBER, LEX_REAL, LEX_STRING , podle typu vytvorit vnitrni promennou s hodnotou argumentu


			instr_type = INSTRUCT_DEFVAR;
			instr1.type = 702;

			//printf("Paramcount %d\n", paramCount);

			sprintf(&s, "%d", paramCount);

			//printf("String paramCount %c\n", s);


			// pridat counter pro params!!!
			instr1.value.s = "$_param1";


			if (token == LEX_NUMBER){
				insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);
				instr_type = INSTRUCT_MOVE;
				instr1.type = 702;
				instr1.value.s = "$_param1";

				instr2.type = 703; //integer
				instr2.value.i = atoi(gToken.data.str);

				insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

			}

			//nacteni a volani term_list2()
			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}

			// zvetsit pocet argumentu
			argCount++;


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


int sth(LocalMap* localMap, char * nazev_promenne){
	int result = SUCCESS;
	expr_return res;
	tDataFunction *tmp;



	switch(token){
		// case LEX_ID_F:
		case LEX_ID_F:
		case LEX_ID:

				//SEMANTICKA AKCE, KONTROLA DEFINICE FUNKCE
				tmp = global_map_get_pointer_to_value(gMap, gToken.data.str);
				if (tmp == NULL){
					fprintf(stderr, "Semanticka chyba, funkce %s neni definovana, radek %d\n", gToken.data.str, gToken.row);
					return SEM_ERR;
				}

				strcpy(tmp_str, gToken.data.str);

				// vytvoreni docasneho ramce pro funkce
				instr_type =  INSTRUCT_CREATEFREAME;
				insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

        	    token = getToken();
				if(!error_lex()){
					return ERROR_LEX;
				} else if (!error_int()){
					return INT_ERR;
				}


				if(!checkTokenType(LEX_L_BRACKET)){
					fprintf(stderr, "Ocekavano '(' na radku %d\n", gToken.row);
					resetToken();
					return SYN_ERR;
				}


				token = getToken();
				if(!error_lex()){
					return ERROR_LEX;
				} else if (!error_int()){
					return INT_ERR;
				}


				//volane term_list()
				result = term_list();
				if(result != SUCCESS){
					resetToken();
					return result;
				}

				//dalsi token je nacten, musi = ')'

				if(!checkTokenType(LEX_R_BRACKET)){
					fprintf(stderr, "Ocekavana ')' na radku %d \n", gToken.row);
					resetToken();
					return SYN_ERR;
				}


				if ( tmp->paramCount != argCount ){
					fprintf(stderr, "Semanticka chyba, pocet parametru funkce %s nesouvisi s poctem argumentu na radku %d\n", tmp_str , gToken.row);
				}

				// pro dalsi volani funkce
				argCount = 0;


				// instrukce pro volani funkce

				instr_type = INSTRUCT_CALL;
				instr1.type = 706;
				//printf("String1: %s\n", instr1.value.s);
				instr1.value.s = "";
				//printf("String1: %s\n", instr1.value.s);
				//printf("String2: %s\n", tmp_str);
				instr1.value.s = tmp_str;
				//printf("String1: %s\n", instr1.value.s);

				insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

		break;

		// semanticka chyba prirazeni
		case LEX_EOL:
			fprintf(stderr, "Ocekavan vyraz v prirazeni na radku %d \n", gToken.row);
			return SEM_ERR;
		break;

		//pokud neni token LEX_ID_F, prozenem to precedencni SA
		default:
//			result = parse_expression();
			res = parse_expr(localMap, ilist, nazev_promenne);
			result = res.result;
	}



	return result;
}

int stat(void){
	int result = SUCCESS;
	expr_return res;



	switch(token){
		//<STAT> -> id = <STH>
		case LEX_ID:


			strcpy(tmp_str2, gToken.data.str);
			// INT ERR dopsat kontrolu

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


			// ulozeni promenne do lokalni mapy, hodnota nil, typ nil

			lData.defined = 1;
			lData.value.nil = true;
			lData.type = 500; // typ nil - NONE

			local_map_put(localMap, tmp_str2, lData);

			// instrukce pro definice promenne s typem nil a hodnotou nil
			instr_type = INSTRUCT_DEFVAR;
			instr1.type = 700; // GF
			instr1.value.s = tmp_str2;
			//printf("Tets %s - %s\n", tmp_str2, instr1.value.s);
			insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);



			// nacteni dalsiho tokenu , musi byt identifikator nebo vyraz
			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}


			// volani pravidla sth()
			result = sth(localMap, tmp_str2);
			if(result != SUCCESS){
				return result;
			}


			// MOVE GF@tmp TF@%retval
			instr_type = INSTRUCT_MOVE;
			instr1.type = 700;
			instr1.value.s = tmp_str2;
			instr2.type = 702;
			instr2.value.s = "%retval";

			insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

			return result;
		break;

		//<STAT> -> if <EXPR> then eol <ST-LIST> else eol <ST-LIST> end if
		case KW_IF:

			//nacteni a predani do vyrazove SA
			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}

			res = parse_expr(localMap, ilist, NULL);
			result = res.result;
//			result = parse_expression();
			if(result != SUCCESS){
				resetToken();
				return result;
			}

			// token nacteny z parse_expr ma byt THEN nebo


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
				fprintf(stderr, "Ocekavano 'eol' na radku %d \n", gToken.row);
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

			token = getToken();


			return SUCCESS;
		break;


		//<STAT> -> while <EXPR> do eol <ST-LIST> end
		case KW_WHILE:

			//vyrazova SA, pro precedencni analyzu
			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}

			res = parse_expr(localMap, ilist, NULL);
			result = res.result;
//			result = parse_expression();
			if(result != SUCCESS){
				resetToken();
				return result;
			}

			// token nacteny z precedencni analyzy musi byt DO

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

			token = getToken();

			return result;
		break;


		default:
			fprintf(stderr, "Ocekavano 'while' 'id' 'if' na radku %d \n", gToken.row); // dopsat
			resetToken();
			return SYN_ERR;
	}

	return result;


}
//int parse_expression(void){
//
//	// testovani pro a = 2 + 2 ( jen dva operandy), ale od precedencni analyzy se ocekava posledni token, bud je to EOL/EOF
//	// nebo dalsi statement
//
//	//returnToken();
// 	token = getToken();
//
//	token = getToken();
//
//	token = getToken();
//	token = getToken();
//
//	return SUCCESS;
//}


int st_list(void){
	int result = SUCCESS;
	//printf("st list %d\n", token);
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

			//printf("kone stat, st-list\n");

			//returnToken();

			// token nacteny z stat() muze byt dalsi statment nebo LEX_EOL/LEX_EOF

			if(checkTokenType(LEX_EOF)){
				return SUCCESS;
			}
			else{

				//printf("%d\n", token);
				token = getToken();
				return st_list();
			}

		// case KW_PRINT

		//<ST-LIST> -> nic
      case KW_ELSE:
      case KW_END:
      		return SUCCESS;
      	break;

      case KW_PRINT:

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

			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}

			result = term_list();

			if(result != SUCCESS){
				resetToken();
				return result;
			}

			// dalsi token pro st_list
			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}

			return st_list();

      break;


      case KW_DEF:

			result = func();
			if(result != SUCCESS){
				resetToken();
				return result;
			}
			// dalsi token je nacteny z func()
			if(!checkTokenType(LEX_EOL)){
				fprintf(stderr, "Ocekavan 'eol' na radku %d\n", gToken.row);
				resetToken();
				return SYN_ERR;
			}

			// dalsi token pro st_list
			token = getToken();
			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}

			return st_list();

      break;

	}

	return result;
}

int pm_list2(){
	switch(token){
		case LEX_COMMA:
			token = getToken();

			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}


			if(!checkTokenType(LEX_ID)){
				fprintf(stderr, "Ocekavno 'id' na radku %d\n", gToken.row);
				resetToken();
				return ERROR_LEX;
			}

			// SEMANTICKE AKCE pridani parametru do LTS funkce


			// + jeden parametr
			paramCount++;


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


int pm_list(){
	int result = SUCCESS;

	// LocalMap* lMap;

	// localMap = (LocalMap*) malloc ( sizeof(LocalMap) );
 //    local_map_init(localMap);


	if (token == LEX_ID){
		token = getToken();


		if(!error_lex()){
			return ERROR_LEX;
		} else if (!error_int()){
			return INT_ERR;
		}

		// SEMANTICKE AKCE
		// ulozeni do vnitrni LTS funkce parametr, nesmi byt jako nazev funkce, ani jako nazev jine funkce

		// dalsi semanticke akce -> pridat....



		// pocet parametru funkce ulozit
		paramCount++;

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

	if(!checkTokenType(LEX_ID) && !checkTokenType(LEX_ID_F)){
		fprintf(stderr, "Ocekavan identifikator na radku %d \n", gToken.row);
		resetToken();
		return SYN_ERR;
	}

	strcpy(tmp_str3, gToken.data.str);

	instr_type = INSTRUCT_LABEL;
	instr1.type = 706;
	instr1.value.s = tmp_str3;

	printf("Test %s\n", tmp_str3);
	printf("Test2 %s\n", instr1.value.s);

	insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

	// KONTROLA TF PRAZDNY - dopsat

	instr_type = INSTRUCT_PUSHFRAME;
	instr1.type = 707;
	insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

	// navratova hodnota

	instr_type = INSTRUCT_DEFVAR;
	instr1.type = 701;
	instr1.value.s = "%retval";
	insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

	instr_type = INSTRUCT_MOVE;
	instr1.type = 701;
	instr1.value.s = "%retval";
	instr2.type = 704;
	instr2.value.d = 0.0;

	insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

	// the next instruction are instr. of function .... <st-list>
	// udelat param-list!!



    // ukladam do GTS definice funkce
    // pokud tam ID neni tak vepsat


    strcpy(str, gToken.data.str);



    if (!global_map_contain(gMap, str)){
    	gData.defined = 1;
    	global_map_put(gMap, str, gData);
    }else {
        //uz byla definovana
        fprintf(stderr, "Radek %d: Semanticka chyba, funkce '%s' jiz byla definovana.\n", gToken.row, gToken.data.str);
        return SEM_ERR;
    }


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



	// ulozeni poctu formalnich parametru funkce
	gData.paramCount = paramCount;
	global_map_put(gMap, str, gData);

	// pro dalsi funkce
	paramCount = 0;



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

	// konec funkce

	instr_type = INSTRUCT_LABEL;
	instr1.type = 707;

	// vymyslet nejakou promennou pro ukonceni funkce, treba $foo$end
	// zatim to bude jen $end, ale muzeme narazit na funkci se stejnym nazvem
	instr1.value.s = "$end";
	insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

	// POPFRAME

	instr_type = INSTRUCT_POPFRAME;
	// otazka: instr1 je prazdny??
	insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);


	// return retval of function
	instr_type = INSTRUCT_RETURN;
	insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

	// end of function, navrat na predchozi pozici


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

	int result = SUCCESS;

	switch(token){
		case KW_IF:
		case LEX_ID:
		case KW_WHILE:
		case KW_PRINT:
		case KW_DEF:
		//case KW_INPUTS:
		// case KW_INPUTI:
		//case KW_INPUTF:
		case KW_LENGTH:
		//case KW_CHR:
		//case KW_ORD:
		//case KW_SUBSTR:

			//volani st_list()
			result = st_list();
			if(result != SUCCESS){
				resetToken();
				return result;
			}

			// //dalsi musi byt EOL / EOF
			token = getToken();

			if(!error_lex()){
				return ERROR_LEX;
			} else if (!error_int()){
				return INT_ERR;
			}

			//printf("main p %d\n", token);

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

			//printf("in prog\n");

			//sprintf("%d\n", token);

			//token nacten z main_p() = EOF
			if(!checkTokenType(LEX_EOL) && !checkTokenType(LEX_EOF)) {
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




int parse(GlobalMap* globalMap, tList *list) {

	int result = SUCCESS;

	gMap = globalMap;
	gDataptr = &gData;
	ilist = list;

	tmp_str = malloc(sizeof(char *));
	str = (char *)malloc(sizeof(char *));
	tmp_str2 = (char *)malloc(sizeof(char *));
	tmp_str3 = malloc(sizeof(char *));

	localMap = local_map_init(MAX_SIZE_OF_HASH_TABLE);

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
		instr_type = INSTRUCT_HEAD;
		insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);
		result = prog();
	}

	strFree(&(gToken.data));

	free(tmp_str);
	free(str);
	free(tmp_str2);
	free(tmp_str3);

	local_map_free(localMap);

	return result;

}