#include "init_configuration.h"


static int keyFromString(char* key)
{
    int i;
    for (i = 0; i < NKEYS; i++)
    {
        t_symstruct *sym = (lookuptable + i);
        // t_symstruct *sym = &lookuptable[i];
        if (strcmp(sym->key, key) == 0)
        {
            fflush(stdout);
            return sym->val;
        }
    }
    return NON_EXISTENT_KEY;
}

int analyzeWord(char* word, FILE** filePointer, config_parameters* config)
{
    int character;
    char value[MAX_NUMBER_OF_ELEMENTS];
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
            printf("Frekvencija: %d\n", atoi(value));
            config->frequency = atoi(value);
            break;
        }
        case BANDWIDTH_KEY:
        {
            printf("bandwidth: %d\n", atoi(value));
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
            printf("apid: %d\n", atoi(value));
            break;
        }
        case VPID_KEY:
        {
            config->service.vpid = atoi(value);
            printf("vpid: %d\n", atoi(value));
            break;
        }
        case ATYPE_KEY:
        {
            if (strcmp("mpeg2", value) == 0)
            {
                config->service.atype = VIDEO_TYPE_MPEG2;
            }
            else 
            {
                return PARSE_ERROR;
            }
            printf("atype: %s\n", value);
            break;
        }
        case VTYPE_KEY:
        {
            if (strcmp("ac3", value) == 0)
            {
                config->service.vtype = AUDIO_TYPE_DOLBY_AC3;
            }
            else 
            {
                return PARSE_ERROR;
            }
            printf("vtype: %s\n", value);
            break;
        }
        case TIME_KEY:
        {
            printf("time: %s\n", value);
            addReminderTime(value, config);
            break;
        }
        case CHANNEL_INDEX_KEY:
        {
            addReminderChannelIndex(atoi(value), config);
            printf("channel_index: %d\n", atoi(value));
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
                fflush(stdout);

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
    return NULL;
}

int freeConfig(config_parameters* config)
{
    free(config);
}

void addReminderTime(char* time, config_parameters* config)
{
    if (config->headReminder == NULL)
    {
        config->headReminder = (reminder*) malloc(sizeof(reminder));
        config->headReminder->time = time;
        config->headReminder->next = NULL;
    }
    else
    {
        reminder* newReminder = config->headReminder->next;
        while (newReminder != NULL)
        {
            newReminder = newReminder->next;
        }
        newReminder = (reminder*) malloc(sizeof (reminder));
        newReminder->time = time;
        newReminder->next = NULL;
    }
}

void addReminderChannelIndex(uint16_t index, config_parameters* config)
{
    reminder* newReminder = config->headReminder;
    while (newReminder->next != NULL)
    {
        newReminder = newReminder->next;
    }
    newReminder->channel_index = index;
}