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
* File declares functions needed for the tuner
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

#define FREQUENCY_MGH 1000000
#define STREAM_NOT_FOUND 0

typedef struct _player_handles {
    uint32_t playerHandle;
    uint32_t sourceHandle;
    uint32_t videoStreamHandle;
    uint32_t audioStreamHandle;
    uint32_t filterHandle;
} player_handles;

typedef struct _player_handles_reminders
{
    player_handles* handles;
    reminder* channelReminders;
} player_handles_reminders;

typedef struct _player_handles_thread_args
{
    player_handles* handles;
    thread_args* threadArguments;
} player_handles_thread_args;

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

/****************************************************************************
*
* @brief
* Function for tuner initialization
*
* @param
*       config - [in] Config in which the initialization parameters are contained
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t tunerInitialization(config_parameters* config);

/****************************************************************************
*
* @brief
* Function for player initialization and start
*
* @param
*       handles - [in] Player handles in which to initialize player and open source
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t startPlayer(player_handles* handles);

/****************************************************************************
*
* @brief
* Function for creating stream
*
* @param
*       handles - [in] structure containing all handles
*       config - [in] Config which contains which programm to start
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t createStream(player_handles* handles, config_parameters* config);

/****************************************************************************
*
* @brief
* Function for changing stream
*
* @param
*       handles - [in] structure containing all handles
*       channelNumber - [in] Channel number of which the stream is played
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t changeStream(player_handles* handles, int32_t channelNumber);

/****************************************************************************
*
* @brief
* Function for starting the parsing of tables
*
* @param
*        ????
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t setupData(player_handles* handles, thread_args* threadArguments);

/****************************************************************************
*
* @brief
* Function for starting the demux filtering
*
* @param
*       filterCallback() - [in] Which filterCallback to playe in demux
*       isTableParsed() - [in] Callback of the table to tell if that table is parsed
*       handles - [in] structure containing all handles
*       tablePID - [in] PID of table to parse
*       tableId - [in] ID of table to parse
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t setFilterToTable(int32_t (*filterCallback)(uint8_t*), int8_t (*isTableParsed)(), player_handles* handles, int32_t tablePID, int32_t tableId);

/****************************************************************************
*
* @brief
* Function for freeing filter
*
* @param
*       filterCallback() - [in] Which filterCallback to playe in demux
*       handles - [in] structure containing all handles
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t freeFilterCallback(int32_t (*filterCallback)(uint8_t*), player_handles* handles);

/****************************************************************************
*
* @brief
* Function for removing stream
*
* @param
*       handles - [in] structure containing all handles
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t removeStream(player_handles* handles);

/****************************************************************************
*
* @brief
* Function for stopping player
*
* @param
*       handles - [in] structure containing all handles
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t stopPlayer(player_handles* handles);

/****************************************************************************
*
* @brief
* Function for tuner deinitialization
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t tunerDeinitialization();

/****************************************************************************
*
* @brief
* Function for getting the stream type from dvb standards
*
* @return
*   stream_audio, enum of the stream
*   STREAM_NOT_FOUND, if there is no error
****************************************************************************/
int8_t getStreamType(uint8_t streamType);

#endif