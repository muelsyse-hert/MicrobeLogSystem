#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/culture_status.h"
#include "include/daily_log_list.h"
#include "include/strain_archive_bst.h"
#include "include/storage.h"
#include "include/app.h"

static void strip_newline(char* text) {
    if (text == NULL) {
        return;
    }

    size_t len = strlen(text);
    while (len > 0 && (text[len - 1] == '\n' || text[len - 1] == '\r')) {
        text[len - 1] = '\0';
        len--;
    }
}

static int read_line(const char* prompt, char* buffer, size_t size) {
    if (prompt != NULL) {
        printf("%s", prompt);
    }

    if (buffer == NULL || size == 0) {
        return 0;
    }

    if (fgets(buffer, (int)size, stdin) == NULL) {
        return 0;
    }

    if (strchr(buffer, '\n') == NULL) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
        }
    }

    strip_newline(buffer);
    return 1;
}

static int read_int_input(const char* prompt, int* value) {
    char line[128];
    while (1) {
        if (!read_line(prompt, line, sizeof(line))) {
            return 0;
        }

        int parsed = 0;
        char extra = '\0';
        if (sscanf(line, " %d %c", &parsed, &extra) == 1) {
            if (value != NULL) {
                *value = parsed;
            }
            return 1;
        }

        printf("Invalid input. Please enter a valid integer.\n");
    }
}

static int read_float_input(const char* prompt, float* value) {
    char line[128];
    while (1) {
        if (!read_line(prompt, line, sizeof(line))) {
            return 0;
        }

        float parsed = 0.0f;
        char extra = '\0';
        if (sscanf(line, " %f %c", &parsed, &extra) == 1) {
            if (value != NULL) {
                *value = parsed;
            }
            return 1;
        }

        printf("Invalid input. Please enter a valid number.\n");
    }
}

static void wait_for_enter(void) {
    char buffer[8];
    printf("Press Enter to continue...");
    (void)fgets(buffer, sizeof(buffer), stdin);
}

static void free_logs_in_tree(StrainNode* root) {
    if (root == NULL) {
        return;
    }

    free_logs_in_tree(root->left);
    free_logs_in_tree(root->right);
    culture_free_all_logs(root->log_head);
    root->log_head = NULL;
    root->log_tail = NULL;
}

static void print_menu(void) {
    printf("===========================================\n");
    printf("  Microorganism Culture Management System\n");
    printf("===========================================\n");
    printf("1. Register New Strain\n");
    printf("2. Append Culture Log\n");
    printf("3. End Culture\n");
    printf("4. Query Strain Status & Logs\n");
    printf("5. Display All Strain Archives\n");
    printf("6. Save Data to Storage\n");
    printf("0. Exit System\n");
    printf("===========================================\n");
}

int main(void) {
    StrainNode* root = NULL;
    
    /* Initialize file storage backend */
    StorageContext* storage = storage_file_init("data");
    if (!storage) {
        fprintf(stderr, "Error: Failed to initialize storage. Exiting.\n");
        return 1;
    }
    printf("Storage initialized. Data will be saved to 'data/' directory.\n\n");
    root = storage_load_all_strains(storage);

    while (1) {
        int choice = -1;
        print_menu();

        if (!read_int_input("Please enter your choice (0-6): ", &choice)) {
            if (root && storage) {
                printf("Saving data...\n");
                storage_save_all_strains(storage, root);
            }
            free_logs_in_tree(root);
            bst_free_strain_tree(root);
            if (storage) {
                storage_close(storage);
            }
            return 0;
        }

        if (choice < 0 || choice > 6) {
            printf("Invalid choice. Please enter a number between 0 and 6.\n");
            continue;
        }

        if (choice == 0) {
            if (root && storage) {
                printf("Saving data before exiting...\n");
                storage_save_all_strains(storage, root);
                printf("Data saved successfully.\n");
            }
            free_logs_in_tree(root);
            bst_free_strain_tree(root);
            if (storage) {
                storage_close(storage);
            }
            printf("Goodbye.\n");
            return 0;
        }

        if (choice == 1) {
            char name[50];
            int id = 0;

            if (!read_line("Enter Strain Name: ", name, sizeof(name))) {
                break;
            }

            if (name[0] == '\0') {
                printf("Error: Strain name cannot be empty.\n");
                continue;
            }

            if (!read_int_input("Enter Strain ID: ", &id)) {
                break;
            }

            root = bst_insert_strain(root, name, id);
            printf("Strain registered successfully.\n");
            continue;
        }

        if (choice == 2) {
            char name[50];
            StrainNode* target = NULL;

            if (!read_line("Enter Strain Name to append log: ", name, sizeof(name))) {
                break;
            }

            target = bst_search_strain(root, name);
            if (target == NULL) {
                printf("Error: Strain \"%s\" not found.\n", name);
                continue;
            }

            char date[15];
            float ph = 0.0f;
            float temp = 0.0f;
            char gas_env[50];
            char observation[256];

            if (!read_line("Enter Date (YYYY-MM-DD): ", date, sizeof(date))) break;
            if (!read_float_input("Enter pH Value: ", &ph)) break;
            if (!read_float_input("Enter Temperature (C): ", &temp)) break;
            if (!read_line("Enter Gas Environment: ", gas_env, sizeof(gas_env))) break;
            if (!read_line("Enter Observation: ", observation, sizeof(observation))) break;

            culture_append_log(target, date, ph, temp, gas_env, observation);
            
            if (storage && !storage_save_strain(storage, target)) {
                fprintf(stderr, "Warning: Failed to save strain data to storage.\n");
            }
            printf("Log appended and saved successfully.\n");
            continue;
        }

        if (choice == 3) {
            char name[50];
            StrainNode* target = NULL;

            if (!read_line("Enter Strain Name to end culture: ", name, sizeof(name))) {
                break;
            }

            target = bst_search_strain(root, name);
            if (target == NULL) {
                printf("Error: Strain \"%s\" not found.\n", name);
                continue;
            }

            culture_end_culture(target);
            
            if (storage && !storage_save_strain(storage, target)) {
                fprintf(stderr, "Warning: Failed to save strain data to storage.\n");
            }
            continue;
        }

        if (choice == 4) {
            char name[50];
            StrainNode* target = NULL;

            if (!read_line("Enter Strain Name to query: ", name, sizeof(name))) {
                break;
            }

            target = bst_search_strain(root, name);
            if (target == NULL) {
                printf("Error: Strain \"%s\" not found.\n", name);
                continue;
            }

            culture_display_strain_status(target);
            continue;
        }

        if (choice == 5) {
            if (root == NULL) {
                printf("No strain archives available.\n");
            } else {
                bst_inorder_print_strains(root);
            }
            continue;
        }
        
        if (choice == 6) {
            printf("Saving all data to storage...\n");
            if (storage && storage_save_all_strains(storage, root)) {
                printf("All data successfully saved to 'data/' directory.\n");
            } else {
                fprintf(stderr, "Error: Failed to save data.\n");
            }
            continue;
        }

        wait_for_enter();
    }
    
    // Fallback cleanup if broken out of loop
    free_logs_in_tree(root);
    bst_free_strain_tree(root);
    if (storage) storage_close(storage);
    return 0;
}