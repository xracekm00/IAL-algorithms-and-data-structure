/* ******************************* c206.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c206 - Dvousměrně vázaný lineární seznam                            */
/*  Návrh a referenční implementace: Bohuslav Křena, říjen 2001               */
/*  Vytvořil: Martin Tuček, říjen 2004                                        */
/*  Upravil: Kamil Jeřábek, září 2020                                         */
/*           Daniel Dolejška, září 2021                                       */
/*           Daniel Dolejška, září 2022                                       */
/* ************************************************************************** */
/*
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int. Seznam bude jako datová
** abstrakce reprezentován proměnnou typu DLList (DL znamená Doubly-Linked
** a slouží pro odlišení jmen konstant, typů a funkcí od jmen u jednosměrně
** vázaného lineárního seznamu). Definici konstant a typů naleznete
** v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ obousměrně
** vázaný lineární seznam:
**
**      DLL_Init ........... inicializace seznamu před prvním použitím,
**      DLL_Dispose ........ zrušení všech prvků seznamu,
**      DLL_InsertFirst .... vložení prvku na začátek seznamu,
**      DLL_InsertLast ..... vložení prvku na konec seznamu,
**      DLL_First .......... nastavení aktivity na první prvek,
**      DLL_Last ........... nastavení aktivity na poslední prvek,
**      DLL_GetFirst ....... vrací hodnotu prvního prvku,
**      DLL_GetLast ........ vrací hodnotu posledního prvku,
**      DLL_DeleteFirst .... zruší první prvek seznamu,
**      DLL_DeleteLast ..... zruší poslední prvek seznamu,
**      DLL_DeleteAfter .... ruší prvek za aktivním prvkem,
**      DLL_DeleteBefore ... ruší prvek před aktivním prvkem,
**      DLL_InsertAfter .... vloží nový prvek za aktivní prvek seznamu,
**      DLL_InsertBefore ... vloží nový prvek před aktivní prvek seznamu,
**      DLL_GetValue ....... vrací hodnotu aktivního prvku,
**      DLL_SetValue ....... přepíše obsah aktivního prvku novou hodnotou,
**      DLL_Previous ....... posune aktivitu na předchozí prvek seznamu,
**      DLL_Next ........... posune aktivitu na další prvek seznamu,
**      DLL_IsActive ....... zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce explicitně
** uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako procedury
** (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c206.h"

bool error_flag;
bool solved;

/**
 * Vytiskne upozornění na to, že došlo k chybě.
 * Tato funkce bude volána z některých dále implementovaných operací.
 */
void DLL_Error(void) {
	printf("*ERROR* The program has performed an illegal operation.\n");
	error_flag = true;
}

/**
 * Provede inicializaci seznamu list před jeho prvním použitím (tzn. žádná
 * z následujících funkcí nebude volána nad neinicializovaným seznamem).
 * Tato inicializace se nikdy nebude provádět nad již inicializovaným seznamem,
 * a proto tuto možnost neošetřujte.
 * Vždy předpokládejte, že neinicializované proměnné mají nedefinovanou hodnotu.
 *
 * @param list Ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_Init(DLList *list){
	//Sets pointers to NULL and length variable to 0
	list->activeElement = NULL;
	list->firstElement = NULL;
	list->lastElement = NULL;
	list->currentLength = 0;
}

/**
 * Zruší všechny prvky seznamu list a uvede seznam do stavu, v jakém se nacházel
 * po inicializaci.
 * Rušené prvky seznamu budou korektně uvolněny voláním operace free.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Dispose(DLList *list){
	//Works when list is not empty
	if (list->currentLength){
		//Temp variable used for iterating through elements
		DLLElementPtr tempPtr = list->firstElement;
		//Disposing and cleaning after all but the last element
		while (list->currentLength > 1){
			tempPtr = tempPtr->nextElement;
			free(tempPtr->previousElement);
			tempPtr->previousElement = NULL;
			list->currentLength--;
		}
		//When the list has only last element or the while loop finished executing
		free(list->lastElement);
		list->lastElement = NULL;

		//Setts list to initial state
		list->activeElement = NULL;
		list->firstElement = NULL;
		list->lastElement = NULL;
		list->currentLength = 0;
	}
}

/**
 * Vloží nový prvek na začátek seznamu list.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na začátek seznamu
 */
