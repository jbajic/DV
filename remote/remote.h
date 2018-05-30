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
* File declares functions needed for remote functionalities
* Made on 08.05.2018.
*
* @Author Jure Bajic
*****************************************************************************/
#ifndef REMOTE_H
#define REMOTE_H

#include "./../player_tuner/player_tuner.h"
#include "./../graphics/graphics.h"
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <errno.h>
#include <time.h>

#define NUM_EVENTS  5

enum remote_keys
{
    REM_CHANNEL_0 = 11,
    REM_CHANNEL_1 = 2,
    REM_CHANNEL_2 = 3,
    REM_CHANNEL_3 = 4,
    REM_CHANNEL_4 = 5,
    REM_CHANNEL_5 = 6,
    REM_CHANNEL_6 = 7,
    REM_CHANNEL_7 = 8,
    REM_CHANNEL_8 = 9,
    REM_CHANNEL_9 = 10,
    REM_OK = 28,
    REM_MUTE = 60,
    REM_CHANNEL_DOWN = 61,
    REM_CHANNEL_UP = 62,
    REM_VOLUME_UP = 63,
    REM_VOLUME_DOWN = 64,
    REM_EXIT = 102,
    REM_ARROW_LEFT = 105,
    REM_ARROW_RIGHT = 106,
    REM_INFO = 358,
    REM_RED_BUTTON = 398
};

typedef struct _timer_remove_graphics_feature
{
    graphics* graphicsStruct;
    enum graphics_features featureToRemove;
} timer_remove_graphics_feature;

typedef struct _back_proc_args
{
    graphics* graphicsStruct;
    reminder* reminderHead;
} back_proc_args;

typedef struct _remote_loop_args {
    struct input_event* eventBuf;
    int32_t inputFileDesc;
    player_handles* handles;
    graphics* graphicsStruct;
} remote_loop_args;

typedef struct _change_channel_args
{
    player_handles* handles;
    int32_t channelNumber;
    int32_t currentChannel;
    int32_t numberOfPrograms;
    graphics* graphicsStruct;
} change_channel_args;

typedef struct _timer_channel_changer_args
{
    change_channel_args* changeChannelArgs;
    timer_struct channelChangerTimer;
    timer_struct* removeChannelNumberTypedTimer;
} timer_channel_changer_args;

/****************************************************************************
*
* @brief
* Function for initiating remote
*
* @param
*   handles - [in] Player handles needed when initiating programm
*   graphicsStruct - [in] Graphics struct for drawing informations
*   reminderHead - [in] Head of reminder list
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t startRemote(player_handles* handles, graphics* graphicsStruct, reminder* reminderHead);

/****************************************************************************
*
* @brief
* Function for checking for new TDT data
*
* @param
*   args - [in] Structure containtg thread related information
*
* @return
*   NO_ERROR, if there is no error
****************************************************************************/
void* checkForTDTData(void* args);

/****************************************************************************
*
* @brief
* Function for initiating remote loop which handles remote related events
*
* @param
*   args - [in] remoteArgs containing all neede variables, for graphics, changing programs and for reminders
*
* @return
*   NO_ERROR, if there is no error
****************************************************************************/
void* initRemoteLoop(void* args);

#endif