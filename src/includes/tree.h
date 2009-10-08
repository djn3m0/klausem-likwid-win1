#ifndef TREE_H
#define TREE_H

#include <types.h>

extern void tree_init(TreeNode** root, int id);
extern void tree_print(TreeNode* nodePtr);
extern void tree_insertNode(TreeNode* nodePtr, int id);
extern int tree_countChildren(TreeNode* nodePtr);
extern void tree_sort(TreeNode* root);
extern TreeNode* tree_getNode(TreeNode* nodePtr, int id);
extern TreeNode* tree_getChildNode(TreeNode* nodePtr);
extern TreeNode* tree_getNextNode(TreeNode* nodePtr);

#endif /*TREE_H*/
