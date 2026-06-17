#ifndef STRAIN_ARCHIVE_BST_H
#define STRAIN_ARCHIVE_BST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct LogNode {
    char date[15];
    float ph_value;
    float temperature;
    char gas_env[50];
    char observation[256];
    struct LogNode* next;
} LogNode;

typedef struct StrainNode {
    char name[50];
    int strain_id;
    int status;

    LogNode standard_data;

    LogNode* log_head;
    LogNode* log_tail;

    struct StrainNode* left;
    struct StrainNode* right;
} StrainNode;

StrainNode* bst_create_strain_node(const char* name, int id);
StrainNode* bst_insert_strain(StrainNode* root, const char* name, int id);
StrainNode* bst_search_strain(StrainNode* root, const char* name);
void bst_inorder_print_strains(StrainNode* root);
void bst_free_strain_tree(StrainNode* root);
void run_bst_demo(void);

#endif
