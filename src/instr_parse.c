/**
 * Predmet:  IFJ
 * Projekt:  Implementace prekladace imperativniho jazyka IFJ18
 * Soubor:   instr_parse.c
 *
 * Popis:  zdrojovy subor
 *
 * Datum: 29.9.2018 10:18
 *
 * Autori:   Maros Orsak       vedouci
 *           Polishchuk Kateryna     <xpolis03@fit.vutbr.cz>
 *           Igor Ignac
 *           Marek Rohel
*/
#include "instr_parse.h"
#include "list.h"



int while_count = 0;
int if_count = 0;
extern int argCount;

/**
 * Ulahcenie vypisu podla typu instrukcie
 * @param instruction instrukcia
 * @return cislo v enume prevedene na retazec
 */
char* instruct_type(tDatType instruction) {
  char *instr_type;

  switch (instruction) {
    case I:
          return instr_type = "int";
    break;

    case F:
          return instr_type = "float";
    break;

    case S:
          return instr_type = "string";
    break;

    case N:
          return instr_type = "nil";
    break;

    case GF:
          return instr_type = "GF";
    break;

    case LF:
          return instr_type = "LF";
    break;

    case TF:
          return instr_type = "TF";
    break;

    case FCE:
    case EMPTY:
      break;
  }

  return NULL;  //else -> ERROR
}
/*
 * Printuje symbol na zaklade jeho typu(int,id,float..)
 * @param instr_operand data a typ instrukcie
 */
void print_symb(tInstructionData instr_operand)  {
  if (instr_operand.type == I)  {
    printf("%d",instr_operand.value.i);
  }
  else if (instr_operand.type == F) printf("%a",instr_operand.value.f);
  else { printf("%s",instr_operand.value.s); }
}

/*
 * Printuje symbol na zaklade jeho typu(int,id,float..) --> pomocna funkcia
 * @param instr_operand data a typ instrukcie
 */
void print_multiple_symb(tInstructionData instr_operand)  {
  if (instr_operand.type == I)  printf("%d ",instr_operand.value.i);
  else if (instr_operand.type == F) printf("%a ",instr_operand.value.f);
  else printf("%s ",instr_operand.value.s);
}

/*
 * Urci typ instrukcie a vrati jej nazov na zaklade enumu
 * @param act_instr instrukcia ktoru skumame
 */
void print_arit_instr(tNode *act_instr) {
  char *name;
  switch (act_instr->data.type) {
    case INSTRUCT_ADD: name = "ADD"; break;
    case INSTRUCT_SUB: name = "SUB"; break;
    case INSTRUCT_MUL: name = "MUL"; break;
    case INSTRUCT_DIV: name = "DIV"; break;
    case INSTRUCT_IDIV: name = "IDIV"; break;
    case INSTRUCT_ADDS: name = "ADDS"; break;
    case INSTRUCT_SUBS: name = "SUBS"; break;
    case INSTRUCT_MULS: name = "MULS"; break;
    case INSTRUCT_DIVS: name = "DIVS"; break;
    case INSTRUCT_IDIVS: name = "IDIVS"; break;
    case INSTRUCT_LT: name = "LT"; break;
    case INSTRUCT_GT: name = "GT"; break;
    case INSTRUCT_EQ: name = "EQ"; break;
    case INSTRUCT_LTS: name = "LTS"; break;
    case INSTRUCT_GTS: name = "GTS"; break;
    case INSTRUCT_EQS: name = "EQS"; break;
    case INSTRUCT_AND: name = "AND"; break;
    case INSTRUCT_OR: name = "OR"; break;
    case INSTRUCT_ANDS: name = "ANDS"; break;
    case INSTRUCT_ORS: name = "ORS"; break;
    case INSTRUCT_STRI2INT: name = "STRI2INT"; break;
    case INSTRUCT_STRI2INTS: name = "STRI2INTS"; break;
    case INSTRUCT_CONCAT: name = "CONCAT"; break;
    case INSTRUCT_GETCHAR: name = "GETCHAR"; break;
    case INSTRUCT_SETCHAR: name = "SETCHAR"; break;
    default:
      break;
  }

  printf("%s %s@", name, instruct_type(act_instr->data.address1.type) );
  print_multiple_symb(act_instr->data.address1);
  printf("%s@",  instruct_type(act_instr->data.address2.type) );
  print_multiple_symb(act_instr->data.address2);
  printf("%s@", instruct_type(act_instr->data.address3.type));
  print_symb(act_instr->data.address3);
  printf("\n");


  // printf("MOVE %s@",instruct_type(act_instr->data.address1.type));
  //         print_symb(act_instr->data.address1);
  //         printf("%s@",instruct_type(act_instr->data.address2.type));
  //         print_symb(act_instr->data.address2);
}



