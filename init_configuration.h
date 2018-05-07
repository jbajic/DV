#ifndef INIT_CONFIGURATION_H
#define INIT_CONFIGURATION_H

#include <string.h>
#include "tdp_api.h"
#include "reminder_node.h"

#define MAX_NUMBER_OF_ELEMENTS 10

#define TRUE 1
#define FALSE 0

enum CONFIG_KEYS {
    FREQUENCY_KEY = 0,
    BANDWIDTH_KEY,
    MODULE_KEY,
    APID_KEY,
    VPID_KEY,
    ATYPE_KEY,
    VTYPE_KEY,
    TIME_KEY,
    CHANNEL_INDEX_KEY,
    NON_EXISTENT_KEY
};


#define PARSE_ERROR -1

typedef struct { char* key; enum CONFIG_KEYS val; } t_symstruct;

static t_symstruct lookuptable[] = {
    {"frequency", FREQUENCY_KEY}, {"bandwidth", BANDWIDTH_KEY}, {"module", MODULE_KEY}, {"apid", APID_KEY}, {"vpid", VPID_KEY}, {"atype", ATYPE_KEY}, {"vtype", VTYPE_KEY}, {"time", TIME_KEY}, {"channel_index", CHANNEL_INDEX_KEY}
};

#define NKEYS (sizeof (lookuptable) / sizeof (t_symstruct))

typedef struct init_service
{
    uint16_t apid;
    uint16_t vpid;
    tStreamType atype;
    tStreamType vtype;
} init_service;

typedef struct _config_parameters
{
    uint16_t frequency;
    uint16_t bandwidth;
    t_Module module;
    struct init_service service;
    reminder* headReminder;
} config_parameters;

config_parameters* loadFile(char** file_path);

int freeConfig(config_parameters*);

int analyzeWord(char*, FILE**, config_parameters*);


void testConfigPrintf(config_parameters*);

#endif
