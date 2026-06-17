#ifndef DAILY_LOG_LIST_H
#define DAILY_LOG_LIST_H

#include "common.h"

LogNode* daily_create_log_node(const char* date, float ph, float temp, const char* gas_env, const char* obs);
void daily_append_log(StrainNode* strain, const char* date, float ph, float temp, const char* gas_env, const char* obs);
void daily_print_all_logs(StrainNode* strain);
void daily_free_all_logs(StrainNode* strain);
void run_daily_log_demo(void);

#endif
