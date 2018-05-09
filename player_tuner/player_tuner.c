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
* \player_tuner.c
* \brief
* Datoteka definira funkcije potrebne za rad tunera i playera
* Made on 08.05.2018.
*
* @Author Jure Bajic
*****************************************************************************/
#include "player_tuner.h"

static inline void textColor(int32_t attr, int32_t fg, int32_t bg)
{
   char command[13];

   /* Command is the control command to the terminal */
   sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
   printf("%s", command);
}

static pthread_cond_t statusCondition = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t statusMutex = PTHREAD_MUTEX_INITIALIZER;

int32_t tunerInitialization(config_parameters* config)
{
    int32_t result;
    struct timespec lockStatusWaitTime;
    struct timeval now;
    
    gettimeofday(&now, NULL);
    lockStatusWaitTime.tv_sec = now.tv_sec + 10;

      /* Initialize tuner */
    result = Tuner_Init();
    ASSERT_TDP_RESULT(result, "Tuner_Init");
    
    /* Register tuner status callback */
    result = Tuner_Register_Status_Callback(myPrivateTunerStatusCallback);
    ASSERT_TDP_RESULT(result, "Tuner_Register_Status_Callback");
    
    /* Lock to frequency */
    result = Tuner_Lock_To_Frequency(818000000, 8, DVB_T);
    ASSERT_TDP_RESULT(result, "Tuner_Lock_To_Frequency");
    
    pthread_mutex_lock(&statusMutex);
    if(ETIMEDOUT == pthread_cond_timedwait(&statusCondition, &statusMutex, &lockStatusWaitTime))
    {
        printf("\n\nLock timeout exceeded!\n\n");
        return -1;
    }
    pthread_mutex_unlock(&statusMutex);

    return NO_ERROR;
}

int32_t startPlayer(player_handles* handles)
{
    int32_t result;
    handles->playerHandle = 0;
    handles->sourceHandle = 0;
    handles->videoStreamHandle = 0;
    handles->audioStreamHandle = 0;

    /* Initialize player (demux is a part of player) */
    result = Player_Init(&handles->playerHandle);
    ASSERT_TDP_RESULT(result, "Player_Init");
    
    /* Open source (open data flow between tuner and demux) */
    result = Player_Source_Open(handles->playerHandle, &handles->sourceHandle);
    ASSERT_TDP_RESULT(result, "Player_Source_Open");
    return NO_ERROR;
}

int32_t setupData(player_handles* player_handles)
{
    // int patParsed = FALSE;
    // pthread_t patParserId;
    // pthread_create(&patParserId, NULL, &setFilterToPAT,
    setFilterToPAT(filterPATParserCallback, player_handles);
    setFilterToPMT(filterPMTParserCallback, player_handles);
}

// int32_t parseTable(int32_t (*parsefilterCallback)(uint8_t*), player_handles* handles)
// {
//     int32_t result;
//     int tableSectionId = 0x0000;
//     int tableId = 0x00;
//     /* Set filter to demux */
//     result = Demux_Set_Filter(handles->playerHandle, tableSectionId, tableId, &handles->filterHandle);
//     ASSERT_TDP_RESULT(result, "Demux_Set_Filter");
    
//     /* Register section filter callback */
//     result = Demux_Register_Section_Filter_Callback(parsefilterCallback);
//     ASSERT_TDP_RESULT(result, "Demux_Register_Section_Filter_Callback");

//     while(!isPatTableParsed())
//     {
//     }
//     freeFilterCallback(parsefilterCallback, handles);
//     //Alles gut PAT is parsed
//     return NO_ERROR;
// }

int32_t setFilterToPAT(int32_t (*filterCallback)(uint8_t*), player_handles* handles)
{
    int32_t result;
    /* Set filter to demux */
    result = Demux_Set_Filter(handles->playerHandle, 0x0000, 0x00, &handles->filterHandle);
    ASSERT_TDP_RESULT(result, "Demux_Set_Filter");
    
    /* Register section filter callback */
    result = Demux_Register_Section_Filter_Callback(filterPATParserCallback);
    ASSERT_TDP_RESULT(result, "Demux_Register_Section_Filter_Callback");

    while(!isPatTableParsed())
    {
    }
    freeFilterCallback(*filterCallback, handles);
    //Alles gut PAT is parsed
    return NO_ERROR;
}

