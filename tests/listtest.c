/**
 * Predmet:  IFJ
 * Projekt:  Implementace prekladace imperativniho jazyka IFJ18
 * Soubor:   list.c
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

#include "../src/list.c"


int main(int argc, char const *argv[]) {

  tList* list = list_init(instr_file);
  tInstructionData instr1;
  tInstructionData instr2;
  tInstructionData instr3;
  tInstructionTypes instr_type;

  instr1.type = TF;
  instr1.value.i = 5;

  instr2.type = LF;
  instr2.value.s = "var";

  instr3.type = FCE;
  instr3.value.d = -3.2;

  instr_type = INSTRUCT_ADD;

  /*  Inserting into list   */
  insert_item(list, &instr_type, &instr1, &instr2, &instr3);
  insert_item(list, &instr_type, &instr1, &instr2, &instr3);

  /*  Print   */
  print_list_elements(list);

  dispose_list(list);

  free(list);
  return 0;
}
