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
    default:
      printf("TODO\n");
      return "instruct_type():i did not find anything\n";
  }
}
/*
 * Printuje symbol na zaklade jeho typu(int,id,float..)
 * @param instr_operand data a typ instrukcie
 */
void print_symb(tInstructionData instr_operand)  {
  if (instr_operand.type == I)  printf("%d\n",instr_operand.value.i);
  else if (instr_operand.type == F) printf("%f\n",instr_operand.value.d);
  else printf("%s\n",instr_operand.value.s);
}

void print_multiple_symb(tInstructionData instr_operand)  {
  if (instr_operand.type == I)  printf("%d ",instr_operand.value.i);
  else if (instr_operand.type == F) printf("%f ",instr_operand.value.d);
  else printf("%s ",instr_operand.value.s);
}

void print_arit_instr( tNode *act_instr) {
  char *name;
  switch (act_instr->data.type) {
    case INSTRUCT_ADD: name = "ADD"; break;
    case INSTRUCT_SUB: name = "SUB"; break;
    default:
      printf("TODO\n" );
    //case INSTRUCT_INT2FLOAT: name = "INT2FLOAT"; break;
    //TODO remaining states
  }

  printf("%s %s@%s %s@", name, instruct_type(act_instr->data.address1.type), act_instr->data.address1.value.s, instruct_type(act_instr->data.address2.type));
  print_multiple_symb(act_instr->data.address2);
  print_symb(act_instr->data.address3);
}
//
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


      // funguje!
      case INSTRUCT_MOVE:
          printf("MOVE %s@%s %s@\n", instruct_type(act_instr->data.address1.type), act_instr->data.address1.value.s, instruct_type(act_instr->data.address2.type));
          //print_symb(act_instr->data.address2);
      break;

      case INSTRUCT_DEFVAR:
          printf("DEFVAR %s@%s\n", instruct_type(act_instr->data.address1.type), act_instr->data.address1.value.s);
      break;

      case INSTRUCT_CALL:
          printf("CALL %s\n", act_instr->data.address1.value.s);
      break;

      case INSTRUCT_LABEL:
          printf("LABEL %s\n", act_instr->data.address1.value.s);
      break;

      case INSTRUCT_PUSHS:
          printf("PUSHS %s@", instruct_type(act_instr->data.address1.type));
          print_symb(act_instr->data.address1);
      break;

      case INSTRUCT_POPS:
          printf("POP %s@%s\n", instruct_type(act_instr->data.address1.type), act_instr->data.address1.value.s);
      break;

      case INSTRUCT_ADD:
          print_arit_instr(act_instr);
          /*printf("ADD %s@%s %s@", instruct_type(act_instr->data.address1.type), act_instr->data.address1.value.s, instruct_type(act_instr->data.address2.type));
          print_multiple_symb(act_instr->data.address2);
          print_symb(act_instr->data.address3);*/
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
      break;

      case INSTRUCT_INT2FLOAT:
          printf("INT2FLOAT %s@%s %s@", instruct_type(act_instr->data.address1.type), act_instr->data.address1.value.s, instruct_type(act_instr->data.address2.type));
          print_symb(act_instr->data.address2);
      break;

      case INSTRUCT_FLOAT2INT:
          printf("FLOAT2INT %s@%s %s@", instruct_type(act_instr->data.address1.type), act_instr->data.address1.value.s, instruct_type(act_instr->data.address2.type));
          print_symb(act_instr->data.address2);
      break;

      case INSTRUCT_INT2CHAR:
          printf("INT2CHAR %s@%s %s@", instruct_type(act_instr->data.address1.type), act_instr->data.address1.value.s, instruct_type(act_instr->data.address2.type));
          print_symb(act_instr->data.address2);
      break;

      case INSTRUCT_STRI2INT:
          print_arit_instr(act_instr);
      break;

      case INSTRUCT_INT2FLOATS:
          printf("INT2FLOATS %s@%s %s@", instruct_type(act_instr->data.address1.type), act_instr->data.address1.value.s, instruct_type(act_instr->data.address2.type));
          print_symb(act_instr->data.address2);
      break;

      case INSTRUCT_FLOAT2INTS:
          printf("INT2FLOAT %s@%s %s@", instruct_type(act_instr->data.address1.type), act_instr->data.address1.value.s, instruct_type(act_instr->data.address2.type));
          print_symb(act_instr->data.address2);
      break;

      case INSTRUCT_INT2CHARS:
          printf("INT2CHARS %s@%s %s@", instruct_type(act_instr->data.address1.type), act_instr->data.address1.value.s, instruct_type(act_instr->data.address2.type));
          print_symb(act_instr->data.address2);
      break;

      case INSTRUCT_STRI2INTS:
          print_arit_instr(act_instr);
      break;

      default:
        printf("TODO\n");

      /*case INSTRUCT_READ:
          printf("READ %s@%s %s\n", instruct_type(act_instr->data.address1.type), act_instr->data.address1.value.s, );
          */
      //...TODO remaining states
    }
  }
}
