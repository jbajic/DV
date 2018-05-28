#ifndef TABLE_H
#define TABLE_H

#include "./../base.h"

extern pthread_mutex_t tableParserMutex;
extern pthread_cond_t tableParserCondition;

int32_t waitForTableToParse();

#endif