#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

typedef struct LogNode {
    char date[15];
    float ph_value;
    float temperature;
    char gas_env[50];
    char observation[256];
    struct LogNode* next;
} LogNode;

typedef enum {
    STATUS_UNTOUCHED = 0,  // Never Cultured
    STATUS_CULTURING = 1,  // Currently Culturing
    STATUS_ENDED = 2       // Culture Ended
} CultureStatus;

typedef struct StrainNode {
    char name[50];
    int strain_id;
    CultureStatus status;

    LogNode standard_data;

    LogNode* log_head;
    LogNode* log_tail;

    struct StrainNode* left;
    struct StrainNode* right;
} StrainNode;

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

static const char* status_text(CultureStatus status) {
    switch (status) {
        case STATUS_UNTOUCHED:
            return "Never Cultured";
        case STATUS_CULTURING:
            return "Culturing";
        case STATUS_ENDED:
            return "Culture Ended";
        default:
            return "Unknown";
    }
}

StrainNode* create_strain_node(const char* name, int id) {
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
    node->status = STATUS_UNTOUCHED;

    memset(&node->standard_data, 0, sizeof(LogNode));
    node->standard_data.next = NULL;
    node->log_head = NULL;
    node->log_tail = NULL;
    node->left = NULL;
    node->right = NULL;

    return node;
}

StrainNode* insert_strain(StrainNode* root, const char* name, int id) {
    if (name == NULL) {
        return root;
    }

    if (root == NULL) {
        return create_strain_node(name, id);
    }

    int cmp = strcmp(name, root->name);
    if (cmp < 0) {
        root->left = insert_strain(root->left, name, id);
    } else if (cmp > 0) {
        root->right = insert_strain(root->right, name, id);
    } else {
        printf("错误：菌株名 \"%s\" 已存在，插入被忽略。\n", name);
    }

    return root;
}

StrainNode* search_strain(StrainNode* root, const char* name) {
    if (root == NULL || name == NULL) {
        return NULL;
    }

    int cmp = strcmp(name, root->name);
    if (cmp == 0) {
        return root;
    }

    if (cmp < 0) {
        return search_strain(root->left, name);
    }

    return search_strain(root->right, name);
}

void inorder_print_strains(StrainNode* root) {
    if (root == NULL) {
        return;
    }

    inorder_print_strains(root->left);
    printf("菌株名称: %s, strain_id: %d, status: %s\n",
           root->name, root->strain_id, status_text(root->status));
    inorder_print_strains(root->right);
}

void free_strain_tree(StrainNode* root) {
    if (root == NULL) {
        return;
    }

    free_strain_tree(root->left);
    free_strain_tree(root->right);
    free(root);
}

int main(void) {
    StrainNode* root = NULL;

    root = insert_strain(root, "Staphylococcus aureus", 1001);
    root = insert_strain(root, "Escherichia coli", 1002);
    root = insert_strain(root, "Bacillus subtilis", 1003);

    root = insert_strain(root, "Escherichia coli", 2000);

    printf("中序遍历结果：\n");
    inorder_print_strains(root);

    StrainNode* found = search_strain(root, "Escherichia coli");
    if (found != NULL) {
        printf("查找成功：找到菌株 \"%s\"，ID 为 %d。\n", found->name, found->strain_id);
    } else {
        printf("查找失败：未找到菌株 \"Escherichia coli\"。\n");
    }

    found = search_strain(root, "Lactobacillus acidophilus");
    if (found != NULL) {
        printf("查找成功：找到菌株 \"%s\"，ID 为 %d。\n", found->name, found->strain_id);
    } else {
        printf("查找失败：未找到菌株 \"Lactobacillus acidophilus\"。\n");
    }

    free_strain_tree(root);
    return 0;
}
