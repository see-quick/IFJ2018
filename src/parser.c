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
#include "instr_parse.h"

int token;        	         // aktualni token


bool is_LF = false;
bool zavorka = false;
char * function_name;
char * variable_name;
char *call_name;
bool in_while = false;
int while_counter = 0;

/*********************************************************************/
/*LOKALNI TABULKA SYMBOLU*/
LocalMap* localMap;
tDataIDF lData;
/*********************************************************************/



/*********************************************************************/
/* GLOBALNI TABULKA SYMBOLU */
GlobalMap* gMap;		     // globalni tabulka symbolu
//tDataFunction *gDataptr;	 // ukazatel na uzel globalni tabulky symbolu
tDataFunction gData;
int paramCount = 0;          // pocet parametru funkce
int argCount = 0;            // pocet argumentu pri volani funkce

/*********************************************************************/





/*********************************************************************/
/*GLOBALNI PROMENNE PRO UKLADANI INSTRUKCI DO PASKY TRIADRESNEHO KODU*/
tList * ilist;               // instruction list
tList * while_list;
tList * tmp_list;
tList * variables_list;
tInstructionTypes instr_type;
tInstructionData instr1;
tInstructionData instr2;
tInstructionData instr3;
/*********************************************************************/



void insert_build_in_functions(){
	gData.paramCount = 1;
    global_map_put(gMap, "length", gData);

    gData.paramCount = 3;
    global_map_put(gMap, "substr", gData);

    gData.paramCount = 2;
    global_map_put(gMap, "ord", gData);

    gData.paramCount = 1;
    global_map_put(gMap, "chr", gData);

    gData.paramCount = 0;
    global_map_put(gMap, "inputs", gData);
    global_map_put(gMap, "inputi", gData);
    global_map_put(gMap, "inputf", gData);
}


char * generate_param(char *string, int d){
	char c;
	sprintf(&c, "%d", d+1);
	size_t length = strlen(string);
	char *generate = malloc(length + 1 + 1);
	strcpy(generate, string);
	generate[length] = c;
	generate[length + 1] = '\0';
	return generate;
	free(generate);
}

void instruction_exit(int ret_val){
	instr_type = INSTRUCT_EXIT;
	instr1.type = I;
	instr1.value.i = ret_val;
	insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);
}


/*Funkce pro vypsani lexikalni chyby na standardni chybovy vystup*/
int error_lex(void){
	if(token == ERROR_LEX || token == LEX_UNKNOWN){
		//fprintf(stderr, "Lexikalni chyba na radku %d\n", gToken.row);
		return 0;
	}

	return 1;
}


/*Funkce pro vypsani interni chyby na standardni chybovy vystup*/
int error_int(void){
	if(token == INT_ERR){
		//fprintf(stderr, "Interni chyba\n");
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
			// semanticka akce
			// if LEX_ID -> zkontrolovat zda je promenna definovana
			// pokud je to LEX_ID, LEX_NUMBER, LEX_REAL, LEX_STRING , podle typu vytvorit vnitrni promennou s hodnotou argumentu

			if ( (result = check_substr_ord_build_in(argCount)) != SUCCESS) return result;

			instr_type = INSTRUCT_DEFVAR;
			instr1.type = TF;
			instr1.value.s = generate_param("$_param", argCount);

			insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);
			instr_type = INSTRUCT_MOVE;
			instr1.type = TF;
			instr1.value.s = generate_param("$_param", argCount);


			if (token == LEX_NUMBER){
				instr2.type = I; //integer
				instr2.value.i = atoi(gToken.data.str);
				insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

			}
			else if (token == LEX_REAL_NUMBER){
				instr2.type = F; //float
				instr2.value.f = atof(gToken.data.str);
				insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);
			}
			else if (token == LEX_STRING){
				instr2.type = S; //string
				instr2.value.s = gToken.data.str;
				insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);
			}
			else if (token == LEX_ID){
				if ((local_map_get_pointer_to_key(localMap, gToken.data.str)) == NULL){
					//fprintf(stderr, "Semanticka chyba, promenna %s neni definovana, radek %d\n", gToken.data.str, gToken.row);
					instruction_exit(SEM_ERR);
					return SEM_ERR;
				}
				else {
					instr2.type = GF; //string
					instr2.value.s = gToken.data.str;
					insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);
				}
			}

			
			token = getToken();
			if(!error_lex()){
				instruction_exit(ERROR_LEX);
				return ERROR_LEX;
			} else if (!error_int()){
				instruction_exit(INT_ERR);
				return INT_ERR;
			}
			return SUCCESS;
		break;

		default:
			if (global_map_contain(gMap, call_name)){
				gData = global_map_get_value(gMap, call_name);
				if (argCount != gData.paramCount){
					instruction_exit(ERR_PARAMS_COUNT);
					return ERR_PARAMS_COUNT;
				}
				else {
					instruction_exit(SYN_ERR);
					return SYN_ERR;
				}
			}
			else {
					//jiny token = chyba!
					//fprintf(stderr, "Ocekavano 'identifikator' 'konstanta' na radku %d \n", gToken.row);
					instruction_exit(SYN_ERR);
					return SYN_ERR;
			}
	}

	return result;
}


int check_length_substr_ord_build_in(){
	if (  ( (strcmp(call_name, "length")) == 0 )  ||  ( (strcmp(call_name, "substr")) == 0 ) ||  ( (strcmp(call_name, "ord")) == 0 ) ) {
				if (token != LEX_STRING){
					if (token != LEX_ID){
						instruction_exit(ERR_INCOMPATIBLE_TYPE);
						return ERR_INCOMPATIBLE_TYPE;
					}
					else {
						if (!local_map_contain(localMap, gToken.data.str)){
							if (is_LF){
								gData = global_map_get_value(gMap, function_name);
								if (!local_map_contain(gData.lMap, gToken.data.str)){
									instruction_exit(SEM_ERR);
									return SEM_ERR;
								}
								else {
									lData = local_map_get_value(gData.lMap, gToken.data.str);
									if (lData.type != STRING){
										instruction_exit(ERR_INCOMPATIBLE_TYPE);
										return ERR_INCOMPATIBLE_TYPE;
									}
								}
							}
						}
						else{
								lData = local_map_get_value(localMap, gToken.data.str);
								if (lData.type != STRING){
									instruction_exit(ERR_INCOMPATIBLE_TYPE);
									return ERR_INCOMPATIBLE_TYPE;
								}
						}
					}
				}
	}
	return SUCCESS;
}


