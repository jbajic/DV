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
#include "./../table_parser/table.h"

#define FREQUENCY_MGH 1000000
#define STREAM_NOT_FOUND 0
#define NO_STREAM_TYPE 1

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
*       handles - [in] Player handles in which to initialize player and open source
*       threadArguments - [in] mutex and condition for starting and finishing thread in which callback for tdt and tot is set as filter
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t setupData(player_handles* handles, thread_args* threadArguments);

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
* Function for getting the video stream type from dvb standards
*
* @return
*   dvb_streams, enum of the stream
*   STREAM_NOT_FOUND, if there is no error
****************************************************************************/
int8_t getVideoStreamType(uint8_t streamType);

/****************************************************************************
*
* @brief
* Function for getting the audio stream type from dvb standards
*
* @return
*   dvb_streams, enum of the stream
*   STREAM_NOT_FOUND, if there is no error
****************************************************************************/
int8_t getAudioStreamType(uint8_t streamType);

#endif