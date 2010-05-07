#ifndef TREE_TYPES_H
#define TREE_TYPES_H

/* For arbitrary trees llink are the children and
 * rlink are the neighbours
 */
typedef struct treeNode {
    int id;
    struct treeNode* llink;
    struct treeNode* rlink;
} TreeNode;


#endif /*TREE_TYPES_H*/