int check_chr_build_in(){
	if ((strcmp(call_name, "chr"))==0){
				if (token != LEX_NUMBER){
					if (token != LEX_ID){
						instruction_exit(ERR_INCOMPATIBLE_TYPE);
						return ERR_INCOMPATIBLE_TYPE;
					}
					else {
						if (!local_map_contain(localMap, gToken.data.str)){
							if (is_LF){
								gData = global_map_get_value(gMap, function_name);
								if (!local_map_contain(gData.lMap, gToken.data.str)){
									instruction_exit(SEM_ERR);
									return SEM_ERR;
								}
								else {
									lData = local_map_get_value(gData.lMap, gToken.data.str);
									if (lData.type != INTEGER){
										instruction_exit(ERR_INCOMPATIBLE_TYPE);
										return ERR_INCOMPATIBLE_TYPE;
									}
									else {
										int i = atoi(gToken.data.str);
										if (i >= 299){
											instruction_exit(ERR_SEMANTIC);
											return ERR_SEMANTIC;
										}
									}
								}
							}
						}
						else{
								lData = local_map_get_value(localMap, gToken.data.str);
								if (lData.type != INTEGER){
									instruction_exit(ERR_INCOMPATIBLE_TYPE);
									return ERR_INCOMPATIBLE_TYPE;
								}
								else {
										int i = atoi(gToken.data.str);
										if (i >= 299){
											instruction_exit(ERR_SEMANTIC);
											return ERR_SEMANTIC;
										}
								}
						}
					}
				} else {
					int i = atoi(gToken.data.str);
					if (i >= 299){
						instruction_exit(ERR_SEMANTIC);
						return ERR_SEMANTIC;
					}
				}
	}
	return SUCCESS;
}


int check_substr_ord_build_in(int param){
	switch(param){
		case 1:
		case 2:
			if (param == 2 &&  ( (strcmp(call_name, "ord")) == 0 ) ){
				instruction_exit(ERR_PARAMS_COUNT);
				return ERR_PARAMS_COUNT;
			}
			else{
				if (  ( (strcmp(call_name, "substr")) == 0 ) ||  ( (strcmp(call_name, "ord")) == 0 ) ) {
					if (token != LEX_NUMBER){
						if (token != LEX_ID){
							instruction_exit(ERR_INCOMPATIBLE_TYPE);
							return ERR_INCOMPATIBLE_TYPE;
						}
						else {
							if (!local_map_contain(localMap, gToken.data.str)){
								if (is_LF){
									gData = global_map_get_value(gMap, function_name);
									if (!local_map_contain(gData.lMap, gToken.data.str)){
										instruction_exit(SEM_ERR);
										return SEM_ERR;
									}
									else {
										lData = local_map_get_value(gData.lMap, gToken.data.str);
										if (lData.type != INTEGER){
											instruction_exit(ERR_INCOMPATIBLE_TYPE);
											return ERR_INCOMPATIBLE_TYPE;
										}
									}
								}
							}
							else{
									lData = local_map_get_value(localMap, gToken.data.str);
									if (lData.type != INTEGER){
										instruction_exit(ERR_INCOMPATIBLE_TYPE);
										return ERR_INCOMPATIBLE_TYPE;
									}
							}
						}
					}
				}
			}
		break;

		case 3:
			instruction_exit(ERR_PARAMS_COUNT);
			return ERR_PARAMS_COUNT;
		break;
	}
	return SUCCESS;
}

int term_list2(bool zavorka){
	int result;

	switch(token){
		//<PM-LIST2> -> , id/lex_number/lex_real_number/lex_string <PM-LIST2>
		case LEX_COMMA:
			token = getToken();
			if(!error_lex()){
				instruction_exit(ERROR_LEX);
				return ERROR_LEX;
			} else if (!error_int()){
				instruction_exit(INT_ERR);
				return INT_ERR;
			}

			result = term();

			if(result != SUCCESS){
				return result;
			}

			argCount++;

			//nacteno z param()
			return term_list2(zavorka);

		break;

		case LEX_R_BRACKET:
			if (zavorka){return SUCCESS;}
			else { //fprintf(stderr, "Syntakticka chyba, neocekavana ')' na radku %d\n", gToken.row);  instruction_exit(SYN_ERR);
			instruction_exit(SYN_ERR);
			return SYN_ERR;}
			
		break;

		case LEX_EOL:
		case LEX_EOF:
			if (!zavorka){return SUCCESS;}
			else{ //fprintf(stderr, "Syntakticka chyba, ocekavana ')' na radku %d\n", gToken.row); instruction_exit(SYN_ERR);
			instruction_exit(SYN_ERR);
			return SYN_ERR; }
		break;

		default:
				//cokoliv jineho = chyba
				//fprintf(stderr, "Ocekavano ',' 'identifikator' 'konstanta' ')' na radku %d \n", gToken.row);
				instruction_exit(SYN_ERR);
				return SYN_ERR;
	}
}

int term_list(bool zavorka){
	int result = SUCCESS;

	switch(token){
		case LEX_ID:
		case LEX_NUMBER:
		case LEX_REAL_NUMBER:
		case LEX_STRING:

			// semanticka akce
			// if LEX_ID -> zkontrolovat zda je promenna definovana
			// pokud je to LEX_ID, LEX_NUMBER, LEX_REAL, LEX_STRING , podle typu vytvorit vnitrni promennou s hodnotou argumentu


			if ( (result = check_length_substr_ord_build_in()) != SUCCESS) return result;
			if ( (result = check_chr_build_in()) != SUCCESS) return result;

			instr_type = INSTRUCT_DEFVAR;
			instr1.type = TF;
			instr1.value.s = generate_param("$_param", argCount);


			insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);
			instr_type = INSTRUCT_MOVE;
			instr1.type = TF;
			instr1.value.s = generate_param("$_param", argCount);


			if (token == LEX_NUMBER){
				instr2.type = I; //integer
				instr2.value.i = atoi(gToken.data.str);
				insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

			}
			else if (token == LEX_REAL_NUMBER){
				instr2.type = F; //float
				instr2.value.f = atof(gToken.data.str);
				insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);
			}
			else if (token == LEX_STRING){
				instr2.type = S; //string
				instr2.value.s = gToken.data.str;
				insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);
			}
			else if (token == LEX_ID){
				if (!local_map_contain(localMap, gToken.data.str)){
					//fprintf(stderr, "Semanticka chyba, promenna %s neni definovana, radek %d\n", gToken.data.str, gToken.row);
					instruction_exit(SEM_ERR);
					return SEM_ERR;
				}
				else if (is_LF){
					gData = global_map_get_value(gMap, function_name);
					if (!local_map_contain(gData.lMap, gToken.data.str)){
						instruction_exit(SEM_ERR);
						return SEM_ERR;
					}
				}
				else {
					instr2.type = GF; //string
					instr2.value.s = gToken.data.str;
					insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);
				}
			}

			// zvetsit pocet argumentu
			argCount++;

			//nacteni a volani term_list2()
			token = getToken();
			if(!error_lex()){
				instruction_exit(ERROR_LEX);
				return ERROR_LEX;
			} else if (!error_int()){
				instruction_exit(INT_ERR);
				return INT_ERR;
			}
		

			return term_list2(zavorka);
		break;

		case LEX_R_BRACKET:
			if (zavorka){ return SUCCESS;}
			else { //fprintf(stderr, "Syntakticka chyba, neocekavana ')' na radku %d\n", gToken.row);
			instruction_exit(SYN_ERR);
			return SYN_ERR;
		}
			
		break;

		case LEX_EOL:
		case LEX_EOF:
			if (!zavorka){return SUCCESS;}
			else{ //fprintf(stderr, "Syntakticka chyba, ocekavana ')' na radlu %d\n", gToken.row);  instruction_exit(SYN_ERR);

			instruction_exit(SYN_ERR);
			return SYN_ERR; }
		break;


		default:
			//fprintf(stderr, "Ocekavano 'identifikator' 'konstanta' ')' na radku %d\n", gToken.row);
			instruction_exit(SYN_ERR);
			return SYN_ERR;
	}

	token = getToken();
	if(!error_lex()){
		instruction_exit(ERROR_LEX);
		return ERROR_LEX;
	} else if (!error_int()){
		instruction_exit(INT_ERR);
		return INT_ERR;
	}


	return result;

}


