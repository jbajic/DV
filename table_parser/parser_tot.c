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
#include "parser_tot.h"

tot_table totTable;
int8_t isTOTParsed = FALSE;

int32_t filterTOTParserCallback(uint8_t* buffer)
{
    uint16_t mjd, numberOfLocales;
    int32_t i;
    printf("\n\nSection TOT arrived!!!\n\n");
	totTable.tableId = (uint8_t)
		*(buffer);

	totTable.sectionSyntaxIndicator = (uint8_t)
		(*(buffer + 1) >> 7);

    totTable.reservedFutureUse = (uint8_t)
		(*(buffer + 1) >> 6);

	totTable.section_length = (uint16_t)
		((*(buffer + 1) << 8) + *(buffer + 2)) & 0x0FFF;

    mjd = (uint16_t)
        ((*(buffer + 3) << 8) + *(buffer + 4));

    totTable.dateTimeUTC.time.hours = (uint8_t)
        (*(buffer + 5) >> 4) * 10 + ((*(buffer + 5)) & 0x0F);

    totTable.dateTimeUTC.time.minutes = (uint8_t)
        (*(buffer + 6) >> 4) * 10 + ((*(buffer + 6)) & 0x0F);
    
    totTable.dateTimeUTC.time.seconds = (uint8_t)
        (*(buffer + 7) >> 4) * 10 + ((*(buffer + 7)) & 0x0F);

    totTable.descriptorsLoopLength = (uint16_t)
        ((*(buffer + 8) << 8) + *(buffer + 9)) & 0x0FFF;

    totTable.descriptor.descriptorTag = (uint8_t)
        *(buffer + 10);

    totTable.descriptor.descriptorLength = (uint8_t)
        *(buffer + 11);

    numberOfLocales = (totTable.descriptorsLoopLength - 2) / 13;
    // printf("Number of locales %d\n", numberOfLocales);
    totTable.offsets = (local_time_offset*) malloc(numberOfLocales * sizeof(local_time_offset));
    for (i = 0; i < numberOfLocales; ++i)
    {
        uint16_t offsetMJD;
        totTable.offsets[i].countryCode[0] = (char)
            *(buffer + 12 + i * 13);

        totTable.offsets[i].countryCode[1] = (char)
            *(buffer + 13 + i * 13);

        totTable.offsets[i].countryCode[2] = (char)
            *(buffer + 14 + i * 13);

        totTable.offsets[i].countryCode[3] = '\0';

        totTable.offsets[i].countryRegionId = (uint8_t) 
            (*(buffer + 15 + i * 13) >> 2) & 0x3f;

        totTable.offsets[i].localTimeOffsetPolarity = (uint8_t) 
            *(buffer + 15 + i * 13) & 0x1;

        totTable.offsets[i].localTimeOffset.hours = (uint8_t) 
            (*(buffer + 16 + i * 13) >> 4) * 10 + ((*(buffer + 16 + i * 13)) & 0x0F);

        totTable.offsets[i].localTimeOffset.minutes = (uint8_t) 
            (*(buffer + 17 + i * 13) >> 4) * 10 + ((*(buffer + 17 + i * 13)) & 0x0F);

        offsetMJD = (uint16_t)
            ((*(buffer + 18 + i * 13) << 8) + *(buffer + 19 + i * 13));
        
        totTable.offsets[i].timeOfChange.time.hours = (uint8_t)
            (*(buffer + 20 + i * 13) >> 4) * 10 + ((*(buffer + 20 + i * 13)) & 0x0F);

        totTable.offsets[i].timeOfChange.time.minutes = (uint8_t)
            (*(buffer + 21 + i * 13) >> 4) * 10 + ((*(buffer + 21 + i * 13)) & 0x0F);
        
        totTable.offsets[i].timeOfChange.time.seconds = (uint8_t)
            (*(buffer + 22 + i * 13) >> 4) * 10 + ((*(buffer + 22 + i * 13)) & 0x0F);

        totTable.offsets[i].nextTimeOffset.hours = (uint8_t) 
            (*(buffer + 23 + i * 13) >> 4) * 10 + ((*(buffer + 23 + i * 13)) & 0x0F);

        totTable.offsets[i].nextTimeOffset.minutes = (uint8_t) 
            (*(buffer + 24 + i * 13) >> 4) * 10 + ((*(buffer + 24 + i * 13)) & 0x0F);

        setDateFromMJD(&totTable.offsets[i].timeOfChange.date, offsetMJD);

        // printf("OFFSET PARSED %d\n", i);
        // printf("countryCode: %s\n", totTable.offsets[i].countryCode);
        // printf("countryRegionId: %d\n", totTable.offsets[i].countryRegionId);
        // printf("localTimeOffsetPolarity: %d\n", totTable.offsets[i].localTimeOffsetPolarity);
        // printf("TIMEOFCHANGE YEAR : %d\n", totTable.offsets[i].timeOfChange.date.year);
        // printf("TIMEOFCHANGE MONTH : %d\n", totTable.offsets[i].timeOfChange.date.month);
        // printf("TIMEOFCHANGE DAY : %d\n", totTable.offsets[i].timeOfChange.date.dayInMonth);
        // printf("TIMEOFCHANGE HOURS : %u\n", totTable.offsets[i].timeOfChange.time.hours);
        // printf("TIMEOFCHANGE MINUTES: %u\n", totTable.offsets[i].timeOfChange.time.minutes);
        // printf("TIMEOFCHANGE SECONDS : %u\n", totTable.offsets[i].timeOfChange.time.seconds);
        // printf("localTimeOffset HOURS : %u\n", totTable.offsets[i].localTimeOffset.hours);
        // printf("localTimeOffset MINUTES: %u\n", totTable.offsets[i].localTimeOffset.minutes);
        // printf("nextTimeOffset HOURS : %u\n", totTable.offsets[i].nextTimeOffset.hours);
        // printf("nextTimeOffset MINUTES: %u\n", totTable.offsets[i].nextTimeOffset.minutes);
    }

    setDateFromMJD(&totTable.dateTimeUTC.date, mjd);

    // printf("TABLE ID: %u\n", totTable.tableId);
    // printf("numberOfLocales: %u\n", numberOfLocales);
    // printf("descriptorLoopLength: %u\n", totTable.descriptorsLoopLength);
    // printf("descriptorLength: %u\n", totTable.descriptor.descriptorLength);

    // printf("MJD : %d\n", mjd);
    // printf("YEAR : %d\n", totTable.dateTimeUTC.date.year);
    // printf("MONTH : %d\n", totTable.dateTimeUTC.date.month);
    // printf("DAY : %d\n", totTable.dateTimeUTC.date.dayInMonth);

    // printf("HOURS : %u\n", totTable.dateTimeUTC.time.hours);
    // printf("MINUTES: %u\n", totTable.dateTimeUTC.time.minutes);
    // printf("SECONDS : %u\n", totTable.dateTimeUTC.time.seconds);
    isTOTParsed = TRUE;
    return NO_ERROR;
}

int8_t isTOTTableParsed()
{
    return isTOTParsed;
}