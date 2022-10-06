#include "rbt.h"
#include "assert.h"
#include "avlvtb.h"

typedef struct rbt_vtable RBTVTB;
#define RBT_VTABLE(x) CAST(RBTVTB *, x)
#define EMPTY         (void *)INT_MIN

enum cmp
{
    EQ = 0,
    LT = -1,
    GT = 1
};

enum color
{
    black,
    red
};

enum direction
{
    LEFT,
    RIGHT
};

struct rbt_vtable
{
    AVLVTB base;
};

extern int CMPS;
extern int ROTATIONS;

RBT *rbt_insert(RBT *const rbt, void *const data, comparer cmp);
void rbt_add(RBT **const this, void *const data, comparer cmp);
void rbt_pretty_print(RBT *const node, cstringfier tocstr, FILE *const stream);
int  rbt_height(RBT *const rbt);
int  rbt_count(RBT *const rbt);
void rbt_inorder(RBT *const rbt, action action);
void rbt_preorder(RBT *const rbt, action action);
void rbt_postorder(RBT *const rbt, action action);
void rbt_delete(RBT *rbt);
RBT *rbt_find(RBT *const rbt, void *const data, comparer cmp);

RBTVTB *rbt_vtb_init()
{
    RBTVTB *vtable                   = RBT_VTABLE(bst_vtb_init());
    COL_VTABLE(vtable)->insert       = (insert_fn)rbt_insert;
    COL_VTABLE(vtable)->add          = (add_fn)rbt_add;
    COL_VTABLE(vtable)->delete       = (delete_fn)rbt_delete;
    BST_VTABLE(vtable)->pretty_print = (print_fn)rbt_pretty_print;
    COL_VTABLE(vtable)->count        = (count_fn)rbt_count;
    BST_VTABLE(vtable)->height       = (height_fn)rbt_height;
    BST_VTABLE(vtable)->preorder     = (for_each_fn)rbt_preorder;
    BST_VTABLE(vtable)->inorder      = (for_each_fn)rbt_inorder;
    BST_VTABLE(vtable)->postorder    = (for_each_fn)rbt_postorder;
    COL_VTABLE(vtable)->find         = (find_fn)rbt_find;

    return vtable;
}

struct rbt_node
{
    RBTVTB *vtable;
    void   *data;
    RBT    *child[2];
    // Same meory location as left and right ptrs
    // RBT    *left = child[cmp < 0];
    // RBT    *right = child[cmp >= 0];
    RBT *parent;
    byte color;
};

RBTVTB                   *RBT_VTABLE;
RBTVTB                   *rbt_vtb_init();
typedef struct rbt_vtable RBTVTB;
/**
 * @brief Sentinel node for the red black tree.
 * Sentinel allows to check the color direcly.
 * In place of N == NULL || N->color == black.
 * We must however override the height count and find functions.
 */
RBT *const Nil = &(RBT){.color = black, .data = NULL};
#define GET_DIR(N) (N == right((N->parent)) ? RIGHT : LEFT)
#define NIL        Nil // NULL or Sentinel
#define left(N)    N->child[LEFT]
#define right(N)   N->child[RIGHT]
// Be shure to declare a variable named dir iwth the result of a comparison before using this macro
#define get_child(N) N->child[dir]

int rbt_height(RBT *const rbt)
{
    if (rbt == NIL) return 0;
    return 1 + max(rbt_height(left(rbt)), rbt_height(right(rbt)));
}
int rbt_count(RBT *const rbt)
{
    if (rbt == NIL) return 0;
    return 1 + rbt_count(left(rbt)) + rbt_count(right(rbt));
}
void rbt_inorder(RBT *const rbt, action action)
{
    if (rbt == NIL) return;
    rbt_inorder(left(rbt), action);
    action(rbt->data);
    rbt_inorder(right(rbt), action);
}
void rbt_preorder(RBT *const rbt, action action)
{
    if (rbt == NIL) return;
    action(rbt->data);
    rbt_preorder(left(rbt), action);
    rbt_preorder(right(rbt), action);
}
void rbt_postorder(RBT *const rbt, action action)
{
    if (rbt == NIL) return;
    rbt_postorder(left(rbt), action);
    rbt_postorder(right(rbt), action);
    action(rbt->data);
}
void rbt_delete(RBT *rbt)
{
    if (rbt == NIL) return;
    rbt_delete(left(rbt));
    rbt_delete(right(rbt));
    free(rbt);
}