int move_value(expr_return res){
	instr_type = INSTRUCT_MOVE;

	lData.type = res.data_type;
	if (is_LF){
		//printf("Move in function %s, promenna je %s\n", function_name, variable_name);
		gData = global_map_get_value(gMap, function_name);
		local_map_put(gData.lMap, variable_name, lData);
	}
	else{
		local_map_put(localMap, variable_name, lData);
	}

	if (is_LF) { instr1.type = LF; } else {instr1.type = GF;}
	instr2.type = GF;
	instr1.value.s = variable_name;
	instr2.value.s = "$result";
	insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);
	return SUCCESS;
}

int check_input(){
	if ( (strcmp(call_name, "inputi") == 0 ) ){
		if (local_map_contain(localMap, variable_name)) { instr1.type = GF;}
		else {instr1.type = LF;}
	 
	 	instr1.value.s = variable_name;
		instr_type = INSTRUCT_INPUT_I;
		insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);
	}
	else if ( (strcmp(call_name, "inputf") == 0 ) ) {
		if (local_map_contain(localMap, variable_name)) { instr1.type = GF;}
		else {instr1.type = LF;}

		instr1.value.s = variable_name;
		instr_type = INSTRUCT_INPUT_F;
		insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);
	}
	else if ( (strcmp(call_name, "inputs") == 0 ) ) {
		if (local_map_contain(localMap, variable_name)) { instr1.type = GF;}
		else {instr1.type = LF;}

		instr1.value.s = variable_name;
		instr_type = INSTRUCT_INPUT_S;
		insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);
	}
	else {
		return 0;
	}

	return 1;
}

