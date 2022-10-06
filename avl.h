#ifndef __AVL_H__
#define __AVL_H__
#include "bst.h"

typedef struct avl AVL;
// constructor: initializes the tree with data pointing to (void*)INT_MIN with is considered to be empty
// NULL wasnt used becasue if data was to be interpret as a arithmetic value, NULL would be 0
Collection *const avl_new();
AVL *const        rotate_left(AVL *const root);
AVL *const        rotate_right(AVL *const root);
AVL *const        rotate_left_right(AVL *const root);
AVL *const        rotate_right_left(AVL *const root);

#endif // __AVL_H__