#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    STATUS_UNTOUCHED = 0,
    STATUS_CULTURING = 1,
    STATUS_ENDED = 2
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

#endif
