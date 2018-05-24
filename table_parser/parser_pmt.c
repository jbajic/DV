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
* \parser_pat.c
* \brief
* File defines functions needed for parsing PMT table
* Made on 08.05.2018.
*
* @Author Jure Bajic
*****************************************************************************/
#include "parser_pmt.h"

static pmt_table* pmtTables;
static int isPMTTableParsed = FALSE;
static int currentPMTTableIndex = 0;

/****************************************************************************
*
* @brief
* Function for parsing PMT table
*
* @param
*       buffer - [in] Pointer to the filtered incoming streamn which contains PMT table packets
*
* @return
*   NO_ERROR, in case of no error
*   ERROR, in case of error
*
****************************************************************************/
int32_t filterPMTParserCallback(uint8_t* buffer)
{
    int i, maxNumberOfStreams, numberOfBytesStreams;
	int offset = 0;
	isPMTTableParsed = FALSE;
	pmt_table* currentPMT = &pmtTables[currentPMTTableIndex];
	printf("PMT arrived\n");

	currentPMT->table_id = (uint8_t)
		*(buffer);
		
	currentPMT->section_syntax_indicator = (uint8_t)
		(*(buffer + 1) >> 7);
	 
	currentPMT->section_length = (uint16_t)
		((*(buffer + 1) << 8) + *(buffer + 2)) & 0x0FFF;
		
	currentPMT->programm_number = (uint16_t) 
		((*(buffer + 3) << 8) + *(buffer + 4));
		
	currentPMT->version_number = (uint8_t)
		(*(buffer + 5) & 0x3D);
		
	currentPMT->current_next_indicator = (uint8_t)
		(*(buffer + 5) & 0x01);
		
	currentPMT->section_number = (uint8_t)
		*(buffer + 6);
		
	currentPMT->last_section_number = (uint8_t)
		*(buffer + 7);
		
	currentPMT->PCR_PID = (uint16_t)
		(((*(buffer + 8) << 8) + *(buffer + 9)) & 0x1FFF);
		
	currentPMT->program_info_length = (uint16_t)
		((*(buffer + 10) << 8) + *(buffer + 11)) & 0x0FFF;
	
	
	maxNumberOfStreams = (currentPMT->section_length - 13) / 5;
	numberOfBytesStreams = (currentPMT->section_length - 13);
	// printf("MAX NUMBER OF STREAMS %d", maxNumberOfStreams);
	// currentPMT->streams = (pmt_streams*) malloc(4 * sizeof(pmt_streams));
	currentPMT->streams = (pmt_streams*) malloc(maxNumberOfStreams * sizeof(pmt_streams));
	
	for (i = 0; i < maxNumberOfStreams; ++i) 
    {
		currentPMT->streams[i].stream_type = (uint8_t) 
			*(buffer + 12 + i * 5 + offset);
			
		currentPMT->streams[i].elementary_PID = (uint16_t) 
			((*(buffer + 13 + i * 5 + offset) << 8) + *(buffer + 14 + i * 5 + offset)) & 0x1FFF;
		
		currentPMT->streams[i].ES_info_length = (uint16_t)
			((*(buffer + 15 + i * 5 + offset) << 8) + *(buffer + 16 + i * 5 + offset)) & 0x0FFF;
			
		currentPMT->streams[i].descriptor = (uint8_t)
			(*(buffer + 17 + i * 5 + offset));
		offset += (currentPMT->streams + i)->ES_info_length;
		numberOfBytesStreams -= (5 + (currentPMT->streams + i)->ES_info_length);
		if (numberOfBytesStreams <= 0)
        {
            break;
        }
		// printf("stream type: %d\n", (currentPMT->streams + i)->stream_type);
		// printf("elementary PID: %d\n", (currentPMT->streams + i)->elementary_PID);
		// printf("ES info length: %d\n", (currentPMT->streams + i)->ES_info_length);
    }
	currentPMT->numberOfStreams = i;
	currentPMTTableIndex++;
	isPMTTableParsed = TRUE;
    return NO_ERROR;
}

/****************************************************************************
*
* @brief
* Function informing other if PAT is parsed or not
*
* @return
*   TRUE, if PAT is parsed
*   FALSE, if PAT is not parsed
*
****************************************************************************/
int8_t isPmtTableParsed()
{
	return isPMTTableParsed;
}

/****************************************************************************
*
* @brief
* Function for setting variable isPMTTableParsed to FALSE
*
****************************************************************************/
void setPmtTableParsedFalse()
{
	isPMTTableParsed = FALSE;
}

/****************************************************************************
*
* @brief
* Function for returnig pointer to the parsed PMT table
*
* @param
*       channelNumber - [in] Number of channel to return
*
* @return
*   pmt_table*
*
****************************************************************************/
pmt_table* getPMTTable(int32_t channelNumber)
{
	return &pmtTables[channelNumber];
}

/****************************************************************************
*
* @brief
* Function for allocating memory for PMT tables
* @param
*       number_of_programs - [in] Number of programms to allocate
*
* @return
*   pmt_table*
*
****************************************************************************/
void allocatePMTTables(int32_t number_of_programs)
{
    pmtTables = (pmt_table*) malloc(number_of_programs * sizeof(pmt_table));
}