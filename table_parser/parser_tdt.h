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
* \parser_tdt.h
* \brief
* File declares functions and structures needed for parsing TDT table
* Made on 21.05.2018.
*
* @Author Jure Bajic
*****************************************************************************/
#ifndef PARSER_TDT_H
#define PARSER_TDT_H

#include "table.h"

typedef struct _tdt_table {
    uint8_t tableId;
    uint8_t sectionSyntaxIndicator;
    uint8_t reservedFutureUse;
	uint16_t section_length;
    datetime dateTimeUTC;
} tdt_table;

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
void setDateFromMJD(date_tdt*, uint16_t);

/****************************************************************************
*
* @brief
* Function for parsing TDT table
*
* @param
*       buffer - [in] Pointer to the filtered incoming streamn which contains TDT table packets
*
* @return
*   NO_ERROR, in case of no error
*   ERROR, in case of error
*
****************************************************************************/
int32_t filterTDTParserCallback(uint8_t*);

/****************************************************************************
*
* @brief
* Function for returnig pointer to the parsed TDT table
*
* @param
*       channelNumber - [in] Number of channel to return
*
* @return
*   tdt*
*
****************************************************************************/
tdt_table* getTDTTable();

#endif