#ifndef INIT_CONFIGURATION_H
#define INIT_CONFIGURATION_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_NUMBER_OF_ELEMENTS 10

struct init_service
{
    uint16_t apid;
    uint16_t vpid;
    char atype[MAX_NUMBER_OF_ELEMENTS];
    char vtype[MAX_NUMBER_OF_ELEMENTS];
};

struct reminder
{
    uint16_t channel_index;
    char time[MAX_NUMBER_OF_ELEMENTS];
};

typedef struct config_parameters
{
    uint16_t frequency;
    uint16_t bandwidth;
    char module[MAX_NUMBER_OF_ELEMENTS];
    struct init_service service;
    struct reminder* reminders;
} config_parameters;

config_parameters* loadFile(char*** file_path);

uint8_t freeConfig(config_parameters*);

#endif
