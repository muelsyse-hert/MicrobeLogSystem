#include "culture_status.h"

static void safe_copy(char* dest, size_t dest_size, const char* src) {
    if (dest == NULL || dest_size == 0) return;
    if (src == NULL) {
        dest[0] = '\0';
        return;
    }
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
}

LogNode* culture_create_log_node(const char* date, float ph, float temp, const char* gas_env, const char* obs) {
    if (date == NULL || gas_env == NULL || obs == NULL) return NULL;
    LogNode* node = (LogNode*)malloc(sizeof(LogNode));
    if (node == NULL) return NULL;

    safe_copy(node->date, sizeof(node->date), date);
    node->ph_value = ph;
    node->temperature = temp;
    safe_copy(node->gas_env, sizeof(node->gas_env), gas_env);
    safe_copy(node->observation, sizeof(node->observation), obs);
    node->next = NULL;
    return node;
}

void culture_append_log(StrainNode* strain, const char* date, float ph, float temp, const char* gas_env, const char* obs) {
    if (strain == NULL) return;
    if (strain->status == STATUS_ENDED) {
        printf("Error: Cannot append log. Culture has already ended.\n");
        return;
    }

    LogNode* new_node = culture_create_log_node(date, ph, temp, gas_env, obs);
    if (new_node == NULL) return;

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
        printf("No culture data available.\n");
        return;
    }
    printf("Date: %s\n", log->date);
    printf("pH: %.2f\n", log->ph_value);
    printf("Temperature: %.2f C\n", log->temperature);
    printf("Gas Environment: %s\n", log->gas_env);
    printf("Observation: %s\n", log->observation);
}

void culture_display_strain_status(StrainNode* strain) {
    if (strain == NULL) return;
    switch (strain->status) {
        case STATUS_UNTOUCHED:
            printf("Current Status: Untouched (Never Cultured)\n");
            printf("No culture data available.\n");
            break;
        case STATUS_CULTURING:
            printf("Current Status: Culturing\n");
            if (strain->log_tail != NULL) {
                print_log_data(strain->log_tail);
            } else {
                printf("No culture data available.\n");
            }
            break;
        case STATUS_ENDED:
            printf("Current Status: Culture Ended. Standard parameters below:\n");
            print_log_data(&strain->standard_data);
            break;
        default:
            printf("Current Status: Unknown\n");
            break;
    }
}

void culture_end_culture(StrainNode* strain) {
    if (strain == NULL) return;
    if (strain->status == STATUS_UNTOUCHED) {
        printf("Error: Cannot end culture. Never cultured.\n");
        return;
    }
    if (strain->status == STATUS_ENDED) {
        printf("Culture has already been ended.\n");
        return;
    }
    if (strain->log_tail == NULL) {
        printf("Error: Cannot end culture. No logs exist.\n");
        return;
    }

    safe_copy(strain->standard_data.date, sizeof(strain->standard_data.date), strain->log_tail->date);
    strain->standard_data.ph_value = strain->log_tail->ph_value;
    strain->standard_data.temperature = strain->log_tail->temperature;
    safe_copy(strain->standard_data.gas_env, sizeof(strain->standard_data.gas_env), strain->log_tail->gas_env);
    safe_copy(strain->standard_data.observation, sizeof(strain->standard_data.observation), strain->log_tail->observation);
    strain->standard_data.next = NULL;
    strain->status = STATUS_ENDED;

    printf("Culture ended successfully. Standard data snapshotted.\n");
}

void culture_free_all_logs(LogNode* head) {
    while (head != NULL) {
        LogNode* next = head->next;
        free(head);
        head = next;
    }
}

void run_status_demo(void) {
    // Demo implementation empty for brevity in production
}