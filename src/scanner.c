/**
 * Predmet:  IFJ
 * Projekt:  Implementace prekladace imperativniho jazyka IFJ18
 * Soubor:   scanner.c
 *
 * Popis: Lexikalni anazyla, zalocena na konecnem automatu s koncovymi stavy
 *
 *
 * Datum:
 *
 * Autori:   Maros Orsak       vedouci
 *           Polishchuk Kateryna     <xpolis03@fit.vutbr.cz>
 *           Igor Ignac
 *           Marek Rohel

*/

#include "scanner.h"
#include "error.h"
#include "string.h"

//trackovanie zapornych cisel mimo vyrazy
bool expr = false;
bool sub = false;
int digit_check = 0;
//bool digit_lock = false;    // pomocnik pro uzamykani cisel


/*********************************************************/
/*************Funkce pro prace se strukturou token********/
/*********************************************************/

int initToken(){
    gToken.row = 1;

    if ( strInit(&(gToken.data)) == INT_ERR)
      return INT_ERR;
    return SUCCESS;
}

void pushToken(int character){
    strAdd(&(gToken.data), (char)character);
}

void returnToken(){
    char* word = gToken.data.str;
    int length = gToken.data.length;
    int i;
    for ( i = 0; i < length; i++){
        printf("%c",word[i]);
    }
    printf("\n");
}

void resetToken(){
    strFree(&(gToken.data));
    //strClear(&(gToken.data));
    strInit(&(gToken.data));
}


const char* keyWords[] = {
    "def" ,"do", "else",
    "end","if",
    "not",
    "nil",
    "then", "while",
    "inputs", "inputsi", "inputsf",
    "print", "length", "substr",
    "ord", "chr",
};


/*****************************************************************************/
/*Funkce ktere hledaji klicove nebo rezervovane slovo a vraci jeho typ lexemy*/
/*****************************************************************************/
int isKeyword(tString *word){
    int i;
    for(i = 0; i < KEYWORD_COUNT; i++){
        if(strcmp(keyWords[i], word->str) == 0)
            return i+SHIFT;
    }
    return SUCCESS;
}


