#ifndef __ABP_H__
#define __ABP_H__
#include "collections.h"
#include "defs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define EMPTY (void *)INT_MIN
typedef struct bst BST;
// constructor: initializes the tree with data pointing to (void*)INT_MIN with is considered to be empty
// NULL wasnt used becasue if data was to be interpret as a arithmetic value, NULL would be 0
Collection *const bst_new();

/**
 * @brief prints the tree in a tree-like format
 * 
 * @param bst BST instance
 * @param stream output stream
 */
void pretty_print(BST *const bst, cstringfier, FILE *const  stream);
/**
 * @brief iterates over the tree in order
 * 
 * @param bst BST instance
 * @param action action to be performed
 */
void inorder(BST *const bst, action action);
/**
 * @brief iterates over the tree in preorder
 * 
 * @param bst BST instance
 * @param action action to be performed
 */
void preorder(BST *const bst, action action);
/**
 * @brief iterates over the tree in postorder
 * 
 * @param bst BST instance
 * @param action action to be performed
 */
void postorder(BST *const bst, action action);
/**
 * @brief gets the height of the tree
 * 
 * @param bst BST instance
 * @return int 
 */
int height(BST *const bst);
#endif // __ABP_H__