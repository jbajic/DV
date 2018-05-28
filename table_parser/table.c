#include "table.h"

pthread_mutex_t tableParserMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t tableParserCondition = PTHREAD_COND_INITIALIZER;

/****************************************************************************
*
* @brief
* Function for waiting for parse to finish
*
* @return
* status
****************************************************************************/
int32_t waitForTableToParse()
{
    pthread_mutex_lock(&tableParserMutex);
    pthread_cond_wait(&tableParserCondition, &tableParserMutex);
    pthread_mutex_unlock(&tableParserMutex);
    return NO_ERROR;
}

/****************************************************************************
*
* @brief
* Function for calculating date from MJD
*
* @param
*       dateStruct - [in] Date struct in which calculated date will be written to
*       mjd - [in] Modified Julian Date
*
****************************************************************************/
void setDateFromMJD(date_tdt* dateStruct, uint16_t mjd)
{
    int16_t k;
    dateStruct->year = (int) ((mjd - (float) 15078.2) / 365.25);
    dateStruct->month = (int) ((mjd - (float) 14956.1 - (dateStruct->year * (float) 365.25)) / (float) 30.6001);
    dateStruct->dayInMonth = (uint8_t) (mjd - 14956 - (int) (dateStruct->year * (float) 365.25) - (int) (dateStruct->month *  (float) 30.6001));
    if (dateStruct->month == 14 || dateStruct->month == 15)
    {
        k = 1;
    }
    else 
    {
        k = 0;
    }
    dateStruct->year += k;
    dateStruct->month -= 1 - 12 * k;
}