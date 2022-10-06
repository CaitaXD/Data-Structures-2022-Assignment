/**
 * @file bstvtb.h
 * Virtual Table Declaration.
 * Include this file in any implementation inheriting from BST.
 */
#ifndef __BSTVTB_H__
#define __BSTVTB_H__

#include "collections.h"
#include "colvtb.h"
#include "defs.h"
#include <stdio.h>


#define BST_VTABLE(x) ((BSTVTB *)x)

typedef struct bst_vtable BSTVTB;
typedef struct bst        BST;
typedef int (*height_fn)(BST *const this);
typedef void (*print_fn)(BST *const this, cstringfier tocstr, FILE *const stream);
typedef BST *(*get_left_fn)(BST *const this);
typedef BST *(*get_right_fn)(BST *const this);

BSTVTB       *BST_VTABLE;
BSTVTB *const bst_vtb_init();

struct bst_vtable
{
    COLVTB       base;
    height_fn    height;
    print_fn     pretty_print;
    for_each_fn  inorder;
    for_each_fn  preorder;
    for_each_fn  postorder;
};

#endif // __BSTVTB_H__