int sth(){
	int result = SUCCESS;
	expr_return res;

	tDataFunction *tmp;


	switch(token){
		// case LEX_ID_F:
		case LEX_ID_F:
		case LEX_ID:
		case KW_LENGTH:
		case KW_SUBSTR:
		case KW_ORD:
		case KW_CHR:
		case KW_INPUT_S:
		case KW_INPUT_I:
		case KW_INPUT_F:

				//SEMANTICKA AKCE, KONTROLA DEFINICE FUNKCE
				tmp = global_map_get_pointer_to_value(gMap, gToken.data.str);
				if (tmp == NULL){
					// neni funkce
					if ( (local_map_get_pointer_to_value(localMap, gToken.data.str)) == NULL){
						//fprintf(stderr, "Semanticka chyba, funkce nebo promenna %s neni definovana, radek %d\n", gToken.data.str, gToken.row);
						instruction_exit(SEM_ERR);
						return SEM_ERR;
					}
					else{
						instr_type = INSTRUCT_MOVE;
						instr1.type = GF;
						instr1.value.s = "$result\0";
						if (is_LF){ instr2.type = LF; } else {instr2.type = GF;}
						instr2.value.s = gToken.data.str;
						insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);


						// je to promenna prirazenu typu a = b
						res = parse_expr(localMap, ilist, false);
						result = res.result;

						if (res.bool_result){
							instruction_exit(ERR_SEMANTIC);
							return ERR_SEMANTIC;
						}

						if (result == SUCCESS){
							move_value(res);
						}
						// to muze byt funkce
						
						else {
							instruction_exit(result);
							return result;
						}

						// cokoliv jineho syntakticka chyba
						if (token != LEX_EOL){
							//fprintf(stderr, "Syntakticka chyba, ocekavano 'eol' na radku %d\n", gToken.row);
							instruction_exit(SYN_ERR);
							return SYN_ERR;
						}

					}
						
				}
				else{
						// je funkce					
						//is_LF = true;	

						call_name = gToken.data.str;

        	    		token = getToken();
						if(!error_lex()){
							instruction_exit(ERROR_LEX);
							return ERROR_LEX;
						} else if (!error_int()){
							instruction_exit(INT_ERR);
							return INT_ERR;
						}

						if ( (token == LEX_EOL || token == LEX_EOF) && tmp->paramCount == 0  ){
							// pro dalsi volani funkce
							argCount = 0;

							result = check_input();
							if (!result) {
								instr_type = INSTRUCT_PUSHFRAME;
								instr1.type = EMPTY;
								insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

								// instrukce pro volani funkce

								instr_type = INSTRUCT_CALL;
								instr1.type = FCE;
								instr1.value.s = call_name;

								insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

								// POPFRAME

								instr_type = INSTRUCT_POPFRAME;
								insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);
							}

							// token = getToken();
							// if(!error_lex()){
							// 	instruction_exit(ERROR_LEX);
							// 	return ERROR_LEX;
							// } else if (!error_int()){
							// 	instruction_exit(INT_ERR);
							// 	return INT_ERR;
							// }

							// returnToken();

							return SUCCESS;

							// is_LF = false; // refresh promenne
						}
						else {
							switch(token){
								case LEX_L_BRACKET:
									token = getToken();
									if(!error_lex()){
										instruction_exit(ERROR_LEX);
										return ERROR_LEX;
									} else if (!error_int()){
										instruction_exit(INT_ERR);
										return INT_ERR;
									}

									//volane term_list()
									zavorka = true;
									result = term_list(zavorka);
									if(result != SUCCESS){
										return result;
									}

									//dalsi token je nacten, musi = ')'

									if(!checkTokenType(LEX_R_BRACKET)){
										//fprintf(stderr, "Ocekavana ')' na radku %d \n", gToken.row);
										instruction_exit(SYN_ERR);
										return SYN_ERR;
									}

									if ( tmp->paramCount != argCount ){
										//fprintf(stderr, "Semanticka chyba, pocet parametru funkce nesouvisi s poctem argumentu na radku %d\n",gToken.row);
										instruction_exit(ERR_PARAMS_COUNT);
										return ERR_PARAMS_COUNT;
									}

									// pro dalsi volani funkce
									argCount = 0;

									result = check_input();
									if (!result){
											instr_type = INSTRUCT_PUSHFRAME;
											instr1.type = EMPTY;
											insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

											// instrukce pro volani funkce

											instr_type = INSTRUCT_CALL;
											instr1.type = FCE;
											instr1.value.s = call_name;

											insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

											// POPFRAME

											instr_type = INSTRUCT_POPFRAME;
											insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);
									}
									token = getToken();
									if(!error_lex()){
										instruction_exit(ERROR_LEX);
										return ERROR_LEX;
									} else if (!error_int()){
										instruction_exit(INT_ERR);
										return INT_ERR;
									}

									return SUCCESS;


								break;
								case LEX_ID:
								case LEX_NUMBER:
								case LEX_REAL_NUMBER:
								case LEX_STRING:

									//volane term_list()
									zavorka = false;
									result = term_list(zavorka);
									if(result != SUCCESS){
										return result;
									}

									if ( tmp->paramCount != argCount ){
										//fprintf(stderr, "Semanticka chyba, pocet parametru funkce nesouvisi s poctem argumentu na radku %d\n",gToken.row);
										instruction_exit(ERR_PARAMS_COUNT);
										return ERR_PARAMS_COUNT;
									}

									// pro dalsi volani funkce
									argCount = 0;


									instr_type = INSTRUCT_PUSHFRAME;
									instr1.type = EMPTY;
									insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

									// instrukce pro volani funkce

									instr_type = INSTRUCT_CALL;
									instr1.type = FCE;
									instr1.value.s = call_name;

									insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

									// POPFRAME

									instr_type = INSTRUCT_POPFRAME;
									insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

									// is_LF = false; // refresh promenne


									return SUCCESS;
								break;
								default:
									//fprintf(stderr, "Syntakticka chyba, ocekavano '(', terminal na radku %d\n", gToken.row);
									instruction_exit(SYN_ERR);
									return SYN_ERR;
							} // end switch pro kontrolu zavorek
	
					}
				}			

		break;

		// semanticka chyba prirazeni
		case LEX_EOL:
		case LEX_EOF:
			//fprintf(stderr, "Ocekavan vyraz v prirazeni na radku %d \n", gToken.row);
			instruction_exit(SEM_ERR);
			return SEM_ERR;
		break;

		//pokud neni token LEX_ID_F, prozenem to precedencni SA
		default:


			instr_type = INSTRUCT_MOVE;
			if (token == LEX_NUMBER){
				instr2.type = I;
				instr2.value.i = atoi(gToken.data.str);
			}
			else if (token == LEX_REAL_NUMBER){
				instr2.type = F;
				instr2.value.f = atof(gToken.data.str);
			}
			else if (token == LEX_STRING) {
				instr2.type = S;
				instr2.value.s = gToken.data.str;
			}
			else if (token == LEX_ID){
				if (is_LF){instr2.type = LF;} else {instr2.type = GF;}
				instr2.value.s = gToken.data.str;
			}

			instr1.type = GF;
			instr1.value.s = "$result\0";

			insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);


			res = parse_expr(localMap, ilist, false);
			result = res.result;

			if (res.bool_result){
				instruction_exit(ERR_SEMANTIC);
				return ERR_SEMANTIC;
			}


			if (res.data_type == FUNCTION){
				// volani funkce 
						tmp = global_map_get_pointer_to_value(gMap, gToken.data.str);
						// is_LF = true;

        	    		token = getToken();
						if(!error_lex()){
							instruction_exit(ERROR_LEX);
							return ERROR_LEX;
						} else if (!error_int()){
							instruction_exit(INT_ERR);
							return INT_ERR;
						}

						if ( (token == LEX_EOL || token == LEX_EOF) && tmp->paramCount == 0 ){}
						else {

							switch(token){
								case LEX_L_BRACKET:
									token = getToken();
									if(!error_lex()){
										instruction_exit(ERROR_LEX);
										return ERROR_LEX;
									} else if (!error_int()){
										instruction_exit(INT_ERR);
										return INT_ERR;
									}

									//volane term_list()
									zavorka = true;
									result = term_list(zavorka);
									if(result != SUCCESS){
										return result;
									}

									//dalsi token je nacten, musi = ')'

									if(!checkTokenType(LEX_R_BRACKET)){
										//fprintf(stderr, "Ocekavana ')' na radku %d \n", gToken.row);
										instruction_exit(SYN_ERR);
										return SYN_ERR;
									}

									return SUCCESS;
								break;
								case LEX_ID:
								case LEX_NUMBER:
								case LEX_REAL_NUMBER:
								case LEX_STRING:

									//volane term_list()
									zavorka = false;
									result = term_list(zavorka);
									if(result != SUCCESS){
										return result;
									}

									return SUCCESS;
								break;
								default:
									//fprintf(stderr, "Syntakticka chyba, ocekavano '(', terminal na radku %d\n", gToken.row);
									instruction_exit(SYN_ERR);
									return SYN_ERR;
							}

						}

						if ( tmp->paramCount != argCount ){
							//fprintf(stderr, "Semanticka chyba, pocet parametru funkce %s nesouvisi s poctem argumentu na radku %d\n",gToken.data.str , gToken.row);
							instruction_exit(ERR_PARAMS_COUNT);
							return ERR_PARAMS_COUNT;
						}

						// pro dalsi volani funkce
						argCount = 0;


						instr_type = INSTRUCT_PUSHFRAME;
						instr1.type = EMPTY;
						insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

						// instrukce pro volani funkce

						instr_type = INSTRUCT_CALL;
						instr1.type = FCE;
						instr1.value.s = call_name;

						insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

						// POPFRAME

						instr_type = INSTRUCT_POPFRAME;
						insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);


						token = getToken();
						if(!error_lex()){
							instruction_exit(ERROR_LEX);
							return ERROR_LEX;
						} else if (!error_int()){
							instruction_exit(INT_ERR);
							return INT_ERR;
						}

						if (token == LEX_ADDITION || token == LEX_SUBSTRACTION || token == LEX_MULTIPLICATION || token == LEX_DIVISION){

							token = getToken();
							if(!error_lex()){
								instruction_exit(ERROR_LEX);
								return ERROR_LEX;
							} else if (!error_int()){
								instruction_exit(INT_ERR);
								return INT_ERR;
							}

							return sth();
						}

						is_LF = false; // refresh promenne

			}

			else if (result == SUCCESS){
					move_value(res);
			}
			else {
				instruction_exit(result);
				return result;
			}

			// cokoliv jineho syntakticka chyba
			if (token != LEX_EOL){
				//fprintf(stderr, "Syntakticka chyba, ocekavano 'eol' na radku %d\n", gToken.row);
				instruction_exit(SYN_ERR);
				return SYN_ERR;
			}

			return result;
	} // end switch 

	return result;
}

