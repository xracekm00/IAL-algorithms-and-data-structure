/*
 * Binární vyhledávací strom — rekurzivní varianta
 *
 * S využitím datových typů ze souboru btree.h a připravených koster funkcí
 * implementujte binární vyhledávací strom pomocí rekurze.
 */

#include "../btree.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Inicializace stromu.
 *
 * Uživatel musí zajistit, že inicializace se nebude opakovaně volat nad
 * inicializovaným stromem. V opačném případě může dojít k úniku paměti (memory
 * leak). Protože neinicializovaný ukazatel má nedefinovanou hodnotu, není
 * možné toto detekovat ve funkci.
 */
void bst_init(bst_node_t **tree)
{
  // Argument check to prevent NULL derefence
  if (tree != NULL)
  {
    (*tree) = NULL;
  }
}

/*
 * Vyhledání uzlu v stromu.
 *
 * V případě úspěchu vrátí funkce hodnotu true a do proměnné value zapíše
 * ukazatel na obsah daného uzlu. V opačném případě funkce vrátí hodnotu false a proměnná
 * value zůstává nezměněná.
 *
 * Funkci implementujte rekurzivně bez použité vlastních pomocných funkcí.
 */
bool bst_search(bst_node_t *tree, char key, bst_node_content_t **value)
{
  // Argument check to prevent NULL derefence
  // Also the reccursion end
  if (tree == NULL || value == NULL)
  {
    return false;
  }

  // ************ Recursion ************ //
  if (tree->key == key) // Match was found, lets end the reccursion
  {
    // Assign value of the matching node to the function parameter
    (*value) = &tree->content;
    return true;
  }
  else // Keep searching
  {
    if (key < tree->key) // Search left Subtree
    {
      return (bst_search(tree->left, key, value));
    }
    else // Search right Subtree
    {
      return (bst_search(tree->right, key, value));
    }
  }
}

/*
 * Vložení uzlu do stromu.
 *
 * Pokud uzel se zadaným klíče už ve stromu existuje, nahraďte jeho hodnotu.
 * Jinak vložte nový listový uzel. V případě úspěchu funkce tuto skutečnost
 * indikuje návratovou hodnotou true, v opačném případě (např. při selhání
 * alokace) vrací funkce false.
 *
 * Výsledný strom musí splňovat podmínku vyhledávacího stromu — levý podstrom
 * uzlu obsahuje jenom menší klíče, pravý větší.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
bool bst_insert(bst_node_t **tree, char key, bst_node_content_t value)
{
  // Argument check to prevent NULL derefence
  if (tree == NULL)
  {
    return false;
  }

  // Node with such key doesnt exist or the tree is empty, lets create a new node
  if ((*tree) == NULL)
  {
    // Allocating new node
    bst_node_t *new_node = malloc(sizeof(bst_node_t));
    // Check whether the allocation failed
    if (new_node == NULL)
    {
      return false;
    }

    // Initialize atributes of new node
    new_node->key = key;
    new_node->content = value;
    new_node->left = NULL;
    new_node->right = NULL;

    // Insert the new node to the correct position
    (*tree) = new_node;

    return true;
  }

  // ************ Recursion ************ //

  //  Node with matching key was found, lets rewrite its data and end the reccursion
  if (key == (*tree)->key)
  {
    // Free previous content.value
    if ((*tree)->content.type == INTEGER && (*tree)->content.value != NULL)
    {
      free((*tree)->content.value);
      (*tree)->content.value = NULL;
    }

    (*tree)->content = value;
    return true;
  }

  if (key < (*tree)->key) // Keep seaching left
  {
    return (bst_insert(&(*tree)->left, key, value));
  }
  else // Keep seaching right
  {
    return (bst_insert(&(*tree)->right, key, value));
  }
}

/*
 * Pomocná funkce která nahradí uzel nejpravějším potomkem.
 *
 * Klíč a hodnota uzlu target budou nahrazeny klíčem a hodnotou nejpravějšího
 * uzlu podstromu tree. Nejpravější potomek bude odstraněný. Funkce korektně
 * uvolní všechny alokované zdroje odstraněného uzlu.
 *
 * Funkce předpokládá, že hodnota tree není NULL.
 *
 * Tato pomocná funkce bude využitá při implementaci funkce bst_delete.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree)
{
  /**
   * @brief NOTE
   *   Since this function will only be used in bst_delete by me, it is not necesary
   *   to argument check the target value.
   *   I can safely assume it is different from NULL.
   */

  // ************ Recursion ************ //

  if ((*tree)->right == NULL) // The most-right node in subtree was reached, lets end the reccursion
  {
    // Copy the nodes key and data
    target->key = (*tree)->key;
    target->content = (*tree)->content;

    bst_node_t *tmp_left = (*tree)->left; // Node can have left child
    free((*tree));                        // Free memory
    (*tree) = tmp_left;                   // Update the node (NULL / left child subtree)

    return;
  }
  else
  {
    bst_replace_by_rightmost(target, &(*tree)->right);
  }
}

