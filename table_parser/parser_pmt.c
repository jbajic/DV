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
* Datoteka deklarira funkcije i strukture potrebne za parsiranje PMT tablice
* Made on 08.05.2018.
*
* @Author Jure Bajic
*****************************************************************************/
#include "parser_pmt.h"

static pmt_table* pmtTables;
static int isPMTTableParsed = FALSE;
static int currentPMTTableIndex = 0;

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
	
	currentPMT->streams = (pmt_streams*) malloc(4 * sizeof(pmt_streams));
	
	maxNumberOfStreams = (currentPMT->section_length - 13) / 5;
	numberOfBytesStreams = (currentPMT->section_length - 13);
	// currentPMT->streams = (pmt_streams*) malloc(maxNumberOfStreams * sizeof(pmt_streams));
	
	for (i = 0; i < 4; ++i) 
    {
	
		currentPMT->streams[i].stream_type = (uint8_t) 
			*(buffer + 12 + i * 5 + offset);
			
		currentPMT->streams[i].elementary_PID = (uint16_t) 
			((*(buffer + 13 + i * 5 + offset) << 8) + *(buffer + 14 + i * 5 + offset)) & 0x1FFF;
		
		currentPMT->streams[i].ES_info_length = (uint16_t)
			((*(buffer + 15 + i * 5 + offset) << 8) + *(buffer + 16 + i * 5 + offset)) & 0x0FFF;
			
		offset += (currentPMT->streams + i)->ES_info_length;
		numberOfBytesStreams -= 5 - (currentPMT->streams + i)->ES_info_length;
		if (numberOfBytesStreams == 0)
        {
            break;
        }
		printf("PMT Parsing:\n");
		printf("stream type: %d\n", (currentPMT->streams + i)->stream_type);
		printf("elementary PID: %d\n", (currentPMT->streams + i)->elementary_PID);
		printf("ES info length: %d\n\n", (currentPMT->streams + i)->ES_info_length);
    }
	currentPMTTableIndex++;
	isPMTTableParsed = TRUE;
    return NO_ERROR;
}

int isPmtTableParsed()
{
	return isPMTTableParsed;
}

void setPmtTableParsedFalse()
{
	isPMTTableParsed = FALSE;
}

pmt_table* getPMTTable(int32_t channelNumber)
{
	return &pmtTables[channelNumber];
}

void allocatePMTTables(int number_of_programs)
{
    pmtTables = (pmt_table*) malloc(number_of_programs * sizeof(pmt_table));
}