int32_t setFilterToPMT(int32_t (*filterCallback)(uint8_t*), player_handles* handles)
{
    int32_t result;
    int i;
    pat_table* patTable = getPATTable();
    //allocate memory for pmt tables
    allocatePMTTables(patTable->number_of_programs);
    printf("NUMERO OF PROGRAMO %d\n", patTable->number_of_programs);
    for (i = 0; i < patTable->number_of_programs; i++)
    {
        printf("NUMERO OF PROGRAMO2 %d\n", i);
        /* Set filter to demux */
        result = Demux_Set_Filter(handles->playerHandle, patTable->pat_programm[i + 1].programm_map_pid, 0x02, &handles->filterHandle);
        ASSERT_TDP_RESULT(result, "Demux_Set_Filter");
        
        /* Register section filter callback */
        result = Demux_Register_Section_Filter_Callback(filterPMTParserCallback);
        ASSERT_TDP_RESULT(result, "Demux_Register_Section_Filter_Callback");

        while(!isPmtTableParsed())
        {
        }
        freeFilterCallback(*filterCallback, handles);
    }
    //Alles gut PMT are parsed
    return NO_ERROR;
}

int32_t freeFilterCallback(int32_t (*filterCallback)(uint8_t*), player_handles* handles)
{
    int32_t result;
    /* Free demux filter */
    result = Demux_Unregister_Section_Filter_Callback(filterCallback);
    ASSERT_TDP_RESULT(result, "Demux_Unregister_Section_Filter_Callback");
	result = Demux_Free_Filter(handles->playerHandle, handles->filterHandle);
    ASSERT_TDP_RESULT(result, "Demux_Free_Filter");

    return NO_ERROR;
}

int32_t createStream(player_handles* handles)
{
    int32_t result;
    result = Player_Stream_Create(handles->playerHandle, handles->sourceHandle, 101, VIDEO_TYPE_MPEG2, &handles->videoStreamHandle);
    ASSERT_TDP_RESULT(result, "Player_Stream_Create");

	result = Player_Stream_Create(handles->playerHandle, handles->sourceHandle, 103, AUDIO_TYPE_MPEG_AUDIO, &handles->audioStreamHandle);
    ASSERT_TDP_RESULT(result, "Player_Stream_Create");

    return NO_ERROR;
}

int32_t removeStream(player_handles* handles)
{
    int32_t result;
    if (handles->videoStreamHandle != 0) 
    {
        result = Player_Stream_Remove(handles->playerHandle, handles->sourceHandle, handles->videoStreamHandle);
        ASSERT_TDP_RESULT(result, "Player_Stream_Remove1");
	}
	if (handles->audioStreamHandle != 0) 
    {
		result = Player_Stream_Remove(handles->playerHandle, handles->sourceHandle, handles->audioStreamHandle);
		ASSERT_TDP_RESULT(result, "Player_Stream_Remove2");	
	}

    return NO_ERROR;
}

int32_t stopPlayer(player_handles* handles)
{
    int32_t result;
    /* Close previously opened source */
    result = Player_Source_Close(handles->playerHandle, handles->sourceHandle);
    ASSERT_TDP_RESULT(result, "Player_Source_Close");
    
    /* Deinit player */
    result = Player_Deinit(handles->playerHandle);
    ASSERT_TDP_RESULT(result, "Player_Deinit");
    free(handles);

    return NO_ERROR;
}

int32_t tunerDeinitialization()
{
    int32_t result;
    /* Deinit tuner */
    result = Tuner_Deinit();
    ASSERT_TDP_RESULT(result, "Tuner_Deinit");

    return NO_ERROR;
}

int32_t myPrivateTunerStatusCallback(t_LockStatus status)
{
    if(status == STATUS_LOCKED)
    {
        pthread_mutex_lock(&statusMutex);
        pthread_cond_signal(&statusCondition);
        pthread_mutex_unlock(&statusMutex);
        printf("\n\n\tCALLBACK LOCKED\n\n");
    }
    else
    {
        printf("\n\n\tCALLBACK NOT LOCKED\n\n");
    }
    return 0;
}