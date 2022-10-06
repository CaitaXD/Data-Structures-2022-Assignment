/**
 * @file colvtb.h
 * Virtual Table Declaration.
 * Include this file in any implementation inheriting from Collection. 
 */
#ifndef __COLVTB_H__
#define __COLVTB_H__
#include "defs.h"
#define COL_VTABLE(this) CAST(COLVTB *const, this)

typedef struct collection Collection;
typedef void (*delete_fn)(Collection *const this);
typedef Collection *(*insert_fn)(Collection *const this, void *const data, comparer cmp);
typedef void (*remove_from_fn)(Collection *const this, void *const data);
typedef void (*for_each_fn)(Collection *const this, action action);
typedef int (*count_fn)(Collection *const this);
typedef Collection *(*find_fn)(Collection *const this, void *const data, comparer cmp);
typedef int (*index_of_fn)(Collection *const this, void *const data, comparer cmp);
typedef void (*add_fn)(Collection **const  this, void *const data, comparer cmp);
typedef struct COLVTB
{
    delete_fn delete;
    insert_fn   insert;
    for_each_fn for_each;
    count_fn    count;
    find_fn     find;
    index_of_fn index_of;
    add_fn      add;
} COLVTB;

struct collection
{
    COLVTB     *vtable;
    void *const data;
};

#endif // __COLVTB_H__