#ifndef STRAIN_ARCHIVE_BST_H
#define STRAIN_ARCHIVE_BST_H

#include "common.h"

StrainNode* bst_create_strain_node(const char* name, int id);
StrainNode* bst_insert_strain(StrainNode* root, const char* name, int id);
StrainNode* bst_search_strain(StrainNode* root, const char* name);
void bst_inorder_print_strains(StrainNode* root);
void bst_free_strain_tree(StrainNode* root);
void run_bst_demo(void);

#endif
