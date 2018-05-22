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
* \player_tuner.h
* \brief
* Datoteka deklarira funkcije potrebne za rad tunera
* Made on 08.05.2018.
*
* @Author Jure Bajic
*****************************************************************************/
#ifndef PLAYER_TUNER_H
#define PLAYER_TUNER_H

#include <sys/time.h>
#include <pthread.h>
#include "errno.h"

#include "./../init_configuration/init_configuration.h"
#include "./../table_parser/parser_pat.h"
#include "./../table_parser/parser_pmt.h"
#include "./../table_parser/parser_tdt.h"
#include "./../table_parser/parser_tot.h"

#define ASSERT_TDP_RESULT(x,y)  if(NO_ERROR == x) \
                                    printf("%s success\n", y); \
                                else{ \
                                    textColor(1,1,0); \
                                    printf("%s fail\n", y); \
                                    textColor(0,7,0); \
                                    return -1; \
                                }
// #define TABLE_PAT_ID 0x00
// #define TABLE_PMT_ID 0x02
// #define TABLE_TDT_ID 0x70
// #define TABLE_TOT_ID 0x73
#define FREQUENCY_MGH 1000000

typedef struct _player_handles {
    uint32_t playerHandle;
    uint32_t sourceHandle;
    uint32_t videoStreamHandle;
    uint32_t audioStreamHandle;
    uint32_t filterHandle;
} player_handles;

enum table_id
{
    pat_table_id = 0x00,
    pmt_table_id = 0x02,
    tdt_table_id = 0x70,
    tot_table_id = 0x73
};

enum table_pid
{
    pat_table_pid = 0x00000,
    tdt_and_tot_table_pid = 0x0014,
};

int32_t tunerInitialization(config_parameters*);

int32_t startPlayer(player_handles*);

int32_t createStream(player_handles*, config_parameters*);

int32_t changeStream(player_handles*, int32_t);

int32_t setupData(player_handles*);

int32_t setFilterToPAT(int32_t (*filterCallback)(uint8_t*), player_handles*);

int32_t setFilterToPMT(int32_t (*filterCallback)(uint8_t*), player_handles*);

int32_t setFilterToTDT(int32_t (*filterCallback)(uint8_t*), player_handles*);

int32_t setFilterToTOT(int32_t (*filterCallback)(uint8_t*), player_handles*);

int32_t setFilterToTable(int32_t (*filterCallback)(uint8_t*), player_handles*, int32_t, int32_t);

int32_t freeFilterCallback(int32_t (*filterCallback)(uint8_t*), player_handles*);

int32_t removeStream(player_handles*);

int32_t stopPlayer(player_handles*);

int32_t tunerDeinitialization();

int32_t myPrivateTunerStatusCallback(t_LockStatus);

void* threadPATParse();

int8_t getStreamType(uint8_t);

#endif