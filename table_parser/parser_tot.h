/****************************************************************************
*
* FERIT
*
* -----------------------------------------------------
* Ispitni zadatak iz predmeta:
*
* Digitalna videotehnika
* -----------------------------------------------------
* TV aplikacija (Sifra: PPUTVIOS_07_2018_OS)
* -----------------------------------------------------
*
* \parser_tot.h
* \brief
* File declares functions needed for parsing TOT table
* Made on 21.05.2018.
*
* @Author Jure Bajic
*****************************************************************************/
#ifndef PARSER_TOT_H
#define PARSER_TOT_H

#include "parser_tdt.h"

typedef struct _local_time_offset
{
    char countryCode[4];
    uint8_t countryRegionId;
    uint8_t localTimeOffsetPolarity;
    time_utc localTimeOffset;
    datetime timeOfChange;
    time_utc nextTimeOffset;
} local_time_offset;

typedef struct _tot_descriptor
{
    uint8_t descriptorTag;
    uint8_t descriptorLength;
} tot_descriptor;

typedef struct _tot_table {
    uint8_t tableId;
    uint8_t sectionSyntaxIndicator;
    uint8_t reservedFutureUse;
	uint16_t section_length;
    datetime dateTimeUTC;
    uint16_t descriptorsLoopLength;
    tot_descriptor descriptor;
    local_time_offset* offsets;
} tot_table;

/****************************************************************************
*
* @brief
* Function for parsing TOT table
*
* @param
*       buffer - [in] Buffer containing table data
*       tableParserMutex - [in] Mutex for parsing tables
*       tableParserCondition - [in] Condition to signal if table is parsed
*
* @return
*   NO_ERROR, in case of no error
*   ERROR, in case of error
*
****************************************************************************/
int32_t filterTOTParserCallback(uint8_t* buffer, pthread_mutex_t* tableParserMutex, pthread_cond_t* tableParserCondition);

/****************************************************************************
*
* @brief
* Function for returnig pointer to the parsed TOT table
*
* @param
*       channelNumber - [in] Number of channel to return
*
* @return
*   tot_table*
*
****************************************************************************/
tot_table* getTOTTable();

#endif