/*
 * Tabulka s rozptýlenými položkami
 *
 * S využitím datových typů ze souboru hashtable.h a připravených koster
 * funkcí implementujte tabulku s rozptýlenými položkami s explicitně
 * zretězenými synonymy.
 *
 * Při implementaci uvažujte velikost tabulky HT_SIZE.
 */

#include "hashtable.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int HT_SIZE = MAX_HT_SIZE;

/*
 * Rozptylovací funkce která přidělí zadanému klíči index z intervalu
 * <0,HT_SIZE-1>. Ideální rozptylovací funkce by měla rozprostírat klíče
 * rovnoměrně po všech indexech. Zamyslete sa nad kvalitou zvolené funkce.
 */
int get_hash(char *key)
{
  /**
   * @brief NOTE
   *    To make quality of hash function a little better I decided to use
   *    SDBM hash function I found on internet.
   *    I have a good experience with very similar function from IJC.
   */
  unsigned hash = 0;

  for (unsigned idx = 0; key[idx] != '\0'; idx++)
    hash = (unsigned char)key[idx] + (hash << 6) + (hash << 16) - hash;

  return (int)(hash % HT_SIZE);
}

/*
 * Inicializace tabulky — zavolá sa před prvním použitím tabulky.
 */
void ht_init(ht_table_t *table)
{
  // Argument check to prevent NULL derefence
  if (table == NULL)
  {
    return;
  }

  for (unsigned idx = 0; idx < HT_SIZE; idx++)
  {
    (*table)[idx] = NULL;
  }
}

/*
 * Vyhledání prvku v tabulce.
 *
 * V případě úspěchu vrací ukazatel na nalezený prvek; v opačném případě vrací
 * hodnotu NULL.
 */
ht_item_t *ht_search(ht_table_t *table, char *key)
{
  // Argument check to prevent NULL derefence
  if (table == NULL || key == NULL)
  {
    return NULL;
  }

  // Index of key in hash table
  int idem_idx = get_hash(key);
  ht_item_t *item_to_return = (*table)[idem_idx];

  // Searching through the linked list for an item with matching key
  while (item_to_return != NULL)
  {
    if (!strcmp(item_to_return->key, key))
    {
      return item_to_return;
    }
    item_to_return = item_to_return->next;
  }

  return NULL;
}

/*
 * Vložení nového prvku do tabulky.
 *
 * Pokud prvek s daným klíčem už v tabulce existuje, nahraďte jeho hodnotu.
 * V případě úspěchu funkce tuto skutečnost indikuje návratovou hodnotou
 * true, v opačném případě (např. při selhání alokace) vrací funkce false.
 *
 * Při implementaci využijte funkci ht_search. Pri vkládání prvku do seznamu
 * synonym zvolte nejefektivnější možnost a vložte prvek na začátek seznamu.
 */
bool ht_insert(ht_table_t *table, char *key, float value)
{
  // Argument check to prevent NULL derefence
  if (table == NULL || key == NULL)
  {
    return false;
  }

  // Look for such item in the existing table
  ht_item_t *item = ht_search(table, key);

  if (item != NULL) // There already is item with the same key
  {
    item->value = value;
    return true;
  }

  // There isnt item with the same key, lets create a new one
  ht_item_t *new_item = malloc(sizeof(ht_item_t));
  if (new_item == NULL)
  {
    return false;
  }

  // Set its attributes
  new_item->key = key;
  new_item->value = value;

  // Insert new element to the beginning of the linked list
  int new_item_index = get_hash(key);
  new_item->next = (*table)[new_item_index];
  (*table)[new_item_index] = new_item;

  return true;
}

/*
 * Získání hodnoty z tabulky.
 *
 * V případě úspěchu vrací funkce ukazatel na hodnotu prvku, v opačném
 * případě hodnotu NULL.
 *
 * Při implementaci využijte funkci ht_search.
 */
float *ht_get(ht_table_t *table, char *key)
{
  // Argument check to prevent NULL derefence
  if (table == NULL || key == NULL)
  {
    return NULL;
  }

  // Look for an item with such key in the existing table
  ht_item_t *tmp = ht_search(table, key);

  if (tmp != NULL) // item with was found
  {
    float *value_ptr = &tmp->value;
    return value_ptr;
  }

  // There isnt item in hash table with such key
  return NULL;
}

/*
 * Smazání prvku z tabulky.
 *
 * Funkce korektně uvolní všechny alokované zdroje přiřazené k danému prvku.
 * Pokud prvek neexistuje, funkce nedělá nic.
 *
 * Při implementaci NEPOUŽÍVEJTE funkci ht_search.
 */
void ht_delete(ht_table_t *table, char *key)
{
  // Argument check to prevent NULL derefence
  if (table == NULL || key == NULL)
  {
    return;
  }

  // Index of the item to be deleted
  int to_delete_idx = get_hash(key);
  // Helpful temporary varaibles
  ht_item_t *previous = NULL;
  ht_item_t *cur_item = (*table)[to_delete_idx];

  // Search through linked list for item with the same key
  while (cur_item != NULL && strcmp(cur_item->key, key))
  {
    previous = cur_item;
    cur_item = cur_item->next;
  }

  if (cur_item == NULL || strcmp(cur_item->key, key)) // In hash table there isnt any item on this index
  {
    return;
  }
  else // After the while loop terminated, there should be item to delete stored in cur_item
  {
    if (previous == NULL)
    {
      (*table)[to_delete_idx] = cur_item->next;
    }
    else
    {
      previous->next = cur_item->next;
    }

    free(cur_item);
    cur_item = NULL;
  }
}

/*
 * Smazání všech prvků z tabulky.
 *
 * Funkce korektně uvolní všechny alokované zdroje a uvede tabulku do stavu po
 * inicializaci.
 */
void ht_delete_all(ht_table_t *table)
{
  // Argument check to prevent NULL derefence
  if (table == NULL)
  {
    return;
  }

  // Helpful temporary varaibles
  ht_item_t *previous = NULL;
  ht_item_t *cur_item = NULL;

  for (unsigned idx = 0; idx < HT_SIZE; idx++) // Iterating through each index in hash table
  {
    // Update the current item pointer
    cur_item = (*table)[idx];

    while (cur_item != NULL) // Iterating through linked list on each index of hash table
    {
      previous = cur_item;       // Update the previous item pointer
      cur_item = cur_item->next; // Advance
      free(previous);            // Free item
      previous = NULL;
    }

    // After the while loop terminated there could be one more item to delete
    if (previous != NULL)
    {
      free(previous);
    }

    // Update previous pointer
    previous = NULL;

    // Set hash table to initial state
    (*table)[idx] = NULL;
  }
}
