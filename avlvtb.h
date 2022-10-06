/**
 * @file avlvtb.h
 * Virtual Table Declaration.
 * Include this file in any implementation inheriting from AVL.
 */
#ifndef __AVLVTB_H__
#define __AVLVTB_H__

#include "bst.h"
#include "bstvtb.h"
#define AVL_VTABLE(x) CAST(AVLVTB *, x)

typedef struct avl_vtable AVLVTB;
typedef struct avl        AVL;
struct avl_vtable
{
    BSTVTB base;
};

AVLVTB *const avl_vtb_init();
AVLVTB       *AVL_VTABLE;

#endif // __AVLVTB_H__