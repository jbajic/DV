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

#include "./../base.h"

typedef struct _time_tdt
{
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} time_tdt;

typedef struct _date_tdt
{
    int32_t dayInMonth;
    int32_t month;
    int32_t year;
} date_tdt;

typedef struct _datetime
{
    date_tdt date;
    time_tdt time;
} datetime;

typedef struct _tdt_table {
    uint8_t tableId;
    uint8_t sectionSyntaxIndicator;
    uint8_t reservedFutureUse;
	uint16_t section_length;
    datetime dateTimeUTC;
} tdt_table;

int8_t isTDTTableParsed();

void setDateFromMJD(date_tdt*, uint16_t);

int32_t filterTDTParserCallback(uint8_t*);

#endif