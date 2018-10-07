/**
 * Predmet:  IFJ
 * Projekt:  Implementace prekladace imperativniho jazyka IFJ18
 * Soubor:   prece.h
 *
 * Popis: SA pre výrazy pomocou precedenčnej tabuľky
 *
 *
 * Datum:
 *
 * Autori:   Maros Orsak       vedouci
 *           Polishchuk Kateryna     <xpolis03@fit.vutbr.cz>
 *           Igor Ignac
 *           Marek Rohel

*/
#include "prece.h"
#include "stack.h"

#define RULE_OF_OPERATORS stack_pops(4, stack);stack_push(stack, E, dataIDF);stack_print_prece(stack);break
#define RULE_OF_IDENTIFICATOR stack_pops(2, stack);stack_push(stack, E, *dataIDF);stack_print_prece(stack)


int counterVar = 1;
int DEBUG = 1;  /* premenna na debugovanie  0 --> pre ziadnej vypis, 1 --> pre vypis */

prece_states prece_table [SIZEOFTABLE][SIZEOFTABLE] = {
/*        +    -    *    /    <    >   <=   >=   ==   !=   i    (    )    $         <------- ACT TOKEN */
/* + */ { G ,  G,   L,   L,   G,   G,  G,   G,   G,   G,   L,   L,   G,   G},
/* - */ { G ,  G,   L,   L,   G,   G,  G,   G,   G,   G,   L,   L,   G,   G},
/* * */ { G ,  G,   G,   G,   G,   G,  G,   G,   G,   G,   L,   L,   G,   G},     // TODO: ocekovat na zakladne zadania
/* / */ { G ,  G,   G,   G,   G,   G,  G,   G,   G,   G,   L,   L,   G,   G},
/* < */ { L ,  L,   L,   L,   G,   G,  G,   G,   G,   G,   L,   L,   G,   G},
/* > */ { L ,  L,   L,   L,   G,   G,  G,   G,   G,   G,   L,   L,   G,   G},
/* <=*/ { L ,  L,   L,   L,   G,   G,  G,   G,   G,   G,   L,   L,   G,   G},
/* >=*/ { L ,  L,   L,   L,   G,   G,  G,   G,   G,   G,   L,   L,   G,   G},
/* ==*/ { L ,  L,   L,   L,   L,   L,  L,   L,   G,   G,   L,   L,   G,   G},
/* !=*/ { L ,  L,   L,   L,   L,   L,  L,   L,   G,   G,   L,   L,   G,   G},
/* i */ { G ,  G,   G,   G,   G,   G,  G,   G,   G,   G, Err, Err,   G,   G},
/* ( */ { L ,  L,   L,   L,   L,   L,  L,   L,   L,   L,   L,   L,  EQ, Err},
/* ) */ { G ,  G,   G,   G,   G,   G,  G,   G,   G,   G, Err, Err,   G,   G},
/* $ */ { L ,  L,   L,   L,   L,   L,  L,   L,   L,   L,   L,   L, Err, Err},
};


/* TOTO TREBA URCITE */
//void generateVariable(tString* variable)
//// generuje jedinecne nazvy identifikatoru
//// nazev se sklada ze znaku $ nasledovanym cislem
//// postupne se tu generuji prirozena cisla a do nazvu promenne se ukladaji
//// v reverzovanem poradi - na funkcnost to nema vliv, ale je jednodussi implementace
//
//{
//    strClear(variable);
//    strAdd(variable, '$');
//    int i;
//    i = counterVar;
//    while (i != 0)
//    {
//        strAdd(variable, (char)(i % 10 + '0'));
//        i = i / 10;
//    }
//    counterVar ++;
//}

void setEmptyDataIDF(tDataIDF dataIDF) {
    dataIDF.type = 500;
    dataIDF.defined = false;
    dataIDF.value.nil = true;
}


