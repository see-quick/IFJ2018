#include "scanner_list.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;            
    return;
}

void DLInitList (tDLList *L) {
/* Provede inicializaci seznamu L před jeho prvním použitím  */
	L->First = NULL;
	L->Act = NULL;
	L->Last = NULL;
}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/
	while(L->First != NULL) 
	{
		if(L->First == L->Act)  
		{
			L->Act = NULL;
		}
		if(L->First == L->Last) 
		{
			L->Last = NULL;
		}
		tDLElemPtr pomocny = L->First; 
		L->First = L->First->rptr;
		free(pomocny);
	}
}

void DLInsertFirst (tDLList *L, char * val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	tDLElemPtr pomocny = malloc(sizeof(struct tDLElem));
	if(pomocny == NULL) 
	{
		DLError();
	}
	else 
	{
		pomocny->data = val; //nahrame data
		pomocny->is_important = 0;
		pomocny->lptr = NULL; 
		pomocny->rptr = L->First; 
		if(L->Last == NULL) 
		{
			L->Last = pomocny; 
		}
		else 
		{
			L->First->lptr = pomocny; 
		}
		L->First = pomocny; 
	}	
}

void DLInsertLast(tDLList *L, char * val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/ 	
	tDLElemPtr pomocny = malloc(sizeof(struct tDLElem));
	if(pomocny == NULL) 
	{
		DLError();
	}
	else 
	{
		pomocny->data = val; 
		pomocny->rptr = NULL; 
		pomocny->lptr = L->Last; 
		if(L->Last == NULL) 
		{
			L->First = pomocny;
		}
		else
		{
			L->Last->rptr = pomocny;
		}
		L->Last = pomocny;
	}
}

void DLFirst (tDLList *L) {
/*Nastaví aktivitu na první prvek seznamu L. */
	L->Act = L->First;
}

void DLLast (tDLList *L) {
/* Nastaví aktivitu na poslední prvek seznamu L.*/
	L->Act = L->Last;	
}

char * DLCopyFirst (tDLList *L) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	if(L->First == NULL) 
	{
		return NULL;
	}
	else 
	{
		return L->First->data;
	}	
}

void DLCopyLast (tDLList *L, char *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	if(L->First == NULL) 
	{
		DLError();
	}
	else
	{
		val = L->Last->data;
	}	
}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
	if(L->First != NULL) 
	{
		tDLElemPtr pomocny = L->First; 
		if(L->First == L->Last) 
		{
			L->First = NULL;
			L->Last = NULL;
			L->Act = NULL;
		}
		else
		{
			if(L->First == L->Act) 
			{
				L->Act = NULL;
			}
			L->First = pomocny->rptr; 
			L->First->lptr = NULL; 
		}
	free(pomocny);
	}
}	

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/ 
	if(L->First != NULL)
	{
		tDLElemPtr pomocny = L->Last;
		if(L->First == L->Last)
		{
			L->First = NULL;
			L->Last = NULL;
			L->Act = NULL;
		}
		else
		{
			if(L->Last == L->Act) 
			{
				L->Act = NULL;
			}
			L->Last = pomocny->lptr; 
			L->Last->rptr = NULL; 
		}
	free(pomocny);
	}	
}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/
	if(L->Act != NULL && L->Act != L->Last) 
	{
		tDLElemPtr pomocny = L->Act->rptr;
		L->Act->rptr = pomocny->rptr; 
		if(L->Last == pomocny) 
		{
			L->Last = L->Act;
		}
		else 
		{
			pomocny->rptr->lptr = L->Act; 
		}
		free(pomocny);
	}	
}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/
	if(L->Act != NULL && L->Act != L->First)
	{
		tDLElemPtr pomocny = L->Act->lptr;
		L->Act->lptr = pomocny->lptr;
		if(L->First == pomocny) 
		{
			L->First = L->Act;
		}
		else 
		{
			pomocny->lptr->rptr = L->Act; 
		}
		free(pomocny);
	}			
}

