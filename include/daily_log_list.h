#ifndef DAILY_LOG_LIST_H
#define DAILY_LOG_LIST_H

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

typedef struct StrainNode {
    char name[50];
    LogNode* log_head;
    LogNode* log_tail;
} StrainNode;

LogNode* daily_create_log_node(const char* date, float ph, float temp, const char* gas_env, const char* obs);
void daily_append_log(StrainNode* strain, const char* date, float ph, float temp, const char* gas_env, const char* obs);
void daily_print_all_logs(StrainNode* strain);
void daily_free_all_logs(StrainNode* strain);
void run_daily_log_demo(void);

#endif
