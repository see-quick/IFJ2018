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
#include "string.h"
//#include "parser.c" // zobratie globalnej mapy

#define RULE_OF_OPERATORS stack_pops(4, stack);stack_push(stack, E, dataIDF);stack_print_prece(stack);break
#define RULE_OF_IDENTIFICATOR stack_pops(2, stack);stack_push(stack, E, *dataIDF);stack_print_prece(stack)

tInstructionTypes instr_type;
tInstructionData instr1;
tInstructionData instr2;
tInstructionData instr3;


int counterVar = 1;
int DEBUG = 1;  /* premenna na debugovanie  0 --> pre ziadnej vypis, 1 --> pre vypis */

prece_states prece_table [SIZEOFTABLE][SIZEOFTABLE] = {
/*        +    -    *    /    <    >   <=   >=   ==   !=   i    (    )    $   ,   f          <------- ACT TOKEN */
/* + */ { G ,  G,   L,   L,   G,   G,  G,   G,   G,   G,   L,   L,   G,   G,  G,  L},
/* - */ { G ,  G,   L,   L,   G,   G,  G,   G,   G,   G,   L,   L,   G,   G,  G,  L},
/* * */ { G ,  G,   G,   G,   G,   G,  G,   G,   G,   G,   L,   L,   G,   G,  G,  L},     // TODO: ocekovat na zakladne zadania
/* / */ { G ,  G,   G,   G,   G,   G,  G,   G,   G,   G,   L,   L,   G,   G,  G,  L},
/* < */ { L ,  L,   L,   L,   G,   G,  G,   G,   G,   G,   L,   L,   G,   G,  G,  L},
/* > */ { L ,  L,   L,   L,   G,   G,  G,   G,   G,   G,   L,   L,   G,   G,  G,  L},
/* <=*/ { L ,  L,   L,   L,   G,   G,  G,   G,   G,   G,   L,   L,   G,   G,  G,  L},
/* >=*/ { L ,  L,   L,   L,   G,   G,  G,   G,   G,   G,   L,   L,   G,   G,  G,  L},
/* ==*/ { L ,  L,   L,   L,   L,   L,  L,   L,   G,   G,   L,   L,   G,   G,  G,  L},
/* !=*/ { L ,  L,   L,   L,   L,   L,  L,   L,   G,   G,   L,   L,   G,   G,  G,  L},
/* i */ { G ,  G,   G,   G,   G,   G,  G,   G,   G,   G, Err, Err,   G,   G,  G,Err},
/* ( */ { L ,  L,   L,   L,   L,   L,  L,   L,   L,   L,   L,   L,  EQ, Err, EQ,  L},
/* ) */ { G ,  G,   G,   G,   G,   G,  G,   G,   G,   G, Err, Err,   G,   G,  G,Err},
/* $ */ { L ,  L,   L,   L,   L,   L,  L,   L,   L,   L,   L,   L, Err, Err, Err, L},
/* , */ { L  , L  , L ,  L ,  L ,  L  ,L  , L  , L  , L  , L ,  L  , EQ , L ,EQ,  L},
/* f */ {Err, Err, Err, Err, Err, Err,Err,Err, Err , Err , Err, EQ, Err, Err,Err,Err},
};

tString* generateVariable(tString* variable){
// generuje jedinecne nazvy identifikatoru
// nazev se sklada ze znaku $ nasledovanym cislem
// postupne se tu generuji prirozena cisla a do nazvu promenne se ukladaji
// v reverzovanem poradi - na funkcnost to nema vliv, ale je jednodussi implementace
    int i = counterVar;
    strClear(variable);
    strAdd(variable, '$');
    while (i != 0) {
        strAdd(variable, (char)(i + '0'));
        i = i / 10;
    }
    counterVar++;

    return variable;
}

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
        case LEX_ID: type = eIDENT;
