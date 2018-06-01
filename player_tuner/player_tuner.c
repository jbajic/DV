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
    // setFilterToTable(filterTOTParserCallback, threadTOTAndTDTArgs->handles, tdt_and_tot_table_pid, tot_table_id);
    // printf("threadTDTAndTOTTableParse2\n");
    // freeFilterCallback(filterTOTParserCallback, threadTOTAndTDTArgs->handles);
    parseTable(TABLE_TOT, threadTOTAndTDTArgs->handles, 0);
    // stopParsing(threadTOTAndTDTArgs->handles);

    // setFilterToTable(filterTDTParserCallback, threadTOTAndTDTArgs->handles, tdt_and_tot_table_pid, tdt_table_id);
    parseTable(TABLE_TDT, threadTOTAndTDTArgs->handles, 0);
    pthread_mutex_lock(&threadTOTAndTDTArgs->threadArguments->mutex);
    pthread_cond_wait( &threadTOTAndTDTArgs->threadArguments->condition, &threadTOTAndTDTArgs->threadArguments->mutex); 
    // stopParsing(threadTOTAndTDTArgs->handles);
    pthread_mutex_unlock(&threadTOTAndTDTArgs->threadArguments->mutex);
    free(threadTOTAndTDTArgs);
    return NO_ERROR;
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
    int32_t i, result;
    pthread_t threadTDTAndTOTTableParseThread;
    player_handles_thread_args* threadTOTAndTDTArgs = (player_handles_thread_args*) malloc(sizeof(player_handles_thread_args));
    threadTOTAndTDTArgs->handles = handles;
    threadTOTAndTDTArgs->threadArguments = threadArguments;

    result = parseTable(TABLE_PAT, handles, 0);
    printf("Parsed PAT %d\n", result);
    // stopParsing(handles);

    pat_table* patTable = getPATTable();
    allocatePMTTables(patTable->number_of_programs);
    for (i = 0; i < patTable->number_of_programs - 1; i++)
    {
        result = parseTable(TABLE_PMT, handles, patTable->pat_programm[i + 1].programm_map_pid);
        printf("Parsed PMT %d %d\n", i, result);
        // stopParsing(handles);
    }
    pthread_create(&threadTDTAndTOTTableParseThread, NULL, threadTDTAndTOTTableParse, threadTOTAndTDTArgs);

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

    result = Player_Stream_Create(handles->playerHandle, handles->sourceHandle, config->service.vpid,
        getVideoStreamType(config->service.vtype), &handles->videoStreamHandle);
    ASSERT_TDP_RESULT(result, "Player_Stream_Create");

	result = Player_Stream_Create(handles->playerHandle, handles->sourceHandle, config->service.apid, 
        getAudioStreamType(config->service.atype), &handles->audioStreamHandle);
    ASSERT_TDP_RESULT(result, "Player_Stream_Create");

    return NO_ERROR;
}

/****************************************************************************
*
* @brief
* Function for starting stream
*
* @param
*       handles - [in] Structure containing all handles
*       streamHandle - [in] Specified which handle to change
*       currentPmt - [in] PMT table of the channel
*       getStreamType() - [in] Function containing all stream types of specified kind
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
static int32_t startStream(player_handles* handles, uint32_t* streamHandle, pmt_table* currentPmt, int8_t (*getStreamType)(uint8_t))
{
    int32_t i, result;
    for (i = 0; i < currentPmt->numberOfStreams; ++i)
    {
        if (getStreamType(currentPmt->streams[i].stream_type) != 0)
        {
            result = Player_Stream_Create(handles->playerHandle, handles->sourceHandle, 
                currentPmt->streams[i].elementary_PID, getStreamType(currentPmt->streams[i].stream_type), streamHandle);
            ASSERT_TDP_RESULT(result, "Player_Stream_Video_Change");

            return NO_ERROR;
        }
    }
    *(streamHandle) = 0;
    return NO_STREAM_TYPE;
}

/****************************************************************************
*
* @brief
* Function for changing stream
*
* @param
*       handles - [in] structure containing all handles
*       channelNumber - [in] Channel number of which the streams are played
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t changeStream(player_handles* handles, int32_t channelNumber)
{
    removeStream(handles);
    pmt_table* currentPmt = getPMTTable(channelNumber - 1);
    startStream(handles, &handles->videoStreamHandle, currentPmt, getVideoStreamType);
    startStream(handles, &handles->audioStreamHandle, currentPmt, getAudioStreamType);

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
* Function for getting the video stream type from dvb standards
*
* @return
*   dvb_streams, enum of the stream
*   STREAM_NOT_FOUND, if there is no error
****************************************************************************/
int8_t getVideoStreamType(uint8_t streamType)
{
	switch(streamType)
	{
		case stream_video_h262:
			return VIDEO_TYPE_MPEG2;
		// case stream_private_sections:
		// 	return VIDEO_TYPE_MPEG2;
		default:
			return STREAM_NOT_FOUND;
	}
}

/****************************************************************************
*
* @brief
* Function for getting the audio stream type from dvb standards
*
* @return
*   dvb_streams, enum of the stream
*   STREAM_NOT_FOUND, if there is no error
****************************************************************************/
int8_t getAudioStreamType(uint8_t streamType)
{
	switch(streamType)
	{
		case stream_audio:
			return AUDIO_TYPE_MPEG_AUDIO;
		// case stream_private_sections:
		// 	return VIDEO_TYPE_MPEG2;
		default:
			return STREAM_NOT_FOUND;
	}
}