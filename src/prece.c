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
#include "parser.h"
//#include "parser.c" // zobratie globalnej mapy

// #define RULE_OF_OPERATORS stack_pops(4, stack);stack_push(stack, E, dataIDF);stack_print_prece(stack);break
// #define RULE_OF_IDENTIFICATOR stack_pops(2, stack);stack_push(stack, E, *dataIDF);stack_print_prece(stack)
#define STACK_POP4 stack_pops(4, stack)
//#define GLOBAL_OR_LOCAL_FRAME

bool isFirstVariable = false;
bool isThirdVariable = false;
bool is_result = false;
tInstructionTypes instr_type;
tInstructionData instr1;
tInstructionData instr2;
tInstructionData instr3;

extern bool is_LF;
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

void setEmptyDataIDF(tDataIDF dataIDF) {
    dataIDF.type = 500;
    dataIDF.defined = false;
    dataIDF.value.nil = true;
}


/* KVOLI INDEXOVANIU NA PRECEDENCNI TABULKU */
int indexerOfPreceTable (int indexer, LocalMap* lMap)
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
        case LEX_ID:
        case LEX_ID_F:
        case KW_LENGTH:
        case KW_SUBSTR:
        case KW_ORD:
        case KW_CHR:
            if(global_map_contain(gMap, gToken.data.str)){
                type = eFCE;            // nachadza sa to v gMap je to funckia
            }
            else if(local_map_contain(lMap, gToken.data.str)){
                type = eIDENT;         // nachadza sa to v lMap je to premenna
            }
            else{
                // nie je to ani fce ani identificator
                return eSYNTERROR;
            }
            break;

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
        case 14: return ",\0";
        case 15: return "f\0";
        case 22: return "[<]\0";
        case 30: return "\0";
        case 42: return "E\0";
    }
    return "\0";
}

