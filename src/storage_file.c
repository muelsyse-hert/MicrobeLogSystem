#include "../include/storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <errno.h>

/* ============================================================================
 * OS 兼容层：目录创建安全包裹函数
 * 彻底避免不同编译器对 mkdir 参数数量定义的冲突
 * ============================================================================ */
#ifdef _WIN32
#include <direct.h>
static int make_dir(const char* path) {
    return _mkdir(path);
}
#else
#include <unistd.h>
static int make_dir(const char* path) {
    return mkdir(path, 0755);
}
#endif

/* ============================================================================
 * File Storage Backend Implementation
 * Directory Structure:
 * data/
 * ├── strains.json       
 * └── logs/
 * ├── strain_1.json  
 * └── ...
 * ============================================================================ */

typedef struct {
    char data_dir[256];
    char strains_file[512];
    char logs_dir[512];
    int has_changes;
    time_t last_save_time;
} FileStorageContext;

/* Ensure the directory structure exists */
static int ensure_data_structure(const char* data_dir) {
    struct stat st = {0};
    int mkdir_result = 0;

    /* Create base data directory */
    if (stat(data_dir, &st) != 0) {
        mkdir_result = make_dir(data_dir);
        if (mkdir_result == -1) {
            if (stat(data_dir, &st) != 0) {
                fprintf(stderr, "Error: Cannot create data directory: %s\n", data_dir);
                return 0;
            }
        } else {
            printf("Created data directory: %s\n", data_dir);
        }
    } else {
        printf("Data directory already exists: %s\n", data_dir);
    }

    /* Create logs subdirectory */
    char logs_dir[512];
#ifdef _WIN32
    snprintf(logs_dir, sizeof(logs_dir), "%s\\logs", data_dir);
#else
    snprintf(logs_dir, sizeof(logs_dir), "%s/logs", data_dir);
#endif
    
    if (stat(logs_dir, &st) != 0) {
        mkdir_result = make_dir(logs_dir);
        if (mkdir_result == -1) {
            if (stat(logs_dir, &st) != 0) {
                fprintf(stderr, "Error: Cannot create logs directory: %s\n", logs_dir);
                return 0;
            }
        } else {
            printf("Created logs directory: %s\n", logs_dir);
        }
    } else {
        printf("Logs directory already exists: %s\n", logs_dir);
    }

    return 1;
}

/* Sanitize strain names to safely use as filenames */
static void sanitize_filename(const char* strain_name, char* filename, size_t size) {
    size_t i = 0;
    while (*strain_name && i < size - 1) {
        if ((*strain_name >= 'a' && *strain_name <= 'z') ||
            (*strain_name >= 'A' && *strain_name <= 'Z') ||
            (*strain_name >= '0' && *strain_name <= '9') ||
            *strain_name == '_' || *strain_name == '-') {
            filename[i++] = *strain_name;
        } else {
            filename[i++] = '_';
        }
        strain_name++;
    }
    filename[i] = '\0';
}

/* Save a single log entry in JSON format */
static void save_log_entry(FILE* fp, LogNode* log) {
    fprintf(fp, "    {\n");
    fprintf(fp, "      \"date\": \"%s\",\n", log->date);
    fprintf(fp, "      \"ph_value\": %.2f,\n", log->ph_value);
    fprintf(fp, "      \"temperature\": %.2f,\n", log->temperature);
    fprintf(fp, "      \"gas_env\": \"%s\",\n", log->gas_env);
    fprintf(fp, "      \"observation\": \"%s\"\n", log->observation);
    fprintf(fp, "    }");
}

/* Save strain logs into a JSON file */
static int save_strain_logs(const char* logs_dir, StrainNode* strain) {
    char filename[512];
    char safe_name[256];

    sanitize_filename(strain->name, safe_name, sizeof(safe_name));
#ifdef _WIN32
    snprintf(filename, sizeof(filename), "%s\\%s.json", logs_dir, safe_name);
#else
    snprintf(filename, sizeof(filename), "%s/%s.json", logs_dir, safe_name);
#endif

    FILE* fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open file for writing: %s\n", filename);
        return 0;
    }

    fprintf(fp, "{\n");
    fprintf(fp, "  \"strain_name\": \"%s\",\n", strain->name);
    fprintf(fp, "  \"strain_id\": %d,\n", strain->strain_id);
    fprintf(fp, "  \"status\": %d,\n", strain->status);
    fprintf(fp, "  \"logs\": [\n");

    int first = 1;
    LogNode* current = strain->log_head;
    while (current) {
        if (!first) {
            fprintf(fp, ",\n");
        }
        save_log_entry(fp, current);
        first = 0;
        current = current->next;
    }

    fprintf(fp, "\n  ]\n");
    fprintf(fp, "}\n");
    fclose(fp);

    return 1;
}

