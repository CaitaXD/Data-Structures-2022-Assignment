#ifndef __RBT_H__
#define __RBT_H__

#include "avl.h"
typedef struct rbt_node RBT;
#define EMPTY (void *)INT_MIN
// constructor: initializes the tree with data pointing to (void*)INT_MIN with is considered to be empty
// NULL wasnt used becasue if data was to be interpret as a arithmetic value, NULL would be 0
Collection *rbt_new();

#endif // __RBT_H__