int stat(){
	int result = SUCCESS;
	expr_return res;
	switch(token){
		//<STAT> -> id = <STH>
		case LEX_ID:
			variable_name = gToken.data.str;

			//dalsi musi byt '='
			token = getToken();
			if(!error_lex()){
				instruction_exit(ERROR_LEX);
				return ERROR_LEX;
			} else if (!error_int()){
				instruction_exit(INT_ERR);
				return INT_ERR;
			}

			if(!checkTokenType(LEX_EQUAL)){
				//fprintf(stderr, "Ocekavano '=' na radku %d\n", gToken.row);
				instruction_exit(SYN_ERR);
				return SYN_ERR;
			}

			if (global_map_contain(gMap, variable_name)){
				//fprintf(stderr, "Semanticka chyba na radku %d, existuje promenna se stejnym jmenem \n", gToken.row);
				instruction_exit(SEM_ERR);
				return SEM_ERR;
			}

			if (is_LF){
				gData = global_map_get_value(gMap, function_name);
				if (!local_map_contain(gData.lMap, variable_name)){
					lData.defined = 1;
					lData.value.nil = true;
					lData.type = 500;

					local_map_put(gData.lMap, variable_name, lData);

					instr_type = INSTRUCT_DEFVAR;
					if (is_LF) {instr1.type = LF;}
					else{
						instr1.type = GF;
					}

					instr1.value.s = variable_name; // nazev promenne
					if (in_while) insert_item(variables_list, &instr_type, &instr1, &instr2, &instr3);
					else {insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);}

				}
			}
			else {
				if ( !local_map_contain(localMap, variable_name) ){
					// ulozeni promenne do lokalni mapy, hodnota nil, typ nil
					lData.defined = 1;
					lData.value.nil = true;
					lData.type = 500; // typ nil - NONE


					// nazev promenne ziskame z list pro tokeny pomoci funkce DLCOPYFISRT
					local_map_put(localMap, variable_name, lData);
		

					// generovani instrukce pro definice promenne s typem nil a hodnotou nil
					instr_type = INSTRUCT_DEFVAR;
					if (is_LF) {instr1.type = LF;}
					else{
						instr1.type = GF;
					}

					instr1.value.s = variable_name; // nazev promenne
					if (in_while) insert_item(variables_list, &instr_type, &instr1, &instr2, &instr3);
					else {insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);}
				
				}
			}
			// jinak promenna je v lokalni mape, nebudeme ukladat promennou s typem none

			
			// nacteni dalsiho tokenu , musi byt  bud' identifikator nebo vyraz nebo funkce
			token = getToken();
			if(!error_lex()){
				instruction_exit(ERROR_LEX);
				return ERROR_LEX;
			} else if (!error_int()){
				instruction_exit(INT_ERR);
				return INT_ERR;
			}


			// volani pravidla sth()
			result = sth(localMap);

			if(result != SUCCESS){
				return result;
			}

			return SUCCESS;
		break;

		case KW_PRINT:
			argCount = 0;

			token = getToken();
			
			if(!error_lex()){
				instruction_exit(ERROR_LEX);
				return ERROR_LEX;
			} else if (!error_int()){
				instruction_exit(INT_ERR);
				return INT_ERR;
			}

			switch(token){
				case LEX_L_BRACKET:

					token = getToken();
					if(!error_lex()){
						instruction_exit(ERROR_LEX);
						return ERROR_LEX;
					} else if (!error_int()){
						instruction_exit(INT_ERR);
						return INT_ERR;
					}

					//volane term_list()
					zavorka = true;

					result = term_list(zavorka);
					if(result != SUCCESS){
						return result;
					}

					token = getToken();
					if(!error_lex()){
						instruction_exit(ERROR_LEX);
						return ERROR_LEX;
					} else if (!error_int()){
						instruction_exit(INT_ERR);
						return INT_ERR;
					}

					if (argCount == 0){
						instruction_exit(ERR_PARAMS_COUNT);
						return ERR_PARAMS_COUNT;
					}


					instr1.type = I;
					instr1.value.i = argCount;

					instr_type = INSTRUCT_PRINT;
					insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

				break;

				case LEX_ID:
				case LEX_NUMBER:
				case LEX_STRING:
				case LEX_REAL_NUMBER:

					//volane term_list()
					zavorka = false;
					result = term_list(zavorka);
					if(result != SUCCESS){
						return result;
					}

					if (argCount == 0){
						instruction_exit(ERR_PARAMS_COUNT);
						return ERR_PARAMS_COUNT;
					}

					instr1.type = I;
					instr1.value.i = argCount;

					instr_type = INSTRUCT_PRINT;
					insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

				break;

				default:
					//fprintf(stderr, "Syntakticka chyba, ocekavano '(',terminal na radku %d\n", gToken.row );
					instruction_exit(ERR_PARAMS_COUNT);
					return ERR_PARAMS_COUNT;
			}


		break;


		//<STAT> -> if <EXPR> then eol <ST-LIST> else eol <ST-LIST> end if
		case KW_IF:


			in_while = true;

			++while_counter;

			if (while_counter == 1){
				//printf("Furst while");
				tmp_list = ilist;
				ilist = while_list;
			}

			//nacteni a predani do vyrazove SA
			token = getToken();
			if(!error_lex()){
				instruction_exit(ERROR_LEX);
				return ERROR_LEX;
			} else if (!error_int()){
				instruction_exit(INT_ERR);
				return INT_ERR;
			}

			res = parse_expr(localMap, ilist, true);
			result = res.result;

			if (!res.bool_result){
				ilist = tmp_list;

				reverse(&(variables_list->first));
      			set_active(variables_list);

      			append_list(ilist, variables_list);

				reverse(&(while_list->first));
      			set_active(while_list);

				append_list(ilist, while_list);
				
				if (result != SUCCESS){
					instruction_exit(result);
					return result;
				}
				else {
					instruction_exit(ERR_SEMANTIC);
					return ERR_SEMANTIC;
				}
			}

			if(result != SUCCESS){
				instruction_exit(result);
				return result;
			}

			//token je jiz nacteny, musi = KW_THEN
			if(!checkTokenType(KW_THEN)){
				//fprintf(stderr, "Ocekavano 'then' na radku %d\n", gToken.row);
				instruction_exit(SYN_ERR);
				return SYN_ERR;
			}

			instr_type =  INSTRUCT_IF_THEN;
			insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

			//dalsi = LEX_EOL
			token = getToken();
			if(!error_lex()){
				instruction_exit(ERROR_LEX);
				return ERROR_LEX;
			} else if (!error_int()){
				instruction_exit(INT_ERR);
				return INT_ERR;
			}

			if(!checkTokenType(LEX_EOL)){
				//fprintf(stderr, "Ocekavano 'eol' na radku %d \n", gToken.row);
				instruction_exit(SYN_ERR);
				return SYN_ERR;
			}

			//dalsi token -> volani st_list()
			token = getToken();
			if(!error_lex()){
				instruction_exit(ERROR_LEX);
				return ERROR_LEX;
			} else if (!error_int()){
				instruction_exit(INT_ERR);
				return INT_ERR;
			}

			result = st_list();
			if(result != SUCCESS){
				return result;
			}

			instr_type = INSTRUCT_JUMP_ENDIF;
			insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);


			//token nacteny z st_list() = KW_ELSE
			if(!checkTokenType(KW_ELSE)){
				//fprintf(stderr, "Ocekavano 'else' na radku %d \n", gToken.row);
				instruction_exit(SYN_ERR);
				return SYN_ERR;
			}

			instr_type = INSTRUCT_IF_ELSE;
			insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);



			//dalsi = LEX_EOL
			token = getToken();
			if(!error_lex()){
				instruction_exit(ERROR_LEX);
				return ERROR_LEX;
			} else if (!error_int()){
				instruction_exit(INT_ERR);
				return INT_ERR;
			}

			if(!checkTokenType(LEX_EOL)){
				//fprintf(stderr, "3 Ocekavano 'eol' na radku %d\n", gToken.row);
				instruction_exit(SYN_ERR);
				return SYN_ERR;
			}

			//nacteni tokenu a volani st_list()
			token = getToken();
			if(!error_lex()){
				instruction_exit(ERROR_LEX);
				return ERROR_LEX;
			} else if (!error_int()){
				instruction_exit(INT_ERR);
				return INT_ERR;
			}

			result = st_list();
			if(result != SUCCESS){
				return result;
			}

			instr_type = INSTRUCT_JUMP_ENDIF;
			insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);


			//token nacten z fce st_list() = KW_END
			if(!checkTokenType(KW_END)){
				//fprintf(stderr, "Ocekano 'end' na radku %d \n", gToken.row);
				instruction_exit(SYN_ERR);
				return SYN_ERR;
			}
			if (while_counter == 1){

				ilist = tmp_list;

				reverse(&(variables_list->first));
      			set_active(variables_list);

      			append_list(ilist, variables_list);

				reverse(&(while_list->first));
      			set_active(while_list);

				append_list(ilist, while_list);

			}

			while_counter--;

			instr_type = INSTRUCT_ENDIF;
			insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

			token = getToken();
			if(!error_lex()){
				instruction_exit(ERROR_LEX);
				return ERROR_LEX;
			} else if (!error_int()){
				instruction_exit(INT_ERR);
				return INT_ERR;
			}

			if (while_counter == 0){
				in_while = false;
			}

			return SUCCESS;
		break;


		//<STAT> -> while <EXPR> do eol <ST-LIST> end
		case KW_WHILE:

			in_while = true;

			++while_counter;

			if (while_counter == 1){
				//printf("Furst while");
				tmp_list = ilist;
				ilist = while_list;
			}

			instr_type = INSTRUCT_WHILE_START;
			insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);


			//vyrazova SA, pro precedencni analyzu
			token = getToken();
			if(!error_lex()){
				instruction_exit(ERROR_LEX);
				return ERROR_LEX;
			} else if (!error_int()){
				instruction_exit(INT_ERR);
				return INT_ERR;
			}

			res = parse_expr(localMap, ilist, true);
			result = res.result;

			if (!res.bool_result){
				ilist = tmp_list;

				reverse(&(variables_list->first));
      			set_active(variables_list);

      			append_list(ilist, variables_list);

				reverse(&(while_list->first));
      			set_active(while_list);

				append_list(ilist, while_list);
				
				if (result != SUCCESS){
					instruction_exit(result);
					return result;
				}
				else {
					instruction_exit(ERR_SEMANTIC);
					return ERR_SEMANTIC;
				}
			}

			instr_type = INSTRUCT_WHILE_STATS;
			insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);


			if(result != SUCCESS){
				instruction_exit(result);
				return result;
			}

			// token nacteny z precedencni analyzy musi byt DO

			if(!checkTokenType(KW_DO)){
				//fprintf(stderr, "Ocekavano 'do' na radku %d\n", gToken.row);
				instruction_exit(SYN_ERR);
				return SYN_ERR;
			}

			token = getToken();
			if(!error_lex()){
				instruction_exit(ERROR_LEX);
				return ERROR_LEX;
			} else if (!error_int()){
				instruction_exit(INT_ERR);
				return INT_ERR;
			}


			if(!checkTokenType(LEX_EOL)){
				//fprintf(stderr, "Ocekavano 'eol' na radku %d\n", gToken.row);
				instruction_exit(SYN_ERR);
				return SYN_ERR;
			}

			//volani st_list
			token = getToken();
			if(!error_lex()){
				instruction_exit(ERROR_LEX);
				return ERROR_LEX;
			} else if (!error_int()){
				instruction_exit(INT_ERR);
				return INT_ERR;
			}

			result = st_list();


			if(result != SUCCESS){
				return result;
			}

			//dalsi token je zase nacten z fce, musi byt END
			if(!checkTokenType(KW_END)){
				//fprintf(stderr, "Ocekavan 'end' na radku %d\n", gToken.row);
				instruction_exit(SYN_ERR);
				return SYN_ERR;
			}

			if (while_counter == 1){

				ilist = tmp_list;

				reverse(&(variables_list->first));
      			set_active(variables_list);

      			append_list(ilist, variables_list);

				reverse(&(while_list->first));
      			set_active(while_list);

				append_list(ilist, while_list);

			}

			while_counter--;

			instr_type = INSTRUCT_WHILE_END;
			insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

			token = getToken();
			if(!error_lex()){
				instruction_exit(ERROR_LEX);
				return ERROR_LEX;
			} else if (!error_int()){
				instruction_exit(INT_ERR);
				return INT_ERR;
			}

			if (while_counter == 0){
					in_while = false;
			}

			return result;
		break;

		default:
			//fprintf(stderr, "Ocekavano 'while' 'id' 'if' na radku %d \n", gToken.row); // dopsat
			instruction_exit(SYN_ERR);
			return SYN_ERR;
	}

	return result;


}