/* 独立抽离的递归辅助函数：中序遍历记录菌株索引 */
static void collect_strains(StrainNode* node, FILE* fp, int* first) {
    if (!node) return;
    
    collect_strains(node->left, fp, first);

    if (!(*first)) {
        fprintf(fp, ",\n");
    }
    
    fprintf(fp, "    {\n");
    fprintf(fp, "      \"name\": \"%s\",\n", node->name);
    fprintf(fp, "      \"id\": %d,\n", node->strain_id);
    fprintf(fp, "      \"status\": %d,\n", node->status);
    fprintf(fp, "      \"log_count\": ");

    int count = 0;
    LogNode* log = node->log_head;
    while (log) {
        count++;
        log = log->next;
    }
    fprintf(fp, "%d\n", count);
    fprintf(fp, "    }");

    *first = 0;
    collect_strains(node->right, fp, first);
}

/* Save strain metadata index into strains.json */
static int save_strain_index(FileStorageContext* ctx, StrainNode* root) {
    FILE* fp = fopen(ctx->strains_file, "w");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open file for writing: %s\n", ctx->strains_file);
        return 0;
    }

    fprintf(fp, "{\n");
    fprintf(fp, "  \"timestamp\": %ld,\n", (long)time(NULL));
    fprintf(fp, "  \"strains\": [\n");

    int first = 1;
    collect_strains(root, fp, &first);

    fprintf(fp, "\n  ]\n");
    fprintf(fp, "}\n");
    fclose(fp);

    return 1;
}

/* Recursively save all strains using in-order traversal */
static int save_all_strains_recursive(FileStorageContext* ctx, StrainNode* root) {
    if (!root) return 1;

    if (!save_all_strains_recursive(ctx, root->left)) return 0;
    if (!save_strain_logs(ctx->logs_dir, root)) return 0;
    if (!save_all_strains_recursive(ctx, root->right)) return 0;

    return 1;
}

/* Load logs from JSON file and rebuild the linked list */
static LogNode* load_strain_logs(const char* logs_dir, const char* strain_name) {
    char filename[512];
    char safe_name[256];

    sanitize_filename(strain_name, safe_name, sizeof(safe_name));
#ifdef _WIN32
    snprintf(filename, sizeof(filename), "%s\\%s.json", logs_dir, safe_name);
#else
    snprintf(filename, sizeof(filename), "%s/%s.json", logs_dir, safe_name);
#endif

    FILE* fp = fopen(filename, "r");
    if (!fp) {
        return NULL;  
    }

    LogNode* head = NULL;
    LogNode* tail = NULL;
    char line[512];

    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "\"date\"")) {
            LogNode* new_log = (LogNode*)malloc(sizeof(LogNode));
            if (!new_log) {
                fclose(fp);
                return head;
            }

            char* ptr;
            if ((ptr = strstr(line, "\"date\": \"")) != NULL) {
                sscanf(ptr + 10, "%14[^\"]", new_log->date);
            }
            if (fgets(line, sizeof(line), fp) && (ptr = strstr(line, "\"ph_value\": ")) != NULL) {
                sscanf(ptr + 12, "%f", &new_log->ph_value);
            }
            if (fgets(line, sizeof(line), fp) && (ptr = strstr(line, "\"temperature\": ")) != NULL) {
                sscanf(ptr + 15, "%f", &new_log->temperature);
            }
            if (fgets(line, sizeof(line), fp) && (ptr = strstr(line, "\"gas_env\": \"")) != NULL) {
                sscanf(ptr + 12, "%49[^\"]", new_log->gas_env);
            }
            if (fgets(line, sizeof(line), fp) && (ptr = strstr(line, "\"observation\": \"")) != NULL) {
                sscanf(ptr + 16, "%255[^\"]", new_log->observation);
            }

            new_log->next = NULL;

            if (!head) {
                head = new_log;
                tail = new_log;
            } else {
                tail->next = new_log;
                tail = new_log;
            }
        }
    }

    fclose(fp);
    return head;
}

/* ============================================================================
 * Public Storage API Implementation
 * ============================================================================ */

StorageContext* storage_file_init(const char* data_dir) {
    if (!data_dir || strlen(data_dir) == 0) {
        data_dir = "data";
    }

    if (!ensure_data_structure(data_dir)) {
        return NULL;
    }

    FileStorageContext* ctx = (FileStorageContext*)malloc(sizeof(FileStorageContext));
    if (!ctx) {
        return NULL;
    }

    strncpy(ctx->data_dir, data_dir, sizeof(ctx->data_dir) - 1);
    ctx->data_dir[sizeof(ctx->data_dir) - 1] = '\0';
#ifdef _WIN32
    snprintf(ctx->strains_file, sizeof(ctx->strains_file), "%s\\strains.json", data_dir);
    snprintf(ctx->logs_dir, sizeof(ctx->logs_dir), "%s\\logs", data_dir);
#else
    snprintf(ctx->strains_file, sizeof(ctx->strains_file), "%s/strains.json", data_dir);
    snprintf(ctx->logs_dir, sizeof(ctx->logs_dir), "%s/logs", data_dir);
#endif
    ctx->has_changes = 0;
    ctx->last_save_time = time(NULL);

    return (StorageContext*)ctx;
}

