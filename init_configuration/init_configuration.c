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

static int keyFromString(char* key)
{
    int i;
    for (i = 0; i < NKEYS; i++)
    {
        t_symstruct *sym = (configLookuptable + i);
        if (strcmp(sym->key, key) == 0)
        {
            return sym->val;
        }
    }
    return NON_EXISTENT_KEY;
}

int analyzeWord(char* word, FILE** filePointer, config_parameters* config)
{
    int character;
    char* value = (char*) malloc(sizeof (char) * MAX_NUMBER_OF_ELEMENTS);
    int valueCounter = 0;
    while ((character = fgetc(*filePointer)) != '<')
    {
        if (character == '\n') continue;
        value[valueCounter] = character;
        valueCounter++;
    }
    value[valueCounter] = '\0';
    switch (keyFromString(word))
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
            if (strcmp("DVB_T", value) == 0)
            {
                config->module = DVB_T;
            }
            else if (strcmp("DVB_T2", value) == 0)
            {
                config->module = DVB_T;
            }
            else
            {
                return PARSE_ERROR;
            }
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
            if (strcmp("ac3", value) == 0)
            {
                config->service.atype = AUDIO_TYPE_DOLBY_AC3;
            }
            else 
            {
                return PARSE_ERROR;
            }
            break;
        }
        case VTYPE_KEY:
        {
            if (strcmp("mpeg2", value) == 0)
            {
                config->service.vtype = VIDEO_TYPE_MPEG2;
            }
            else 
            {
                return PARSE_ERROR;
            }
            break;
        }
        case TIME_KEY:
        {
            addReminderTime(&value, &config->headReminder);
            break;
        }
        case CHANNEL_INDEX_KEY:
        {
            addReminderChannelIndex(atoi(value), &config->headReminder);
            break;
        }
    }
}

static int isNonValueKeyWord(char* word)
{
    return strcmp("config", word) == 0 || strcmp("reminder", word) == 0 || strcmp("init_service", word) == 0;
}

config_parameters* loadFile(char** file_path)
{
    FILE* filePointer;
    int character;
    char word[100];
    int wordIndex;
    int isEnclosing = FALSE;
    config_parameters* config = (config_parameters*) malloc(sizeof(config_parameters));
    config->headReminder = NULL;
    
    filePointer = fopen(*file_path, "r");

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
    printf("File Path: %s\n", *(file_path));
    return config;
}

int freeConfig(config_parameters* config)
{
    free(config);
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
    printf("reminder1 time %s\n", config->headReminder->time);
    printf("reminder1 index %d\n", config->headReminder->channel_index);
    printf("reminder2 time %s\n", config->headReminder->next->time);
    printf("reminder2 index %d\n", config->headReminder->next->channel_index);
}