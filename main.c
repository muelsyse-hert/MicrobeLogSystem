#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

typedef enum {
    STATUS_UNTOUCHED = 0,  // Never Cultured
    STATUS_CULTURING = 1,  // Currently Culturing
    STATUS_ENDED = 2       // Culture Ended
} CultureStatus;

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
        case STATUS_UNTOUCHED: return "Never Cultured";
        case STATUS_CULTURING: return "Culturing";
        case STATUS_ENDED: return "Culture Ended";
        default: return "Unknown";
    }
}

static LogNode* create_log_node(const char* date, float ph, float temp, const char* gas_env, const char* obs) {
    if (date == NULL || gas_env == NULL || obs == NULL) {
        return NULL;
    }

    LogNode* node = (LogNode*)malloc(sizeof(LogNode));
    if (node == NULL) {
        return NULL;
    }

    safe_copy(node->date, sizeof(node->date), date);
    node->ph_value = ph;
    node->temperature = temp;
    safe_copy(node->gas_env, sizeof(node->gas_env), gas_env);
    safe_copy(node->observation, sizeof(node->observation), obs);
    node->next = NULL;
    return node;
}

static void free_log_list(LogNode* head) {
    while (head != NULL) {
        LogNode* next = head->next;
        free(head);
        head = next;
    }
}

static void append_log(StrainNode* strain, const char* date, float ph, float temp, const char* gas_env, const char* obs) {
    if (strain == NULL) {
        return;
    }
    if (strain->status == STATUS_ENDED) {
        printf("Cannot append log: Culture has already ended.\n");
        return;
    }

    LogNode* new_node = create_log_node(date, ph, temp, gas_env, obs);
    if (new_node == NULL) {
        return;
    }

    if (strain->log_head == NULL) {
        strain->log_head = new_node;
        strain->log_tail = new_node;
    } else {
        if (strain->log_tail == NULL) {
            strain->log_tail = strain->log_head;
            while (strain->log_tail->next != NULL) {
                strain->log_tail = strain->log_tail->next;
            }
        }
        strain->log_tail->next = new_node;
        strain->log_tail = new_node;
    }

    if (strain->status == STATUS_UNTOUCHED) {
        strain->status = STATUS_CULTURING;
    }
}

static void print_log_data(const LogNode* log) {
    if (log == NULL) {
        printf("No culture data available yet.\n");
        return;
    }

    printf("Date: %s\n", log->date);
    printf("pH: %.2f\n", log->ph_value);
    printf("Temperature: %.2f C\n", log->temperature);
    printf("Gas Environment: %s\n", log->gas_env);
    printf("Observation: %s\n", log->observation);
}

static void print_all_logs(StrainNode* strain) {
    if (strain == NULL) {
        return;
    }

    printf("Strain Name: %s\n", strain->name);
    printf("Culture Logs:\n");
    if (strain->log_head == NULL) {
        printf("  No logs available.\n");
        return;
    }

    LogNode* current = strain->log_head;
    int index = 1;
    while (current != NULL) {
        printf("  Log #%d\n", index);
        printf("    Date        : %s\n", current->date);
        printf("    pH Value    : %.2f\n", current->ph_value);
        printf("    Temperature : %.2f C\n", current->temperature);
        printf("    Gas Env     : %s\n", current->gas_env);
        printf("    Observation : %s\n", current->observation);
        printf("\n");
        current = current->next;
        index++;
    }
}

static StrainNode* create_strain_node(const char* name, int id) {
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
    node->left = NULL;
    node->right = NULL;
    node->log_head = NULL;
    node->log_tail = NULL;
    memset(&node->standard_data, 0, sizeof(LogNode));
    return node;
}

