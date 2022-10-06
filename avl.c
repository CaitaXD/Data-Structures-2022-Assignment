#include "avl.h"
#include "avlvtb.h"
#include "bstvtb.h"

extern int ROTATIONS;
#define EMPTY (void *)INT_MIN
struct avl
{
    AVLVTB     *vtable;
    const void *data;
    struct avl *child[2];
    // Same meory location as left and right ptrs
    // AVL    *left = child[cmp < 0];
    // AVL    *right = child[cmp >= 0];
    int height;
};
enum direction
{
    LEFT,
    RIGHT
};
#define left  child[LEFT]
#define right child[RIGHT]
#define next  child[dir]
#define other child[1 - dir]

AVL *const avl_insert(AVL *const root, void *const data, comparer cmp);
int        avl_height(AVL *const root);
int        avl_balance(AVL *const root);
AVL *const do_balance_avl(AVL *const root);
int        calcule_height(AVL *const avl);
void       avl_add(AVL **const root, void *const data, comparer cmp);

AVLVTB *const avl_vtb_init()
{
    AVLVTB *vtable             = AVL_VTABLE(bst_vtb_init());
    COL_VTABLE(vtable)->insert = (insert_fn)avl_insert;
    COL_VTABLE(vtable)->add    = (add_fn)avl_add;
    BST_VTABLE(vtable)->height = (height_fn)avl_height;
    return vtable;
}
Collection *const avl_new()
{
    if (!AVL_VTABLE) AVL_VTABLE = avl_vtb_init();
    AVL *avl    = malloc(sizeof(AVL));
    avl->data   = EMPTY;
    avl->left   = NULL;
    avl->right  = NULL;
    avl->height = 0;
    avl->vtable = AVL_VTABLE;
    return (Collection *)avl;
}
#define ever (;;)
void       avl_add(AVL **const avl, void *const data, comparer cmp) { *avl = avl_insert(*avl, data, cmp); }
AVL *const avl_insert(AVL *const avl, void *const data, comparer cmp)
{
    if (!avl)
    {
        AVL *n    = (AVL *)avl_new();
        n->data   = data;
        n->height = 1;
        return n;
    }
    if (avl->data == EMPTY)
    {
        avl->data   = data;
        avl->height = 1;
        return avl;
    }
    enum direction dir = cmp(data, avl->data) >= 0;

    avl->next   = avl_insert(avl->next, data, cmp);
    avl->height = max(avl->height, avl_height(avl->next) + 1);
    return do_balance_avl(avl);
}
AVL *const do_balance_avl(AVL *const root)
{
    int balance = avl_balance(root);
    if (balance > 1 && avl_balance(root->left) >= 0)
        return rotate_right(root);
    else if (balance < -1 && avl_balance(root->right) <= 0)
        return rotate_left(root);
    else if (balance > 1 && avl_balance(root->left) < 0)
        return rotate_left_right(root);
    else if (balance < -1 && avl_balance(root->right) > 0)
        return rotate_right_left(root);
    return root;
}
int avl_balance(AVL *const root) { return root ? avl_height(root->left) - avl_height(root->right) : 0; }
int avl_height(AVL *const root) { return root ? root->height : 0; }
int calcule_height(AVL *const avl)
{
    if (!avl)
        return 0;
    else
        return 1 + max(avl_height(avl->left), avl_height(avl->right));
}
AVL *const rotate_left(AVL *const avl)
{
    ROTATIONS++;
    AVL *rightNode = avl->right;
    AVL *r_left    = rightNode->left;

    rightNode->left = avl;
    avl->right      = r_left;

    avl->height       = calcule_height(avl);
    rightNode->height = calcule_height(rightNode);

    return rightNode;
}
AVL *const rotate_right(AVL *const avl)
{
    ROTATIONS++;
    AVL *leftNode = avl->left;
    AVL *l_right  = leftNode->right;

    leftNode->right = avl;
    avl->left       = l_right;

    avl->height      = calcule_height(avl);
    leftNode->height = calcule_height(leftNode);

    return leftNode;
}
AVL *const rotate_left_right(AVL *const root)
{
    root->left = rotate_left(root->left);
    return rotate_right(root);
}
AVL *const rotate_right_left(AVL *const root)
{
    root->right = rotate_right(root->right);
    return rotate_left(root);
}