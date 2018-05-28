#ifndef TABLE_H
#define TABLE_H

#include "./../base.h"

extern pthread_mutex_t tableParserMutex;
extern pthread_cond_t tableParserCondition;

/****************************************************************************
*
* @brief
* Function for waiting for parse to finish
*
* @return
* status
****************************************************************************/
int32_t waitForTableToParse();

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
void setDateFromMJD(date_tdt* dateStruct, uint16_t mjd);

#endif