#include "strain_archive_bst.h"

static void safe_copy(char* dest, size_t dest_size, const char* src) {
    if (dest == NULL || dest_size == 0) return;
    if (src == NULL) {
        dest[0] = '\0';
        return;
    }
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
}

static const char* status_text(int status) {
    switch (status) {
        case 0: return "Untouched";
        case 1: return "Culturing";
        case 2: return "Ended";
        default: return "Unknown";
    }
}

StrainNode* bst_create_strain_node(const char* name, int id) {
    if (name == NULL) return NULL;
    StrainNode* node = (StrainNode*)malloc(sizeof(StrainNode));
    if (node == NULL) return NULL;

    memset(node, 0, sizeof(StrainNode));
    safe_copy(node->name, sizeof(node->name), name);
    node->strain_id = id;
    node->status = 0;
    memset(&node->standard_data, 0, sizeof(LogNode));
    node->left = NULL;
    node->right = NULL;
    return node;
}

StrainNode* bst_insert_strain(StrainNode* root, const char* name, int id) {
    if (name == NULL) return root;
    if (root == NULL) return bst_create_strain_node(name, id);

    int cmp = strcmp(name, root->name);
    if (cmp < 0) {
        root->left = bst_insert_strain(root->left, name, id);
    } else if (cmp > 0) {
        root->right = bst_insert_strain(root->right, name, id);
    } else {
        printf("Warning: Strain \"%s\" already exists. Insertion ignored.\n", name);
    }
    return root;
}

StrainNode* bst_search_strain(StrainNode* root, const char* name) {
    if (root == NULL || name == NULL) return NULL;
    int cmp = strcmp(name, root->name);
    if (cmp == 0) return root;
    if (cmp < 0) return bst_search_strain(root->left, name);
    return bst_search_strain(root->right, name);
}

void bst_inorder_print_strains(StrainNode* root) {
    if (root == NULL) return;
    bst_inorder_print_strains(root->left);
    printf("Strain Name: %s, ID: %d, Status: %s\n",
           root->name, root->strain_id, status_text(root->status));
    bst_inorder_print_strains(root->right);
}

void bst_free_strain_tree(StrainNode* root) {
    if (root == NULL) return;
    bst_free_strain_tree(root->left);
    bst_free_strain_tree(root->right);
    free(root);
}

void run_bst_demo(void) {
    // Demo implementation empty for brevity in production
}