RBT *rbt_find(RBT *const rbt, void *const data, comparer cmp)
{
    assert(rbt->data != EMPTY && rbt->data != NIL);
    for (RBT *p = rbt; p != NIL;)
    {
        CMPS++; // Null check
        enum direction dir = cmp(data, p->data);

        if (dir == EQ)
        {
            CMPS++; // Equality check
            return p;
        }
        else
        {
            p = p->child[(dir >= 0)];
        }
    }
    return NULL;
}

struct rbt_tree
{
    RBTVTB *vtable;
    RBT    *root; // == NIL if tree is empty
};
RBT *rbt_rotate(RBT **treeRoot, RBT *parent, enum direction dir)
{
    ROTATIONS++;
    RBT *granps  = parent->parent;
    RBT *sibling = parent->child[1 - dir];
    RBT *cousin;
    assert(sibling != NIL); // pointer to true node required
    cousin                 = sibling->child[dir];
    parent->child[1 - dir] = cousin;
    if (cousin != NIL) cousin->parent = parent;
    get_child(sibling) = parent;
    parent->parent     = sibling;
    sibling->parent    = granps;
    if (granps != NIL)
        granps->child[parent == right(granps) ? RIGHT : LEFT] = sibling;
    else
        (*treeRoot) = sibling;
    return sibling; // new root of subtree
}

#define IS_BLACK(N) (!N->color) // when not using sentinel N == NULL || N->color == black
#define byte        unsigned char

/**
 * @brief Preserves the red black tree properties.
 * @param treeRoot Pointer to the root of the tree.
 * @param node Node to be inserted.
 * @param dir Direction of the node, i.e it was inserted to the left or right of its parent.
 * Source code: https://en.wikipedia.org/wiki/Red%E2%80%93black_tree#Insertion
 * I don't normally use goto but the algorithm is easier to understand with it.
 */
void rbt_insert_fix(RBT **treeRoot, RBT *node, enum direction dir)
{
    RBT *parent = node->parent;
    RBT *granps;
    RBT *uncle;
    node->color       = red;
    get_child(parent) = node;
    // clang-format off
    for (; node->parent != NIL; parent = node->parent)
    {
        if (parent->color == black)
        {
            // If the parent is black, then the tree is still valid.
            return;
        }
        if ((granps = parent->parent) == NIL)
        {
            // If the parent is red and the parent is the root
            // the tree is still valid, but the root must be black.
            goto CASE4;
        }
        dir   = GET_DIR(parent);
        uncle = granps->child[1 - dir];
        if (IS_BLACK(uncle))
        {
            // If the uncle is black
            // A rotation is required
            goto CASE5;
        }
        parent->color = black;
        uncle->color  = black;
        granps->color = red;
        node          = granps;
    }
    CASE3: // Insertion done
    {
        return;
    }
    CASE4: // Root is red
    {
        parent->color = black;
        return;
    }
    CASE5: // If the node is the right child of the parent 
    {
        if (node == parent->child[1 - dir])
        {
            // Rotate the parent
            rbt_rotate(treeRoot, parent, dir);
            node   = parent;
            parent = granps->child[dir];
        }
    // fall through to CASE6
    CASE6: // Rotate the grandparent and flip the colors

        rbt_rotate(treeRoot, granps, 1 - dir);
        parent->color = black;
        granps->color = red;
        return;
    }
    // clang-format on
}