/********************************************************************/
/*Hlavni funkce implementujici konecny automat Lexikalniho analytoru*/
/*Navratovy typ - typ lexemu ****************************************/
/********************************************************************/
int getToken(){
    //Reset tokenu do pocatecniho stavu
    resetToken();

    int state = S_START;

    bool flag = false;
    int zero_cnt = 0;

    int c, ascii_cnt;
    char ascii_val[2];
    int temp;

    //Hlavni nacitaci smycka
    while(1){

        c = getchar();
        switch(state){
            case S_START:


                if(isspace(c)){
                    if (c == '\n') {
                        gToken.row++;
                        state = S_EOL;
                        continue;
                    }
                else {
                    state = S_START;
                }
                }

                if(c == ' ' || c == '\r' || c == '\t') break;
                else if(c == '\n'){ pushToken(c); return LEX_EOL; }
                else if(c == EOF){ return LEX_EOF; }
                else if(c == '(') { pushToken(c); return LEX_L_BRACKET; }      // leva zavorka
                else if(c == ')') { pushToken(c); return LEX_R_BRACKET; }      // prava zavorka
                else if(c == '[') { pushToken(c); return LEX_L_SBRACKET; }     // leva hranata zavorka
                else if(c == ']') { pushToken(c); return LEX_R_SBRACKET; }     // prava hranata zavorka
                else if(c == '+') { pushToken(c); return LEX_ADDITION; }       // plus
                else if(c == '-') { pushToken(c); state = S_NEG_NUMBER; }   // minus || zaporne cislo
                else if(c == '*') { pushToken(c); return LEX_MULTIPLICATION; } // hvezdicka
                else if(c == '/') { pushToken(c); return LEX_DIVISION; }       // deleni
                else if(c == '=') { pushToken(c); state = S_EQUAL; }          // rovnitko
                else if(c == ',') { pushToken(c); return LEX_COMMA; }          // carka
                else if(c == ':') { pushToken(c); return LEX_COLON; }          // dvojtecka
                else if(c == '.') { pushToken(c); return LEX_DOT; }            // tecka
                else if(c == ';') { pushToken(c); return LEX_SEMICOLON;}       // strednik

                else if(c == '!') { pushToken(c); state = S_AS_EXCM; }                       // vykricnik
                else if(c == '>') { pushToken(c); state = S_GREATER; }                       // vetsitko
                else if(c == '<') { pushToken(c); state = S_LESSER; }                        // mensitko
                else if(c == '\"'){ state = S_STRING; }


                else if(c == '#'){ pushToken(c); state = S_COMMENT_ROW; }                    // radkovy komentar


                else if (isdigit(c)) {
                    if ( c == '0'){
                        flag = true;
                        zero_cnt = 1;
                    }
                    pushToken(c);
                    state = S_NUMBER;
                }                     // Cislo
                else if(islower(c) || c == '_'){ pushToken(c); state = S_ID; }               // Identifikator (a-z, '_')
                else{
                    pushToken(c);                                              //Chybny znak
                    return ERROR_LEX;
                }


                break;

            case S_COMMENT_BLOCK_B:
                if (c == 'e'){
                    state = S_COMMENT_BLOCK_E;
                }
                else {
                    ungetc(c, stdin);
                    return ERROR_LEX;
                }
            break;

            case S_COMMENT_BLOCK_E:
                if (c == 'g'){
                    state = S_COMMENT_BLOCK_G;
                }
                else {
                    ungetc(c, stdin);
                    return ERROR_LEX;
                }
            break;

            case S_COMMENT_BLOCK_G:
                if (c == 'i'){
                    state = S_COMMENT_BLOCK_I;
                }
                else {
                    ungetc(c, stdin);
                    return ERROR_LEX;
                }
            break;

            case S_COMMENT_BLOCK_I:
                if (c == 'n'){
                    state = S_COMMENT_BLOCK_N;
                }
                else{
                    ungetc(c, stdin);
                    return ERROR_LEX;
                }
            break;

            case S_COMMENT_BLOCK_N:
                if(c == EOF){
                    return ERROR_LEX;
                }
                else if( c == '\n'){
                  state = S_COMMENT_BLOCK_N_NEWLINE;
                  gToken.row++;
                 }
                else if( c == '=' || c == ' ' || c == '\r' || c == '\t'){
                  state = S_COMMENT_BLOCK_IN;
                }
                else{
                    state = S_COMMENT_BLOCK_N;
                }
            break;

            case S_COMMENT_BLOCK_N_NEWLINE:
              if(c == EOF){
                return ERROR_LEX;
              }
              else if(c == '=') state = S_COMMENT_END;
              else state = S_COMMENT_BLOCK_N_NEWLINE;
            break;

            case S_COMMENT_BLOCK_IN:
              if(c == 'e'){
                state = S_COMMENT_BLOCK_IN_E;
              }
              else {
                state = S_COMMENT_BLOCK_N;
              }
            break;

            case S_COMMENT_BLOCK_IN_E:
              if(c == 'n'){
                state = S_COMMENT_BLOCK_IN_N;
              }
              else{
                state = S_COMMENT_BLOCK_N;
              }
            break;

            case S_COMMENT_BLOCK_IN_N:
              if(c == 'd'){
                return ERROR_LEX;
              }
              else {
                state = S_COMMENT_BLOCK_N;
              }
            break;

            case S_COMMENT_END:
                if (c == 'e'){
                    state = S_COMMENT_END_E;
                }
                else{
                    ungetc(c, stdin);
                    return ERROR_LEX;
                }
            break;

            case S_COMMENT_END_E:
                if (c == 'n') {state = S_COMMENT_END_N;}
                else {
                    ungetc(c, stdin);
                    return ERROR_LEX;
                }
            break;

            case S_COMMENT_END_N:
                if (c == 'd'){state = S_START;}
                    else{
                        ungetc(c, stdin);
                        return ERROR_LEX;
                    }
            break;

            case S_EQUAL:
                if (c == '='){
                    pushToken(c);
                    return LEX_DOUBLE_EQUAL;
                }
                else if (c == 'b'){
                    resetToken();
                    state = S_COMMENT_BLOCK_B;
                }
                else{
                    expr = false;
                    ungetc(c, stdin);
                    return LEX_EQUAL;
                }
            break;


            case S_EOL:
                    if(c == '\n'){
                        gToken.row++;
                    }
                    else{
                        expr = false;
                        ungetc(c, stdin);
                        return LEX_EOL;
                    }
            break;

            case S_NEG_NUMBER:
              if(isdigit(c)){
                pushToken(c);
                state = S_NUMBER;
                sub = true;
              }
              else {
                ungetc(c, stdin);
                sub = false;
                return LEX_SUBSTRACTION;
              }
            break;

            //Cislo - cela cast
            case S_NUMBER:
                    if(isdigit(c)){
                        if(c == '0' && zero_cnt > 1 && flag) return ERROR_LEX;
                        else if(c == '0') zero_cnt++;
                        pushToken(c);
                        state = S_NUMBER;
                    }
                    else if(expr == false && sub == true && c == '\n') return ERROR_LEX;
                    else if(c == '.'){
                        state = S_NUMBER_POINT;
                        pushToken(c);
                    }
                    else if (c == 'e' || c == 'E'){
                        state = S_NUMBER_EXPONENT;
                        pushToken(c);
                    }
                    else{
                        ungetc(c, stdin);
                        if (flag && zero_cnt > 1){
                            return ERROR_LEX;
                        }
                        /*else if (digit_check == 1){
                            return ERROR_LEX;
                        }*/
                        else if(isspace(c) || c == ',' || c == ')' || c == '+' || c == '-' || c == '*' || c == '/'
                                  || c == '>' || c == '<'    ){     // is delimiter     && digit_lock == false
                            expr = true;
                            return LEX_NUMBER;
                        }
                        else return ERROR_LEX;

                    }
                    //digit_lock == false;
                    break;

            //Cislo - desetina cast
            case S_NUMBER_POINT:
                if(isdigit(c)){
                    pushToken(c);
                    state = S_REAL;
                }
                else
                    return ERROR_LEX;
                break;

            //Cislo - exponent
            case S_NUMBER_EXPONENT:
                if(isdigit(c) || c == '+' || c == '-'){
                    //if(c == '0') zero_cnt++;
                    pushToken(c);
                    state = S_REAL;
                    if (c == '+' || c == '-'){
                        digit_check = 1;    // pro kontrolu 1e+ erroru
                    }
                    else if (isdigit(c)){
                        printf("cisloo");
                        digit_check = 0;
                    }
                    //digit_check = 1;
                }
                else
                    return ERROR_LEX;
                break;

            // Realne cislo
            case S_REAL:
                if(isdigit(c)){
                    pushToken(c);
                    state = S_REAL;
                    //printf("lllllll");
                    //digit_check = 0;
                }
                else if(expr == false && sub == true && c == '\n') return ERROR_LEX;
                else if (c == 'e' || c == 'E') {
                    pushToken(c);
                    state = S_NUMBER_EXPONENT;
                }
                else{
                    ungetc(c, stdin);
                    //printf("qqqqqq");
                    if (flag && zero_cnt > 1){
                        return ERROR_LEX;
                    }
                    else if (digit_check == 1){
                        return ERROR_LEX;
                    }
                    else{
                        zero_cnt = 0;
                        expr = true;
                        return LEX_REAL_NUMBER;
                    }
                }
                digit_check = 0;
                break;

            case S_ID:
                if (isalnum(c) || c == '_'){
                  pushToken(c);
                  state = S_ID;
                }
                else if(c == '!' || c == '?'){
                    pushToken(c);
                    state = S_ID_F_END;
                }
                else{
                  ungetc(c, stdin);
                  if ( (temp = isKeyword(&(gToken.data))) != SUCCESS)
                    return temp;
                  else expr = true; return LEX_ID;
                }
                break;
                
            case S_ID_F_END:
                if (isspace(c) || ',' || ')'){ // is delimiter
                    ungetc(c, stdin);
                    return LEX_ID_F;
                }
                else{
                    return ERROR_LEX;
                }
                break;
            //Identifikator
            /*case S_ID:
                if (isalnum(c) || c == '_'){
                  pushToken(c);
                  state = S_ID;
                }
                 else if(c == '!' || c == '?'){
                    pushToken(c);
                    state = S_ID_F_END;
                }
                else{
                  ungetc(c, stdin);
                  if ( (temp = isKeyword(&(gToken.data))) != SUCCESS)
                    return temp;
                  else expr = true; return LEX_ID;
                }
                break;


            // dodelat
            case S_ID_END:
                if (isalnum(c)){
                    pushToken(c);
                    state = S_ID;
                }
                else{
                    ungetc(c, stdin);
                    return LEX_ID;
                }
                break;
            
            case S_ID_F_END:
                if (c == '?' || c == '!'){
                    pushToken(c);
                    state = S_ID;
                }
                else{
                    ungetc(c, stdin);
                    return LEX_ID_F;
                }
                break;*/
            // Radkovy komentar
            case S_COMMENT_ROW:
                if( c == EOF )
                  return LEX_EOF;
                else if( c == '\n'){
                    resetToken(); // ignoruje komentare
                    gToken.row++;
                    state = S_EOL;
                }
                else state = S_COMMENT_ROW;
                break;


            //Mensi nez nebo mensi nebo rovno
            case S_LESSER:
                if(c == '='){
                    pushToken(c);
                    return LEX_LESSER_EQUAL;
                }
                else{
                    ungetc(c, stdin);
                    return LEX_LESSER;
                }
                break;

            //Vetsi nez nebo vetsi nebo rovno
            case S_GREATER:
                if(c == '='){
                    pushToken(c);
                    return LEX_GREATER_EQUAL;
                }
                else{
                    ungetc(c, stdin);
                    return LEX_GREATER;
                }
                break;

            //Retezec
            case S_STRING:
                    if(c == EOF || c == '\n')
                        return ERROR_LEX;
                    else if (c == '\\'){
                        state = S_STRING_ESCAPED;
                        //pushToken(c);
                    }
                    else if(c == '\"'){
                        return LEX_STRING;
                    }
                    else
                        pushToken(c);
                break;

            case S_STRING_ESCAPED:
                if(c == 'n'){
                    state = S_STRING;
                    pushToken('\n');
                }
                else if(c == 't'){
                    state = S_STRING;
                    pushToken('\t');

                }
                else if(c == '\\'){
                    state = S_STRING;
                    pushToken('\\');

                }
                else if(c == '"'){
                    state = S_STRING;
                    pushToken('\"');

                }
                else if(c == 's'){
                    state = S_STRING;
                    pushToken(' ');
                }
                else if (c == 'x') {
                    state = S_STRING_ASCII;
                    ascii_cnt = 0;
                }
                else
                    return ERROR_LEX;
                break;

            case S_STRING_ASCII:
                //printf("%c * \n", c);
                if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
                    if (ascii_cnt < 2)
                        ascii_val[ascii_cnt++] = c;
                    else {
                        ungetc(c, stdin);
                        char *endptr = NULL;
                        long ascii_tmp = strtol(ascii_val, &endptr, 16);

                        if (*endptr != '\0' || strcmp(endptr, ascii_val) == 0){
                            printf("asci err here 1");
                            return ERROR_LEX;
                        }
                            

                        pushToken((int) ascii_tmp);
                        state = S_STRING;
                    }
                }
                else if (ascii_cnt == 2) {
                    ungetc(c, stdin);
                    char *endptr = NULL;
                    long ascii_tmp = strtol(ascii_val, &endptr, 16);
                    printf("%d  %ld  %s ", *endptr, ascii_tmp, ascii_val);
                    if (*endptr != '\0' || strcmp(endptr, ascii_val) == 0){
                        printf("asci err here 2");
                        return ERROR_LEX;
                    }
                        

                    pushToken((int) ascii_tmp);
                    state = S_STRING;
                }
                else{
                    printf("asci err here 3");
                    return ERROR_LEX;
                }
                break;

            case S_AS_EXCM:
                if(c == '='){
                    pushToken(c);
                    return LEX_UNEQUAL;
                }
                else{
                    ungetc(c, stdin);
                    return LEX_EXCM;
                }
                break;


            default:
                return ERROR_LEX;
                break;
        }
    }
    return ERROR_LEX;
}