void DLL_InsertFirst(DLList *list, long data){
	//Allocating memory for new element
	DLLElementPtr newElement;
	if ((newElement = malloc(sizeof(struct DLLElement))) == NULL){
		DLL_Error();
		return;
	}
	//Sets all information of new First Element
	newElement->data = data;
	newElement->nextElement = list->firstElement;
	newElement->previousElement = NULL;

	//Has to check whether the list is empty or not
	if (!list->currentLength){
		list->lastElement = newElement;
	}
	//When there's at least one element in the list
	else{
		list->firstElement->previousElement = newElement;
	}
	//Inserts new element to the beginning of DLL
	list->firstElement = newElement;

	//Dont forget to increment the counter
	list->currentLength++;
}

/**
 * Vloží nový prvek na konec seznamu list (symetrická operace k DLL_InsertFirst).
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na konec seznamu
 */
void DLL_InsertLast(DLList *list, long data){
	//Allocating memory for new element
	DLLElementPtr newElement;
	if ((newElement = malloc(sizeof(struct DLLElement))) == NULL){
		DLL_Error();
		return;
	}
	//Sets all information of new Last Element
	newElement->data = data;
	newElement->nextElement = NULL;
	newElement->previousElement = list->lastElement;
	
	//Has to check whether the list is empty or not
	if (!list->currentLength){
		list->firstElement = newElement;
	}
	//When there's at least one element in the list
	else{
		list->lastElement->nextElement = newElement;
	}
	//Inserts new element to the end of DLL
	list->lastElement = newElement;

	//Dont forget to increment the counter
	list->currentLength++;
}

/**
 * Nastaví první prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_First(DLList *list){
	list->activeElement = list->firstElement;
}

/**
 * Nastaví poslední prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Last(DLList *list){
	list->activeElement = list->lastElement;
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu prvního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetFirst(DLList *list, long *dataPtr){
	//Make sure that lsit isn't empty
	if (list->currentLength == 0){
		DLL_Error();
		return;
	}
	//Assigns the first elements value to the parameter
	*dataPtr = list->firstElement->data;
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu posledního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetLast( DLList *list, long *dataPtr ) {
	//Make sure that lsit isn't empty
	if (list->currentLength == 0){
		DLL_Error();
		return;
	}
	//Assigns the last elements value to the parameter
	*dataPtr = list->lastElement->data;
}

/**
 * Zruší první prvek seznamu list.
 * Pokud byl první prvek aktivní, aktivita se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteFirst(DLList *list){
	//Workts only when the list is not empty
	if (list->currentLength){
		//Temp variable storying pointer to the element about to be deleted
		DLLElementPtr toDelete = list->firstElement;

		//If active element was the first element
		if (list->activeElement == list->firstElement){
			list->activeElement = NULL;
		}

		//Sets firstElement pointer to the second element of the list
		list->firstElement = list->firstElement->nextElement;

		//When the list has only one element
		if (list->currentLength == 1){
			list->lastElement = NULL;
		}
		//Otherwise there has to be at least one more element
		else{
			//Sets information of new First Element
            list->firstElement->previousElement = NULL;
        }

		//Frees memory used by the first element
		free(toDelete);
		toDelete = NULL;
		
		//Dont forget to increment the counter
		list->currentLength--;
	}
}

/**
 * Zruší poslední prvek seznamu list.
 * Pokud byl poslední prvek aktivní, aktivita seznamu se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteLast(DLList *list){
	//Workts only when the list is not empty
	if (list->currentLength){
		//Temp variable storying pointer to the element about to be deleted
		DLLElementPtr toDelete = list->lastElement;

		//If active element was the last element
		if (list->activeElement == list->lastElement){
			list->activeElement = NULL;
		}

		//Sets lastElement pointer to the second to last element of the list
		list->lastElement = list->lastElement->previousElement;

		//When the list has only one element
		if (list->currentLength == 1){
			list->firstElement = NULL;
		}
		//Otherwise there has to be at least one more element
		else{
			//Sets information of new Last Element
			list->lastElement->nextElement = NULL;
		}
		
		//Frees memory used by the last element
		free(toDelete);
		toDelete = NULL;

		//Dont forget to increment the counter
		list->currentLength--;
	}
}

/**
 * Zruší prvek seznamu list za aktivním prvkem.
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * posledním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteAfter(DLList *list){
	//Only works when the list is active and the active element isn't the last element
	if (list->activeElement && list->activeElement != list->lastElement){
		//temp variable to make the problem more readable
		DLLElementPtr activeNextElem = list->activeElement->nextElement;

		//When the element being deleted is the last one
		if (list->activeElement->nextElement == list->lastElement){
			list->lastElement = list->activeElement;
		}
		//When the element being deleted isn't the last one
		else{
			activeNextElem->nextElement->previousElement = list->activeElement;
		}
		//Reorganizing list elements
		list->activeElement->nextElement = activeNextElem->nextElement;
		
		//Frees memory used by the element after the active one
		free(activeNextElem);
		activeNextElem = NULL;

		//Dont forget to increment the counter
		list->currentLength--;
	}
}

/**
 * Zruší prvek před aktivním prvkem seznamu list .
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * prvním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteBefore( DLList *list ) {
	//Only works when the list is active and the active element isn't the first element
	if (list->activeElement && list->activeElement != list->firstElement){
		//temp variable to make the problem more readable
		DLLElementPtr activePreviousElement = list->activeElement->previousElement;

		//When the element being deleted is the first one
		if (list->activeElement->previousElement == list->firstElement){
			list->firstElement = list->activeElement;
		}
		//When the element being deleted isn't the first one
		else{
			activePreviousElement->previousElement->nextElement = list->activeElement;
		}
		//Reorganizing list elements
		list->activeElement->previousElement = activePreviousElement->previousElement;

		//Frees memory used by the element before the active one
		free(activePreviousElement);
		activePreviousElement = NULL;

		//Dont forget to increment the counter
		list->currentLength--;
	}
}

/**
 * Vloží prvek za aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu za právě aktivní prvek
 */
