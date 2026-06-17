#include "strain_archive_bst.h"

#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

static void safe_copy(char* dest, size_t dest_size, const char* src) {
    if (dest == NULL || dest_size == 0) {
        return;
    }

    if (src == NULL) {
        dest[0] = '\0';
        return;
    }

    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
}

static const char* status_text(int status) {
    switch (status) {
        case 0: return "Never Cultured";
        case 1: return "Culturing";
        case 2: return "Culture Ended";
        default: return "Unknown";
    }
}

StrainNode* bst_create_strain_node(const char* name, int id) {
    if (name == NULL) {
        return NULL;
    }

    StrainNode* node = (StrainNode*)malloc(sizeof(StrainNode));
    if (node == NULL) {
        return NULL;
    }

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
    if (name == NULL) {
        return root;
    }

    if (root == NULL) {
        return bst_create_strain_node(name, id);
    }

    int cmp = strcmp(name, root->name);
    if (cmp < 0) {
        root->left = bst_insert_strain(root->left, name, id);
    } else if (cmp > 0) {
        root->right = bst_insert_strain(root->right, name, id);
    } else {
        printf("错误：菌株名 \"%s\" 已存在，插入被忽略。\n", name);
    }

    return root;
}

StrainNode* bst_search_strain(StrainNode* root, const char* name) {
    if (root == NULL || name == NULL) {
        return NULL;
    }

    int cmp = strcmp(name, root->name);
    if (cmp == 0) {
        return root;
    }

    if (cmp < 0) {
        return bst_search_strain(root->left, name);
    }

    return bst_search_strain(root->right, name);
}

void bst_inorder_print_strains(StrainNode* root) {
    if (root == NULL) {
        return;
    }

    bst_inorder_print_strains(root->left);
    printf("菌株名称: %s, strain_id: %d, status: %s\n",
           root->name, root->strain_id, status_text(root->status));
    bst_inorder_print_strains(root->right);
}

void bst_free_strain_tree(StrainNode* root) {
    if (root == NULL) {
        return;
    }

    bst_free_strain_tree(root->left);
    bst_free_strain_tree(root->right);
    free(root);
}

void run_bst_demo(void) {
    StrainNode* root = NULL;

    root = bst_insert_strain(root, "Staphylococcus aureus", 1001);
    root = bst_insert_strain(root, "Escherichia coli", 1002);
    root = bst_insert_strain(root, "Bacillus subtilis", 1003);
    root = bst_insert_strain(root, "Escherichia coli", 2000);

    printf("中序遍历结果：\n");
    bst_inorder_print_strains(root);

    StrainNode* found = bst_search_strain(root, "Escherichia coli");
    if (found != NULL) {
        printf("查找成功：找到菌株 \"%s\"，ID 为 %d。\n", found->name, found->strain_id);
    } else {
        printf("查找失败：未找到菌株 \"Escherichia coli\"。\n");
    }

    found = bst_search_strain(root, "Lactobacillus acidophilus");
    if (found != NULL) {
        printf("查找成功：找到菌株 \"%s\"，ID 为 %d。\n", found->name, found->strain_id);
    } else {
        printf("查找失败：未找到菌株 \"Lactobacillus acidophilus\"。\n");
    }

    bst_free_strain_tree(root);
}
