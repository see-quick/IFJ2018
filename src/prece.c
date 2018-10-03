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


#define RULE_OF_OPERATORS stack_pops(4, stack);stack_push(stack, E);stack_print(stack);break
#define RULE_OF_IDENTIFICATOR stack_pops(2, stack);stack_push(stack, E);stack_print(stack)

int counterVar = 1;

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

/* KVOLI INDEXOVANIU NA PRECEDENCNI TABULKU */
int indexerOfPreceTable (int indexer)        // definovanie aktualneho tokenu TODO: there will be not char but tToken(as getToken)
{
    int type = indexer;          // vyberieme si co je aktualny token a budeme ho indexovat
    /* 0 */
    // getToken mi vyhodi cislo ?? 
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
        /* ZLA POSTUPNOST TOKENOV */
        case LEX_ERROR: return ERROR_LEX;               // v pripade ziskania zlej $i<<i
        default:
            /* ZLY TOKEN */
            printf("indexerOfPreceTable():There is not such a symbol\n");
    }
    return type;
}

expr_return parse_expr(LocalMap* lMap, tList* list){

    printf("Som v precedenčnej analýze\n");

    local_map_print(lMap);


    /* INICIALIZACIA STRUKTUR */
    expr_return resultOfPrece = {.result=SUCCESS};
    tStack* stack = stack_init(15);

    stack_refresh(stack);   // vycistenie stacku
    stack_push(stack, eDOLAR); // pushnutie na stack $

    printf("Current token  is -> %d\n", token);
    printf("StackTop  is -> %d\n", stack_top(stack));

    int actTokenIndexToPreceTable = 0;
    int stackTopTokenIndexToPreceTable = 0;

    actTokenIndexToPreceTable = indexerOfPreceTable(token);  // pretypovanie na dany index

    do{
        stack->finderOfParenthesis = stack->top; // vyrovnanie top so finderom

        // v pripade lexikalnej chyb
        if (actTokenIndexToPreceTable == ERROR_LEX){
            resultOfPrece.result = ERROR_LEX;
            return resultOfPrece;
        }

        //v pripade ze je na vrchole zasobniku non terminal E pozerame sa o 1 miesto nizsie
        if(stack->array[stack->top] == E){
            actTokenIndexToPreceTable = indexerOfPreceTable(token);  // pretypovanie na dany index
            stackTopTokenIndexToPreceTable = indexerOfPreceTable(stack->array[stack->finderOfParenthesis - 1]);     // pozerame sa o jedno miesto nizsie
        }
        else{
            actTokenIndexToPreceTable = indexerOfPreceTable(token);  // pretypovanie na dany index
            stackTopTokenIndexToPreceTable  = indexerOfPreceTable(stack_top(stack)); // pretypovanie na dany index

        }

        switch(prece_table[stackTopTokenIndexToPreceTable][actTokenIndexToPreceTable]){
            case EQ:
                stack->finderOfParenthesis = stack->top;
                stack_search_for_theorem(stack);
                stack_push(stack, actTokenIndexToPreceTable);
                stack_print(stack);
                token = getToken();     //zavolanie si noveho tokenu
                break;
            case L:
                // SEM BUDEME VYHLADAVAT ZA NON - TERMINAL -> pre jednoduchost bez neho...
                if(stack_top(stack) == E){
                    stack_pop(stack);
                    stack_push(stack, eSOLVING_RULE);
                    stack_push(stack, E);
                    stack_push(stack, actTokenIndexToPreceTable);
                    stack_print(stack);
                }
                else{
                    stack_push(stack, eSOLVING_RULE);
                    stack_push(stack, actTokenIndexToPreceTable);
                    stack_print(stack);
                }
                token = getToken();     //zavolanie si noveho tokenu
                break;
            case G:
                stack_search_for_theorem(stack);
                // PRAVIDLO E -> i
                printf("This is value -> %d\n",stack->array[stack->finderOfParenthesis+1]);
                if ((stack->array[stack->finderOfParenthesis+1]) == eIDENT){
                    RULE_OF_IDENTIFICATOR;
                }
                    // PRAVIDLO E -> (E)
                else if(((stack->array[stack->finderOfParenthesis+1]) == eLBAR) && ((stack->array[stack->finderOfParenthesis + 2]) == E) && ((stack->array[stack->finderOfParenthesis+3]) == eRBAR)){
                    RULE_OF_OPERATORS;
                }
                else {
                    int concreteOperator = stack->array[stack->finderOfParenthesis + 2];
                    switch(concreteOperator){
                        // PRAVIDLO E -> E + E
                        case ePLUS:

                            RULE_OF_OPERATORS;
                        // PRAVIDLO E -> E - E
                        case eMINUS:
                            RULE_OF_OPERATORS;
                        // PRAVIDLO E -> E * E
                        case eMUL:
                            RULE_OF_OPERATORS;
                        // PRAVIDLO E -> E / E
                        case eDIV:
                            RULE_OF_OPERATORS;
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
                // TODO: dokoncit pravidlo, a urobit funciu generateNonTermn
            case Err:
                if(actTokenIndexToPreceTable == eDOLAR){
                    printf("STATE: $E$ -> EVERYTHING OK\n");

                    resultOfPrece.bool_result = SUCCESS;
                    resultOfPrece.string->str = "true";
                    return resultOfPrece;
                }
                printf("Error\n");
                // TODO: dorobit navratovu hodnotu
                return resultOfPrece;
        }


    }while(actTokenIndexToPreceTable != eDOLAR);

    return resultOfPrece;
}