/* KVOLI INDEXOVANIU NA PRECEDENCNI TABULKU */
int indexerOfPreceTable (int indexer)
{
    int type = indexer;          // vyberieme si co je aktualny token a budeme ho indexovat
    switch (type)
    {
        /* OPERACIE */
        case LEX_ADDITION: type = ePLUS; break;         // +
        case LEX_SUBSTRACTION: type = eMINUS; break;    // -
        case LEX_MULTIPLICATION: type = eMUL; break;    // *
        case LEX_DIVISION: type = eDIV; break;          // /
        case LEX_LESSER: type = eLESS; break;           // <
        case LEX_GREATER: type = eGREAT; break;         // >
        case LEX_LESSER_EQUAL: type = eLEQUAL; break;   // <=
        case LEX_GREATER_EQUAL: type = eGEQUAL; break;  // >=
        case LEX_DOUBLE_EQUAL: type = eEQUAL; break;    // ==
        case LEX_UNEQUAL: type = eNEQUAL; break;        // !=
        case LEX_L_BRACKET: type = eLBAR; break;        // (
        case LEX_R_BRACKET: type = eRBAR; break;        // )
        /* IDENTIFIKATOR STAVY */
        case LEX_ID: type = eIDENT; break;              // id // TODO: sem bude treba skontrolovat ci sa nachadza v lmap -> ak ano tak vyhodit chybu
        case LEX_STRING: type = eIDENT; break;          // ked pride string  premenime ho na index 10 cize identifikator
        case LEX_NUMBER: type = eIDENT; break;          // ked pride cislo -> i
        case LEX_REAL_NUMBER: type= eIDENT; break;      // ked pride cislo -> i
        /* KONIEC PRECE */
        case LEX_EOL: type = eDOLAR; break;             // v pripade ze to bude EOL napriklad bude then tak sa to bude spravat ako $
        case LEX_EOF: type = eDOLAR; break;             // v pripade ze to bude EOF na konci suboru
        case KW_THEN: type = eDOLAR; break;             // v pripade ze to bude then taktiez ukoncuj
        case KW_DO: type = eDOLAR; break;               // v pripade ze pripde DO
        default:
            /* SYNTAKTICKA CHYBA */
            printf("indexerOfPreceTable():Syntactic Error\n");
            return eSYNTERROR;
    }
    return type;
}

/**
 * Funkcia, ktora je primarne urcena na debugovanie, premiena cisla tokenov na znaky aby vypis bol rychlejsie pochopitelny
 * @param token aktualny token
 * @return konvertovany znak
 */
char* convert_to_char(int token){
    switch(token){
        case 0: return "+\0";
        case 1: return "-\0";
        case 2: return "*\0";
        case 3: return "/\0";
        case 4: return "<\0";
        case 5: return ">\0";
        case 6: return "<=\0";
        case 7: return ">=\0";
        case 8: return "==\0";
        case 9: return "!=\0";
        case 10: return "i\0";
        case 11: return "(\0";
        case 12: return ")\0";
        case 13: return "$\0";
        case 15: return " \0";
        case 22: return "[<]\0";
        case 42: return "E\0";
    }
    return "\0";
}

