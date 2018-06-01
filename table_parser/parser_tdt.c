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
* \parser_tdt.c
* \brief
* File defines functions needed for parsing TDT table
* Made on 21.05.2018.
*
* @Author Jure Bajic
*****************************************************************************/
#include "parser_tdt.h"

tdt_table tdtTable;

/****************************************************************************
*
* @brief
* Function for parsing TDT table
*
* @param
*       buffer - [in] Pointer to the filtered incoming streamn which contains TDT table packets
*       tableParserMutex - [in] Mutex for parsing tables
*       tableParserCondition - [in] Condition to signal if table is parsed
*
* @return
*   NO_ERROR, in case of no error
*   ERROR, in case of error
*
****************************************************************************/
int32_t filterTDTParserCallback(uint8_t* buffer, pthread_mutex_t* tableParserMutex, pthread_cond_t* tableParserCondition)
{
    uint16_t mjd;
    printf("\n\nSection TDT arrived!!!\n\n");
	tdtTable.tableId = (uint8_t)
		*(buffer);

	tdtTable.sectionSyntaxIndicator = (uint8_t)
		(*(buffer + 1) >> 7);

    tdtTable.reservedFutureUse = (uint8_t)
		(*(buffer + 1) >> 6);

	tdtTable.section_length = (uint16_t)
		((*(buffer + 1) << 8) + *(buffer + 2)) & 0x0FFF;

    mjd = (uint16_t)
        ((*(buffer + 3) << 8) + *(buffer + 4));

    tdtTable.dateTimeUTC.time.hours = (uint8_t)
        ((*(buffer + 5) >> 4) * 10) + (((*(buffer + 5)) & 0x0F));

    tdtTable.dateTimeUTC.time.minutes = (uint8_t)
        ((*(buffer + 6) >> 4) * 10) + (((*(buffer + 6)) & 0x0F));
    
    tdtTable.dateTimeUTC.time.seconds = (uint8_t)
        ((*(buffer + 7) >> 4) * 10) + (((*(buffer + 7)) & 0x0F));

    setDateFromMJD(&tdtTable.dateTimeUTC.date, mjd);

    // printf("TABLE ID: %u\n", tdtTable.tableId);
    // printf("sectionSyntaxIndicator: %u\n", tdtTable.sectionSyntaxIndicator);
    // printf("reservedFutureUse: %u\n", tdtTable.reservedFutureUse);
    // printf("section_length: %u\n", tdtTable.section_length);

    // printf("MJD : %d\n", mjd);
    // printf("YEAR : %d\n", tdtTable.dateTimeUTC.date.year);
    // printf("MONTH : %d\n", tdtTable.dateTimeUTC.date.month);
    // printf("DAY : %d\n", tdtTable.dateTimeUTC.date.dayInMonth);

    // printf("HOURS : %u\n", tdtTable.dateTimeUTC.time.hours);
    // printf("MINUTES: %u\n", tdtTable.dateTimeUTC.time.minutes);
    // printf("SECONDS : %u\n", tdtTable.dateTimeUTC.time.seconds);
    pthread_mutex_lock(tableParserMutex);
	pthread_cond_signal(tableParserCondition);
	pthread_mutex_unlock(tableParserMutex);
    return NO_ERROR;
}

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
tdt_table* getTDTTable()
{
    return &tdtTable;
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