static StrainNode* insert_strain(StrainNode* root, const char* name, int id) {
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

static StrainNode* search_strain(StrainNode* root, const char* name) {
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

static void inorder_print_strains(StrainNode* root) {
    if (root == NULL) {
        return;
    }

    inorder_print_strains(root->left);
    printf("菌株名称: %s, strain_id: %d, status: %s\n",
           root->name, root->strain_id, status_text(root->status));
    inorder_print_strains(root->right);
}

static void free_strain_tree(StrainNode* root) {
    if (root == NULL) {
        return;
    }

    free_strain_tree(root->left);
    free_strain_tree(root->right);
    free_log_list(root->log_head);
    free(root);
}

static void display_strain_status(StrainNode* strain) {
    if (strain == NULL) {
        return;
    }

    switch (strain->status) {
        case STATUS_UNTOUCHED:
            printf("Current Status: Never Cultured\n");
            printf("No culture data available yet.\n");
            break;
        case STATUS_CULTURING:
            printf("Current Status: Culturing\n");
            if (strain->log_tail != NULL) {
                print_log_data(strain->log_tail);
            } else {
                printf("No culture data available yet.\n");
            }
            break;
        case STATUS_ENDED:
            printf("Current Status: Culture Ended. Standard Culture Parameters below:\n");
            print_log_data(&strain->standard_data);
            break;
        default:
            printf("Current Status: Unknown\n");
            break;
    }
}

static void end_culture(StrainNode* strain) {
    if (strain == NULL) {
        return;
    }

    if (strain->status == STATUS_UNTOUCHED) {
        printf("Cannot end culture: Never cultured.\n");
        return;
    }

    if (strain->status == STATUS_ENDED) {
        printf("Culture has already been ended.\n");
        return;
    }

    if (strain->log_tail == NULL) {
        printf("Cannot end culture. No logs exist.\n");
        return;
    }

    safe_copy(strain->standard_data.date, sizeof(strain->standard_data.date), strain->log_tail->date);
    strain->standard_data.ph_value = strain->log_tail->ph_value;
    strain->standard_data.temperature = strain->log_tail->temperature;
    safe_copy(strain->standard_data.gas_env, sizeof(strain->standard_data.gas_env), strain->log_tail->gas_env);
    safe_copy(strain->standard_data.observation, sizeof(strain->standard_data.observation), strain->log_tail->observation);
    strain->standard_data.next = NULL;

    strain->status = STATUS_ENDED;
    printf("Culture ended successfully. Standard data has been snapshotted.\n");
}

static void demo_daily_log_module(void) {
    printf("=== Daily Log Module ===\n");

    StrainNode strain;
    memset(&strain, 0, sizeof(StrainNode));
    safe_copy(strain.name, sizeof(strain.name), "E.coli");

    append_log(&strain, "2026-06-15", 7.20f, 37.00f, "Aerobic", "Initial inoculation; medium clear.");
    append_log(&strain, "2026-06-16", 6.85f, 36.80f, "Aerobic", "Turbidity increased; slight sediment observed.");
    append_log(&strain, "2026-06-17", 6.50f, 37.10f, "5% CO2", "Dense growth; colony formation visible.");

    print_all_logs(&strain);
    free_log_list(strain.log_head);
}

static void demo_bst_module(void) {
    printf("=== BST Module ===\n");

    StrainNode* root = NULL;
    root = insert_strain(root, "Staphylococcus aureus", 1001);
    root = insert_strain(root, "Escherichia coli", 1002);
    root = insert_strain(root, "Bacillus subtilis", 1003);
    root = insert_strain(root, "Escherichia coli", 2000);

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
}

static void demo_status_module(void) {
    printf("=== Status Module ===\n");

    StrainNode strain;
    memset(&strain, 0, sizeof(StrainNode));
    safe_copy(strain.name, sizeof(strain.name), "E.coli");
    strain.strain_id = 2001;
    strain.status = STATUS_UNTOUCHED;

    end_culture(&strain);

    append_log(&strain, "2026-06-17", 6.72f, 37.00f, "Aerobic", "Initial culture growth observed.");
    display_strain_status(&strain);

    end_culture(&strain);
    display_strain_status(&strain);

    free_log_list(strain.log_head);
}

int main(void) {
    demo_daily_log_module();
    printf("\n");
    demo_bst_module();
    printf("\n");
    demo_status_module();
    return 0;
}