// @varName pravidlo id = <sth>
// @lMap je lokalni Mapa
expr_return parse_expr(LocalMap* lMap, tList* list){
    /* INICIALIZACIA STRUKTUR */
    tItem* tempItemForPositionOne; /* GLOBALNY ITEM pre stack pri pravidle E -> i */
    tItem* tempItemForPositionThree; /* GLOBALNY ITEM pre stack pri pravidlach E -> E + E, E -> E - E a podobne. */
    expr_return resultOfPrece = {.result=SUCCESS, .string="", .bool_result=""};
    tStack* stack = stack_init(12);
    int actTokenIndexToPreceTable = 0;
    int stackTopTokenIndexToPreceTable = 0;

    stack_refresh(stack);                       // vycistenie stacku
    /* ZACIATOK PRECEDENCNEJ ANALYZY */
    stack_push(stack, eDOLAR, dataIDF);         // pushnutie na stack $

    if(DEBUG)stack_print_prece(stack);
    if(DEBUG)stack_print(stack);
    setEmptyDataIDF(dataIDF);                   // nastavenie default hodnot
    do{
        stack->finderOfParenthesis = stack->top; // vyrovnanie top so finderom
        //v pripade ze je na vrchole zasobniku non terminal E pozerame sa o 1 miesto nizsie
        if(stack->arrayOfNumbers[stack->top] == E){
            actTokenIndexToPreceTable = indexerOfPreceTable(token);  // pretypovanie na dany index
            stackTopTokenIndexToPreceTable = stack->arrayOfNumbers[stack->finderOfParenthesis - 1];     // pozerame sa o jedno miesto nizsie
        }
        else{
            actTokenIndexToPreceTable = indexerOfPreceTable(token);  // pretypovanie na dany index
            stackTopTokenIndexToPreceTable  = stack_top_token_number(stack); // pretypovanie na dany index
        }

        // akonahle sa vo fucnkii indexerOfPreceTable nenajde ziadny znak tak vyhadzujem syntaticku chybu
        if(actTokenIndexToPreceTable == eSYNTERROR){
            resultOfPrece.result = SYN_ERR;
            return resultOfPrece;
        }

        // SEMANTICKA AKCE
        // pushnuti do stackofItems strukturu pro token, ktery prave prisel od PARSERU
        // muze byt LEX_ID, LEX_NUMBER, LEX_REAL_NUMBER, LEX_STRING
        if (token == LEX_NUMBER){
            dataIDF.type = INTEGER;
            dataIDF.value.i = atoi(gToken.data.str);
            // generovanie kodu MOVE %s@%s int@%s
        }
        else if (token == LEX_REAL_NUMBER){
            dataIDF.type = FLOAT;
            dataIDF.value.d = atof(gToken.data.str);
            // generovanie kodu "MOVE %s@%s float@%s

        }
        else if (token == LEX_STRING){
            dataIDF.type = STRING;
            dataIDF.value.string.str = gToken.data.str;
            // generovanie kodu  MOVE %s@%s string@%s
        }
        else if(token == LEX_ID){
            // ak sa premenna nachadza v lokalnej mape tak
            if(local_map_contain(lMap, dataIDF.value.string.str)){
                dataIDF = local_map_get_value(lMap, dataIDF.value.string.str);
                // sem sa bude generovat MOVE %s@%s %s@%s
                // generovanie buduceho $1
            }
            else{
                // premenna nebola najdena v localnej mape a tym padom sa jedna o semanticku chybu
                resultOfPrece.result = SEM_ERR;
                return resultOfPrece;
            }
            dataIDF.defined = true;
        }

        if(DEBUG)printf("This is act token    number -> |%d| and char -> |%s|\n", actTokenIndexToPreceTable, convert_to_char(actTokenIndexToPreceTable));
        if(DEBUG) printf("This is act stackTop number -> |%d| and char -> |%s|\n", stackTopTokenIndexToPreceTable, convert_to_char(stackTopTokenIndexToPreceTable));

        switch(prece_table[stackTopTokenIndexToPreceTable][actTokenIndexToPreceTable]){
            case EQ:
                if(DEBUG)printf("CASE: |=| (equal)\n");
                stack->finderOfParenthesis = stack->top;
                stack_search_for_theorem(stack);
                stack_push(stack, actTokenIndexToPreceTable, dataIDF);
                if(DEBUG)stack_print_prece(stack);
                token = getToken();     //zavolanie si noveho tokenu
                break;
            case L:
                if(DEBUG)printf("CASE: |<| (shifting)\n");
                // SEM BUDEME VYHLADAVAT ZA NON - TERMINAL -> pre jednoduchost bez neho...
                if(stack_top_token_number(stack) == E){
                    tempItemForPositionOne = stack_pop(stack);
                    dataIDF = tempItemForPositionOne->token_data;
                    stack_push(stack, eSOLVING_RULE, dataIDF);
                    stack_push(stack, E, dataIDF);
                    stack_push(stack, actTokenIndexToPreceTable, dataIDF);
                    if(DEBUG)stack_print_prece(stack);
                }
                else{
                    stack_push(stack, eSOLVING_RULE, dataIDF);
                    stack_push(stack, actTokenIndexToPreceTable, dataIDF);
                    if(DEBUG)stack_print_prece(stack);
                }
                token = getToken();     //zavolanie si noveho tokenu
                break;
            case G:
                if(DEBUG)printf("CASE: |>| (reduction)\n");
                stack_search_for_theorem(stack);

                             /** PRAVIDLO E -> i **/
                 if((stack->arrayOfNumbers[stack->finderOfParenthesis+1]) == eIDENT){
                    tempItemForPositionOne = stack_pop(stack);
                    stack_pop(stack);
                    stack_push(stack, E, tempItemForPositionOne->token_data);
                    if(DEBUG) printf("Toto su data -> %d\n", tempItemForPositionOne->token_data.value.i);
                    if(DEBUG) printf("StackTop NUMBER -> %s\n", convert_to_char(stack_top_token_number(stack)));
                    if(DEBUG) printf("StackTop DATA -> %d\n", stack_top_token_data(stack)->value.i);
                    if(DEBUG)stack_print_prece(stack);
//                    RULE_OF_IDENTIFICATOR;
                }
                            /** PRAVIDLO E -> (E) **/
                else if(((stack->arrayOfNumbers[stack->finderOfParenthesis+1]) == eLBAR) && ((stack->arrayOfNumbers[stack->finderOfParenthesis + 2]) == E) && ((stack->arrayOfNumbers[stack->finderOfParenthesis+3]) == eRBAR)){
                    stack_pop(stack);                               // popnutie zatvorky )
                    tempItemForPositionOne = stack_pop(stack);      // ulozenie si E
                    stack_pop(stack);                               // popnutie zatvorky (
                    stack_pop(stack);                               // popnutie znamienka <
                    stack_push(stack, E, dataIDF);                 // nakoniec pushneme E + datovu strukturu

                    dataIDF = tempItemForPositionOne->token_data;   // do struktury nahrame adresu token->data
                    break;
//                     RULE_OF_OPERATORS;
                }
                else {
                    int concreteOperator = stack->arrayOfNumbers[stack->finderOfParenthesis + 2];
                    switch(concreteOperator) {
                        // PRAVIDLO E -> E + E
                        case ePLUS:
                            if ((&stack->arrayOfNumbers[stack->finderOfParenthesis + 3]) != NULL) {
                                // generovanie DEFVAR %s@%s
                                if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == STRING) &&
                                    (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == STRING)) {
                                    dataIDF.type = STRING;
                                    // generovnaie CONCAT %s@%s %s@%s %s@%s
                                }
                                else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == STRING) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type != STRING)) {
                                    resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                    return resultOfPrece;
                                }
                                else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type != STRING) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == STRING)) {
                                    resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                    return resultOfPrece;
                                }
                                else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                    dataIDF.type = INTEGER;
                                    // generovanie ADD %s@%s %s@%s %s@%s
                                }
                                else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                    dataIDF.type = FLOAT;
                                    // generovanie  INT2FLOAT %s@%s %s@%s
                                    // generovanie  ADD %s@%s %s@%s %s@%s
                                }
                                else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                    dataIDF.type = FLOAT;
                                    // generovanie INT@FLOAT %s@%s %s@%s
                                    // generovanie ADD %s@%s %s@%s %s@%s
                                }
                                else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                    dataIDF.type = FLOAT;
                                    // generovanie ADD %s@%s %s@%s %s@%s
                                }
                                else {
                                    resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                }
                            }
                            else {
                                resultOfPrece.result = SYN_ERR;
                                return resultOfPrece;
                            }

                            tempItemForPositionOne = stack_pop(stack);      // ulozenie si prveho E
                            stack_pop(stack);                               // popnutie znamienka +
                            tempItemForPositionThree = stack_pop(stack);    // ulozenie si druheho E
                            stack_pop(stack);                               // popnutie znamienak <
                            tempItemForPositionOne->token_data.value.i += tempItemForPositionThree->token_data.value.i; // pricitanie E + E // TOTO JE IBA PRE KONTROLU
                            dataIDF = tempItemForPositionOne->token_data;  // do struktury nahrame adresu token->data
                            stack_push(stack, E, dataIDF);                 // nakoniec pushneme E + datovu strukturu
                            if(DEBUG)
                                stack_print(stack);
                            break;
