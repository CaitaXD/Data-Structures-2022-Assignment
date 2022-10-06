#include "bst.h"
#include "bstvtb.h"
#include "defs.h"
#include "stdint.h"

extern int CMPS;
#define EMPTY (void *)INT_MIN

enum direction
{
    LEFT,
    RIGHT
};
enum cmp
{
    EQ = 0,
    LT = -1,
    GT = 1
};
#define left  child[LEFT]
#define right child[RIGHT]
#define next  child[dir]
#define other child[1 - dir]

struct bst
{
    BSTVTB *vtable;
    void   *data;
    BST    *child[2];
    // Same meory location as left and right ptrs
    // BST    *left = child[cmp < 0];
    // BST    *right = child[cmp >= 0];
};

BST *const bst_insert(BST *const bst, void *, comparer);
BST *const bst_find(BST *const bst, void *, comparer);
void       delete_bst(BST *bst);
int        bst_height(BST *const bst);
int        bst_count(BST *const bst);
void       bst_pretty_print(BST *const node, cstringfier tocstr, FILE *const stream);
void       bst_add(BST **const this, void *const data, comparer cmp);
void       bst_inorder(BST *const bst, action action);
void       bst_preorder(BST *const bst, action action);
void       bst_postorder(BST *const bst, action action);
BST       *bst_get_left(BST *const bst) { return bst->left; }
BST       *bst_get_right(BST *const bst) { return bst->right; }

BSTVTB *const bst_vtb_init()
{
    BSTVTB *vtable               = BST_VTABLE(malloc(sizeof(BSTVTB)));
    COL_VTABLE(vtable)->count    = (count_fn)bst_count;
    COL_VTABLE(vtable)->for_each = (for_each_fn)inorder;
    COL_VTABLE(vtable)->find     = (find_fn)bst_find;
    COL_VTABLE(vtable)->insert   = (insert_fn)bst_insert;
    COL_VTABLE(vtable)->delete   = (delete_fn)delete_bst;
    COL_VTABLE(vtable)->add      = (add_fn)bst_add;
    vtable->height               = (height_fn)bst_height;
    vtable->pretty_print         = (print_fn)bst_pretty_print;
    vtable->inorder              = (for_each_fn)bst_inorder;
    vtable->preorder             = (for_each_fn)bst_preorder;
    vtable->postorder            = (for_each_fn)bst_postorder;
    return vtable;
};

Collection *const bst_new()
{
    if (!BST_VTABLE) BST_VTABLE = bst_vtb_init();
    BST *bst    = (BST *)malloc(sizeof(BST));
    bst->data   = EMPTY;
    bst->left   = NULL;
    bst->right  = NULL;
    bst->vtable = BST_VTABLE;
    return (Collection *)bst;
}
void delete_bst(BST *bst)
{
    if (!bst) return;
    if (bst->left) delete_bst(bst->left);
    if (bst->right) delete_bst(bst->right);
    free(bst);
}

void inorder(BST *const bst, action action) { return bst->vtable->inorder((Collection *)bst, action); }
void preorder(BST *const bst, action action) { return bst->vtable->preorder((Collection *)bst, action); }
void postorder(BST *const bst, action action) { return bst->vtable->postorder((Collection *)bst, action); }
void pretty_print(BST *const bst, cstringfier tocstr, FILE *const stream)
{
    return bst->vtable->pretty_print(bst, tocstr, stream);
}
void _pretty_print(string prefix, BST *const node, cstringfier tocstr, bool isLeft, FILE *const stream)
{
    char *BUFFER;
    if (node != NULL)
    {
        BST *const bst = node;
        fprintf(stream, prefix);
        fprintf(stream, isLeft ? "├──" : "└──");

        // print the value of the node
        fprintf(stream, "%s\n", tocstr(node->data));

        // enter the get_child tree level - left and right branch
        BUFFER = calloc(1, strlen(prefix) + 4);
        strcpy(BUFFER, prefix);
        int l = strlen(BUFFER);
        strcpy(&BUFFER[l], isLeft ? "│   " : "    ");
        _pretty_print(BUFFER, node->left, tocstr, true, stream);
        strcpy(BUFFER, prefix);
        strcpy(&BUFFER[l], isLeft ? "│   " : "    ");
        _pretty_print(BUFFER, node->right, tocstr, true, stream);
    }
}
void bst_pretty_print(BST *const node, cstringfier tocstr, FILE *const stream)
{
    _pretty_print("", node, tocstr, false, stream);
}
BST *const bst_insert(BST *const bst, void *const data, comparer cmp)
{
    if (!bst)
    {
        BST *n  = (BST *)bst_new();
        n->data = data;
        return n;
    }
    if (bst->data == EMPTY)
    {
        bst->data = data;
        return bst;
    }
    enum direction dir = cmp(data, bst->data) >= 0;
    bst->next          = bst_insert(bst->next, data, cmp);
    return bst;
}
int bst_height(BST *const bst)
{
    if (bst == NULL) return 0;
    int leftNode  = bst_height(bst->left);
    int rightNode = bst_height(bst->right);
    return 1 + max(leftNode, rightNode);
}
int bst_count(BST *const bst)
{
    if (bst == NULL) return 0;
    return 1 + bst_count(bst->left) + bst_count(bst->right);
}
void bst_add(BST **const bst, void *const data, comparer cmp) { *bst = bst_insert(*bst, data, cmp); }
BST *const bst_find(BST *const bst, void *const data, comparer cmp)
{
    assert(bst->data != EMPTY && bst->data != NULL);
    for (BST *p = bst; p;)
    {
        enum cmp dir = cmp(data, p->data);
        CMPS++; // Null check

        if (dir == EQ)
        {
            CMPS++; // if equals
            return p;
        }
        else
        {
            p = p->child[(dir >= 0)];
        }
    }
    return NULL;
}
void bst_inorder(BST *const bst, action action)
{
    if (bst->left != NULL) bst_inorder(bst->left, action);
    action(bst->data);
    if (bst->right != NULL) bst_inorder(bst->right, action);
}
void bst_preorder(BST *const bst, action action)
{
    action(bst->data);
    if (bst->left != NULL) bst_preorder(bst->left, action);
    if (bst->right != NULL) bst_preorder(bst->right, action);
}
void bst_postorder(BST *const bst, action action)
{
    if (bst->left != NULL) bst_postorder(bst->left, action);
    if (bst->right != NULL) bst_postorder(bst->right, action);
    action(bst->data);
}
int height(BST *const bst) { return bst->vtable->height(bst); }