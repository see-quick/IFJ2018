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
  tInstructionData instr4;
  tInstructionData instr5;
  tInstructionData instr6;
  tInstructionTypes instr_type;

  tNode *tmp_instr1;
  tNode *tmp_instr2;

  instr1.type = I;
  instr1.value.i = 5;
  instr2.type = LF;
  instr2.value.s = "var";
  instr3.type = F;
  instr3.value.d = -3.2;

  instr4.type = I;
  instr4.value.i = 40;
  instr5.type = F;
  instr5.value.d = 20.798;
  instr6.type = GF;
  instr6.value.s = "tmp";

  instr_type = INSTRUCT_ADD;

  /*  Inserting into list */
  insert_item(list, &instr_type, &instr1, &instr2, &instr3);
  insert_item(list, &instr_type, &instr4, &instr5, &instr6);
  set_active(list); //nastavime list ako aktivny
  /*  Getting instruction */
  tmp_instr1 = return_instruct(list);
  printf("---TESTING LIST---\n");
  printf("FIRST: %d\n", list->first->data.address3.type);
  printf("ACTIVE: %d\n", list->act->data.address3.type);
  printf("NEXT: %d\n", list->first->next->data.address3.type);
  printf("RETURNED VALUE: %d\n", tmp_instr1->data.address3.type);
  printf("Moving activity...\n");
  /*  Moving activity */
  move_activity(list);
  printf("ACTIVE: %d\n",list->act->data.address3.type);
  /*  Getting instruction */
  tmp_instr2 = return_instruct(list);
  printf("RETURNED VALUE: %d\n", tmp_instr2->data.address3.type);
  printf("LAST NODE OF THE LIST: %d\n", list->last->data.address3.type);
  printf("---END OF TEST---\n\n");
  /*  Print */
  printf("Printing whole list...\n\n");
  print_list_elements(list);

  dispose_list(list);

  free(list);
  return 0;
}
