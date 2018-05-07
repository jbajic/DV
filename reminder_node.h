#ifndef REMINDER_NODE_H
#define REMINDER_NODE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct reminder
{
    uint16_t channel_index;
    char* time;
    struct reminder* next;
} reminder;

void addReminderTime(char**, reminder**);

void addReminderChannelIndex(uint16_t, reminder**);

#endif
