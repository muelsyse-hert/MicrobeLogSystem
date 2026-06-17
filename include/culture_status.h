#ifndef CULTURE_STATUS_H
#define CULTURE_STATUS_H

#include "common.h"

LogNode* culture_create_log_node(const char* date, float ph, float temp, const char* gas_env, const char* obs);
void culture_append_log(StrainNode* strain, const char* date, float ph, float temp, const char* gas_env, const char* obs);
void culture_display_strain_status(StrainNode* strain);
void culture_end_culture(StrainNode* strain);
void culture_free_all_logs(LogNode* head);
void run_status_demo(void);

#endif