/*
 * Odstranění uzlu ze stromu.
 *
 * Pokud uzel se zadaným klíčem neexistuje, funkce nic nedělá.
 * Pokud má odstraněný uzel jeden podstrom, zdědí ho rodič odstraněného uzlu.
 * Pokud má odstraněný uzel oba podstromy, je nahrazený nejpravějším uzlem
 * levého podstromu. Nejpravější uzel nemusí být listem.
 *
 * Funkce korektně uvolní všechny alokované zdroje odstraněného uzlu.
 *
 * Funkci implementujte rekurzivně pomocí bst_replace_by_rightmost a bez
 * použití vlastních pomocných funkcí.
 */
void bst_delete(bst_node_t **tree, char key)
{
  // Argument check to prevent NULL derefence
  if (tree == NULL || (*tree) == NULL)
  {
    // The tree is empty / node with such key doesnt exist, do nothing
    return;
  }

  // ************ Recursion ************ //
  if (key < (*tree)->key) // Node to be deleted is potentialy located in left subtree
  {
    bst_delete(&(*tree)->left, key);
    return;
  }
  else if (key > (*tree)->key) // Node to be deleted is potentialy located in right subtree
  {
    bst_delete(&(*tree)->right, key);
    return;
  }
  else // The node with matching key was found
  {
    if ((*tree)->right == NULL && (*tree)->left == NULL) // Node to be deleted doesnt have any children
    {
      // Free content
      if ((*tree)->content.type == INTEGER && (*tree)->content.value != NULL)
      {
        free((*tree)->content.value);
        (*tree)->content.value = NULL;
      }

      free((*tree)); // Free memory
      *tree = NULL;  // Update the pointer
      return;
    }
    else if ((*tree)->left != NULL && (*tree)->right == NULL) // Node to be deleted has left child
    {
      bst_node_t *tmp_left = (*tree)->left; // Node can have left child

      // Free content
      if ((*tree)->content.type == INTEGER && (*tree)->content.value != NULL)
      {
        free((*tree)->content.value);
        (*tree)->content.value = NULL;
      }

      free((*tree));                        // Free memory
      (*tree) = tmp_left;                   // Update the node (NULL / left child subtree)
      return;
    }
    else if ((*tree)->left == NULL && (*tree)->right != NULL) // Node to be deleted has right child
    {
      bst_node_t *tmp_right = (*tree)->right; // Node can have right child

      // Free content
      if ((*tree)->content.type == INTEGER && (*tree)->content.value != NULL)
      {
        free((*tree)->content.value);
        (*tree)->content.value = NULL;
      }

      free((*tree));                          // Free memory
      (*tree) = tmp_right;                    // Update the node (NULL / left child subtree)
      return;
    }
    else // Node to be deleted has both children (left and right)
    {
      // Free previous content
      if ((*tree)->content.type == INTEGER && (*tree)->content.value != NULL)
      {
        free((*tree)->content.value);
        (*tree)->content.value = NULL;
      }

      bst_replace_by_rightmost((*tree), &(*tree)->left);
      return;
    }
  }
}

/*
 * Zrušení celého stromu.
 *
 * Po zrušení se celý strom bude nacházet ve stejném stavu jako po
 * inicializaci. Funkce korektně uvolní všechny alokované zdroje rušených
 * uzlů.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_dispose(bst_node_t **tree)
{
  // Argument check to prevent NULL derefence
  if (tree == NULL)
  {
    return;
  }

  // ************ Recursion ************ //
  if ((*tree) != NULL)
  {
    bst_dispose(&(*tree)->left);
    bst_dispose(&(*tree)->right);

    // Free content
    if ((*tree)->content.type == INTEGER && (*tree)->content.value != NULL)
    {
      free((*tree)->content.value);
      (*tree)->content.value = NULL;
    }

    // Free the node itself
    free((*tree));
    (*tree) = NULL;

    return;
  }
}

/*
 * Preorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_preorder(bst_node_t *tree, bst_items_t *items)
{
  // ************ Recursion ************ //
  if (tree != NULL)
  {
    bst_add_node_to_items(tree, items);
    bst_preorder(tree->left, items);
    bst_preorder(tree->right, items);
  }
}

/*
 * Inorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_inorder(bst_node_t *tree, bst_items_t *items)
{
  // ************ Recursion ************ //
  if (tree != NULL)
  {
    bst_inorder(tree->left, items);
    bst_add_node_to_items(tree, items);
    bst_inorder(tree->right, items);
  }
}

/*
 * Postorder průchod stromem.
 *
 * Pro aktuálně zpracovávaný uzel zavolejte funkci bst_add_node_to_items.
 *
 * Funkci implementujte rekurzivně bez použití vlastních pomocných funkcí.
 */
void bst_postorder(bst_node_t *tree, bst_items_t *items)
{
  // ************ Recursion ************ //
  if (tree != NULL)
  {
    bst_postorder(tree->left, items);
    bst_postorder(tree->right, items);
    bst_add_node_to_items(tree, items);
  }
}
