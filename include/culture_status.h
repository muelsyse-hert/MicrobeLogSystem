#ifndef CULTURE_STATUS_H
#define CULTURE_STATUS_H

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

LogNode* culture_create_log_node(const char* date, float ph, float temp, const char* gas_env, const char* obs);
void culture_append_log(StrainNode* strain, const char* date, float ph, float temp, const char* gas_env, const char* obs);
void culture_display_strain_status(StrainNode* strain);
void culture_end_culture(StrainNode* strain);
void culture_free_all_logs(LogNode* head);
void run_status_demo(void);

#endif