void DLL_InsertAfter(DLList *list, long data){
	//Only works when the list is active
	if (list->activeElement){
		//Allocating memory for new element
		DLLElementPtr newElement;
		if ((newElement = malloc(sizeof(struct DLLElement))) == NULL){
			DLL_Error();
			return;
		}
		//Sets information of the new element
		newElement->nextElement = list->activeElement->nextElement;
		newElement->previousElement = list->activeElement;
		newElement->data = data;
		
		//Has to check whether the active element isn't the last one
		if (list->activeElement == list->lastElement){
			list->lastElement = newElement;
		}
		//When there's at least one element in the list
		else{
			list->activeElement->nextElement->previousElement = newElement;
		}
		//Reorganizing surrounding elements
		list->activeElement->nextElement = newElement;
		
		//Dont forget to increment the counter
		list->currentLength++;
	}
}

/**
 * Vloží prvek před aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu před právě aktivní prvek
 */
void DLL_InsertBefore(DLList *list, long data){
	//Only works when the list is active
	if (list->activeElement){
		//Allocating memory for new element
		DLLElementPtr newElement;
		if ((newElement = malloc(sizeof(struct DLLElement))) == NULL){
			DLL_Error();
			return;
		}
		//Sets information of the new element
		newElement->nextElement = list->activeElement;
		newElement->previousElement = list->activeElement->previousElement;
		newElement->data = data;

		//Has to check whether the active element isn't the last one
		if (list->activeElement == list->firstElement){
			list->firstElement = newElement;
		}
		//When there's at least one element in the list
		else{
			list->activeElement->previousElement->nextElement = newElement;
		}
		//Reorganizing surrounding elements
		list->activeElement->previousElement = newElement;
		
		//Dont forget to increment the counter
		list->currentLength++;
	}
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, volá funkci DLL_Error ().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetValue(DLList *list, long *dataPtr){
	//Make sure the list is active
	if (!list->activeElement){
		DLL_Error();
		return;
	}
	//Sets value of activeElement to parameter
	*dataPtr = list->activeElement->data;
}

/**
 * Přepíše obsah aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, nedělá nic.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Nová hodnota právě aktivního prvku
 */
void DLL_SetValue(DLList *list, long data){
	//Works only when list is active
	if (list->activeElement){
		//Updates value of active element
		list->activeElement->data = data;
	}
}

/**
 * Posune aktivitu na následující prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Next(DLList *list){
	//Works only when list is active
	if (list->activeElement){
		//When the active element is the last one
		if (list->activeElement == list->lastElement){
			list->activeElement = NULL;
		}
		//Moves activity to the next element
		else{
			list->activeElement = list->activeElement->nextElement;
		}	
	}
}

/**
 * Posune aktivitu na předchozí prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Previous(DLList *list){
	//Works only when list is active
	if (list->activeElement){
		//When the active element is the first one
		if (list->activeElement == list->firstElement){
			list->activeElement = NULL;
		}
		//Moves activity to the previous element
		else{
			list->activeElement = list->activeElement->previousElement;
		}	
	}
}

/**
 * Je-li seznam list aktivní, vrací nenulovou hodnotu, jinak vrací 0.
 * Funkci je vhodné implementovat jedním příkazem return.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 *
 * @returns Nenulovou hodnotu v případě aktivity prvku seznamu, jinak nulu
 */
bool DLL_IsActive(DLList *list){
	//ternart operator
	return (list->activeElement) ? true:false;
}

/* Konec c206.c */
