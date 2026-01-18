/*
 *  Předmět: Algoritmy (IAL) - FIT VUT v Brně
 *  Rozšíření pro příklad c206.c (Dvousměrně vázaný lineární seznam)
 *  Vytvořil: Daniel Dolejška, září 2024
 */

#include "c206-ext.h"

bool error_flag;
bool solved;

/**
 * Tato metoda simuluje příjem síťových paketů s určenou úrovní priority.
 * Přijaté pakety jsou zařazeny do odpovídajících front dle jejich priorit.
 * "Fronty" jsou v tomto cvičení reprezentovány dvousměrně vázanými seznamy
 * - ty totiž umožňují snazší úpravy pro již zařazené položky.
 * 
 * Parametr `packetLists` je dvousměrně vázaný seznam a jako položky obsahuje
 * jednotlivé seznamy (fronty) paketů (`QosPacketListPtr`). Pokud fronta
 * s odpovídající prioritou neexistuje, tato funkce ji alokuje a inicializuje.
 * Za jejich korektní uvolnení odpovídá volající.
 * 
 * V případě, že by po zařazení paketu do seznamu počet prvků v cílovém seznamu
 * překročil stanovený MAX_PACKET_COUNT, dojde nejdříve k promazání položek seznamu.
 * V takovémto případě bude každá druhá položka ze seznamu zahozena nehledě
 * na její vlastní prioritu ovšem v pořadí přijetí.
 * 
 * @param packetLists Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param packet Ukazatel na strukturu přijatého paketu
 */

void receive_packet(DLList *packetLists, PacketPtr packet){
	/** NOTE
	 *  To make sure that doubly-linked list is implemented as a queue
	 *  operations like insertLast and popFirst will be needed
	 */

	//First let's try to find queue with corresponding packets priority
	DLLElementPtr currentElment = packetLists->firstElement;
	QosPacketListPtr currentQue = NULL;
   	QosPacketListPtr correctQue = NULL;
	
	//Iterating through DLList
	while (currentElment){
		/** NOTE
		 *  Each element of 'packetLists' is header of another queue
		 * 	implemented as a 'QosPacketListPtr'. Therefore the top pointer
		 *  has to be "hidden" inside the 'currentElment->data'. 
		 *  Its data type should have enough space to hold an address.
		 */
		currentQue = (QosPacketListPtr)currentElment->data;
		//Let's check the queue's priority
		if (currentQue->priority == packet->priority){
			correctQue = currentQue;
			break;
		}
		//Else move to the next element
		currentElment = currentElment->nextElement;
	}

	//When appropriate queue wasn't found, it doesn't exist and we have to create it
	if (!correctQue){
		//Memory allocation
		if ((correctQue = malloc(sizeof(QosPacketList))) == NULL){
			/** NOTE
			 *  I'm note sure how to handle errors since the function
			 *  doesn't have a return type
			 */
			return;
		}
		//Sets information of the new queue
		correctQue->priority = packet->priority;
		if ((correctQue->list = malloc(sizeof(DLList))) == NULL){
			//Same situation as before, but to prevent memory leaks, 'correspondingQue' has to be freed
			free(correctQue);
			correctQue = NULL;
			return;
		}
		DLL_Init(correctQue->list);

		//Add the queue to the packetLists
		DLL_InsertFirst(packetLists, (long)correctQue);
	}
	
	//Now that it's been figured out which queue the packet belongs to, let's insert him
	DLL_InsertLast(correctQue->list, (long)packet);

	//When the MAX_PACKET_COUNT of the que has been exceeded, every other one has to be deleted
	if (correctQue->list->currentLength > MAX_PACKET_COUNT){
		//Set firstElement to be the active one
		DLL_First(correctQue->list);

		//Iterating through all elements and deleting everyother one
		while (correctQue->list->activeElement){
			DLL_DeleteAfter(correctQue->list);
			DLL_Next(correctQue->list);
		}
	}
}

/**
 * Tato metoda simuluje výběr síťových paketů k odeslání. Výběr respektuje
 * relativní priority paketů mezi sebou, kde pakety s nejvyšší prioritou
 * jsou vždy odeslány nejdříve. Odesílání dále respektuje pořadí, ve kterém
 * byly pakety přijaty metodou `receive_packet`.
 * 
 * Odeslané pakety jsou ze zdrojového seznamu při odeslání odstraněny.
 * 
 * Parametr `packetLists` obsahuje ukazatele na jednotlivé seznamy paketů (`QosPacketListPtr`).
 * Parametr `outputPacketList` obsahuje ukazatele na odeslané pakety (`PacketPtr`).
 * 
 * @param packetLists Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param outputPacketList Ukazatel na seznam paketů k odeslání
 * @param maxPacketCount Maximální počet paketů k odeslání
 */
void send_packets(DLList *packetLists, DLList *outputPacketList, int maxPacketCount){
	//Temp variables
	int sentCount = 0;
	DLLElementPtr currentElment;
	QosPacketListPtr currentQueue;
	QosPacketListPtr highestPriorityQueue;
	int highestPriority;
	long packetPtr;
	
	//Collecting as many packets as possible
	while (sentCount < maxPacketCount){
		//Anulating tracking variables
		currentElment = packetLists->firstElement;
		highestPriorityQueue = NULL;
		highestPriority = -200; //This number is intentionally smaller than char can store

		//Finding queue with highest priority
		while (currentElment){
			//Again the top pointer of the queue is "hidden" in the data variable
			currentQueue = (QosPacketListPtr)currentElment->data;

			//When que isn't empty and it's priority is higher
			if (currentQueue->list->currentLength && currentQueue->priority > highestPriority){
				highestPriority = currentQueue->priority;
				highestPriorityQueue = currentQueue;
			}
			//Advancing forward
			currentElment = currentElment->nextElement;
		}
		
		/** NOTE
		 *  After the while loop executed, there are only two options
		 *  a) packetLists is empty or in its queues there are no elements
		 *  b) que was found
		 */

		//Handeling the first situation
		if (highestPriorityQueue == NULL){
			break;
		}

		//Handeling the second situation
		DLL_GetFirst(highestPriorityQueue->list, &packetPtr);
		DLL_DeleteFirst(highestPriorityQueue->list);
		DLL_InsertLast(outputPacketList, packetPtr);

		//Can't forget to incement the count of packets sent
		sentCount++;
	}
}
