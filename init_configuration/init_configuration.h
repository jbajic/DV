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
* \init_configuration.h
* \brief
* Datoteka sadrzi deklaracije potrebnih metoda i struktura za parsiranje XML datoteke
* Made on 07.05.2018.
*
* @Author Jure Bajic
*****************************************************************************/
#ifndef INIT_CONFIGURATION_H
#define INIT_CONFIGURATION_H

#include <string.h>
#include "./../base.h"
#include "reminder_node.h"

#define MAX_NUMBER_OF_ELEMENTS 10

enum config_keys {
    FREQUENCY_KEY = 0,
    BANDWIDTH_KEY,
    MODULE_KEY,
    APID_KEY,
    VPID_KEY,
    ATYPE_KEY,
    VTYPE_KEY,
    TIME_KEY,
    CHANNEL_INDEX_KEY,
    NON_EXISTENT_KEY
};

enum dvb_streams {
    stream_reserved0        = 0x00,
    stream_video            = 0x01,
    stream_video_h262       = 0x02,
    stream_audio            = 0x03,
    stream_audio_13818_3    = 0x04,
    stream_private_sections = 0x05,
    stream_private_data     = 0x06,
    stream_mheg             = 0x07,
    stream_h222             = 0x08,
    stream_h222_1           = 0x09,
    stream_13818_6_A        = 0x0A,
    stream_13818_6_B        = 0x0B,
    stream_13818_6_C        = 0x0C,
    stream_13818_6_D        = 0x0D,
    stream_h222_aux         = 0x0E,
    stream_audio_adts       = 0x0F,
    stream_video_14496_2    = 0x10,
    stream_audio_latm       = 0x11,
    stream_14496_1_pes      = 0x12,
    stream_14496_1_iso      = 0x13,
    stream_download         = 0x14,
    stream_reserved         = 0x15,
    stream_private          = 0x80
};

#define PARSE_ERROR -1

typedef struct { char* key; enum config_keys val; } t_symstruct;

typedef struct init_service
{
    uint16_t apid;
    uint16_t vpid;
    tStreamType atype;
    tStreamType vtype;
} init_service;

typedef struct _config_parameters
{
    uint16_t frequency;
    uint16_t bandwidth;
    t_Module module;
    struct init_service service;
    reminder* headReminder;
} config_parameters;

/****************************************************************************
*
* @brief
* Function takes in file path from which it parses neccesary data for config file
*
* @param
*       filePath - [in] Field of characters representig file path to config
*       config - [in] Pointer to structure of config parameters
*
* @return
*   NO_ERROR, in case of no error
*   ERROR, in case of error
*
****************************************************************************/
int32_t loadFile(char** file_path, config_parameters* config);

/****************************************************************************
*
* @brief
* Function analyzes word and parses it into appropriate value
*
* @param
*       word - [in] Field of characters representig possible key word
*       filePointer - [in] Pointer to the opened file
*       config - [in] Pointer to the config structure to which the parsed data will be written
*
* @return
*   NO_ERROR, in case of no error
*   ERROR, in case of error
*
****************************************************************************/
int32_t analyzeWord(char* word, FILE** filePointer, config_parameters* config);

/****************************************************************************
*
* @brief
* Function is used only for testing purposes, to show config file
*
* @param
*       config - [in] Pointer to the config structure to which the parsed data is written
*
****************************************************************************/
void testConfigPrintf(config_parameters* config);

#endif