void DLPostInsert (tDLList *L, char * val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	if(L->Act != NULL)
	{	
		tDLElemPtr pomocny = malloc(sizeof(struct tDLElem));
		if(pomocny == NULL)
		{
			DLError();
		}
		else
		{
			pomocny->data = val;
			pomocny->lptr = L->Act;
			pomocny->rptr = L->Act->rptr;
			L->Act->rptr = pomocny;
			if(L->Act != L->Last) 
			{
				pomocny->rptr->lptr = pomocny;
			}
			else
			{
				L->Last = pomocny;
			}
		}
	}	
}

void DLPreInsert (tDLList *L, char * val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	if(L->Act != NULL) 
	{	
		tDLElemPtr pomocny = malloc(sizeof(struct tDLElem));
		if(pomocny == NULL)
		{
			DLError();
		}
		else 
		{
			pomocny->data = val;
			pomocny->rptr = L->Act;
			pomocny->lptr = L->Act->lptr;
			L->Act->lptr = pomocny;
			if(L->Act != L->First) 
			{
				pomocny->lptr->rptr = pomocny;
			}
			else
			{
				L->First = pomocny;
			}
		}
	}	
}

void DLCopy (tDLList *L, char *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
	if(L->Act == NULL) 
	{
		DLError();
	}
	else 
	{
		val = L->Act->data;
	}	
}

void DLActualize (tDLList *L, char * val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
	if(L->Act != NULL)
	{
		L->Act->data = val;
	}	
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
**/
	if(L->Act != NULL) 
	{
		if(L->Act == L->Last) 
		{
			L->Act = NULL;
		}
		else 
		{
			L->Act = L->Act->rptr;
		}
	}	
}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
**/
	if(L->Act != NULL) 
	{
		if(L->Act == L->First) 
		{
			L->Act = NULL;
		}
		else 
		{
			L->Act = L->Act->lptr;
		}
	}	
}

int DLActive (tDLList *L) {
/*Je-li seznam aktivní, vrací true. V opačném případě vrací false.*/
	return(L->Act != NULL);
}



void print_elements_of_list(tDLList TL)	{
	
	tDLList TempList=TL;

	int CurrListLength = 0;

	printf("-------TOKEN LIST-------");

	while ((TempList.First!=NULL) && (CurrListLength<100))	{
		// printf("\n \t%s - important: %d",TempList.First->data, TempList.First->is_important);
		printf("\n \t%s %d",TempList.First->data, TempList.First->is_important);
		if ((TempList.First==TL.Act) && (TL.Act!=NULL))
			printf("\t <= toto je aktivní prvek ");
		TempList.First=TempList.First->rptr;
		CurrListLength++;
	}
    if (CurrListLength>=100)
        printf("\nList exceeded maximum length!");
	printf("\n----------------------\n");     
}


void DLIsImportant(tDLList *L){
	L->First->is_important = 1;
}


char * DLFirstImportant(tDLList *L){
	if (L->First == L->Last){
		if (L->First->is_important == 1){
			return L->First->data;
		}
	}
	else{
		while(L->Last->lptr != NULL){
			if (L->Last->is_important == 1){
				return L->Last->data;
			}
			else{
				L->Last = L->Last->lptr;
			}
		}
	}
	
	return NULL;
}

void DLNotImportant(tDLList *L){
	while((L->Last->lptr != NULL)){
		if (L->Last->is_important == 1){
			L->Last->is_important = 0;
			return;
		}
		else{
			L->Last = L->Last->lptr;
		}
	}
}


char * DLLastImportant(tDLList *L){
	if (L->First == L->Last){
		if (L->First->is_important == 1){
			return L->First->data;
		}
	}
	else{
		while((L->First->rptr != NULL)){
			if (L->First->is_important == 1){
				return L->First->data;
			}
			else{
				L->First = L->First->rptr;
			}
		}
	}	
	return NULL;
}
