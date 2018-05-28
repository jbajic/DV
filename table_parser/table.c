#include "table.h"

pthread_mutex_t tableParserMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t tableParserCondition = PTHREAD_COND_INITIALIZER;

int32_t waitForTableToParse()
{
    pthread_mutex_lock(&tableParserMutex);
    pthread_cond_wait(&tableParserCondition, &tableParserMutex);
    pthread_mutex_unlock(&tableParserMutex);
    return NO_ERROR;
}