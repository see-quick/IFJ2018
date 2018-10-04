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


#define RULE_OF_OPERATORS stack_pops(4, stack);stack_push(stack, E);stack_print_prece(stack);break
#define RULE_OF_IDENTIFICATOR stack_pops(2, stack);stack_push(stack, E);stack_print_prece(stack)

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
        case eDOLAR: type = eDOLAR; break;          // TODO: toto ak tak vymaz
        // TODO: while <vyraz> do treba este ukoncovaci vyraz z lexiklanej do...
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
}

expr_return parse_expr(LocalMap* lMap, tList* list){

    printf("Som v precedenčnej analýze\n");

    local_map_print(lMap);


    /* INICIALIZACIA STRUKTUR */
    expr_return resultOfPrece = {.result=SUCCESS};

    tStack* stack = stack_init(15);

    stack_refresh(stack);   // vycistenie stacku
    stack_push(stack, eDOLAR); // pushnutie na stack $

    stack_print_prece(stack);

    int actTokenIndexToPreceTable = 0;
    int stackTopTokenIndexToPreceTable = 0;

    actTokenIndexToPreceTable = indexerOfPreceTable(token);  // pretypovanie na dany index

    do{
        stack->finderOfParenthesis = stack->top; // vyrovnanie top so finderom

        //v pripade ze je na vrchole zasobniku non terminal E pozerame sa o 1 miesto nizsie
        if(stack->array[stack->top] == E){
            actTokenIndexToPreceTable = indexerOfPreceTable(token);  // pretypovanie na dany index
            stackTopTokenIndexToPreceTable = stack->array[stack->finderOfParenthesis - 1];     // pozerame sa o jedno miesto nizsie
        }
        else{
            actTokenIndexToPreceTable = indexerOfPreceTable(token);  // pretypovanie na dany index
            stackTopTokenIndexToPreceTable  = stack_top(stack); // pretypovanie na dany index

        }

        // akonahle sa vo fucnkii indexerOfPreceTable nenajde ziadny znak tak vyhadzujem syntaticku chybu
        if(actTokenIndexToPreceTable == eSYNTERROR){
            resultOfPrece.result = SYN_ERR;
            return resultOfPrece;
        }



        printf("This is act token    number -> |%d| and char -> |%s|\n", actTokenIndexToPreceTable, convert_to_char(actTokenIndexToPreceTable));
        printf("This is act stackTop number -> |%d| and char -> |%s|\n", stackTopTokenIndexToPreceTable, convert_to_char(stackTopTokenIndexToPreceTable));

        switch(prece_table[stackTopTokenIndexToPreceTable][actTokenIndexToPreceTable]){
            case EQ:
                printf("CASE: |=| (equal)\n");
                stack->finderOfParenthesis = stack->top;
                stack_search_for_theorem(stack);
                stack_push(stack, actTokenIndexToPreceTable);
                stack_print_prece(stack);
                token = getToken();     //zavolanie si noveho tokenu
                break;
            case L:
                printf("CASE: |<| (shifting)\n");
                // SEM BUDEME VYHLADAVAT ZA NON - TERMINAL -> pre jednoduchost bez neho...
                if(stack_top(stack) == E){
                    stack_pop(stack);
                    stack_push(stack, eSOLVING_RULE);
                    stack_push(stack, E);
                    stack_push(stack, actTokenIndexToPreceTable);
                    stack_print_prece(stack);
                }
                else{
                    stack_push(stack, eSOLVING_RULE);
                    stack_push(stack, actTokenIndexToPreceTable);
                    stack_print_prece(stack);
                }
                token = getToken();     //zavolanie si noveho tokenu
                break;
            case G:
                printf("CASE: |>| (reduction)\n");
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
                printf("This is act token    number -> |%d| and char -> |%s|\n", actTokenIndexToPreceTable, convert_to_char(actTokenIndexToPreceTable));
                break;
                // TODO: dokoncit pravidlo, a urobit funciu stack_print_preceeNonTermn
            case Err:
                if(actTokenIndexToPreceTable == eDOLAR){
                    printf("This is stackTop %s\n", convert_to_char(stack_top(stack)));
                    /* v pripade ak prijde vyraz a = then  v preklade $$ medzi nimi nebude nic ziaden nontermian(E) tak to vychodi syntakticku chybu */
                    if(stack->top == 0){
                        resultOfPrece.result = SYN_ERR;
                        return resultOfPrece;
                    }
//                    if(stack_top(stack) == eDOLAR)
                    printf("STATE: $E$ -> EVERYTHING OK\n");
                    // uvolnenie stacku
                    stack_free(stack);
                    printf("Vraciam -> %d\n", resultOfPrece.result);
                    resultOfPrece.result = SUCCESS;
                    return resultOfPrece;
                }
                printf("Error\n");
                // TODO: dorobit navratovu hodnotu
                return resultOfPrece;
        }


    }while(1);
    // (actTokenIndexToPreceTable != eDOLAR) && (stackTopTokenIndexToPreceTable != eDOLAR)

    return resultOfPrece;
}