int st_list(){
	int result = SUCCESS;
	//pravidlo <ST-LIST> -> <STAT> eol <ST-LIST>
	switch(token){

		case KW_IF:
		case KW_WHILE:
		case LEX_ID:
		case KW_PRINT:

			//podle pravidla zavolame stat()
			result = stat();
			if(result != SUCCESS){
				return result;
			}

			// token nacteny z stat() muze byt dalsi statment nebo LEX_EOL/LEX_EOF

			if(checkTokenType(LEX_EOF)){
				return SUCCESS;
			}
			else{

				token = getToken();
			
				if(!error_lex()){
					instruction_exit(ERROR_LEX);
					return ERROR_LEX;
				} else if (!error_int()){
					instruction_exit(INT_ERR);
					return INT_ERR;
				}
				return st_list();
			}


		//<ST-LIST> -> nic
      case KW_ELSE:
      case KW_END:
      		return SUCCESS;
      	break;

      case KW_DEF:

      		is_LF = true;

			result = func();
			if(result != SUCCESS){
				return result;
			}
			// dalsi token je nacteny z func()
			if(!checkTokenType(LEX_EOL)){
				//fprintf(stderr, "Ocekavan 'eol' na radku %d\n", gToken.row);
				instruction_exit(SYN_ERR);
				return SYN_ERR;
			}

			is_LF = false;

			// dalsi token pro st_list
			token = getToken();
			if(!error_lex()){
				instruction_exit(ERROR_LEX);
				return ERROR_LEX;
			} else if (!error_int()){
				instruction_exit(INT_ERR);
				return INT_ERR;
			}

		
			return st_list();


      break;


      // ignoring EOL....

      case LEX_EOL:
      		token = getToken();
      		if(!error_lex()){
				instruction_exit(ERROR_LEX);
				return ERROR_LEX;
			} else if (!error_int()){
				instruction_exit(INT_ERR);
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
				instruction_exit(ERROR_LEX);
				return ERROR_LEX;
			} else if (!error_int()){
				instruction_exit(INT_ERR);
				return INT_ERR;
			}


			if(!checkTokenType(LEX_ID)){
				//fprintf(stderr, "Ocekavno 'id' na radku %d\n", gToken.row);
				instruction_exit(SYN_ERR);
				return SYN_ERR;
			}


			if (!local_map_contain(localMap, gToken.data.str)){
				// definovat tuto promennou s typem nil a hodnotou nil
				lData.defined = 1;
				lData.value.nil = true;
				lData.type = 500; // typ nil - NONE
				gData = global_map_get_value(gMap, function_name);
				local_map_put(gData.lMap, gToken.data.str, lData);
			}

			// + jeden parametr
			paramCount++;

			token = getToken();

			if(!error_lex()){
				instruction_exit(ERROR_LEX);
				return ERROR_LEX;
			} else if (!error_int()){
				instruction_exit(INT_ERR);
				return INT_ERR;
			}


			return pm_list2();
		break;

		case LEX_R_BRACKET:
			return SUCCESS;
		break;

		default:
			//fprintf(stderr, "Syntakticka chyba, ocekavano ',', ')' na radku %d\n", gToken.row);
			instruction_exit(SYN_ERR);
			return SYN_ERR;
	}

	token = getToken();
	if(!error_lex()){
		instruction_exit(ERROR_LEX);
		return ERROR_LEX;
	} else if(!error_int()){
		instruction_exit(INT_ERR);
		return INT_ERR;
	}

	return SUCCESS;
}


