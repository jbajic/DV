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
* \table.h
* \brief
* File declares functions needed for setting ceratin table for parsing
* Made on 21.05.2018.
*
* @Author Jure Bajic
*****************************************************************************/
#ifndef TABLE_H
#define TABLE_H

#include "./../base.h"
#include "parser_pat.h"
#include "parser_pmt.h"
#include "parser_tdt.h"
#include "parser_tot.h"

enum table_types
{
    TABLE_PAT = 1,
    TABLE_PMT,
    TABLE_TOT,
    TABLE_TDT
};

enum table_id
{
    PAT_TABLE_ID = 0x00,
    PMT_TABLE_ID = 0x02,
    TDT_TABLE_ID = 0x70,
    TOT_TABLE_ID = 0x73
};

enum table_pid
{
    PAT_TABLE_PID = 0x00000,
    TDT_AND_TOT_TABLE_PID = 0x0014,
};

/****************************************************************************
*
* @brief
* Function for parsing table
*
* @param
*       tableType - [in] Enum specifying which table to parse
*       handles - [in] Structure containing all handles
*       pmtPID - [in] Exists only if the parsing table is PMT
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t parseTable(enum table_types tableType, player_handles* handles, int32_t pmtPID);

/****************************************************************************
*
* @brief
* Function for freeing filter
*
* @param
*       handles - [in] structure containing all handles
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t stopParsing(player_handles* handles);

/****************************************************************************
*
* @brief
* Function for waiting for parse to finish
*
* @return
* status
****************************************************************************/
int32_t waitForTableToParse();

#endif