/**
 * Append `list` to root list
 * @param root Main list
 * @param list Appended list
 * @return True on success, otherwise False
 */
void append_list(tList* root, tList* list) {
    if (list->first == NULL) {
        return;
    }

    for (tNode* tmp = list->first; tmp != NULL; tmp = tmp->next) {
        tData data = tmp->data;
        insert_item(root, &data.type, &data.address1, &data.address2, &data.address3);
    }
}



/*
 * Parsuje list s instrukciami a printuje na stdout instrukcie v IFJcode18
 * @param instr_list list s instrukciami
 * @warning predtym ako sa zavola parse_instructions musis/musite zavolat reverse() a set_active() na list
 */
void parse_instructions(tList *instr_list)  {
  tNode *act_instr;

  while(1) {

    if((act_instr = return_instruct(instr_list)) == NULL )  break;
    move_activity(instr_list);

    switch (act_instr->data.type) {
      case INSTRUCT_HEAD:
          printf(".IFJcode18\n");
          printf("JUMP $$main\n");
          printf("LABEL $$main\n");
          printf("DEFVAR GF@$$var_integer\n");
          printf("MOVE GF@$$var_integer int@0\n");
          printf("DEFVAR GF@$$var_double\n");
          // nejaka chyba??
          printf("MOVE GF@$$var_double float@0x0p+0\n");
          printf("DEFVAR GF@$$var_string\n");
          printf("MOVE GF@$$var_string string@\n");
          printf("DEFVAR GF@$$EXPR\n");
          printf("DEFVAR GF@$result\n");
          printf("MOVE GF@$$EXPR int@0\n");
      break;

      case INSTRUCT_CREATEFREAME:
          printf("CREATEFRAME\n");
      break;

      case INSTRUCT_PUSHFRAME:
          printf("PUSHFRAME\n");
      break;

      case INSTRUCT_POPFRAME:
          printf("POPFRAME\n");
      break;

      case INSTRUCT_RETURN:
          printf("RETURN\n");
      break;

      case INSTRUCT_CLEARS:
          printf("CLEARS\n");
      break;

      case INSTRUCT_BREAK:
          printf("BREAK\n");
      break;

      case INSTRUCT_MOVE:
          printf("MOVE %s@",instruct_type(act_instr->data.address1.type));
          print_symb(act_instr->data.address1);
          printf(" %s@",instruct_type(act_instr->data.address2.type));
          print_symb(act_instr->data.address2);
          printf("\n");
      break;

      case INSTRUCT_DEFVAR:
          printf("DEFVAR %s@%s\n", instruct_type(act_instr->data.address1.type), act_instr->data.address1.value.s);
      break;

      case INSTRUCT_CALL:
          printf("CALL %s\n", act_instr->data.address1.value.s);
      break;

      case INSTRUCT_PUSHS:
          printf("PUSHS %s@", instruct_type(act_instr->data.address1.type));
          print_symb(act_instr->data.address1);
          printf("\n");
      break;

      case INSTRUCT_POPS:
          printf("POP %s@%s\n", instruct_type(act_instr->data.address1.type), act_instr->data.address1.value.s);
      break;

      case INSTRUCT_ADD:
          print_arit_instr(act_instr);
      break;

      case INSTRUCT_SUB:
          print_arit_instr(act_instr);
      break;

      case INSTRUCT_MUL:
          print_arit_instr(act_instr);
      break;

      case INSTRUCT_DIV:
          print_arit_instr(act_instr);
      break;

      case INSTRUCT_IDIV:
          print_arit_instr(act_instr);
      break;

      case INSTRUCT_ADDS:
          print_arit_instr(act_instr);
      break;

      case INSTRUCT_SUBS:
          print_arit_instr(act_instr);
      break;

      case INSTRUCT_MULS:
          print_arit_instr(act_instr);
      break;

      case INSTRUCT_DIVS:
          print_arit_instr(act_instr);
      break;

      case INSTRUCT_IDIVS:
          print_arit_instr(act_instr);
      break;

      case INSTRUCT_LT:
          print_arit_instr(act_instr);
      break;

      case INSTRUCT_GT:
          print_arit_instr(act_instr);
      break;

      case INSTRUCT_EQ:
          print_arit_instr(act_instr);
      break;

      case INSTRUCT_LTS:
          print_arit_instr(act_instr);
      break;

      case INSTRUCT_GTS:
          print_arit_instr(act_instr);
      break;

      case INSTRUCT_EQS:
          print_arit_instr(act_instr);
      break;

      case INSTRUCT_AND:
          print_arit_instr(act_instr);
      break;

      case INSTRUCT_OR:
          print_arit_instr(act_instr);
      break;

      case INSTRUCT_NOT:
          printf("NOT %s@%s %s@", instruct_type(act_instr->data.address1.type), act_instr->data.address1.value.s, instruct_type(act_instr->data.address2.type));
          print_symb(act_instr->data.address2);
          printf("\n");
      break;

      case INSTRUCT_ANDS:
          print_arit_instr(act_instr);
      break;

      case INSTRUCT_ORS:
          print_arit_instr(act_instr);
      break;

      case INSTRUCT_NOTS:
          printf("NOTS %s@%s %s@", instruct_type(act_instr->data.address1.type), act_instr->data.address1.value.s, instruct_type(act_instr->data.address2.type));
          print_symb(act_instr->data.address2);
          printf("\n");
      break;

      case INSTRUCT_INT2FLOAT:
          printf("INT2FLOAT %s@%s %s@", instruct_type(act_instr->data.address1.type), act_instr->data.address1.value.s, instruct_type(act_instr->data.address2.type));
          print_symb(act_instr->data.address2);
          printf("\n");
      break;

      case INSTRUCT_FLOAT2INT:
          printf("FLOAT2INT %s@%s %s@", instruct_type(act_instr->data.address1.type), act_instr->data.address1.value.s, instruct_type(act_instr->data.address2.type));
          print_symb(act_instr->data.address2);
          printf("\n");
      break;

      case INSTRUCT_INT2CHAR:
          printf("INT2CHAR %s@%s %s@", instruct_type(act_instr->data.address1.type), act_instr->data.address1.value.s, instruct_type(act_instr->data.address2.type));
          print_symb(act_instr->data.address2);
          printf("\n");
      break;

      case INSTRUCT_STRI2INT:
          print_arit_instr(act_instr);
      break;

      case INSTRUCT_INT2FLOATS:
          printf("INT2FLOATS %s@%s %s@", instruct_type(act_instr->data.address1.type), act_instr->data.address1.value.s, instruct_type(act_instr->data.address2.type));
          print_symb(act_instr->data.address2);
          printf("\n");
      break;

      case INSTRUCT_FLOAT2INTS:
          printf("INT2FLOAT %s@%s %s@", instruct_type(act_instr->data.address1.type), act_instr->data.address1.value.s, instruct_type(act_instr->data.address2.type));
          print_symb(act_instr->data.address2);
          printf("\n");
      break;

      case INSTRUCT_INT2CHARS:
          printf("INT2CHARS %s@%s %s@", instruct_type(act_instr->data.address1.type), act_instr->data.address1.value.s, instruct_type(act_instr->data.address2.type));
          print_symb(act_instr->data.address2);
          printf("\n");
      break;

      case INSTRUCT_STRI2INTS:
          print_arit_instr(act_instr);
      break;

      case INSTRUCT_READ:
          printf("READ %s@%s %s\n", instruct_type(act_instr->data.address1.type), act_instr->data.address1.value.s, instruct_type(act_instr->data.address2.type));
      break;

      case INSTRUCT_WRITE:
          printf("WRITE %s@", instruct_type(act_instr->data.address1.type));
          print_symb(act_instr->data.address1);
          printf("\n");
      break;

      case INSTRUCT_CONCAT:
          print_arit_instr(act_instr);
      break;

      case INSTRUCT_STRLEN:
          printf("STRLEN %s@%s %s@", instruct_type(act_instr->data.address1.type), act_instr->data.address1.value.s, instruct_type(act_instr->data.address2.type));
          print_symb(act_instr->data.address2);
          printf("\n");
      break;

      case INSTRUCT_GETCHAR:
          print_arit_instr(act_instr);
      break;

      case INSTRUCT_SETCHAR:
          print_arit_instr(act_instr);
      break;

      case INSTRUCT_TYPE:
          printf("TYPE %s@%s %s@", instruct_type(act_instr->data.address1.type), act_instr->data.address1.value.s, instruct_type(act_instr->data.address2.type));
          print_symb(act_instr->data.address2);
          printf("\n");
      break;

      case INSTRUCT_LABEL:
          printf("LABEL %s\n", act_instr->data.address1.value.s);
      break;

      case INSTRUCT_JUMP:
          printf("JUMP %s\n", act_instr->data.address1.value.s);
      break;

      case INSTRUCT_JUMPIFEQ:
          printf("JUMPIFEQ %s %s@", act_instr->data.address1.value.s, instruct_type(act_instr->data.address2.type));
          print_multiple_symb(act_instr->data.address2);
          printf("%s@", instruct_type(act_instr->data.address3.type));
          print_symb(act_instr->data.address3);
          printf("\n");
      break;

      case INSTRUCT_JUMPIFNEQ:
          printf("JUMPIFNEQ %s %s@", act_instr->data.address1.value.s, instruct_type(act_instr->data.address2.type));
          print_multiple_symb(act_instr->data.address2);
          printf("%s@", instruct_type(act_instr->data.address3.type));
          print_symb(act_instr->data.address3);
          printf("\n");

      break;

      case INSTRUCT_JUMPIFEQS:
          printf("JUMPIFEQS %s\n", act_instr->data.address1.value.s);
      break;

      case INSTRUCT_JUMPIFNEQS:
          printf("JUMPIFNEQS %s\n", act_instr->data.address1.value.s);
      break;

      case INSTRUCT_EXIT:
          printf("EXIT %s@", instruct_type(act_instr->data.address1.type));
          print_symb(act_instr->data.address1);
          printf("\n");
      break;

      case INSTRUCT_DPRINT:
          printf("DPRINT %s@", instruct_type(act_instr->data.address1.type));
          print_symb(act_instr->data.address1);
          printf("\n");
      break;

      case INSTRUCT_LENGTH:
          printf("LABEL length\n");
          printf("STRLEN GF$$var_integer LF@_param1\n");
          printf("RETURN\n");
      break;


      case INSTRUCT_INPUT_S:
           printf("LABEL inputs\n");
           printf("READ GF@$$var_string string\n");
           printf("RETURN\n");
      break;
      case INSTRUCT_INPUT_I:
           printf("LABEL inputi\n");
           printf("READ GF@$$var_integer int\n");
           printf("RETURN\n");
      break;
      case INSTRUCT_INPUT_F:
           printf("LABEL inputf\n");
           printf("READ GF@$$var_double float\n");
           printf("RETURN\n");
      break;

      case INSTRUCT_CHR:
          printf("LABEL chr\n");
          printf("INT2CHAR GF@$$var_string LF@_param1\n");
          printf("RETURN\n");
      break;

      case INSTRUCT_ORD:
          printf("LABEL ord\n");
          printf("STRLEN $$var_integer LF@_param1\n");
          printf("LT GF@$$var_double GF@$$var_integer LF@_param2\n");
          printf("JUMPIFEQ label_ord bool@true GF@$$var_double\n");

          printf("SUB LF@_param2 LF@_param2 int@1\n");
          printf("GETCHAR GF@$$var_string LF@_param1 LF@_param2\n");
          printf("STRI2INT GF@$$var_integer GF@$$var_string int@0\n");

          printf("JUMP label_end_ord\n");
          printf("LABEL label_ord\n");

          printf("MOVE GF@$$var_integer int@0\n");
          printf("LABEL label_end_ord\n");
          printf("MOVE GF@$$var_double float@0x0p+0\n");
          printf("RETURN\n");

      break;


      case INSTRUCT_PRINT:
          // while pocet parametru ... zatim vypisu jen jeden parametr
          //printf("Terms count %d\n", argCount);
          for (int i=1; i <= argCount; i++){
              printf("WRITE TF@$_param%d\n", i);
          }
      break;

      case INSTRUCT_SUBSTR:
          printf("LABEL substr\n");
          printf("STRLEN $$var_integer LF@_param2\n");
          printf("LT GF@$$var_double GF@$$var_integer LF@_param1\n"); // i out of 0 - length(s)
          printf("JUMPIFEQ label_substr bool@true GF@$$var_double\n");

          printf("MOVE GF@$$var_integer int@0\n");
          printf("LT GF@$$var_integer LF@_param3 GF@$$var_integer\n");  // n < 0
          printf("JUMPIFEQ label_substr bool@true GF@$$var_integer\n");
          printf("MOVE GF@$$var_integer int@0\n");  // GF@var_int = 0

          printf("SUB LF@_param2 LF@_param2 int@1\n");  //index 0

          printf("LABEL label_substr_read\n");
          printf("EQ GF@$$var_integer LF@_param3 int@0\n"); // while n == 0 -> end of loop
            printf("JUMPIFEQ label_substr bool@true GF@$$var_integer\n");
            printf("GETCHAR GF@$$var_string LF@_param1 LF@_param2\n");
            printf("CONCAT GF@$result GF@$result GF@$$var_string\n");
            printf("SUB LF@_param3 LF@_param3 int@1\n");  //n--
          printf("JUMP label_substr_read\n");

          printf("LABEL label_substr\n");

          printf("MOVE GF@$$var_integer int@0\n");
          printf("LABEl label_end_substr\n");
          printf("MOVE GF@$$var_double float@0x0p+0\n");
          printf("MOVE GF@$$var_string string@\n");
          printf("RETURN\n");
      break;

      case INSTRUCT_WHILE_START:
          printf("LABEL while_label%d\n",++while_count);
      break;
      case INSTRUCT_WHILE_STATS:
          printf("JUMPIFEQ while_label%d_end GF@$$EXPR bool@false\n", while_count);
      break;

      case INSTRUCT_WHILE_END:
          printf("JUMP while_label%d\n", while_count);
          printf("LABEL while_label%d_end\n",while_count--);
      break;

      case INSTRUCT_IF_THEN:
          printf("JUMPIFEQ if_label_then%d GF@$$EXPR bool@true\n", ++if_count);
          printf("JUMP if_label_else%d\n", if_count);
          printf("LABEL if_label_then%d\n", if_count);
      break;

      case INSTRUCT_IF_ELSE:
          printf("LABEL if_label_else%d\n", if_count);
      break;

      case INSTRUCT_JUMP_ENDIF:
          printf("JUMP if_label%d_end\n", if_count);
      break;

      case INSTRUCT_ENDIF:
          printf("LABEL if_label%d_end\n", if_count);
      break;


    }
  }
}
