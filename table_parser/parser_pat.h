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
* Datoteka deklarira funkcije potrebne za parsiranje PAT tablice
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
	uint16_t number_of_programs;
	pat_programm* pat_programm;
} pat_table;

int32_t filterPATParserCallback(uint8_t*);

int isPatTableParsed();

pat_table* getPATTable();

#endif