Collection *rbt_new()
{
    if (!RBT_VTABLE) RBT_VTABLE = rbt_vtb_init();
    RBT *rbt    = malloc(sizeof(RBT));
    rbt->vtable = RBT_VTABLE;
    rbt->data   = EMPTY;
    left(rbt)   = NIL;
    right(rbt)  = NIL;
    rbt->parent = NIL;
    rbt->color  = red;
    return (Collection *)rbt;
}
RBT *rbt_insert(RBT *rbt, void *data, comparer cmp)
{
    if (rbt == NIL)
    {
        rbt        = (RBT *)rbt_new();
        rbt->data  = data;
        rbt->color = black;
        return rbt;
    }
    if (rbt->data == EMPTY)
    {
        rbt->data  = data;
        rbt->color = black;
        return rbt;
    }
    enum direction dir;
    RBT           *node   = (RBT *)rbt_new();
    RBT           *parent = rbt;
    node->data            = data;
    // clang-format off
    for (;
        dir = cmp(data, parent->data) >= 0, 
        parent->child[dir] != NIL;
    ) parent = parent->child[dir];
    node->parent = parent;
    // clang-format on
    rbt_insert_fix(&rbt, node, dir);
    return rbt;
}
void rbt_add(RBT **root, void *data, comparer cmp)
{
    if ((*root) == NIL)
    {
        *root          = (RBT *)rbt_new();
        (*root)->data  = data;
        (*root)->color = black;
        return;
    }
    if ((*root)->data == EMPTY)
    {
        (*root)->data  = data;
        (*root)->color = black;
        return;
    }
    enum direction dir;
    RBT           *node   = (RBT *)rbt_new();
    RBT           *parent = *root;
    node->data            = data;
    // clang-format off
    for (;
        dir = cmp(data, parent->data) >= 0, 
        parent->child[dir] != NIL;
    ) parent = parent->child[dir];
    node->parent = parent;
    // clang-format on
    rbt_insert_fix(root, node, dir);
}

void rbt_prettty_print_recursive(string prefix, RBT *const node, cstringfier tocstr, bool isLeft, FILE *const stream);
void rbt_pretty_print(RBT *const node, cstringfier tocstr, FILE *const stream)
{
    rbt_prettty_print_recursive("", node, tocstr, false, stream);
}
/**
 * @brief Prints the tree in a pretty way.
 * @param prefix Prefix to be printed before the node.
 * @param node Node to be printed.
 * @param tocstr Function to convert the data to a string.
 * @param isLeft Whether the node is a left child or not.
 * @param stream Stream to print to *Note: when printed to stdout red nodes will be collored red.
 */
void rbt_prettty_print_recursive(string prefix, RBT *const node, cstringfier tocstr, bool isLeft, FILE *const stream)
{
    char *BUFFER;
    if (node != NIL)
    {
        RBT *const rbt = node;
        if (stream == stdout && rbt->color == red)
        {
            printf(RED);
        }
        else if (stream == stdout && rbt->color == black)
        {
            printf(RESET);
        }
        fprintf(stream, prefix);
        fprintf(stream, isLeft ? "├──" : "└──");

        // print the value of the node
        if (stream == stdout && rbt->color == red)
        {
            fprintf(stream, "%s\n", tocstr(node->data));
        }
        else if (rbt->color)
        {
            fprintf(stream, "R:%s\n", tocstr(node->data));
        }
        if (stream == stdout && !rbt->color)
        {
            fprintf(stream, "%s\n", tocstr(node->data));
        }
        else if (rbt->color == black)
        {
            fprintf(stream, "B:%s\n", tocstr(node->data));
        }
        // enter the get_child tree level - left and right branch
        BUFFER = malloc(strlen(prefix) + 4);
        strcpy(BUFFER, prefix);
        int l = strlen(BUFFER);
        strcpy(&BUFFER[l], isLeft ? "│   " : "    ");
        rbt_prettty_print_recursive(BUFFER, left(node), tocstr, true, stream);
        strcpy(BUFFER, prefix);
        int l2 = strlen(BUFFER);
        strcpy(&BUFFER[l], isLeft ? "│   " : "    ");
        rbt_prettty_print_recursive(BUFFER, right(node), tocstr, true, stream);
    }
    else
    {
        if (stream == stdout)
        {
            printf(RESET);
        }
        fprintf(stream, prefix);
        fprintf(stream, isLeft ? "├──" : "└──");
        fprintf(stream, "nil\n");
    }
}