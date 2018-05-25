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
* File defines functions needed for the tuner and the player
* Made on 08.05.2018.
*
* @Author Jure Bajic
*****************************************************************************/
#include "player_tuner.h"

static pthread_cond_t statusCondition = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t statusMutex = PTHREAD_MUTEX_INITIALIZER;

/****************************************************************************
*
* @brief
* Function for waiting for the tuner init to finish
*
* @param
*   status - [in] 
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
static int32_t myPrivateTunerStatusCallback(t_LockStatus status)
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
    return NO_ERROR;
}

/****************************************************************************
*
* @brief
* Function for executed in thread which parses TOT and TDT tables
*
* @param
*       handles - [in] args
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
static void* threadTDTAndTOTTableParse(void* args)
{
    printf("threadTDTAndTOTTableParse1\n");
    player_handles_thread_args* threadTOTAndTDTArgs = (player_handles_thread_args*) args;
    printf("threadTDTAndTOTTableParse3\n");
    setFilterToTable(filterTOTParserCallback, isTOTTableParsed, threadTOTAndTDTArgs->handles, tdt_and_tot_table_pid, tot_table_id);
    printf("threadTDTAndTOTTableParse2\n");
    freeFilterCallback(filterTOTParserCallback, threadTOTAndTDTArgs->handles);

    setFilterToTable(filterTDTParserCallback, isTDTTableParsed, threadTOTAndTDTArgs->handles, tdt_and_tot_table_pid, tdt_table_id);
    // pthread_create(&tdtThread, NULL, checkForTDTData, (void*) threadArgs->channelReminders);
    printf("started tdt\n");
    pthread_mutex_lock(&threadTOTAndTDTArgs->threadArguments->mutex);
    printf("threadTDTAndTOTTableParse unlock\n");
    pthread_cond_wait( &threadTOTAndTDTArgs->threadArguments->condition, &threadTOTAndTDTArgs->threadArguments->mutex ); 
    freeFilterCallback(filterTDTParserCallback, threadTOTAndTDTArgs->handles);
    printf("threadTDTAndTOTTableParse finish unlock\n");
    pthread_mutex_unlock(&threadTOTAndTDTArgs->threadArguments->mutex);
    free(threadTOTAndTDTArgs);
    return NO_ERROR;
}

/****************************************************************************
*
* @brief
* Function for changing coloc in console if there is error
*
* @param
*       attr - [in] attributes
*       fg - [in] foreground
*       bg - [in] background
*
****************************************************************************/
static inline void textColor(int32_t attr, int32_t fg, int32_t bg)
{
   char command[13];

   /* Command is the control command to the terminal */
   sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
   printf("%s", command);
}

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
    result = Tuner_Lock_To_Frequency(config->frequency * FREQUENCY_MGH, config->bandwidth, config->module);
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
int32_t setupData(player_handles* handles, thread_args* threadArguments)
{
    int32_t i;
    pthread_t threadTDTAndTOTTableParseThread;
    player_handles_thread_args* threadTOTAndTDTArgs = (player_handles_thread_args*) malloc(sizeof(player_handles_thread_args));
    threadTOTAndTDTArgs->handles = handles;
    threadTOTAndTDTArgs->threadArguments = threadArguments;

    setFilterToTable(filterPATParserCallback, isPatTableParsed, handles, pat_table_pid, pat_table_id);
    freeFilterCallback(filterPATParserCallback, handles);

    pat_table* patTable = getPATTable();
    allocatePMTTables(patTable->number_of_programs);
    for (i = 0; i < patTable->number_of_programs - 1; i++)
    {
        setFilterToTable(filterPMTParserCallback, isPmtTableParsed, handles, patTable->pat_programm[i + 1].programm_map_pid, pmt_table_id);
        freeFilterCallback(filterPMTParserCallback, handles);
        setPmtTableParsedFalse();
    }
    pthread_create(&threadTDTAndTOTTableParseThread, NULL, threadTDTAndTOTTableParse, threadTOTAndTDTArgs);

    return NO_ERROR;
}

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
int32_t setFilterToTable(int32_t (*filterCallback)(uint8_t*), int8_t (*isTableParsed)(), player_handles* handles, int32_t tablePID, int32_t tableId)
{
    int32_t result;
    /* Set filter to demux */
    result = Demux_Set_Filter(handles->playerHandle, tablePID, tableId, &handles->filterHandle);
    ASSERT_TDP_RESULT(result, "Demux_Set_Filter");
    
    /* Register section filter callback */
    result = Demux_Register_Section_Filter_Callback(filterCallback);
    ASSERT_TDP_RESULT(result, "Demux_Register_Section_Filter_Callback");

    while (!isTableParsed())
    {
    }
    return NO_ERROR;
}

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
int32_t createStream(player_handles* handles, config_parameters* config)
{
    int32_t result;
    // printf("Create: vpid %d vtype %d\n", config->service.vpid, config->service.vtype);
    // printf("Create: apid %d atype %d\n", config->service.apid, config->service.atype);
    result = Player_Stream_Create(handles->playerHandle, handles->sourceHandle, config->service.vpid, getStreamType(config->service.vtype), &handles->videoStreamHandle);
    ASSERT_TDP_RESULT(result, "Player_Stream_Create");

	result = Player_Stream_Create(handles->playerHandle, handles->sourceHandle, config->service.apid, getStreamType(config->service.atype), &handles->audioStreamHandle);
    ASSERT_TDP_RESULT(result, "Player_Stream_Create");

    return NO_ERROR;
}


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
int32_t changeStream(player_handles* handles, int32_t channelNumber)
{
    int32_t result;
    removeStream(handles);
    pmt_table* currentPmt = getPMTTable(channelNumber - 1);
    if (getStreamType(currentPmt->streams[0].stream_type))
    {
        result = Player_Stream_Create(handles->playerHandle, handles->sourceHandle, 
            currentPmt->streams[0].elementary_PID, getStreamType(currentPmt->streams[0].stream_type), &handles->videoStreamHandle);
        ASSERT_TDP_RESULT(result, "Player_Stream_Video_Change");
    }
    else
    {
        handles->videoStreamHandle = 0;
    }

    if (getStreamType(currentPmt->streams[2].stream_type))
    {
        result = Player_Stream_Create(handles->playerHandle, handles->sourceHandle, 
            currentPmt->streams[2].elementary_PID, getStreamType(currentPmt->streams[2].stream_type), &handles->audioStreamHandle);
        ASSERT_TDP_RESULT(result, "Player_Stream_Audio_Change");
    }
    else
    {
        handles->audioStreamHandle = 0;
    }

    return NO_ERROR;
}

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
int32_t removeStream(player_handles* handles)
{
    int32_t result;
    printf("removing playerHandle %d sourceHandle %d\n", handles->playerHandle, handles->sourceHandle);
    if (handles->videoStreamHandle != 0) 
    {
        printf("removing video\n");
        result = Player_Stream_Remove(handles->playerHandle, handles->sourceHandle, handles->videoStreamHandle);
        ASSERT_TDP_RESULT(result, "Player_Stream_Remove1");
	}
	if (handles->audioStreamHandle != 0) 
    {
        printf("removing audio\n");
		result = Player_Stream_Remove(handles->playerHandle, handles->sourceHandle, handles->audioStreamHandle);
		ASSERT_TDP_RESULT(result, "Player_Stream_Remove2");	
	}

    return NO_ERROR;
}

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
int32_t stopPlayer(player_handles* handles)
{
    int32_t result;
    /* Close previously opened source */
    result = Player_Source_Close(handles->playerHandle, handles->sourceHandle);
    ASSERT_TDP_RESULT(result, "Player_Source_Close");
    
    /* Deinit player */
    result = Player_Deinit(handles->playerHandle);
    ASSERT_TDP_RESULT(result, "Player_Deinit");

    return NO_ERROR;
}

/****************************************************************************
*
* @brief
* Function for tuner deinitialization
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t tunerDeinitialization()
{
    int32_t result;
    /* Deinit tuner */
    result = Tuner_Deinit();
    ASSERT_TDP_RESULT(result, "Tuner_Deinit");

    return NO_ERROR;
}

/****************************************************************************
*
* @brief
* Function for getting the stream type from dvb standards
*
* @return
*   stream_audio, enum of the stream
*   STREAM_NOT_FOUND, if there is no error
****************************************************************************/
int8_t getStreamType(uint8_t streamType)
{
	switch(streamType)
	{
		case stream_video_h262:
			return VIDEO_TYPE_MPEG2;
		case stream_audio:
			return AUDIO_TYPE_MPEG_AUDIO;
		// case stream_private_sections:
		// 	return VIDEO_TYPE_MPEG2;
		default:
			return STREAM_NOT_FOUND;
	}
}
