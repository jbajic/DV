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
* \parser_pat.h
* \brief
* File declares functions and structures needed for parsing PMT table
* Made on 08.05.2018.
*
* @Author Jure Bajic
*****************************************************************************/
#ifndef PARSER_PMT_H
#define PARSER_PMT_H

#include "table.h"

typedef struct _pmt_streams {
	uint8_t stream_type;
	uint16_t elementary_PID;
	uint16_t ES_info_length;
	uint8_t descriptor;
} pmt_streams;

typedef struct _pmt_table {
	uint8_t table_id;
	uint8_t section_syntax_indicator;
	uint16_t section_length;
	uint16_t programm_number;
	uint8_t version_number;
	uint8_t current_next_indicator;
	uint8_t section_number;
	uint8_t last_section_number;
	uint16_t PCR_PID;
	uint16_t program_info_length;
	uint16_t numberOfStreams;
	pmt_streams* streams;
} pmt_table;

int32_t filterPMTParserCallback(uint8_t*);

pmt_table* getPMTTable(int32_t channelNumber);

void allocatePMTTables(int32_t);

#endif