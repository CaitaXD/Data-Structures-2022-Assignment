#ifndef __AVL_H__
#define __AVL_H__
#include "bst.h"

typedef struct avl AVL;

Collection *const avl_new();
AVL *const        rotate_left(AVL *const root);
AVL *const        rotate_right(AVL *const root);
AVL *const        rotate_left_right(AVL *const root);
AVL *const        rotate_right_left(AVL *const root);

#endif // __AVL_H__