int pm_list(){
	int result = SUCCESS;

	if (token == LEX_ID){

		if (!local_map_contain(localMap, gToken.data.str)){
			// definovat tuto promennou s typem nil a hodnotou nil
			lData.defined = 1;
			lData.value.nil = true;
			lData.type = 500; // typ nil - NONE
			gData = global_map_get_value(gMap, function_name);
			local_map_put(gData.lMap, gToken.data.str, lData);
		}

		paramCount++;

		token = getToken();

		if(!error_lex()){
			instruction_exit(ERROR_LEX);
			return ERROR_LEX;
		} else if (!error_int()){
			instruction_exit(INT_ERR);
			return INT_ERR;
		}

		return pm_list2();
	}
	else{
		return SUCCESS;
	}

	return result;
}

int func(){
	int result;

	//pravidlo <func> -> def ID ( <pm_list> ) eol <ST-LIST> end

	//dalsi token musi byt typu LEX_ID
	token = getToken();
	if(!error_lex()){
		instruction_exit(ERROR_LEX);
		return ERROR_LEX;
	} else if(!error_int()){
		instruction_exit(INT_ERR);
		return INT_ERR;
	}

	if(!checkTokenType(LEX_ID) && !checkTokenType(LEX_ID_F)){
		//fprintf(stderr, "Ocekavan identifikator na radku %d \n", gToken.row);
		instruction_exit(SYN_ERR);
		return SYN_ERR;
	}

	// ukladam do GTS definice funkce
    // pokud tam ID neni tak vepsat


    if (!global_map_contain(gMap, gToken.data.str)){
    	if (local_map_contain(localMap, gToken.data.str)){
    		//fprintf(stderr, "Semanticka chyba na radku %d, existuje promenna se stejnym jmenem\n", gToken.row);
    		instruction_exit(SEM_ERR);
    		return SEM_ERR;
    	}
    	else{
    		gData.defined = 1;
    		gData.lMap = local_map_init(MAX_SIZE_OF_HASH_TABLE);
    		global_map_put(gMap, gToken.data.str, gData);
    	}
    }
    else {
        //uz byla definovana
        //fprintf(stderr, "Radek %d: Semanticka chyba, funkce '%s' jiz byla definovana.\n", gToken.row, gToken.data.str);
        instruction_exit(SEM_ERR);
        return SEM_ERR;
    }


	instr_type = INSTRUCT_LABEL;
	instr1.type = FCE;
	instr1.value.s = DLCopyFirst(&tlist);

	insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

	function_name = gToken.data.str;
	//printf("Function name is %s\n", function_name);

	// navratova hodnota
	instr_type = INSTRUCT_DEFVAR;
	instr1.type = LF;
	instr1.value.s = "%retval";
	insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

	instr_type = INSTRUCT_MOVE;
	instr1.type = LF;
	instr1.value.s = "%retval";
	instr2.type = F;
	instr2.value.f = 0.0;

	insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);



   	//dalsi token musi byt '('
	token = getToken();
	if(!error_lex()){
		instruction_exit(ERROR_LEX);
		return ERROR_LEX;
	} else if (!error_int()){
		instruction_exit(INT_ERR);
		return INT_ERR;
	}

	if(!checkTokenType(LEX_L_BRACKET)){
		//fprintf(stderr, "Ocekavana '(' na radku %d\n", gToken.row);
		instruction_exit(SYN_ERR);
		return SYN_ERR;
	}

	//po kontrole dalsiho tokenu volane pm_list()

	token = getToken();
	if(!error_lex()){
		instruction_exit(ERROR_LEX);
		return ERROR_LEX;
	} else if (!error_int()){
		instruction_exit(INT_ERR);
		return INT_ERR;
	}

	result = pm_list();
	if(result != SUCCESS){
		return result;
	}

	//dalsi token mame z fce pm_list(), musi byt ')'

	if(!error_lex()){
		instruction_exit(ERROR_LEX);
		return ERROR_LEX;
	} else if (!error_int()){
		instruction_exit(INT_ERR);
		return INT_ERR;
	}

	if(!checkTokenType(LEX_R_BRACKET)){
		//fprintf(stderr, "Ocekavana ')' na radku %d\n", gToken.row);
		instruction_exit(SYN_ERR);
		return SYN_ERR;
	}

	// ulozeni poctu formalnich parametru funkce
	gData.paramCount = paramCount;
	global_map_put(gMap, function_name, gData);

	// pro dalsi funkce
	paramCount = 0;



	token = getToken();
	if(!error_lex()){
		instruction_exit(ERROR_LEX);
		return ERROR_LEX;
	} else if (!error_int()){
		instruction_exit(INT_ERR);
		return INT_ERR;
	}

	if(!checkTokenType(LEX_EOL)){
		//fprintf(stderr, "Ocekavan 'eol' na radku %d \n", gToken.row);
		instruction_exit(SYN_ERR);
		return SYN_ERR;
	}


	//dalsi token pude do funce ST-LIST
	token = getToken();
	if(!error_lex()){
		instruction_exit(ERROR_LEX);
		return ERROR_LEX;
	} else if (!error_int()){
		instruction_exit(INT_ERR);
		return INT_ERR;
	}

	result = st_list();
	if(result != SUCCESS){
		return result;
	}

	//dalsi token je nacten, musi = KW_END

	if(!error_lex()){
		instruction_exit(ERROR_LEX);
		return ERROR_LEX;
	} else if (!error_int()){
		instruction_exit(INT_ERR);
		return INT_ERR;
	}

	if(!(checkTokenType(KW_END))){
		//fprintf(stderr,"Ocekavan 'end' na radku %d\n", gToken.row);
		instruction_exit(SYN_ERR);
		return SYN_ERR;
	}

	// konec funkce

	instr_type = INSTRUCT_LABEL;
	instr1.type = EMPTY;

	// vymyslet nejakou promennou pro ukonceni funkce, treba $foo$end
	instr1.value.s = "$end";
	insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

	// return retval of function
	instr_type = INSTRUCT_RETURN;
	insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);
	// end of function, navrat na predchozi pozici


	//uvolneni lokalni mapy z polozky globalni mapy pro funkce

	gData = global_map_get_value(gMap, function_name);
	local_map_free(gData.lMap);


	//nacteni a kontrola dalsiho tokenu
	token = getToken();
	if(!error_lex()){
		instruction_exit(ERROR_LEX);
		return ERROR_LEX;
	} else if (!error_int()){
		instruction_exit(INT_ERR);
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

			//volani st_list()
			result = st_list();
			if(result != SUCCESS){
				return result;
			}

			// //dalsi musi byt EOL / EOF
			token = getToken();

			if(!error_lex()){
				instruction_exit(ERROR_LEX);
				return ERROR_LEX;
			} else if (!error_int()){
				instruction_exit(INT_ERR);
				return INT_ERR;
			}


			return SUCCESS;
		break;


		default:
			if(!error_lex()){
				instruction_exit(ERROR_LEX);
				return ERROR_LEX;
			} else if (!error_int()){
				instruction_exit(INT_ERR);
				return INT_ERR;
			}
			//fprintf(stderr, "Ocekavano zacatek programu na radku %d\n", gToken.row);
			instruction_exit(SYN_ERR);
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
			if(!checkTokenType(LEX_EOL) && !checkTokenType(LEX_EOF)) {
				//fprintf(stderr, "Ocekavano 'eof' 'eol' na radku %d \n", gToken.row);
				instruction_exit(SYN_ERR);
				return SYN_ERR;
			}

    		return SUCCESS;
    	break;

    	default:
    		if(!error_lex()){
    			instruction_exit(ERROR_LEX);
				return ERROR_LEX;
			} else if (!error_int()){
				instruction_exit(INT_ERR);
				return INT_ERR;
			}
			else{
				// prazdny program a je to spravne
				return SUCCESS;
			}
    		
	}
}




