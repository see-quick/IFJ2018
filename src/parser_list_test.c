#include "parser_list.h"


int Parser_list_test(){
	tListOfIDF * list = (tListOfIDF *)malloc(sizeof(tListOfIDF));
	char *promenna;
	
	promenna = "OK";

	listInit(list);

	listInsertLast(list, promenna);

	char * tmp = listGetData(list);
	printf("Test: %s\n", tmp);


	listFree(list);

	return 0;
}