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

typedef struct StrainNode {
    char name[50];
    int strain_id;
    int status;                  // 0: 培养中, 1: 培养结束

    LogNode standard_data;       // 最终培养数据快照（深拷贝）

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

void display_strain_status(StrainNode* strain) {
    if (strain == NULL) {
        return;
    }

    if (strain->status == 0) {
        printf("Current Status: Culturing\n");
        if (strain->log_tail != NULL) {
            print_log_data(strain->log_tail);
        } else {
            printf("No culture data available yet.\n");
        }
    } else if (strain->status == 1) {
        printf("Current Status: Culture Ended. Standard Culture Parameters below:\n");
        print_log_data(&strain->standard_data);
    }
}

void end_culture(StrainNode* strain) {
    if (strain == NULL) {
        return;
    }

    if (strain->status == 1) {
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

    strain->status = 1;
    printf("Culture ended successfully. Standard data has been snapshotted.\n");
}

static void free_log_list(LogNode* head) {
    while (head != NULL) {
        LogNode* next = head->next;
        free(head);
        head = next;
    }
}

int main(void) {
    StrainNode strain;
    memset(&strain, 0, sizeof(StrainNode));

    safe_copy(strain.name, sizeof(strain.name), "E.coli");
    strain.strain_id = 2001;
    strain.status = 0;
    strain.log_head = NULL;
    strain.log_tail = NULL;
    strain.left = NULL;
    strain.right = NULL;
    memset(&strain.standard_data, 0, sizeof(LogNode));

    end_culture(&strain);

    strain.log_head = create_log_node("2026-06-17", 6.72f, 37.00f, "Aerobic", "Initial culture growth observed.");
    strain.log_tail = strain.log_head;

    display_strain_status(&strain);

    end_culture(&strain);

    display_strain_status(&strain);

    free_log_list(strain.log_head);
    strain.log_head = NULL;
    strain.log_tail = NULL;

    return 0;
}
