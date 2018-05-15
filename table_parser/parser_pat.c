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
* Datoteka definira funkcije potrebne za parsiranje PAT tablice
* Made on 08.05.2018.
*
* @Author Jure Bajic
*****************************************************************************/
#include "parser_pat.h"

static pat_table patTable;
static int isPATParsed = FALSE;

int32_t filterPATParserCallback(uint8_t* buffer)
{
    int i;
    printf("\n\nSection arrived!!!\n\n");
	patTable.pat_header.table_id = (uint8_t)
		*(buffer);

	patTable.pat_header.section_syntax_indicator = (uint8_t)
		(*(buffer + 1) >> 7);

	patTable.pat_header.section_length = (uint16_t)
		((*(buffer+1) << 8) + *(buffer + 2)) & 0x0FFF;

	patTable.pat_header.transport_stream_id = (uint16_t)
		((*(buffer + 3) << 8) + *(buffer + 4));
	
	patTable.pat_header.version_number = (uint8_t)
		(*(buffer + 5) >> 1) & 0x1F;

	patTable.current_next_indicator = (uint8_t)
		*(buffer + 5) & 0x1;
	
	patTable.section_number = (uint8_t)
		*(buffer + 6);

	patTable.last_section_number = (uint8_t)
		*(buffer + 7);

	patTable.number_of_programs = (patTable.pat_header.section_length - 5 - 4) / 4;
	patTable.pat_programm = (pat_programm*) malloc(patTable.number_of_programs * sizeof(pat_programm));

	for (i = 0; i < patTable.number_of_programs; ++i) 
	{
		(patTable.pat_programm + i)->programm_number = (uint16_t)
			(*(buffer + 8 +(i * 4)) << 8) + *(buffer + 9 + (i * 4));

		(patTable.pat_programm + i)->programm_map_pid = (uint16_t)
			((*(buffer + 10 +(i*4)) << 8) + *(buffer + 11 + (i * 4))) & 0x1FFF;
		printf("programm_number: %d\n", (patTable.pat_programm + i)->programm_number);
		printf("programm_map_pid: %d\n\n", (patTable.pat_programm + i)->programm_map_pid);
	}

	// printf("table Id: %d\n", patTable.pat_header.table_id);
	// printf("section_syntax_indicator: %d\n", patTable.pat_header.section_syntax_indicator);
	// printf("section length: %d\n", patTable.pat_header.section_length);
	// printf("transport_stream_id: %d\n", patTable.pat_header.transport_stream_id);
	// printf("version_number: %d\n", patTable.pat_header.version_number);
	// printf("current_next_indicator: %d\n", patTable.current_next_indicator);
	// printf("section_number: %d\n", patTable.section_number);
	// printf("last_section_number: %d\n", patTable.last_section_number);
	// printf("broj bajtova u foru: %d\n", patTable.pat_header.section_length - 5 - 4);

	isPATParsed = TRUE;
    return 0;
}

int isPatTableParsed()
{
	return isPATParsed;
}

pat_table* getPATTable()
{
	return &patTable;
}