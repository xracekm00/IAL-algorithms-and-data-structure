/*
 * Použití binárních vyhledávacích stromů.
 *
 * S využitím Vámi implementovaného binárního vyhledávacího stromu (soubory ../iter/btree.c a ../rec/btree.c)
 * implementujte triviální funkci letter_count. Všimněte si, že výstupní strom může být značně degradovaný
 * (až na úroveň lineárního seznamu). Jako typ hodnoty v uzlu stromu využijte 'INTEGER'.
 *
 */

#include "../btree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Vypočítání frekvence výskytů znaků ve vstupním řetězci.
 *
 * Funkce inicilializuje strom a následně zjistí počet výskytů znaků a-z (case insensitive), znaku
 * mezery ' ', a ostatních znaků (ve stromu reprezentováno znakem podtržítka '_'). Výstup je
 * ukládán průběžně do stromu (klíč vždy lowercase). V případě úspěchu funkce tuto skutečnost
 * indikuje návratovou hodnotou true, v opačném případě (např. při selhání
 * operace insert) vrací funkce false.
 *
 * Například pro vstupní řetězec: "abBccc_ 123 *" bude strom po běhu funkce obsahovat:
 *
 * key | value
 * 'a'     1
 * 'b'     2
 * 'c'     3
 * ' '     2
 * '_'     5
 *
 * Pro implementaci si můžete v tomto souboru nadefinovat vlastní pomocné funkce.
 */
bool letter_count(bst_node_t **tree, char *input)
{
  // Initializing tree
  bst_init(tree);

  // Temporary bst_node_content_t variable so I can call bst_search function
  bst_node_content_t *tmp = NULL;

  // Iterating through all characters of input string
  for (unsigned idx = 0; input[idx] != '\0'; idx++)
  {
    char element = input[idx];

    if (isalpha(element))
    {
      element = tolower(element);
    }
    else if (element != ' ')
    {
      element = '_';
    }
    else
    {
      element = ' ';
    }

    // Set tmp value back to NULL
    tmp = NULL;

    if (bst_search(*tree, element, &tmp)) // Node with such key exists
    {
      int *value_ptr = (int *)tmp->value;
      (*value_ptr)++;
    }
    else // There isnt node with such key
    {
      // Allocating memory for data (element counter)
      int *counter = malloc(sizeof(int));
      if (counter == NULL)
      {
        return false;
      }

      // Initialize counter value to 1
      *counter = 1;

      // Create and assign corresponding value to node data
      bst_node_content_t data;
      data.type = INTEGER;
      data.value = counter;

      // Insert new node into tree
      if (!bst_insert(tree, element, data))
      {
        // When insertion failed we have to free allocated memory
        free(counter);

        return false;
      }
    }
  }

  return true;
}