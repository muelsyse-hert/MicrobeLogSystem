#include "daily_log_list.h"

static void safe_copy(char* dest, size_t dest_size, const char* src) {
    if (dest == NULL || dest_size == 0) return;
    if (src == NULL) {
        dest[0] = '\0';
        return;
    }
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
}

LogNode* daily_create_log_node(const char* date, float ph, float temp, const char* gas_env, const char* obs) {
    if (date == NULL || gas_env == NULL || obs == NULL) return NULL;
    LogNode* node = (LogNode*)malloc(sizeof(LogNode));
    if (node == NULL) return NULL;

    strncpy(node->date, date, sizeof(node->date) - 1);
    node->date[sizeof(node->date) - 1] = '\0';
    node->ph_value = ph;
    node->temperature = temp;
    strncpy(node->gas_env, gas_env, sizeof(node->gas_env) - 1);
    node->gas_env[sizeof(node->gas_env) - 1] = '\0';
    strncpy(node->observation, obs, sizeof(node->observation) - 1);
    node->observation[sizeof(node->observation) - 1] = '\0';
    node->next = NULL;

    return node;
}

void daily_append_log(StrainNode* strain, const char* date, float ph, float temp, const char* gas_env, const char* obs) {
    if (strain == NULL) return;
    LogNode* new_node = daily_create_log_node(date, ph, temp, gas_env, obs);
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
}

void daily_print_all_logs(StrainNode* strain) {
    if (strain == NULL) {
        printf("Error: strain pointer is NULL.\n");
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
        printf("    Gas Env.    : %s\n", current->gas_env);
        printf("    Observation : %s\n\n", current->observation);
        current = current->next;
        index++;
    }
}

void daily_free_all_logs(StrainNode* strain) {
    if (strain == NULL) return;
    LogNode* current = strain->log_head;
    while (current != NULL) {
        LogNode* next = current->next;
        free(current);
        current = next;
    }
    strain->log_head = NULL;
    strain->log_tail = NULL;
}

void run_daily_log_demo(void) {
    // Demo implementation empty for brevity in production
}