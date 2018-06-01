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
* File declares functions and structures needed for parsing PAT table
* Made on 08.05.2018.
*
* @Author Jure Bajic
*****************************************************************************/
#ifndef PARSER_PAT_H
#define PARSER_PAT_H

#include "./../base.h"

typedef struct _pat_header {
	uint8_t table_id;
	uint8_t section_syntax_indicator;
	uint16_t section_length;
	uint16_t transport_stream_id;
	uint8_t version_number;
} pat_header;

typedef struct _pat_programm {
	uint16_t programm_number;
	uint16_t programm_map_pid;
} pat_programm;

typedef struct _pat_table {
	pat_header pat_header;
	uint8_t current_next_indicator;
	uint8_t section_number;
	uint8_t last_section_number;
	int32_t number_of_programs;
	pat_programm* pat_programm;
} pat_table;


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
int32_t filterPATParserCallback(uint8_t* buffer, pthread_mutex_t* tableParserMutex, pthread_cond_t* tableParserCondition);

/****************************************************************************
*
* @brief
* Function fo returnig pointer to the parsed PAT table
*
* @return
*   pat_table*
*
****************************************************************************/
pat_table* getPATTable();

#endif