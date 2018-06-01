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
* \table.c
* \brief
* File defines functions needed for setting ceratin table for parsing
* Made on 21.05.2018.
*
* @Author Jure Bajic
*****************************************************************************/
#include "table.h"

static pthread_mutex_t tableParserMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t tableParserCondition = PTHREAD_COND_INITIALIZER;
static uint8_t currentParsingTable = 0;

/****************************************************************************
*
* @brief
* Function for parsing PAT table
*
* @param
*       buffer - [in] Pointer to the filtered incoming streamn which contains PAT table packets
*
* @return
*   NO_ERROR, in case of no error
*   ERROR, in case of error
*
****************************************************************************/
static int32_t filterCallback(uint8_t* buffer)
{
    switch(currentParsingTable)
    {
        case TABLE_PAT:
        {
            filterPATParserCallback(buffer, &tableParserMutex, &tableParserCondition);
            break;
        }
        case TABLE_PMT:
        {
            filterPMTParserCallback(buffer, &tableParserMutex, &tableParserCondition);
            break;
        }
        case TABLE_TOT:
        {
            filterTOTParserCallback(buffer, &tableParserMutex, &tableParserCondition);
            break;
        }
        case TABLE_TDT:
        {
            filterTDTParserCallback(buffer, &tableParserMutex, &tableParserCondition);
            break;
        }
        default:
        {
            return ERROR;
        }
    }
    return NO_ERROR;
}

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
int32_t stopParsing(player_handles* handles)
{
    int32_t result;
    /* Free demux filter */
    result = Demux_Unregister_Section_Filter_Callback(filterCallback);
    ASSERT_TDP_RESULT(result, "Demux_Unregister_Section_Filter_Callback");
	result = Demux_Free_Filter(handles->playerHandle, handles->filterHandle);
    ASSERT_TDP_RESULT(result, "Demux_Free_Filter");

    return NO_ERROR;
}

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
* Function for starting the demux filtering
*
* @param
*       handles - [in] structure containing all handles
*       tablePID - [in] PID of table to parse
*       tableId - [in] ID of table to parse
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
static int32_t setFilterToTable(player_handles* handles, int32_t tablePID, int32_t tableId)
{
    int32_t result;
    /* Set filter to demux */
    result = Demux_Set_Filter(handles->playerHandle, tablePID, tableId, &handles->filterHandle);
    ASSERT_TDP_RESULT(result, "Demux_Set_Filter");
    
    /* Register section filter callback */
    result = Demux_Register_Section_Filter_Callback(filterCallback);
    ASSERT_TDP_RESULT(result, "Demux_Register_Section_Filter_Callback");

    waitForTableToParse();

    if (currentParsingTable != TABLE_TDT)
    {
        stopParsing(handles);
    }
    return NO_ERROR;
}

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
int32_t parseTable(enum table_types tableType, player_handles* handles, int32_t pmtPID)
{
    currentParsingTable = tableType;
    switch(tableType)
    {
        case TABLE_PAT:
        {
            setFilterToTable(handles, PAT_TABLE_PID, PAT_TABLE_ID);
            break;
        }
        case TABLE_PMT:
        {
            setFilterToTable(handles, pmtPID, PMT_TABLE_ID);
            break;
        }
        case TABLE_TOT:
        {
            setFilterToTable(handles, TDT_AND_TOT_TABLE_PID, TOT_TABLE_ID);
            break;
        }
        case TABLE_TDT:
        {
            setFilterToTable(handles, TDT_AND_TOT_TABLE_PID, TDT_TABLE_ID);
            break;
        }
        default:
        {
            return ERROR;
        }
    }

    return NO_ERROR;
}
