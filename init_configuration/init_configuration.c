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
* \init_configuration.c
* \brief
* Datoteka sadrzi funkcije za parsiranje XML datoteke u config_parameters strukturu
* Made on 07.05.2018.
*
* @Author Jure Bajic
*****************************************************************************/
#include "init_configuration.h"

static t_symstruct configKeysLookupTable[] = {
    {"frequency", FREQUENCY_KEY}, {"bandwidth", BANDWIDTH_KEY}, {"module", MODULE_KEY}, 
    {"apid", APID_KEY}, {"vpid", VPID_KEY}, {"atype", ATYPE_KEY}, {"vtype", VTYPE_KEY}, 
    {"time", TIME_KEY}, {"channel_index", CHANNEL_INDEX_KEY}
};

static t_symstruct streamTypesLookUpTable[] = {
    {"mpeg2", stream_video_h262}, {"ac3", stream_audio}, {"audio", stream_audio}
};

static t_symstruct modulesLookUpTable[] = {
    {"DVB-T", DVB_T}, {"DVB-T2", DVB_T2}
};

#define N_KEY_CONFIG_TABLE (sizeof (configKeysLookupTable) / sizeof (t_symstruct))
#define N_KEY_STREAM_TYPE_TABLE (sizeof (streamTypesLookUpTable) / sizeof (t_symstruct))
#define N_KEY_MODULES_TABLE (sizeof (modulesLookUpTable) / sizeof (t_symstruct))

/****************************************************************************
*
* @brief
* Function searches through lookuptable in which certain keyword are bound to specific cases (enums)
*
* @param
*       key - [in] Key of the case
*       table - [in] Lookuptable in which the key should be found
*       numberOfValues - [in] Number of values in the lookuptable
*
* @return
*   NO_ERROR, in case of no error
*   ERROR, in case of error
*
****************************************************************************/
static int32_t keyFromString(char* key, t_symstruct* table, int32_t numberOfValues)
{
    int32_t i;
    for (i = 0; i < numberOfValues; i++)
    {
        t_symstruct* sym = (table + i);
        if (strcmp(sym->key, key) == 0)
        {
            return sym->val;
        }
    }
    return NON_EXISTENT_KEY;
}

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
int32_t analyzeWord(char* word, FILE** filePointer, config_parameters* config)
{
    int32_t character;
    char* value = (char*) malloc(sizeof (char) * MAX_NUMBER_OF_ELEMENTS);
    int32_t hours, minutes;
    int32_t valueCounter = 0;
    while ((character = fgetc(*filePointer)) != '<')
    {
        if (character == '\n') continue;
        value[valueCounter] = character;
        valueCounter++;
    }
    value[valueCounter] = '\0';
    switch (keyFromString(word, configKeysLookupTable, N_KEY_CONFIG_TABLE))
    {
        case FREQUENCY_KEY:
        {
            config->frequency = atoi(value);
            break;
        }
        case BANDWIDTH_KEY:
        {
            config->bandwidth = atoi(value);
            break;
        }
        case MODULE_KEY:
        {
            config->module = keyFromString(value, modulesLookUpTable, N_KEY_MODULES_TABLE);
            break;
        }
        case APID_KEY:
        {
            config->service.apid = atoi(value);
            break;
        }
        case VPID_KEY:
        {
            config->service.vpid = atoi(value);
            break;
        }
        case ATYPE_KEY:
        {
            config->service.atype = keyFromString(value, streamTypesLookUpTable, N_KEY_STREAM_TYPE_TABLE);
            break;
        }
        case VTYPE_KEY:
        {
            config->service.vtype = keyFromString(value, streamTypesLookUpTable, N_KEY_STREAM_TYPE_TABLE);
            break;
        }
        case TIME_KEY:
        {
            hours = atoi(&value[0]);
            minutes = atoi(&value[3]);
            addReminderTime(hours, minutes, &config->headReminder);
            break;
        }
        case CHANNEL_INDEX_KEY:
        {
            addReminderChannelIndex(atoi(value), &config->headReminder);
            break;
        }
    }
    return NO_ERROR;
}

/****************************************************************************
*
* @brief
* Function decides if the given string is a key word or not
*
* @param
*       word - [in] Field of characters representig possible key word
*
* @return
*   NO_ERROR, in case of no error
*   ERROR, in case of error
*
****************************************************************************/
static int32_t isNonValueKeyWord(char* word)
{
    return strcmp("config", word) == 0 || strcmp("reminder", word) == 0 || strcmp("init_service", word) == 0;
}

/****************************************************************************
*
* @brief
* Function takes in file path from which it parses neccesary data for config file
*
* @param
*       file_path - [in] Field of characters representig file path to config
*       config - [in] Pointer to structure of config parameters
*
* @return
*   NO_ERROR, in case of no error
*   ERROR, in case of error
*
****************************************************************************/
int32_t loadFile(char** file_path, config_parameters* config)
{
    FILE* filePointer;
    int character;
    char word[100];
    int wordIndex;
    int isEnclosing = FALSE;
    config->headReminder = NULL;
    
    filePointer = fopen(*file_path, "r");
    if (filePointer == NULL)
    {
        return ERROR;
    }

    while ((character = fgetc(filePointer)) != EOF)
    {
        switch (character)
        {
            case '<':
            {
                wordIndex = 0;
                break;
            }
            case '>':
            {
                word[wordIndex] = '\0';

                if (isNonValueKeyWord(word))
                {
                    wordIndex = 0;
                }
                else if (isEnclosing == FALSE)
                {
                    analyzeWord(word, &filePointer, config);
                }
                isEnclosing = FALSE;
                break;
            }
            case '/':
            {
                isEnclosing = TRUE;
                break;
            }
            case '\n':
            {
                break;
            }
            default:
            {
                word[wordIndex] = character;
                wordIndex++;
            }
        }
    }
    
    fclose(filePointer);
    return NO_ERROR;
}

void testConfigPrintf(config_parameters* config)
{
    printf("Frequency %d\n", config->frequency);
    printf("BAND %d\n", config->bandwidth);
    printf("module %d\n", config->module);
    printf("apid %d\n", config->service.apid);
    printf("vpid %d\n", config->service.vpid);
    printf("atype %d\n", config->service.atype);
    printf("vtype %d\n", config->service.vtype);
    printf("reminder1 time %d:%d\n", config->headReminder->time.hours, config->headReminder->time.minutes);
    printf("reminder1 index %d\n", config->headReminder->channel_index);
    printf("reminder2 time \"%d:%d\"\n", config->headReminder->next->time.hours, config->headReminder->next->time.minutes);
    printf("reminder2 index %d\n", config->headReminder->next->channel_index);
}