//            if(global_map_contain(gMap, gToken.data.str)){
////
////            }


            break;              // id // TODO: sem bude treba skontrolovat ci sa nachadza v lmap -> ak ano tak vyhodit chybu

        case LEX_STRING: type = eIDENT; break;          // ked pride string  premenime ho na index 10 cize identifikator
        case LEX_NUMBER: type = eIDENT; break;          // ked pride cislo -> i
        case LEX_REAL_NUMBER: type= eIDENT; break;      // ked pride cislo -> i
        /* KONIEC PRECE */
        case LEX_EOL: type = eDOLAR; break;             // v pripade ze to bude EOL napriklad bude then tak sa to bude spravat ako $
        case LEX_EOF: type = eDOLAR; break;             // v pripade ze to bude EOF na konci suboru
        case KW_THEN: type = eDOLAR; break;             // v pripade ze to bude then taktiez ukoncuj
        case KW_DO: type = eDOLAR; break;               // v pripade ze pripde DO
        /* COMMA a FCE */
        case LEX_COMMA: type = eCOMMA; break;       // comma ,
        case KW_DEF: type = eFCE; break;            // funkcie
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
    expr_return resultOfPrece = {.result=SUCCESS, .bool_result=""};
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
        if(actTokenIndexToPreceTable == eSYNTERROR) {
            resultOfPrece.result = SYN_ERR;
            return resultOfPrece;
        }
        printf("This is token -> %s\n", gToken.data.str);

        // SEMANTICKA AKCE
        // pushnuti do stackofItems strukturu pro token, ktery prave prisel od PARSERU
        // muze byt LEX_ID, LEX_NUMBER, LEX_REAL_NUMBER, LEX_STRING
        instr_type = INSTRUCT_MOVE;

        if (token == LEX_NUMBER){
            dataIDF.type = INTEGER;
            dataIDF.value.i = atoi(gToken.data.str);
            resultOfPrece.uniqueID = &gToken.data;
            instr2.type = I;
            instr2.value.i = dataIDF.value.i;
            // generovanie kodu MOVE %s@%s int@%s
        }
        else if (token == LEX_REAL_NUMBER){
            dataIDF.type = FLOAT;
            dataIDF.value.f = atof(gToken.data.str);
            resultOfPrece.uniqueID = &gToken.data;
            instr2.type = F;
            instr2.value.f = dataIDF.value.f;
            // generovanie kodu "MOVE %s@%s float@%s

        }
        else if (token == LEX_STRING){
            dataIDF.type = STRING;
            dataIDF.value.string.str = gToken.data.str;
            resultOfPrece.uniqueID = &gToken.data;
            instr2.type = S;
            instr2.value.s = gToken.data.str;
            // generovanie kodu  MOVE %s@%s string@%s
        }
        else if(token == LEX_ID){
            // ak sa premenna nachadza v lokalnej mape tak
            if(local_map_contain(lMap, dataIDF.value.string.str)){
                dataIDF = local_map_get_value(lMap, dataIDF.value.string.str);
                resultOfPrece.uniqueID = &gToken.data;
                switch (dataIDF.type) {
                    case INTEGER:
                        instr2.type = I;
                        insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                        break;
                    case FLOAT:
                        instr2.type = F;
                        insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                        break;
                    case STRING:
                        instr2.type = S;
                        insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                        break;
                }
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

        stack_print(stack);
        stack_print_prece(stack);
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
                    // generovanie non termu
                    tempItemForPositionOne = stack_pop(stack);
                    tempItemForPositionOne->token_data.nameOfTheNonTerminal = generateVariable(resultOfPrece.uniqueID)->str;        // generovanie UNIQUE
                    stack_pop(stack);
                    stack_push(stack, E, tempItemForPositionOne->token_data);
                    if (DEBUG) printf("Toto su data -> %d\n", tempItemForPositionOne->token_data.value.i);
                    if (DEBUG) printf("StackTop NUMBER -> %s\n", convert_to_char(stack_top_token_number(stack)));
                    if (DEBUG) printf("StackTop DATA -> %d\n", stack_top_token_data(stack)->value.i);
                    if (DEBUG)stack_print_prece(stack);
                    break;
//                    RULE_OF_IDENTIFICATOR;
                }
                            /** PRAVIDLO E -> (E) **/
                else if(((stack->arrayOfNumbers[stack->finderOfParenthesis+1]) == eLBAR) && ((stack->arrayOfNumbers[stack->finderOfParenthesis + 2]) == E) && ((stack->arrayOfNumbers[stack->finderOfParenthesis+3]) == eRBAR)){
                    stack_pop_free(stack);                               // popnutie zatvorky )
                    tempItemForPositionOne = stack_pop(stack);           // ulozenie si E
                    stack_pop_free(stack);                               // popnutie zatvorky (
                    stack_pop_free(stack);                               // popnutie znamienka <
                    stack_push(stack, E, dataIDF);                       // nakoniec pushneme E + datovu strukturu
                    dataIDF = tempItemForPositionOne->token_data;   // do struktury nahrame adresu token->data
                    break;
                }
                        /** PRAVIDLOO E -> f()  **/
                else if(((stack->arrayOfNumbers[stack->finderOfParenthesis+1]) == eFCE) && ((stack->arrayOfNumbers[stack->finderOfParenthesis + 2]) == eLBAR) && ((stack->arrayOfNumbers[stack->finderOfParenthesis+3]) == eRBAR)){
                     stack_pop_free(stack);                               // popnutie zatvorky )
                     stack_pop_free(stack);                               // popnutie zatvorky (
                     tempItemForPositionOne = stack_pop(stack);           // popnutie ale ziskanie si f
                     // nachadza sa v globalnej tabulke
                     // treba skontroloval pocet parametrov == 0, ak nebude vyhodit semanticku, + ziskat si return typ pre buduci $1
                     stack_pop_free(stack);                               // popnutie znamienka <
                     stack_push(stack, E, dataIDF);                       // nakoniec pushneme E + datovu strukturu
                }
                else {
                     int concreteOperator = stack->arrayOfNumbers[stack->finderOfParenthesis + 2];
                     switch (concreteOperator) {
                         // PRAVIDLO E -> E + E
                         case ePLUS:
                             if ((&stack->arrayOfNumbers[stack->finderOfParenthesis + 3]) != NULL) {
                                 instr_type = INSTRUCT_DEFVAR;
                                 // generovanie DEFVAR %s@%s
                                 if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == STRING) &&
                                     (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == STRING)) {
                                     dataIDF.type = STRING;
                                     instr_type = INSTRUCT_CONCAT;
                                     // generovnaie CONCAT %s@%s %s@%s %s@%s
                                 } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == STRING) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type != STRING)) {
                                     resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                     return resultOfPrece;
                                 } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type != STRING) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == STRING)) {
                                     resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                     return resultOfPrece;
                                 } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                     dataIDF.type = INTEGER;
                                     instr_type = INSTRUCT_ADD;
                                     // generovanie ADD %s@%s %s@%s %s@%s
                                 } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                     dataIDF.type = FLOAT;
                                     instr_type = INSTRUCT_INT2FLOAT;
                                     instr_type = INSTRUCT_ADD;
                                     // generovanie  INT2FLOAT %s@%s %s@%s
                                     // generovanie  ADD %s@%s %s@%s %s@%s
                                 } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                     dataIDF.type = FLOAT;
                                     instr_type = INSTRUCT_INT2FLOAT;
                                     instr_type = INSTRUCT_ADD;
                                     // generovanie INT@FLOAT %s@%s %s@%s
                                     // generovanie ADD %s@%s %s@%s %s@%s
                                 } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                     dataIDF.type = FLOAT;
                                     instr_type = INSTRUCT_ADD;
                                     // generovanie ADD %s@%s %s@%s %s@%s
                                 } else {
                                     resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                 }
                             } else {
                                 resultOfPrece.result = SYN_ERR;
                                 return resultOfPrece;
                             }

                             tempItemForPositionOne = stack_pop(stack);      // ulozenie si prveho E
                             stack_pop_free(stack);                               // popnutie znamienka +
                             tempItemForPositionThree = stack_pop(stack);    // ulozenie si druheho E
                             stack_pop_free(stack);                               // popnutie znamienak <
                             tempItemForPositionOne->token_data.value.i += tempItemForPositionThree->token_data.value.i; //  E + E // TOTO JE IBA PRE KONTROLU
                             dataIDF = tempItemForPositionOne->token_data;  // do struktury nahrame adresu token->data
                             stack_push(stack, E, dataIDF);                 // nakoniec pushneme E + datovu strukturu
                             if (DEBUG)
                                 stack_print(stack);
                             break;

                             // PRAVIDLO E -> E - E
                         case eMINUS:
                             if ((&stack->arrayOfNumbers[stack->finderOfParenthesis + 3]) != NULL) {
                                 // generovanie DEFVAR %s@%s
                                 instr_type = INSTRUCT_DEFVAR;
                                 if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == STRING) ||
                                     (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == STRING)) {
                                     resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                     return resultOfPrece;
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                     dataIDF.type = INTEGER;
                                     instr_type = INSTRUCT_SUB;
                                     // generovanie SUB %s@%s %s@%s %s@%s
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                     dataIDF.type = FLOAT;
                                     instr_type = INSTRUCT_INT2FLOAT;
                                     instr_type = INSTRUCT_SUB;
                                     // generovanie  INT2FLOAT %s@%s %s@%s
                                     // generovnaie  SUB %s@%s %s@%s %s@%s
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                     dataIDF.type = FLOAT;
                                     instr_type = INSTRUCT_INT2FLOAT;
                                     instr_type = INSTRUCT_SUB;
                                     // generovanie  INT2FLOAT %s@%s %s@%s
                                     // generovnaie  SUB %s@%s %s@%s %s@%s
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                     dataIDF.type = FLOAT;
                                     instr_type = INSTRUCT_SUB;
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
                             tempItemForPositionOne->token_data.value.i -= tempItemForPositionThree->token_data.value.i; //  E - E // TOTO JE IBA PRE KONTROLU
                             dataIDF = tempItemForPositionOne->token_data;  // do struktury nahrame adresu token->data
                             stack_push(stack, E, dataIDF);                 // nakoniec pushneme E + datovu strukturu
                             if (DEBUG)
                                 stack_print(stack);
                             break;
                             // PRAVIDLO E -> E * E
                         case eMUL:
                             if (&stack->arrayOfItems[stack->finderOfParenthesis + 3] != NULL) {
                                 // generovanie DEFVAR %s@%s
                                 instr_type = INSTRUCT_DEFVAR;
                                 if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == STRING) ||
                                     (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == STRING)) {
                                     resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                     return resultOfPrece;
                                 } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                     dataIDF.type = INTEGER;
                                     instr_type = INSTRUCT_MUL;
                                     //generovanie MUL %s@%s %s@%s %s@%s
                                 } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                     dataIDF.type = FLOAT;
                                     instr_type = INSTRUCT_INT2FLOAT;
                                     instr_type = INSTRUCT_MUL;
                                     // generovanie INT2FLOAT %s@%s %s@%s
                                     //generovanie MUL %s@%s %s@%s %s@%s
                                 } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                     dataIDF.type = FLOAT;
                                     instr_type = INSTRUCT_INT2FLOAT;
                                     instr_type = INSTRUCT_MUL;
                                     //generovanie INT2FLOAT %s@%s %s@%
                                     //generovanie MUL %s@%s %s@%s %s@%s

                                 } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                     dataIDF.type = FLOAT;
                                     instr_type = INSTRUCT_MUL;
                                     //generovanie MUL %s@%s %s@%s %s@%s
                                 } else {
                                     resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                 }

                             } else {
                                 resultOfPrece.result = SYN_ERR;
                                 return resultOfPrece;
                             }
                             tempItemForPositionOne = stack_pop(stack);      // ulozenie si prveho E
                             stack_pop(stack);                               // popnutie znamienka -
                             tempItemForPositionThree = stack_pop(stack);    // ulozenie si druheho E
                             stack_pop(stack);                               // popnutie znamienak <
                             tempItemForPositionOne->token_data.value.i *= tempItemForPositionThree->token_data.value.i; //  E 8 E // TOTO JE IBA PRE KONTROLU
                             dataIDF = tempItemForPositionOne->token_data;  // do struktury nahrame adresu token->data
                             stack_push(stack, E, dataIDF);                 // nakoniec pushneme E + datovu strukturu
                             if (DEBUG)
                                 stack_print(stack);
                             break;
                             // PRAVIDLO E -> E / E
                         case eDIV:
                             if (&stack->arrayOfItems[stack->finderOfParenthesis + 3] != NULL) {
                                 // generovanie DEFVAR %s@%s
                                 instr_type = INSTRUCT_DEFVAR;
                                 if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == STRING) ||
                                     (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == STRING)) {
                                     resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                     return resultOfPrece;
                                 } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                     dataIDF.type = FLOAT;
                                     instr_type = INSTRUCT_DIV;
                                     //generovanie DIV %s@%s %s@%s %s@%s
                                 } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                     dataIDF.type = FLOAT;
                                     instr_type = INSTRUCT_DIV;
                                     // generovanie DIV %s@%s %s@%s %s@%s
                                 } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                     dataIDF.type = FLOAT;
                                     instr_type = INSTRUCT_DIV;
                                     // generovanie  DIV %s@%s %s@%s %s@%s
                                 } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                     dataIDF.type = FLOAT;
                                     instr_type = INSTRUCT_DIV;
                                     // generovanie  DIV %s@%s %s@%s %s@%s
                                 } else {   //printf("semanticky error BOOL \n");
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
                             tempItemForPositionOne->token_data.value.i /= tempItemForPositionThree->token_data.value.i; //  E / E // TOTO JE IBA PRE KONTROLU
                             dataIDF = tempItemForPositionOne->token_data;  // do struktury nahrame adresu token->data
                             stack_push(stack, E, dataIDF);                 // nakoniec pushneme E + datovu strukturu
                             if (DEBUG)
                                 stack_print(stack);
                             break;
                             // PRAVIDLO E -> E < E
                         case eLESS:
                             if (&stack->arrayOfItems[stack->finderOfParenthesis + 3] != NULL) {
                                 // generovanie DEFVAR %s@%s
                                 instr_type = INSTRUCT_DEFVAR;
                                 if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == STRING) &&
                                     (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == STRING)) {
                                     int result = strCompare(&stack->arrayOfItems[stack->finderOfParenthesis + 1].value.string, &stack->arrayOfItems[stack->finderOfParenthesis + 3].value.string);
                                     if(result < 0){
                                         resultOfPrece.bool_result = 1; // true
                                     }
                                     resultOfPrece.bool_result = 0; // false
                                     dataIDF.type = BOOLEAN;
                                     instr_type = INSTRUCT_LT;
                                     // generovanie LT %s@%s %s@%s %s@%s
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                          (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                     dataIDF.type = BOOLEAN;
                                     resultOfPrece.bool_result = 1; // true
                                     instr_type = INSTRUCT_LT;
                                     // generovanie LT %s@%s %s@%s %s@%s
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                          (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                     dataIDF.type = BOOLEAN;
                                     resultOfPrece.bool_result = 1; // true
                                     instr_type = INSTRUCT_FLOAT2INT; // asi najskor previest na integer a potom porovanat
                                     instr_type = INSTRUCT_LT;
                                     // generovanie LT %s@%s %s@%s %s@%s
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                          (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                     dataIDF.type = BOOLEAN;
                                     resultOfPrece.bool_result = 1; // true
                                     instr_type = INSTRUCT_FLOAT2INT; // asi najskor previest na integer a potom porovanat
                                     instr_type = INSTRUCT_LT;
                                     // generovanie LT %s@%s %s@%s %s@%s
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                          (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                     dataIDF.type = BOOLEAN;
                                     resultOfPrece.bool_result = 1; // true
                                     instr_type = INSTRUCT_LT;
                                     // generovanie LT %s@%s %s@%s %s@%s
                                 }
                                 else {
                                     resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                     return resultOfPrece;
                                 }
                             }
                             else {
                                 resultOfPrece.result = SYN_ERR;
                                 return resultOfPrece;
                             }
                             tempItemForPositionOne = stack_pop(stack);      // ulozenie si prveho E
                             stack_pop(stack);                               // popnutie znamienka >=
                             tempItemForPositionThree = stack_pop(stack);    // ulozenie si druheho E
                             stack_pop(stack);                               // popnutie znamienak <
//                            tempItemForPositionOne->token_data.value.i != tempItemForPositionThree->token_data.value.i; //  E >= E // TOTO JE IBA PRE KONTROLU
                            dataIDF = tempItemForPositionOne->token_data;  // do struktury nahrame adresu token->data
                            stack_push(stack, E, dataIDF);                 // nakoniec pushneme E + datovu strukturu
                            if (DEBUG)
                                stack_print(stack);
                            break;
                            // PRAVIDLO E -> E > E
                        case eGREAT:

                             if (&stack->arrayOfItems[stack->finderOfParenthesis + 3] != NULL) {
                                 // generovanie DEFVAR %s@%s
                                 instr_type = INSTRUCT_DEFVAR;
                                 if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == STRING) &&
                                     (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == STRING)) {
                                     int result = strCompare(&stack->arrayOfItems[stack->finderOfParenthesis + 1].value.string, &stack->arrayOfItems[stack->finderOfParenthesis + 3].value.string);
                                     if(result > 0){
                                         resultOfPrece.bool_result = 1; // true
                                     }
                                     resultOfPrece.bool_result = 0; // false
                                     dataIDF.type = BOOLEAN;
                                     // generovanie GT %s@%s %s@%s %s@%s
                                     instr_type = INSTRUCT_GT;
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                          (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                     dataIDF.type = BOOLEAN;
                                     resultOfPrece.bool_result = 1; // true
                                     // generovanie GT %s@%s %s@%s %s@%s
                                     instr_type = INSTRUCT_GT;
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                          (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                     dataIDF.type = BOOLEAN;
                                     resultOfPrece.bool_result = 1; // true
                                     // generovanie GT %s@%s %s@%s %s@%s
                                     instr_type = INSTRUCT_FLOAT2INT;
                                     instr_type = INSTRUCT_GT;
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                          (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                     dataIDF.type = BOOLEAN;
                                     resultOfPrece.bool_result = 1; // true
                                     // generovanie GT %s@%s %s@%s %s@%s
                                     instr_type = INSTRUCT_FLOAT2INT;
                                     instr_type = INSTRUCT_GT;
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                          (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                     dataIDF.type = BOOLEAN;
                                     resultOfPrece.bool_result = 1; // true
                                     // generovanie GT %s@%s %s@%s %s@%s
                                     instr_type = INSTRUCT_GT;
                                 }
                                 else {
                                     resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                     return resultOfPrece;
                                 }
                             }
                             else {
                                 resultOfPrece.result = SYN_ERR;
                                 return resultOfPrece;
                             }
                             tempItemForPositionOne = stack_pop(stack);      // ulozenie si prveho E
                             stack_pop(stack);                               // popnutie znamienka >=
                             tempItemForPositionThree = stack_pop(stack);    // ulozenie si druheho E
                             stack_pop(stack);                               // popnutie znamienak <
//                            tempItemForPositionOne->token_data.value.i != tempItemForPositionThree->token_data.value.i; //  E > E // TOTO JE IBA PRE KONTROLU
                             dataIDF = tempItemForPositionOne->token_data;  // do struktury nahrame adresu token->data
                             stack_push(stack, E, dataIDF);                 // nakoniec pushneme E + datovu strukturu
                             if (DEBUG)
                                 stack_print(stack);
                             break;
                         RULE_OF_OPERATORS;
                             // PRAVIDLO E -> E <= E
                         case eLEQUAL: // <=
                             if (&stack->arrayOfItems[stack->finderOfParenthesis + 3] != NULL) {
                                 // generovanie DEFVAR %s@%s
                                 instr_type = INSTRUCT_DEFVAR;
                                 if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == STRING) &&
                                     (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == STRING)) {
                                     resultOfPrece.bool_result = 1; // true
                                     dataIDF.type = BOOLEAN;
                                     // generovanie LTS %s@%s %s@%s %s@%s
                                     instr_type = INSTRUCT_LTS;
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                          (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                     resultOfPrece.bool_result = 1; // true
                                     dataIDF.type = BOOLEAN;
                                     // generovanie LTS %s@%s %s@%s %s@%s
                                     instr_type = INSTRUCT_LTS;
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                          (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                     resultOfPrece.bool_result = 1; // true
                                     dataIDF.type = BOOLEAN;
                                     // generovanie LTS %s@%s %s@%s %s@%s
                                     instr_type = INSTRUCT_FLOAT2INT;
                                     instr_type = INSTRUCT_LTS;
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                          (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                     resultOfPrece.bool_result = 1; // true
                                     dataIDF.type = BOOLEAN;
                                     // generovanie LTS %s@%s %s@%s %s@%s
                                     instr_type = INSTRUCT_FLOAT2INT;
                                     instr_type = INSTRUCT_LTS;
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                          (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                     resultOfPrece.bool_result = 1; // true
                                     dataIDF.type = BOOLEAN;
                                     // generovanie LTS %s@%s %s@%s %s@%s
                                     instr_type = INSTRUCT_LTS;
                                 }
                                 else {
                                     resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                     return resultOfPrece;
                                 }
                             }
                             else {
                                 resultOfPrece.result = SYN_ERR;
                                 return resultOfPrece;
                             }
                             tempItemForPositionOne = stack_pop(stack);      // ulozenie si prveho E
                             stack_pop(stack);                               // popnutie znamienka >=
                             tempItemForPositionThree = stack_pop(stack);    // ulozenie si druheho E
                             stack_pop(stack);                               // popnutie znamienak <
//                            tempItemForPositionOne->token_data.value.i != tempItemForPositionThree->token_data.value.i; //  E >= E // TOTO JE IBA PRE KONTROLU
                             dataIDF = tempItemForPositionOne->token_data;  // do struktury nahrame adresu token->data
                             stack_push(stack, E, dataIDF);                 // nakoniec pushneme E + datovu strukturu
                             if (DEBUG)
                                 stack_print(stack);
                             break;
                             // PRAVIDLO E -> E >= E
                         case eGEQUAL: // >=
                             // generovanie DEFVAR %s@%s
                             instr_type = INSTRUCT_DEFVAR;
                             if (&stack->arrayOfItems[stack->finderOfParenthesis + 3] != NULL) {
                                 if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == STRING) &&
                                     (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == STRING)) {
                                     resultOfPrece.bool_result = 1; // true
                                     dataIDF.type = BOOLEAN;
                                     // generovanie GTS %s@%s %s@%s %s@%s
                                     instr_type = INSTRUCT_GTS;
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                     resultOfPrece.bool_result = 1; // true
                                     dataIDF.type = BOOLEAN;
                                     // generovanie GTS %s@%s %s@%s %s@%s
                                     instr_type = INSTRUCT_GTS;
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                     resultOfPrece.bool_result = 1; // true
                                     dataIDF.type = BOOLEAN;
                                     // generovanie GTS %s@%s %s@%s %s@%s
                                     instr_type = INSTRUCT_FLOAT2INT;
                                     instr_type = INSTRUCT_GTS;
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                     resultOfPrece.bool_result = 1; // true
                                     dataIDF.type = BOOLEAN;
                                     // generovanie GTS %s@%s %s@%s %s@%s
                                     instr_type = INSTRUCT_FLOAT2INT;
                                     instr_type = INSTRUCT_GTS;
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                     resultOfPrece.bool_result = 1; // true
                                     dataIDF.type = BOOLEAN;
                                     // generovanie GTS %s@%s %s@%s %s@%s
                                     instr_type = INSTRUCT_GTS;
                                 }
                                 else {
                                     resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                     return resultOfPrece;
                                 }
                             }
                             else {
                                 resultOfPrece.result = SYN_ERR;
                                 return resultOfPrece;
                             }
                             tempItemForPositionOne = stack_pop(stack);      // ulozenie si prveho E
                             stack_pop(stack);                               // popnutie znamienka >=
                             tempItemForPositionThree = stack_pop(stack);    // ulozenie si druheho E
                             stack_pop(stack);                               // popnutie znamienak <
//                            tempItemForPositionOne->token_data.value.i != tempItemForPositionThree->token_data.value.i; //  E >= E // TOTO JE IBA PRE KONTROLU
                             dataIDF = tempItemForPositionOne->token_data;  // do struktury nahrame adresu token->data
                             stack_push(stack, E, dataIDF);                 // nakoniec pushneme E + datovu strukturu
                             if (DEBUG)
                                 stack_print(stack);
                             break;
                             // PRAVIDLO E -> E == E
                         case eEQUAL: // ==
                             // generovanie DEFVAR %s@%s
                             instr_type = INSTRUCT_DEFVAR;
                             if (&stack->arrayOfItems[stack->finderOfParenthesis + 3] != NULL) {
                                 if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == STRING) &&
                                     (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == STRING)) {
                                     int result = strCompare(&stack->arrayOfItems[stack->finderOfParenthesis + 1].value.string, &stack->arrayOfItems[stack->finderOfParenthesis + 3].value.string);
                                     if(result < 0){
                                         resultOfPrece.bool_result = 1; // true
                                     }
                                     resultOfPrece.bool_result = 0; // false
                                     dataIDF.type = BOOLEAN;
                                     // generovanie EQ %s@%s %s@%s %s@%s
                                     instr_type = INSTRUCT_EQ;
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                     dataIDF.type = BOOLEAN;
                                     resultOfPrece.bool_result = 1; // true
                                     // generovanie EQ %s@%s %s@%s %s@%s
                                     instr_type = INSTRUCT_EQ;
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                     dataIDF.type = BOOLEAN;
                                     resultOfPrece.bool_result = 1; // true
                                     // generovanie EQ %s@%s %s@%s %s@%s
                                     instr_type = INSTRUCT_FLOAT2INT;
                                     instr_type = INSTRUCT_EQ;
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                     dataIDF.type = BOOLEAN;
                                     resultOfPrece.bool_result = 1; // true
                                     // generovanie EQ %s@%s %s@%s %s@%s
                                     instr_type = INSTRUCT_FLOAT2INT;
                                     instr_type = INSTRUCT_EQ;
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                     dataIDF.type = BOOLEAN;
                                     resultOfPrece.bool_result = 1; // true
                                     instr_type = INSTRUCT_EQ;
                                 }
                                 else {
                                     resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                     return resultOfPrece;
                                 }
                             }
                             else {
                                 resultOfPrece.result = SYN_ERR;
                                 return resultOfPrece;
                             }
                             tempItemForPositionOne = stack_pop(stack);      // ulozenie si prveho E
                             stack_pop(stack);                               // popnutie znamienka ==
                             tempItemForPositionThree = stack_pop(stack);    // ulozenie si druheho E
                             stack_pop(stack);                               // popnutie znamienak <
//                            tempItemForPositionOne->token_data.value.i != tempItemForPositionThree->token_data.value.i; //  E == E // TOTO JE IBA PRE KONTROLU
                             dataIDF = tempItemForPositionOne->token_data;  // do struktury nahrame adresu token->data
                             stack_push(stack, E, dataIDF);                 // nakoniec pushneme E + datovu strukturu
                             if (DEBUG)
                                 stack_print(stack);
                             break;
                             // PRAVIDLO E -> E != E
                         case eNEQUAL: // !=
                             // generovanie DEFVAR %s@%s
                             instr_type = INSTRUCT_DEFVAR;
                             if (&stack->arrayOfItems[stack->finderOfParenthesis + 3] != NULL) {
                                 if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == STRING) &&
                                     (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == STRING)) {
                                     int result = strCompare(&stack->arrayOfItems[stack->finderOfParenthesis + 1].value.string, &stack->arrayOfItems[stack->finderOfParenthesis + 3].value.string);
                                     if(result != 0){
                                         resultOfPrece.bool_result = 1; // true
                                     }
                                     resultOfPrece.bool_result = 0; // false
                                     dataIDF.type = BOOLEAN;
                                     // generovanie NOT %s@%s %s@%s %s@%s
                                     // generovanie EQ %s@%s %s@%s %s@%s
                                     instr_type = INSTRUCT_NOT;
                                     instr_type = INSTRUCT_EQ;
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                     dataIDF.type = BOOLEAN;
                                     resultOfPrece.bool_result = 1; // true
                                     // generovanie NOT %s@%s %s@%s %s@%s
                                     // generovanie EQ %s@%s %s@%s %s@%s
                                     instr_type = INSTRUCT_NOT;
                                     instr_type = INSTRUCT_EQ;
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                     dataIDF.type = BOOLEAN;
                                     resultOfPrece.bool_result = 1; // true
                                     // generovanie NOT %s@%s %s@%s %s@%s
                                     // generovanie EQ %s@%s %s@%s %s@%s
                                     instr_type = INSTRUCT_FLOAT2INT;
                                     instr_type = INSTRUCT_NOT;
                                     instr_type = INSTRUCT_EQ;
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                     dataIDF.type = BOOLEAN;
                                     resultOfPrece.bool_result = 1; // true
                                     // generovanie NOT %s@%s %s@%s %s@%s
                                     // generovanie EQ %s@%s %s@%s %s@%s
                                     instr_type = INSTRUCT_FLOAT2INT;
                                     instr_type = INSTRUCT_NOT;
                                     instr_type = INSTRUCT_EQ;
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                     dataIDF.type = BOOLEAN;
                                     resultOfPrece.bool_result = 1; // true
                                     // generovanie NOT %s@%s %s@%s %s@%s
                                     // generovanie EQ %s@%s %s@%s %s@%s
                                     instr_type = INSTRUCT_NOT;
                                     instr_type = INSTRUCT_EQ;
                                 }
                                 else {
                                     resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                     return resultOfPrece;
                                 }
                             }
                             else {
                                 resultOfPrece.result = SYN_ERR;
                                 return resultOfPrece;
                             }
                             tempItemForPositionOne = stack_pop(stack);      // ulozenie si prveho E
                             stack_pop(stack);                               // popnutie znamienka !=
                             tempItemForPositionThree = stack_pop(stack);    // ulozenie si druheho E
                             stack_pop(stack);                               // popnutie znamienak <
//                            tempItemForPositionOne->token_data.value.i != tempItemForPositionThree->token_data.value.i; //  E != E // TOTO JE IBA PRE KONTROLU
                             dataIDF = tempItemForPositionOne->token_data;  // do struktury nahrame adresu token->data
                             stack_push(stack, E, dataIDF);                 // nakoniec pushneme E + datovu strukturu
                             if (DEBUG)
                                 stack_print(stack);
                             break;
                     }
                 }

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
                     if(DEBUG)printf("Vysledok operacie pre INT %d\n", dataIDF.value.i);

                    resultOfPrece.result = SUCCESS;                                 // vratenie navratovej hodnoty
                    resultOfPrece.uniqueID->str = stack_top_token_data(stack)->nameOfTheNonTerminal;     // vratenie UNIQUE nazvu identifikatora
                    resultOfPrece.data_type = stack_top_token_data(stack)->type;                    // vratenie typu identificatora

//                    stack_print(stack);
//                    stack_print_prece(stack);
                    stack_free(stack);
                    if(DEBUG)printf("Return exiting value -> |%d|, returning value -> |%s| abd returning type -> |%d|\n", resultOfPrece.result, resultOfPrece.uniqueID->str, resultOfPrece.data_type);
                    return resultOfPrece;
                }
                if(DEBUG)printf("Error\n");
                return resultOfPrece;
        }
    }while(1);
}