// @varName pravidlo id = <sth>
// @lMap je lokalni Mapa
expr_return parse_expr(LocalMap* lMap, tList* list){
    /* INICIALIZACIA STRUKTUR */
//    tItem* tempItemForPositionOne; /* GLOBALNY ITEM pre stack pri pravidle E -> i */
//    tItem* tempItemForPositionThree; /* GLOBALNY ITEM pre stack pri pravidlach E -> E + E, E -> E - E a podobne. */
    expr_return resultOfPrece = {.result=SUCCESS, .bool_result=""};
    tStack* stack = stack_init(6);
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
            actTokenIndexToPreceTable = indexerOfPreceTable(token, lMap);  // pretypovanie na dany index
            stackTopTokenIndexToPreceTable = stack->arrayOfNumbers[stack->finderOfParenthesis - 1];     // pozerame sa o jedno miesto nizsie
        }
        else{
            actTokenIndexToPreceTable = indexerOfPreceTable(token, lMap);  // pretypovanie na dany index
            stackTopTokenIndexToPreceTable  = stack_top_token_number(stack); // pretypovanie na dany index
        }

        // akonahle sa vo fucnkii indexerOfPreceTable nenajde ziadny znak tak vyhadzujem syntaticku chybu
        if(actTokenIndexToPreceTable == eSYNTERROR) {
            resultOfPrece.result = SYN_ERR;
            return resultOfPrece;
        }
        // SEMANTICKA AKCE
        // pushnuti do stackofItems strukturu pro token, ktery prave prisel od PARSERU
        // muze byt LEX_ID, LEX_NUMBER, LEX_REAL_NUMBER, LEX_STRING

        if (token == LEX_NUMBER){
            dataIDF.type = INTEGER;
            dataIDF.value.i = atoi(gToken.data.str);
            resultOfPrece.uniqueID = &gToken.data;
            dataIDF.isVariable = false;
        }
        else if (token == LEX_REAL_NUMBER){
            dataIDF.type = FLOAT;
            dataIDF.value.f = atof(gToken.data.str);
            resultOfPrece.uniqueID = &gToken.data;
            dataIDF.isVariable = false;
            instr2.type = F;
            instr2.value.f = dataIDF.value.f;
        }
        else if (token == LEX_STRING){
            dataIDF.type = STRING;
            dataIDF.value.string.str = gToken.data.str;
            resultOfPrece.uniqueID = &gToken.data;
            dataIDF.isVariable = false;
            instr2.type = S;
            instr2.value.s = gToken.data.str;
        }
        else if(token == LEX_ID || token == LEX_ID_F || token == KW_LENGTH || token == KW_CHR || token == KW_ORD || token == KW_SUBSTR ){
            if(global_map_contain(gMap, gToken.data.str)){
                dataIDF.type = FUNCTION;
                dataIDF.value.string.str = gToken.data.str;
                resultOfPrece.data_type = FUNCTION;
                resultOfPrece.uniqueID = &gToken.data;
                return resultOfPrece;       // predavam riadenie parseru
                // je to funckia
            }
            // ak sa premenna nachadza v lokalnej mape tak
            else if(local_map_contain(lMap, gToken.data.str)){
                dataIDF = local_map_get_value(lMap, gToken.data.str);
                dataIDF.nameOfTheVariable = gToken.data.str;
                dataIDF.isVariable = true;
                resultOfPrece.uniqueID = &gToken.data;
            }
            else{
                // premenna nebola najdena v localnej mape a tym padom sa jedna o semanticku chybu
                fprintf(stderr, "Promenna %s neni definovana, radek %d\n", gToken.data.str, gToken.row);
                resultOfPrece.result = SEM_ERR;
                return resultOfPrece;
            }
            dataIDF.defined = true;
        }

        if(DEBUG)printf("This is act token    number -> |%d| and char -> |%s|\n", actTokenIndexToPreceTable, convert_to_char(actTokenIndexToPreceTable));
        if(DEBUG) printf("This is act stackTop number -> |%d| and char -> |%s|\n", stackTopTokenIndexToPreceTable, convert_to_char(stackTopTokenIndexToPreceTable));

        switch(prece_table[stackTopTokenIndexToPreceTable][actTokenIndexToPreceTable]) {
            case EQ:
                if (DEBUG)printf("CASE: |=| (equal)\n");
                stack->finderOfParenthesis = stack->top;
                stack_search_for_theorem(stack);
                stack_push(stack, actTokenIndexToPreceTable, dataIDF);
                if (DEBUG)stack_print_prece(stack);
                token = getToken();     //zavolanie si noveho tokenu
                break;
            case L:
                if (DEBUG)printf("CASE: |<| (shifting)\n");
                if (stack_top_token_number(stack) == E) {
                    dataIDF = stack->arrayOfItems[stack->finderOfParenthesis];
                    stack_pop(stack);
                    stack_push(stack, eSOLVING_RULE, dataIDF);
                    stack_push(stack, E, dataIDF);
                    stack_push(stack, actTokenIndexToPreceTable, dataIDF);
                    if (DEBUG)stack_print_prece(stack);
                } else {
                    stack_push(stack, eSOLVING_RULE, dataIDF);
                    stack_push(stack, actTokenIndexToPreceTable, dataIDF);
                    if (DEBUG)stack_print_prece(stack);
                }
                token = getToken();     //zavolanie si noveho tokenu
                break;
            case G:
                if (DEBUG)printf("CASE: |>| (reduction)\n");
                stack_search_for_theorem(stack);
                /** PRAVIDLO E -> i **/
                if ((stack->arrayOfNumbers[stack->finderOfParenthesis + 1]) == eIDENT) {
                    dataIDF = stack->arrayOfItems[stack->finderOfParenthesis];
                    dataIDF.nameOfTheNonTerminal =  "$result\0";      // generovanie UNIQUE
                    stack_pop(stack);
                    stack_pop(stack);
                    stack_push(stack, E, dataIDF);
                    if (DEBUG) printf("StackTop NUMBER -> %s\n", convert_to_char(stack_top_token_number(stack)));
                    if (DEBUG) printf("StackTop DATA -> %d\n", stack_top_token_data(stack)->value.i);
                    if (DEBUG)stack_print_prece(stack);
                    break;
                }
                    /** PRAVIDLO E -> (E) **/
                else if (((stack->arrayOfNumbers[stack->finderOfParenthesis + 1]) == eLBAR) &&
                         ((stack->arrayOfNumbers[stack->finderOfParenthesis + 2]) == E) &&
                         ((stack->arrayOfNumbers[stack->finderOfParenthesis + 3]) == eRBAR)) {
                    dataIDF = stack->arrayOfItems[stack->finderOfParenthesis + 2];
                    dataIDF.nameOfTheNonTerminal =  "$result\0";      // generovanie UNIQUE
                    STACK_POP4;
                    stack_push(stack, E, dataIDF);                       // nakoniec pushneme E + datovu strukturu
                    break;
                } else {  // TODO: pravidlo -> E -> f(E), E -> f(E, E), E -> f(E, E , E ), E -> f(E, E, ...)
                    int concreteOperator = stack->arrayOfNumbers[stack->finderOfParenthesis + 2];
                    switch (concreteOperator) {
                        case ePLUS:
                            // PRAVIDLO E -> E + E
                            if (DEBUG) stack_print_prece(stack);
                             if ((stack->arrayOfNumbers[stack->finderOfParenthesis + 3]) != EMPTY_CHAR) {
                                 if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == STRING) &&
                                     (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == STRING)) {
                                     dataIDF.type = STRING;
                                     instr_type = INSTRUCT_CONCAT;
                                     instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                     // TODO: i do not know why it does not work
//                                     instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
//                                     instr2.type = S;
//                                     instr3.type = S;
//                                     instr2.value.s = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.string.str;
//                                     instr3.value.s = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.string.str;
//                                     printf("This is 1st string -> %s second -> %s\n", instr2.value.s, instr3.value.s);
//                                     dataIDF.value.string.str = strcat(instr2.value.s, instr3.value.s);
//                                     printf("This is DATA %s\n", dataIDF.value.string.str);
//                                     insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                     // generovnaie CONCAT %s@%s %s@%s %s@%s
                                 } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == STRING) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type != STRING)) {
                                    fprintf(stderr, "Semanticka chyba typové kompatibility v aritmetických vyrazech, radek %d\n", gToken.row);
                                    resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                    return resultOfPrece;
                                 } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type != STRING) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == STRING)) {
                                    fprintf(stderr, "Semanticka chyba typové kompatibility v aritmetických vyrazech, radek %d\n", gToken.row);
                                    resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                    return resultOfPrece;
                                 }
                                 else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {

                                     if(stack->arrayOfItems[stack->finderOfParenthesis + 1].isVariable){
                                         isFirstVariable = true;
                                     }
                                     else{ isFirstVariable = false;
                                     }
                                     if(stack->arrayOfItems[stack->finderOfParenthesis + 3].isVariable){
                                         isThirdVariable = true;
                                     }
                                     else{ isThirdVariable = false;
                                     }

                                     if (is_result){
                                        instr2.type = GF;
                                        instr2.value.s = "$result\0";
                                    } else{
                                        if (isFirstVariable){
                                            if (is_LF) { instr2.type = LF;} else { instr2.type = GF;}
                                            instr2.value.s = stack->arrayOfItems[stack->finderOfParenthesis + 1].nameOfTheVariable;
                                        }
                                        else {
                                            instr2.type = I;
                                            instr2.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i;
                                        }
                                    }
                                    if (isThirdVariable == true ){
                                        if (is_LF) { instr3.type = LF;} else {instr3.type = GF;}
                                        instr3.value.s = stack->arrayOfItems[stack->finderOfParenthesis + 3].nameOfTheVariable;

                                    }
                                    else{
                                        instr3.type = I;
                                        instr3.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    }

                                     dataIDF.type = INTEGER;
                                     instr_type = INSTRUCT_ADD;

                                     instr1.type = GF;
                                     instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov

                                     dataIDF.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i + stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                     insert_item(list, &instr_type, &instr1, &instr2, &instr3);

                                     is_result = true;
                                     // generovanie ADD %s@%s %s@%s %s@%s

                                     isFirstVariable = false;
                                     isThirdVariable = false;

                                 } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                     dataIDF.type = FLOAT;
                                     instr_type = INSTRUCT_INT2FLOAT;
                                     instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                     instr2.type = I;
                                     instr2.value.i = (float) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i;
                                     insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                     instr_type = INSTRUCT_ADD;
                                     instr2.type = F;
                                     instr3.type = F;
                                     instr2.value.f = (float) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i;
                                     instr3.value.f = (float) stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                     dataIDF.value.f = (float) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i + stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                     insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                     // generovanie  INT2FLOAT %s@%s %s@%s
                                     // generovanie  ADD %s@%s %s@%s %s@%s
                                 } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                     dataIDF.type = FLOAT;
                                     instr_type = INSTRUCT_INT2FLOAT;
                                     instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                     instr2.type = I;
                                     instr2.value.i = (float) stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                     insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                     instr_type = INSTRUCT_ADD;
                                     instr2.type = F;
                                     instr3.type = F;
                                     instr2.value.f = (float) stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                     instr3.value.f = (float) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f;
                                     dataIDF.value.f = (float) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f + stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                     insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                     // generovanie INT@FLOAT %s@%s %s@%s
                                     // generovanie ADD %s@%s %s@%s %s@%s
                                 } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                            (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                     dataIDF.type = FLOAT;
                                     instr_type = INSTRUCT_ADD;
                                     instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                     instr2.type = F;
                                     instr3.type = F;
                                     instr2.value.f = (float) stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                     instr3.value.f = (float) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f;
                                     dataIDF.value.f = (float) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f + stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                     insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                     // generovanie ADD %s@%s %s@%s %s@%s
                                 }
                                 else if (  (stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) && (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER) ){
                                    instr_type = INSTRUCT_ADD;
                                    instr1.type = GF;
                                    instr1.value.s = "$result\0";

                                    instr2.type = S;
                                    printf("Tests %s\n", dataIDF.value.string.str);
                                    instr2.value.s = dataIDF.value.string.str;

                                    instr3.type = I;
                                    instr3.value.i =  stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;

                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                 }


                                 else {
                                    fprintf(stderr, "Semanticka chyba typové kompatibility v aritmetických vyrazech, radek %d\n", gToken.row);
                                    resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                    return resultOfPrece;
                                 }

                             } else {
                                 resultOfPrece.result = SYN_ERR;
                                 return resultOfPrece;
                             }
                            STACK_POP4;
                            stack_push(stack, E, dataIDF);                 // nakoniec pushneme E + datovu strukturu
                        break;

                            // PRAVIDLO E -> E - E
                        case eMINUS:
                            if ((stack->arrayOfNumbers[stack->finderOfParenthesis + 3]) != EMPTY_CHAR) {
                                if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == STRING) ||
                                    (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == STRING)) {

                                    fprintf(stderr, "Semanticka chyba typové kompatibility v aritmetických vyrazech, radek %d\n", gToken.row);
                                    resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                    return resultOfPrece;

                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                    dataIDF.type = INTEGER;
                                    instr_type = INSTRUCT_SUB;
                                    instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                    instr2.type = I;
                                    instr3.type = I;
                                    instr2.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i;
                                    instr3.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    dataIDF.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i - stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    // generovanie SUB %s@%s %s@%s %s@%s
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                    dataIDF.type = FLOAT;
                                    instr_type = INSTRUCT_INT2FLOAT;
                                    instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                    instr2.type = I;
                                    instr2.value.i = (float) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    instr_type = INSTRUCT_SUB;
                                    instr2.type = F;
                                    instr3.type = F;
                                    instr2.value.f = (float) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i;
                                    instr3.value.f = (float) stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    dataIDF.value.f = (float) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i - stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    // generovanie  INT2FLOAT %s@%s %s@%s
                                    // generovnaie  SUB %s@%s %s@%s %s@%s
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                    dataIDF.type = FLOAT;
                                    instr_type = INSTRUCT_INT2FLOAT;
                                    instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                    instr2.type = I;
                                    instr2.value.i = (float) stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    instr_type = INSTRUCT_SUB;
                                    instr2.type = F;
                                    instr3.type = F;
                                    instr2.value.f = (float) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f;
                                    instr3.value.f = (float) stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    dataIDF.value.f = (float) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f - stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    // generovanie  INT2FLOAT %s@%s %s@%s
                                    // generovnaie  SUB %s@%s %s@%s %s@%s
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                    dataIDF.type = FLOAT;
                                    instr_type = INSTRUCT_SUB;
                                    instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                    instr2.type = F;
                                    instr3.type = F;
                                    instr2.value.f = (float) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f;
                                    instr3.value.f = (float) stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    dataIDF.value.f = (float) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f - stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    // generovanie  SUB %s@%s %s@%s %s@%s
                                } else {
                                    fprintf(stderr, "Semanticka chyba typové kompatibility v aritmetických vyrazech, radek %d\n", gToken.row);
                                    resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                    return resultOfPrece;
                                }
                            } else {
                                resultOfPrece.result = SYN_ERR;
                                return resultOfPrece;
                            }
                            STACK_POP4;
                            stack_push(stack, E, dataIDF);                 // nakoniec pushneme E + datovu strukturu
                            if (DEBUG)
                                stack_print(stack);
                        break;
                            // PRAVIDLO E -> E * E
                        case eMUL:
                            if ((stack->arrayOfNumbers[stack->finderOfParenthesis + 3]) != EMPTY_CHAR) {
                                if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == STRING) ||
                                    (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == STRING)) {

                                    fprintf(stderr, "Semanticka chyba typové kompatibility v aritmetických vyrazech, radek %d\n", gToken.row);
                                    resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                    return resultOfPrece;

                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                    dataIDF.type = INTEGER;
                                    instr_type = INSTRUCT_MUL;
                                    instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                    instr2.type = I;
                                    instr3.type = I;
                                    instr2.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i;
                                    instr3.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    dataIDF.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i * stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    //generovanie MUL %s@%s %s@%s %s@%s
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                    dataIDF.type = FLOAT;
                                    instr_type = INSTRUCT_INT2FLOAT;
                                    instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                    instr2.type = I;
                                    instr2.value.i = (float) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    instr_type = INSTRUCT_MUL;
                                    instr2.type = F;
                                    instr3.type = F;
                                    instr2.value.f = (float) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i;
                                    instr3.value.f = (float) stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    dataIDF.value.f = (float) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i * stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    // generovanie INT2FLOAT %s@%s %s@%s
                                    //generovanie MUL %s@%s %s@%s %s@%s
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                    dataIDF.type = FLOAT;
                                    instr_type = INSTRUCT_INT2FLOAT;
                                    instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                    instr2.type = I;
                                    instr2.value.i = (float) stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    instr_type = INSTRUCT_MUL;
                                    instr2.type = F;
                                    instr3.type = F;
                                    instr2.value.f = (float) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f;
                                    instr3.value.f = (float) stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    dataIDF.value.f = (float) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f * stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    //generovanie INT2FLOAT %s@%s %s@%
                                    //generovanie MUL %s@%s %s@%s %s@%s

                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                    dataIDF.type = FLOAT;
                                    instr_type = INSTRUCT_MUL;
                                    instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                    instr2.type = F;
                                    instr3.type = F;
                                    instr2.value.f = (float) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f;
                                    instr3.value.f = (float) stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    dataIDF.value.f = (float) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f * stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    //generovanie MUL %s@%s %s@%s %s@%s
                                } else {
                                    fprintf(stderr, "Semanticka chyba typové kompatibility v aritmetických vyrazech, radek %d\n", gToken.row);
                                    resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                    return resultOfPrece;
                                }

                            } else {
                                resultOfPrece.result = SYN_ERR;
                                return resultOfPrece;
                            }
                            STACK_POP4;
                            stack_push(stack, E, dataIDF);                 // nakoniec pushneme E + datovu strukturu
                            if (DEBUG)
                                stack_print(stack);
                        break;
                            // PRAVIDLO E -> E / E
                        case eDIV:
                            if ((stack->arrayOfNumbers[stack->finderOfParenthesis + 3]) != EMPTY_CHAR) {
                                if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == STRING) ||
                                    (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == STRING)) {

                                    fprintf(stderr, "Semanticka chyba typové kompatibility v aritmetických vyrazech, radek %d\n", gToken.row);
                                    resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                    return resultOfPrece;

                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                    dataIDF.type = INTEGER;
                                    instr_type = INSTRUCT_IDIV;
                                    instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                    instr2.type = I;
                                    instr3.type = I;
                                    instr2.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i;
                                    instr3.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    if (instr3.value.i == 0){
                                        fprintf(stderr, "Deleni nulou, radek cislo %d\n",gToken.row);
                                        resultOfPrece.result = ERR_DIVISION;
                                        return resultOfPrece;
                                    }
                                    else{
                                        dataIDF.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i / stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                        insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                        //generovanie IDIV %s@%s %s@%s %s@%s
                                    }


                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                    dataIDF.type = FLOAT;
                                    instr_type = INSTRUCT_INT2FLOAT;
                                    instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                    instr2.type = I;
                                    instr2.value.i = (float) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    instr_type = INSTRUCT_DIV;
                                    instr2.type = F;
                                    instr3.type = F;
                                    instr2.value.f =stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i;
                                    instr3.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;

                                    if (instr3.value.f == 0.0){
                                        fprintf(stderr, "Deleni nulou, radek cislo %d\n",gToken.row);
                                        resultOfPrece.result = ERR_DIVISION;
                                        return resultOfPrece;
                                    }
                                    else {
                                        dataIDF.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i / stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                        insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    }
                                    // generovanie DIV %s@%s %s@%s %s@%s
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                    dataIDF.type = FLOAT;
                                    instr_type = INSTRUCT_INT2FLOAT;
                                    instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                    instr2.type = I;
                                    instr2.value.i = (float) stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    instr_type = INSTRUCT_DIV;
                                    instr2.type = F;
                                    instr3.type = F;
                                    instr2.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f;
                                    instr3.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;

                                    if (instr3.value.i == 0){
                                        fprintf(stderr, "Deleni nulou, radek cislo %d\n",gToken.row);
                                        resultOfPrece.result = ERR_DIVISION;
                                        return resultOfPrece;
                                    }

                                    else {
                                        dataIDF.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f / stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                        insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                        // generovanie  DIV %s@%s %s@%s %s@%s
                                    }
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                    dataIDF.type = FLOAT;
                                    instr_type = INSTRUCT_DIV;
                                    instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                    instr2.type = F;
                                    instr3.type = F;
                                    instr2.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f;
                                    instr3.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;

                                    if (instr3.value.f == 0.0){
                                        fprintf(stderr, "Deleni nulou, radek cislo %d\n",gToken.row);
                                        resultOfPrece.result = ERR_DIVISION;
                                        return resultOfPrece;
                                    }
                                    else{
                                        dataIDF.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f * stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                        insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                        // generovanie  DIV %s@%s %s@%s %s@%s
                                    }


                                } else {   //printf("semanticky error BOOL \n");
                                    fprintf(stderr, "Semanticka chyba typové kompatibility v aritmetických vyrazech, radek %d\n", gToken.row);
                                    resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                    return resultOfPrece;
                                }
                            } else {
                                resultOfPrece.result = SYN_ERR;
                                return resultOfPrece;
                            }
                            STACK_POP4;
                            stack_push(stack, E, dataIDF);                 // nakoniec pushneme E + datovu strukturu
                            if (DEBUG)
                                stack_print(stack);
                        break;
                            // PRAVIDLO E -> E < E
                        case eLESS:
                            if ((stack->arrayOfNumbers[stack->finderOfParenthesis + 3]) != EMPTY_CHAR) {
                                if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == STRING) &&
                                    (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == STRING)) {
                                    int result = strcmp(
                                            stack->arrayOfItems[stack->finderOfParenthesis + 1].value.string.str,
                                            stack->arrayOfItems[stack->finderOfParenthesis + 3].value.string.str);
                                    if (result < 0) {
                                        dataIDF.type = BOOLEAN;
                                        instr_type = INSTRUCT_LT;
                                        instr1.value.s = "$$EXPR\0";
                                        instr2.type = S;
                                        instr3.type = S;
                                        instr2.value.s = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.string.str;
                                        instr3.value.s = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.string.str;
                                        insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    }
                                    // generovanie LT %s@%s %s@%s %s@%s
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                    dataIDF.type = BOOLEAN;
                                    instr_type = INSTRUCT_LT;
                                    instr1.value.s = "$$EXPR\0";
                                    instr2.type = I;
                                    instr3.type = I;
                                    instr2.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i;
                                    instr3.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    dataIDF.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i < stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    // generovanie LT %s@%s %s@%s %s@%s
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                    dataIDF.type = BOOLEAN;
                                    instr_type = INSTRUCT_FLOAT2INT;    // asi najskor previest na integer a potom porovanat
                                    instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                    instr2.type = F;
                                    instr2.value.f = (int) stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    instr_type = INSTRUCT_LT;
                                    instr1.value.s = "$$EXPR\0";
                                    instr2.type = I;
                                    instr3.type = I;
                                    instr2.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i;
                                    instr3.value.i = (int)stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    dataIDF.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i < stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    // generovanie LT %s@%s %s@%s %s@%s
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                    dataIDF.type = BOOLEAN;
                                    instr_type = INSTRUCT_FLOAT2INT;        // asi najskor previest na integer a potom porovanat
                                    instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                    instr2.type = F;
                                    instr2.value.f = (int) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    instr_type = INSTRUCT_LT;
                                    instr1.value.s = "$$EXPR\0";
                                    instr2.type = I;
                                    instr3.type = I;
                                    instr2.value.i = (int)stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f;
                                    instr3.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    dataIDF.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i < stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    // generovanie LT %s@%s %s@%s %s@%s
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                    dataIDF.type = BOOLEAN;
                                    instr_type = INSTRUCT_LT;
                                    instr1.value.s = "$$EXPR\0";
                                    instr2.type = F;
                                    instr3.type = F;
                                    instr2.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f;
                                    instr3.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    dataIDF.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f <  stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    // generovanie LT %s@%s %s@%s %s@%s
                                } else {
                                    fprintf(stderr, "Semanticka chyba typové kompatibility v aritmetických vyrazech, radek %d\n", gToken.row);
                                    resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                    return resultOfPrece;
                                }
                            } else {
                                resultOfPrece.result = SYN_ERR;
                                return resultOfPrece;
                            }
                            STACK_POP4;
                            stack_push(stack, E, dataIDF);                 // nakoniec pushneme E + datovu strukturu
                            if (DEBUG)
                                stack_print(stack);
                        break;
                            // PRAVIDLO E -> E > E
                        case eGREAT:
                            if ((stack->arrayOfNumbers[stack->finderOfParenthesis + 3]) != EMPTY_CHAR) {
                                if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == STRING) &&
                                    (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == STRING)) {
                                    int result = strcmp(
                                            stack->arrayOfItems[stack->finderOfParenthesis + 1].value.string.str,
                                            stack->arrayOfItems[stack->finderOfParenthesis + 3].value.string.str);
                                    if (result > 0) {
                                        dataIDF.type = BOOLEAN;
                                        instr_type = INSTRUCT_GT;
                                        instr1.value.s = "$$EXPR\0";
                                        instr2.type = S;
                                        instr3.type = S;
                                        instr2.value.s = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.string.str;
                                        instr3.value.s = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.string.str;
                                        insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    }
                                    // generovanie GT %s@%s %s@%s %s@%s

                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                    dataIDF.type = BOOLEAN;
                                    instr_type = INSTRUCT_GT;
                                    instr1.value.s = "$$EXPR\0";
                                    instr2.type = I;
                                    instr3.type = I;
                                    instr2.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i;
                                    instr3.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    dataIDF.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i > stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    // generovanie GT %s@%s %s@%s %s@%s
                                    instr_type = INSTRUCT_GT;
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                    dataIDF.type = BOOLEAN;
                                    instr_type = INSTRUCT_FLOAT2INT;    // asi najskor previest na integer a potom porovanat
                                    instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                    instr2.type = F;
                                    instr2.value.f = (int) stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    instr_type = INSTRUCT_LT;
                                    instr1.value.s = "$$EXPR\0";
                                    instr2.type = I;
                                    instr3.type = I;
                                    instr2.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i;
                                    instr3.value.i = (int)stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    dataIDF.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i > stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    // generovanie GT %s@%s %s@%s %s@%s
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                    dataIDF.type = BOOLEAN;
                                    instr_type = INSTRUCT_FLOAT2INT;        // asi najskor previest na integer a potom porovanat
                                    instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                    instr2.type = F;
                                    instr2.value.f = (int) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    instr_type = INSTRUCT_LT;
                                    instr1.value.s = "$$EXPR\0";
                                    instr2.type = I;
                                    instr3.type = I;
                                    instr2.value.i = (int)stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f;
                                    instr3.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    dataIDF.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i < stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    // generovanie GT %s@%s %s@%s %s@%s
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                    dataIDF.type = BOOLEAN;
                                    instr_type = INSTRUCT_GT;
                                    instr1.value.s = "$$EXPR\0";
                                    instr2.type = F;
                                    instr3.type = F;
                                    instr2.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f;
                                    instr3.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    dataIDF.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f >  stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    // generovanie GT %s@%s %s@%s %s@%s
                                } else {
                                    fprintf(stderr, "Semanticka chyba typové kompatibility v aritmetických vyrazech, radek %d\n", gToken.row);
                                    resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                    return resultOfPrece;
                                }
                            } else {
                                resultOfPrece.result = SYN_ERR;
                                return resultOfPrece;
                            }

                            isFirstVariable = false;
                            isThirdVariable = false;
                            STACK_POP4;
                            stack_push(stack, E, dataIDF);                 // nakoniec pushneme E + datovu strukturu
                            if (DEBUG)
                                stack_print(stack);
                        break;
                            // PRAVIDLO E -> E <= E
                        case eLEQUAL: // <=
                            if ((stack->arrayOfNumbers[stack->finderOfParenthesis + 3]) != EMPTY_CHAR) {
                                if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == STRING) &&
                                    (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == STRING)) {
                                    dataIDF.type = BOOLEAN;
                                    int result = strcmp(
                                            stack->arrayOfItems[stack->finderOfParenthesis + 1].value.string.str,
                                            stack->arrayOfItems[stack->finderOfParenthesis + 3].value.string.str);
                                    if (result >= 0) {
                                        dataIDF.type = BOOLEAN;
                                        instr_type = INSTRUCT_LTS;
                                        instr1.value.s = "$$EXPR\0";
                                        instr2.type = S;
                                        instr3.type = S;
                                        instr2.value.s = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.string.str;
                                        instr3.value.s = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.string.str;
                                        insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    }
                                    // generovanie LTS %s@%s %s@%s %s@%s
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                    dataIDF.type = BOOLEAN;
                                    instr_type = INSTRUCT_LTS;
                                    instr1.value.s = "$$EXPR\0";
                                    instr2.type = I;
                                    instr3.type = I;
                                    instr2.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i;
                                    instr3.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    dataIDF.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i <= stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);

                                    // generovanie LTS %s@%s %s@%s %s@%s
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                    dataIDF.type = BOOLEAN;
                                    instr_type = INSTRUCT_FLOAT2INT;    // asi najskor previest na integer a potom porovanat
                                    instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                    instr2.type = F;
                                    instr2.value.f = (int) stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    instr_type = INSTRUCT_LTS;
                                    instr1.value.s = "$$EXPR\0";
                                    instr2.type = I;
                                    instr3.type = I;
                                    instr2.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i;
                                    instr3.value.i = (int)stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    dataIDF.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i <= stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    // generovanie LTS %s@%s %s@%s %s@%s
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                    dataIDF.type = BOOLEAN;
                                    instr_type = INSTRUCT_FLOAT2INT;        // asi najskor previest na integer a potom porovanat
                                    instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                    instr2.type = F;
                                    instr2.value.f = (int) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    instr_type = INSTRUCT_LTS;
                                    instr1.value.s = "$$EXPR\0";
                                    instr2.type = I;
                                    instr3.type = I;
                                    instr2.value.i = (int)stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f;
                                    instr3.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    dataIDF.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i <= stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    // generovanie LTS %s@%s %s@%s %s@%s
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                    dataIDF.type = BOOLEAN;
                                    instr_type = INSTRUCT_LTS;
                                    instr1.value.s = "$$EXPR\0";
                                    instr2.type = F;
                                    instr3.type = F;
                                    instr2.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f;
                                    instr3.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    dataIDF.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f <= stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    // generovanie LTS %s@%s %s@%s %s@%s
                                } else {
                                    fprintf(stderr, "Semanticka chyba typové kompatibility v aritmetických vyrazech, radek %d\n", gToken.row);
                                    resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                    return resultOfPrece;
                                }
                            } else {
                                resultOfPrece.result = SYN_ERR;
                                return resultOfPrece;
                            }
                            STACK_POP4;
                            stack_push(stack, E, dataIDF);                 // nakoniec pushneme E + datovu strukturu
                            if (DEBUG)
                                stack_print(stack);
                        break;
                            // PRAVIDLO E -> E >= E
                        case eGEQUAL: // >=
                            if ((stack->arrayOfNumbers[stack->finderOfParenthesis + 3]) != EMPTY_CHAR) {
                                if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == STRING) &&
                                    (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == STRING)) {
                                    dataIDF.type = BOOLEAN;
                                    int result = strcmp(
                                            stack->arrayOfItems[stack->finderOfParenthesis + 1].value.string.str,
                                            stack->arrayOfItems[stack->finderOfParenthesis + 3].value.string.str);
                                    if (result <= 0) {
                                        dataIDF.type = BOOLEAN;
                                        instr_type = INSTRUCT_GTS;
                                        instr1.value.s = "$$EXPR\0";
                                        instr2.type = S;
                                        instr3.type = S;
                                        instr2.value.s = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.string.str;
                                        instr3.value.s = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.string.str;
                                        insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    }
                                    // generovanie GTS %s@%s %s@%s %s@%s
                                    instr_type = INSTRUCT_GTS;
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                    dataIDF.type = BOOLEAN;
                                    instr_type = INSTRUCT_GTS;
                                    instr1.value.s = "$$EXPR\0";
                                    instr2.type = I;
                                    instr3.type = I;
                                    instr2.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i;
                                    instr3.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    dataIDF.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i >= stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    // generovanie GTS %s@%s %s@%s %s@%s
                                    instr_type = INSTRUCT_GTS;
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                    dataIDF.type = BOOLEAN;
                                    instr_type = INSTRUCT_FLOAT2INT;    // asi najskor previest na integer a potom porovanat
                                    instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                    instr2.type = F;
                                    instr2.value.f = (int) stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    instr_type = INSTRUCT_LTS;
                                    instr1.value.s = "$$EXPR\0";
                                    instr2.type = I;
                                    instr3.type = I;
                                    instr2.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i;
                                    instr3.value.i = (int)stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    dataIDF.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i >= stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    // generovanie GTS %s@%s %s@%s %s@%s
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                    dataIDF.type = BOOLEAN;
                                    instr_type = INSTRUCT_FLOAT2INT;        // asi najskor previest na integer a potom porovanat
                                    instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                    instr2.type = F;
                                    instr2.value.f = (int) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    instr_type = INSTRUCT_GTS;
                                    instr1.value.s = "$$EXPR\0";
                                    instr2.type = I;
                                    instr3.type = I;
                                    instr2.value.i = (int)stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f;
                                    instr3.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    dataIDF.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i >= stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                    dataIDF.type = BOOLEAN;
                                    instr_type = INSTRUCT_GTS;
                                    instr1.value.s = "$$EXPR\0";
                                    instr2.type = F;
                                    instr3.type = F;
                                    instr2.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f;
                                    instr3.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    dataIDF.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f >= stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                } else {
                                    fprintf(stderr, "Semanticka chyba typové kompatibility v aritmetických vyrazech, radek %d\n", gToken.row);
                                    resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                    return resultOfPrece;
                                }
                            } else {
                                resultOfPrece.result = SYN_ERR;
                                return resultOfPrece;
                            }
                            STACK_POP4;
                            stack_push(stack, E, dataIDF);                 // nakoniec pushneme E + datovu strukturu
                            if (DEBUG)
                                stack_print(stack);
                        break;
                            // PRAVIDLO E -> E == E
                        case eEQUAL: // ==
                            if ((stack->arrayOfNumbers[stack->finderOfParenthesis + 3]) != EMPTY_CHAR) {
                                if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == STRING) &&
                                    (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == STRING)) {
                                    int result = strcmp(
                                            stack->arrayOfItems[stack->finderOfParenthesis + 1].value.string.str,
                                            stack->arrayOfItems[stack->finderOfParenthesis + 3].value.string.str);
                                    if (result == 0) {
                                        dataIDF.type = BOOLEAN;
                                        instr_type = INSTRUCT_EQ;
                                        instr1.value.s = "$$EXPR\0";
                                        instr2.type = S;
                                        instr3.type = S;
                                        instr2.value.s = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.string.str;
                                        instr3.value.s = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.string.str;
                                        insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    }
                                    // generovanie EQ %s@%s %s@%s %s@%s
                                    instr_type = INSTRUCT_EQ;
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                    dataIDF.type = BOOLEAN;
                                    instr_type = INSTRUCT_EQ;
                                    instr1.value.s = "$$EXPR\0";
                                    instr2.type = I;
                                    instr3.type = I;
                                    instr2.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i;
                                    instr3.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    dataIDF.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i == stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    // generovanie EQ %s@%s %s@%s %s@%s
                                    instr_type = INSTRUCT_EQ;
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                    dataIDF.type = BOOLEAN;
                                    instr_type = INSTRUCT_FLOAT2INT;    // asi najskor previest na integer a potom porovanat
                                    instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                    instr2.type = F;
                                    instr2.value.f = (int) stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    instr_type = INSTRUCT_EQ;
                                    instr1.value.s = "$$EXPR\0";
                                    instr2.type = I;
                                    instr3.type = I;
                                    instr2.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i;
                                    instr3.value.i = (int)stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    dataIDF.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i == stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    // generovanie EQ %s@%s %s@%s %s@%s
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                    dataIDF.type = BOOLEAN;
                                    instr_type = INSTRUCT_FLOAT2INT;        // asi najskor previest na integer a potom porovanat
                                    instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                    instr2.type = F;
                                    instr2.value.f = (int) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    instr_type = INSTRUCT_EQ;
                                    instr1.value.s = "$$EXPR\0";
                                    instr2.type = I;
                                    instr3.type = I;
                                    instr2.value.i = (int)stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f;
                                    instr3.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    dataIDF.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i == stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    // generovanie EQ %s@%s %s@%s %s@%s
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                    dataIDF.type = BOOLEAN;
                                    instr_type = INSTRUCT_EQ;
                                    instr1.value.s = "$$EXPR\0";
                                    instr2.type = F;
                                    instr3.type = F;
                                    instr2.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f;
                                    instr3.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    dataIDF.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f == stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                } else {
                                    fprintf(stderr, "Semanticka chyba typové kompatibility v aritmetických vyrazech, radek %d\n", gToken.row);
                                    resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                    return resultOfPrece;
                                }
                            } else {
                                resultOfPrece.result = SYN_ERR;
                                return resultOfPrece;
                            }
                            STACK_POP4;
                            stack_push(stack, E, dataIDF);                 // nakoniec pushneme E + datovu strukturu
                            if (DEBUG)
                                stack_print(stack);
                        break;
                            // PRAVIDLO E -> E != E
                        case eNEQUAL: // !=
                            if ((stack->arrayOfNumbers[stack->finderOfParenthesis + 3]) != EMPTY_CHAR) {
                                if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == STRING) &&
                                    (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == STRING)) {
                                    int result = strCompare(
                                            &stack->arrayOfItems[stack->finderOfParenthesis + 1].value.string,
                                            &stack->arrayOfItems[stack->finderOfParenthesis + 3].value.string);
                                    if (result != 0) {
                                        dataIDF.type = BOOLEAN;
                                        instr1.value.s = "$$EXPR\0";
                                        instr2.type = S;
                                        instr2.value.s = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.string.str;
                                        instr_type = INSTRUCT_EQ;
                                        instr3.type = S;
                                        instr3.value.s = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.string.str;
                                        insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                        instr_type = INSTRUCT_NOT;
                                        insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    }
                                    // generovanie NOT %s@%s %s@%s %s@%s
                                    // generovanie EQ %s@%s %s@%s %s@%s
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                    dataIDF.type = BOOLEAN;
                                    instr1.value.s = "$$EXPR\0";
                                    instr2.type = I;
                                    instr2.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i;
                                    instr_type = INSTRUCT_EQ;
                                    instr3.type = I;
                                    instr3.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    dataIDF.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i != stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    instr_type = INSTRUCT_NOT;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    // generovanie NOT %s@%s %s@%s %s@%s
                                    // generovanie EQ %s@%s %s@%s %s@%s
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == INTEGER) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                    dataIDF.type = BOOLEAN;
                                    instr_type = INSTRUCT_FLOAT2INT;    // asi najskor previest na integer a potom porovanat
                                    instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                    instr2.type = F;
                                    instr2.value.f = (int) stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    instr_type = INSTRUCT_EQ;
                                    instr1.value.s = "$$EXPR\0";
                                    instr2.type = I;
                                    instr3.type = I;
                                    instr2.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i;
                                    instr3.value.i = (int)stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    dataIDF.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i != stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    instr_type = INSTRUCT_NOT;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    // generovanie NOT %s@%s %s@%s %s@%s
                                    // generovanie EQ %s@%s %s@%s %s@%s
                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == INTEGER)) {
                                    dataIDF.type = BOOLEAN;
                                    instr_type = INSTRUCT_FLOAT2INT;        // asi najskor previest na integer a potom porovanat
                                    instr1.value.s = "$result\0";        // generovanie UNIQUE // generate non Term -> Unikatny nazov
                                    instr2.type = F;
                                    instr2.value.f = (int) stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    instr_type = INSTRUCT_EQ;
                                    instr1.value.s = "$$EXPR\0";
                                    instr2.type = I;
                                    instr3.type = I;
                                    instr2.value.i = (int)stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f;
                                    instr3.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    dataIDF.value.i = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.i == stack->arrayOfItems[stack->finderOfParenthesis + 3].value.i;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    instr_type = INSTRUCT_NOT;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    // generovanie NOT %s@%s %s@%s %s@%s
                                    // generovanie EQ %s@%s %s@%s %s@%s

                                } else if ((stack->arrayOfItems[stack->finderOfParenthesis + 1].type == FLOAT) &&
                                           (stack->arrayOfItems[stack->finderOfParenthesis + 3].type == FLOAT)) {
                                    dataIDF.type = BOOLEAN;
                                    instr1.value.s = "$$EXPR\0";
                                    instr2.type = F;
                                    instr2.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f;
                                    instr_type = INSTRUCT_EQ;
                                    instr3.type = F;
                                    instr3.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    dataIDF.value.f = stack->arrayOfItems[stack->finderOfParenthesis + 1].value.f != stack->arrayOfItems[stack->finderOfParenthesis + 3].value.f;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    instr_type = INSTRUCT_NOT;
                                    insert_item(list, &instr_type, &instr1, &instr2, &instr3);
                                    // generovanie NOT %s@%s %s@%s %s@%s
                                    // generovanie EQ %s@%s %s@%s %s@%s

                                } else {
                                    fprintf(stderr, "Semanticka chyba typové kompatibility v aritmetických vyrazech, radek %d\n", gToken.row);
                                    resultOfPrece.result = ERR_INCOMPATIBLE_TYPE;
                                    return resultOfPrece;
                                }
                            } else {
                                resultOfPrece.result = SYN_ERR;
                                return resultOfPrece;
                            }
                            STACK_POP4;
                            stack_push(stack, E, dataIDF);                 // nakoniec pushneme E + datovu strukturu
                            if (DEBUG)
                                stack_print(stack);
                        break;

                        // tato podmienka je kvoli prikladu a = ++ ....
                        default:
                            resultOfPrece.result = SYN_ERR;
                            return resultOfPrece;
                    }
                }
            break;
                // TODO: dokoncit pravidlo, a urobit funciu stack_print_preceeNonTermn
            case Err:
                if(actTokenIndexToPreceTable == eDOLAR){
                    /* v pripade ak prijde vyraz a = then  v preklade $$ medzi nimi nebude nic ziaden nontermian(E) tak to vychodi syntakticku chybu */
                    if(stack->top == 0){
                        resultOfPrece.result = SYN_ERR;
                        return resultOfPrece;
                    }
                    if(DEBUG)printf("STATE: $E$ -> EVERYTHING OK\n");
                    resultOfPrece.result = SUCCESS;                                                         // vratenie navratovej hodnoty
                    resultOfPrece.uniqueID->str = stack_top_token_data(stack)->nameOfTheNonTerminal;        // vratenie UNIQUE nazvu identifikatora
                    resultOfPrece.data_type = stack_top_token_data(stack)->type;                        // vratenie typu identificatora
                    stack_free(stack);
                    if(DEBUG)printf("Return exiting value -> |%d|, returning value -> |%s| abd returning type -> |%d|\n", resultOfPrece.result, resultOfPrece.uniqueID->str, resultOfPrece.data_type);
                    return resultOfPrece;
                }
                if(DEBUG)printf("Error in prece.c\n");
                return resultOfPrece;
        }
    }while(1);
}