int parse(GlobalMap* globalMap, tList *list) {

	int result = SUCCESS;
	gMap = globalMap;

	ilist = list;
	localMap = local_map_init(MAX_SIZE_OF_HASH_TABLE);
	insert_build_in_functions();

	function_name = (char *) malloc( sizeof(char));
	variable_name = (char *) malloc( sizeof(char));
	call_name = (char *)malloc(sizeof(char));


	if(initToken() == INT_ERR){
		//fprintf(stderr, "Nepodařilo se inicializovat strukturu pro token \n");
		result = INT_ERR;
	}

	// inicializace listu pro tokeny
	DLInitList(&tlist);

	while_list = list_init();
	tmp_list = list_init();
	variables_list = list_init();

	do{
		if((token = getToken()) == ERROR_LEX) {
			//fprintf(stderr, "Lexikalni chyba programu\n");
			instruction_exit(ERROR_LEX);
			result = ERROR_LEX;
		} else if (token == INT_ERR) {
			instruction_exit(INT_ERR);
			return INT_ERR;
			//fprintf(stderr, "Interni chyba\n");
		}
	}while(token == LEX_EOL);


	if(result == SUCCESS){
		instr_type = INSTRUCT_HEAD;
		insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);
		// vytvoreni docasneho ramce pro funkce
		instr_type =  INSTRUCT_CREATEFREAME;
		insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);
		// instr_type = INSTRUCT_LENGTH;
		// insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);
		// instr_type = INSTRUCT_CHR;
		// insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);
		// instr_type = INSTRUCT_ORD;
		// insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);
		// instr_type = INSTRUCT_SUBSTR;
		// insert_item(ilist, &instr_type, &instr1, &instr2, &instr3);

		result = prog();
	}


	DLDisposeList(&tlist);
	freeToken();
	local_map_free(localMap);
	return result;

}
