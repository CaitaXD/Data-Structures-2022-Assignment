#ifndef __Collection_H__
#define __Collection_H__
#include "colvtb.h"
#include "defs.h"
#include <stdbool.h>

/*
 *  Virtual function declarations
 */

/**
 * @brief frees all the momory allocated for this data structure
 *
 * @param this Collection instance
 * @param free_data if true, frees the data stored in the collection
 */
static void delete(Collection *const this) { this->vtable->delete (this); }
/**
 * @brief inserts an element on to the Collection
 *
 * @param this Collection instance
 * @param data value to be inserted
 * @param cmp function to compare two elements
 * @return the root of the collection
 */
static Collection *insert(Collection *const this, void *const data, comparer cmp)
{
    return this->vtable->insert(this, data, cmp);
}
/**
 * @brief adds an element to the Collection
 * 
 * @param this Collection instance
 * @param data value to be added
 * @param cmp function to compare two elements
 */
static void add(Collection **const this, void *const data, comparer cmp)
{
    return (*this)->vtable->add(this, data, cmp);
}
/**
 * @brief finds the first occurence of the given data
 *
 * @param Collection Collection instance
 * @param data value to be found
 * @param cmp comparer function
 * @return Collection* the node containing the data
 */
static Collection *find(Collection *const this, void *const data, comparer cmp)
{
    return this->vtable->find(this, data, cmp);
}
/**
 * @brief Calls action function pointer once for each element of the Collection
 *
 * @param this Collection instance
 * @param action action to be performed
 */
static void for_each(Collection *const this, action action) { this->vtable->for_each(this, action); }
/**
 * @brief counts the number of elements in the Collection
 *
 * @param this Collection instance
 * @return int number of elements
 */
static int count(Collection *bst) { return ((count_fn)bst->vtable->count)(bst); }

#endif // __Collection_H__