//                            RULE_OF_OPERATORS;
                        // PRAVIDLO E -> E - E
                        case eMINUS:
                            if ((&stack->arrayOfNumbers[stack->finderOfParenthesis + 3]) != NULL) {
                                // generovanie DEFVAR %s@%s
                                if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == STRING) ||
                                    (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == STRING)) {
                                    resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                    return resultOfPrece;
                                }
                                else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                    dataIDF.type = INTEGER;
                                    // generovanie SUB %s@%s %s@%s %s@%s
                                }
                                else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                       (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                    dataIDF.type = FLOAT;
                                    // generovanie  INT2FLOAT %s@%s %s@%s
                                    // generovnaie  SUB %s@%s %s@%s %s@%s
                                }
                                else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                       (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                    dataIDF.type = FLOAT;
                                    // generovanie  INT2FLOAT %s@%s %s@%s
                                    // generovnaie  SUB %s@%s %s@%s %s@%s
                                }
                                else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                       (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                    dataIDF.type = FLOAT;
                                    // generovanie  INT2FLOAT %s@%s %s@%s
                                    // generovanie  SUB %s@%s %s@%s %s@%s
                                }
                                else {
                                    resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                }
                            }
                            else {
                                resultOfPrece.result = SYN_ERR;
                                return resultOfPrece;
                            }
                            tempItemForPositionOne = stack_pop(stack);      // ulozenie si prveho E
                            stack_pop(stack);                               // popnutie znamienka -
                            tempItemForPositionThree = stack_pop(stack);    // ulozenie si druheho E
                            stack_pop(stack);                               // popnutie znamienak <
                            tempItemForPositionOne->token_data.value.i -= tempItemForPositionThree->token_data.value.i; // odcitanie E - E // TOTO JE IBA PRE KONTROLU
                            dataIDF = tempItemForPositionOne->token_data;  // do struktury nahrame adresu token->data
                            stack_push(stack, E, dataIDF);                 // nakoniec pushneme E + datovu strukturu
                            if(DEBUG)
                                stack_print(stack);
                            break;
                        // PRAVIDLO E -> E * E
                        case eMUL:
                            if (&stack->arrayOfItems[stack->finderOfParenthesis + 3] != NULL) {
                                // generovanie DEFVAR %s@%s
                                if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == STRING) ||
                                    (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == STRING)) {
                                    resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                    return resultOfPrece;
                                }
                                else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                    dataIDF.type = INTEGER;
                                    //generovanie MUL %s@%s %s@%s %s@%s
                                }
                                else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                    dataIDF.type = FLOAT;
                                    // generovanie INT2FLOAT %s@%s %s@%s
                                    //generovanie MUL %s@%s %s@%s %s@%s
                                }
                                else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                    dataIDF.type = FLOAT;
                                    //generovanie INT2FLOAT %s@%s %s@%
                                    //generovanie MUL %s@%s %s@%s %s@%s
                                }
                                else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                    dataIDF.type = FLOAT;
                                    //generovanie MUL %s@%s %s@%s %s@%s
                                }
                                else {   //printf("semnticky eror BOOL \n");
                                    resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                }

                            } else {
                                // printf("ERROR dal si zle tokeny\n");
                                resultOfPrece.result = SYN_ERR;
                                return resultOfPrece;
                            }
                            tempItemForPositionOne = stack_pop(stack);      // ulozenie si prveho E
                            stack_pop(stack);                               // popnutie znamienka -
                            tempItemForPositionThree = stack_pop(stack);    // ulozenie si druheho E
                            stack_pop(stack);                               // popnutie znamienak <
                            tempItemForPositionOne->token_data.value.i *= tempItemForPositionThree->token_data.value.i; // odcitanie E 8 E // TOTO JE IBA PRE KONTROLU
                            dataIDF = tempItemForPositionOne->token_data;  // do struktury nahrame adresu token->data
                            stack_push(stack, E, dataIDF);                 // nakoniec pushneme E + datovu strukturu
                            if(DEBUG)
                                stack_print(stack);
                            break;
                        // PRAVIDLO E -> E / E
                        case eDIV:
                            if (&stack->arrayOfItems[stack->finderOfParenthesis + 3] != NULL) {
                                // generovanie DEFVAR %s@%s
                                if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == STRING) ||
                                    (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == STRING)) {
                                    resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                    return resultOfPrece;
                                }
                                else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                    dataIDF.type = FLOAT;
                                    //generovanie DIV %s@%s %s@%s %s@%s
                                }
                                else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                    dataIDF.type = FLOAT;
                                    // generovanie DIV %s@%s %s@%s %s@%s
                                }
                                else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                    dataIDF.type = FLOAT;
                                    // generovanie  DIV %s@%s %s@%s %s@%s
                                }
                                else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                    dataIDF.type = FLOAT;
                                    // generovanie  DIV %s@%s %s@%s %s@%s
                                }
                                else {   //printf("semanticky error BOOL \n");
                                    resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                }
                            } else {
                                // error dal si zle tokeny
                                resultOfPrece.result = SYN_ERR;
                                return resultOfPrece;
                            }
                            tempItemForPositionOne = stack_pop(stack);      // ulozenie si prveho E
                            stack_pop(stack);                               // popnutie znamienka -
                            tempItemForPositionThree = stack_pop(stack);    // ulozenie si druheho E
                            stack_pop(stack);                               // popnutie znamienak <
                            tempItemForPositionOne->token_data.value.i *= tempItemForPositionThree->token_data.value.i; // odcitanie E 8 E // TOTO JE IBA PRE KONTROLU
                            dataIDF = tempItemForPositionOne->token_data;  // do struktury nahrame adresu token->data
                            stack_push(stack, E, dataIDF);                 // nakoniec pushneme E + datovu strukturu
                            if(DEBUG)
                                stack_print(stack);
                            break;
                        // PRAVIDLO E -> E < E
                        case eLESS:
                            RULE_OF_OPERATORS;
                        // PRAVIDLO E -> E > E
                        case eGREAT:
                            RULE_OF_OPERATORS;
                        // PRAVIDLO E -> E <= E
                        case eLEQUAL: // <=
                            RULE_OF_OPERATORS;
                        // PRAVIDLO E -> E >= E
                        case eGEQUAL: // >=
                            RULE_OF_OPERATORS;
                        // PRAVIDLO E -> E == E
                        case eEQUAL: // ==
                            RULE_OF_OPERATORS;
                        // PRAVIDLO E -> E != E
                        case eNEQUAL: // !=
                            RULE_OF_OPERATORS;
                    }
                }
                break;
                // TODO: dokoncit pravidlo, a urobit funciu stack_print_preceeNonTermn
            case Err:
                if(actTokenIndexToPreceTable == eDOLAR){
                    if(DEBUG)printf("This is stackTop %s\n", convert_to_char(stack_top_token_number(stack)));
                    /* v pripade ak prijde vyraz a = then  v preklade $$ medzi nimi nebude nic ziaden nontermian(E) tak to vychodi syntakticku chybu */
                    if(stack->top == 0){
                        resultOfPrece.result = SYN_ERR;
                        return resultOfPrece;
                    }
                     if(DEBUG)printf("STATE: $E$ -> EVERYTHING OK\n");
                    // uvolnenie stacku
                    stack_free(stack);
                     if(DEBUG)printf("Vysledok operacie pre INT %d\n", dataIDF.value.i);

                    resultOfPrece.result = SUCCESS;
                    if(DEBUG)printf("Return exiting value -> |%d|, mapkey -> |%s||\n", resultOfPrece.result, resultOfPrece.string);
                    return resultOfPrece;
                }
                if(DEBUG)printf("Error\n");
                return resultOfPrece;
        }
    }while(1);
}