StorageContext* storage_db_init(const char* connection_string) {
    (void)connection_string;
    fprintf(stderr, "Warning: Database storage backend not implemented.\n");
    fprintf(stderr, "Falling back to file storage backend.\n");
    return storage_file_init("data");
}

StorageContext* storage_init(StorageBackend backend, const char* location) {
    switch (backend) {
        case STORAGE_BACKEND_FILE:
            return storage_file_init(location);
        case STORAGE_BACKEND_DATABASE:
            return storage_db_init(location);
        default:
            return NULL;
    }
}

/* The core loading engine: reconstructs the BST and Linked Lists from JSON files */
StrainNode* storage_load_all_strains(StorageContext* context) {
    FileStorageContext* ctx = (FileStorageContext*)context;
    if (!ctx) return NULL;

    FILE* fp = fopen(ctx->strains_file, "r");
    if (!fp) return NULL; 

    StrainNode* root = NULL;
    char line[512];
    char current_name[50] = "";
    int current_id = 0;
    int current_status = 0;

    while (fgets(line, sizeof(line), fp)) {
        char* ptr;
        if ((ptr = strstr(line, "\"name\": \"")) != NULL) {
            sscanf(ptr + 9, "%49[^\"]", current_name);
        } else if ((ptr = strstr(line, "\"id\": ")) != NULL) {
            sscanf(ptr + 6, "%d", &current_id);
        } else if ((ptr = strstr(line, "\"status\": ")) != NULL) {
            sscanf(ptr + 10, "%d", &current_status);

            StrainNode* node = (StrainNode*)malloc(sizeof(StrainNode));
            memset(node, 0, sizeof(StrainNode));
            strncpy(node->name, current_name, sizeof(node->name) - 1);
            node->strain_id = current_id;
            node->status = (CultureStatus)current_status;

            node->log_head = load_strain_logs(ctx->logs_dir, node->name);

            LogNode* curr = node->log_head;
            while (curr && curr->next) {
                curr = curr->next;
            }
            node->log_tail = curr;

            if (node->status == STATUS_ENDED && node->log_tail != NULL) {
                strncpy(node->standard_data.date, node->log_tail->date, sizeof(node->standard_data.date) - 1);
                node->standard_data.ph_value = node->log_tail->ph_value;
                node->standard_data.temperature = node->log_tail->temperature;
                strncpy(node->standard_data.gas_env, node->log_tail->gas_env, sizeof(node->standard_data.gas_env) - 1);
                strncpy(node->standard_data.observation, node->log_tail->observation, sizeof(node->standard_data.observation) - 1);
                node->standard_data.next = NULL;
            }

            if (root == NULL) {
                root = node;
            } else {
                StrainNode* curr_tree = root;
                while (1) {
                    int cmp = strcmp(node->name, curr_tree->name);
                    if (cmp < 0) {
                        if (curr_tree->left == NULL) {
                            curr_tree->left = node;
                            break;
                        }
                        curr_tree = curr_tree->left;
                    } else if (cmp > 0) {
                        if (curr_tree->right == NULL) {
                            curr_tree->right = node;
                            break;
                        }
                        curr_tree = curr_tree->right;
                    } else {
                        free(node); 
                        break;
                    }
                }
            }
            current_name[0] = '\0'; 
        }
    }
    fclose(fp);
    return root;
}

int storage_save_strain(StorageContext* context, StrainNode* strain) {
    FileStorageContext* ctx = (FileStorageContext*)context;
    if (!ctx || !strain) return 0;
    if (!save_strain_logs(ctx->logs_dir, strain)) return 0;
    ctx->has_changes = 1;
    return 1;
}

int storage_save_all_strains(StorageContext* context, StrainNode* root) {
    FileStorageContext* ctx = (FileStorageContext*)context;
    if (!ctx) return 0;
    if (!save_all_strains_recursive(ctx, root)) return 0;
    if (!save_strain_index(ctx, root)) return 0;
    ctx->has_changes = 0;
    ctx->last_save_time = time(NULL);
    return 1;
}

int storage_delete_strain(StorageContext* context, const char* strain_name) {
    FileStorageContext* ctx = (FileStorageContext*)context;
    if (!ctx || !strain_name) return 0;

    char filename[512];
    char safe_name[256];
    sanitize_filename(strain_name, safe_name, sizeof(safe_name));
#ifdef _WIN32
    snprintf(filename, sizeof(filename), "%s\\%s.json", ctx->logs_dir, safe_name);
#else
    snprintf(filename, sizeof(filename), "%s/%s.json", ctx->logs_dir, safe_name);
#endif

    if (remove(filename) == -1) {
        return 1; 
    }
    ctx->has_changes = 1;
    return 1;
}

int storage_has_changes(StorageContext* context) {
    FileStorageContext* ctx = (FileStorageContext*)context;
    if (!ctx) return 0;
    return ctx->has_changes;
}

void storage_clear_changes(StorageContext* context) {
    FileStorageContext* ctx = (FileStorageContext*)context;
    if (ctx) ctx->has_changes = 0;
}

int storage_close(StorageContext* context) {
    if (!context) return 0;
    free(context);
    return 1;
}