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
* \remote.c
* \brief
* File defines functions needed for remote functionalities
* Made on 09.05.2018.
*
* @Author Jure Bajic
*****************************************************************************/
#include "remote.h"

static reminder* activeReminder = NULL;

static int32_t getKeys(int32_t count, uint8_t* buf, uint32_t* eventsRead, int32_t inputFileDesc)
{
    int32_t ret = 0;
    
    ret = read(inputFileDesc, buf, (size_t)(count * (int) sizeof(struct input_event)));
    if (ret <= 0)
    {
        printf("Error code %d\n", ret);
        return ERROR;
    }
    /* calculate number of read events */
    *eventsRead = ret / (int) sizeof(struct input_event);
    
    return NO_ERROR;
}

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
int32_t startRemote(player_handles* handles, graphics* graphicsStruct, reminder* reminderHead)
{
    pthread_t tdtThread, remoteThreadId;
    back_proc_args backgroundProc = {.graphicsStruct = graphicsStruct, .reminderHead = reminderHead};
    const char* dev = "/dev/input/event0";
    char deviceName[20];
    remote_loop_args* args = (remote_loop_args*) malloc(sizeof(remote_loop_args));
    args->handles = handles;
    args->inputFileDesc = open(dev, O_RDWR);
    args->graphicsStruct = graphicsStruct;
    if (args->inputFileDesc == -1)
    {
        printf("Error while opening device (%s) !", strerror(errno));
	    return ERROR;
    }
    
    ioctl(args->inputFileDesc, EVIOCGNAME(sizeof(deviceName)), deviceName);
	printf("RC device opened succesfully [%s]\n", deviceName);
    
    args->eventBuf = (struct input_event*) malloc(NUM_EVENTS * sizeof(struct input_event));
    if (!args->eventBuf)
    {
        printf("Error allocating memory !");
        return ERROR;
    }

    pthread_create(&remoteThreadId, NULL, initRemoteLoop, args);
    pthread_create(&tdtThread, NULL, checkForTDTData, &backgroundProc);
    pthread_join(remoteThreadId, NULL);
    pthread_cancel(tdtThread);
    printf("ladida\n");
    free(args->eventBuf);
    free(args);
    return NO_ERROR;
}

/****************************************************************************
*
* @brief
* Function for removing one of the graphics elements
*
* @param
*   args - [in] Graphics struct
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
static void removeGraphicsFeature(void* args)
{
    timer_remove_graphics_feature* graphicsFeaturesTimer = (timer_remove_graphics_feature*) args;
    printf("Clear screen\n");
    clearGraphicsFeatures(graphicsFeaturesTimer->graphicsStruct, graphicsFeaturesTimer->featureToRemove);
}

/****************************************************************************
*
* @brief
* Function for clearing screen
*
* @param
*   graphicsStruct - [in] Graphics struct
*   currentChannel - [in] Number of channel
*   removeChannelInfo - [in] Timer structure for removing channel information
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
static void showChannelInfo(graphics* graphicsStruct, int32_t currentChannel, timer_struct* removeChannelInfo)
{
    int8_t isThereTeletext = FALSE;
    int32_t i;
    pmt_table* currentPmtTable = getPMTTable((currentChannel) - 1);

    for (i = 0; i < currentPmtTable->numberOfStreams; i++)
    {
        if (currentPmtTable->streams[i].descriptor == 0x56)
        {
            isThereTeletext = TRUE;
            break;
        }
    }
    graphicsStruct->graphicsElements.channelNumber = currentChannel;
    graphicsStruct->graphicsElements.isThereTTX = isThereTeletext;
    drawGraphicsFeatures(graphicsStruct, G_FEATURE_CHANNEL_INFO);

    // drawChannelInfo(graphicsStruct, currentChannel, isThereTeletext);
    timer_settime(removeChannelInfo->timerId, removeChannelInfo->timerFlags, &removeChannelInfo->timerSpec, &removeChannelInfo->timerSpecOld);
}

/****************************************************************************
*
* @brief
* Function for changing channel
*
* @param
*   changeChannelArgs - [in] Change channel args struct needed for chancingn channel and initiaint timer when info is showed for removing the same
*   removeChannelInfoTimer - [in] Timer for removing channel information
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
static void changeChannel(change_channel_args* changeChannelArgs, timer_struct* removeChannelNumberTypedTimer)
{
    if (changeChannelArgs->channelNumber != changeChannelArgs->currentChannel)
    {
        if (changeChannelArgs->channelNumber == 0)
        {
            changeChannelArgs->currentChannel = changeChannelArgs->numberOfPrograms - 1;
            changeStream(changeChannelArgs->handles, changeChannelArgs->currentChannel);
        }
        else if (changeChannelArgs->channelNumber == changeChannelArgs->numberOfPrograms)
        {
            changeChannelArgs->currentChannel = 1;
            changeStream(changeChannelArgs->handles, changeChannelArgs->currentChannel);
        }   
        else if (changeChannelArgs->channelNumber > 0 && changeChannelArgs->channelNumber < changeChannelArgs->numberOfPrograms)
        {
            changeChannelArgs->currentChannel = changeChannelArgs->channelNumber;
            changeStream(changeChannelArgs->handles, changeChannelArgs->currentChannel);
        }
        changeChannelArgs->channelNumber = 0;
    }
    showChannelInfo(changeChannelArgs->graphicsStruct, changeChannelArgs->currentChannel, removeChannelNumberTypedTimer);
}

/****************************************************************************
*
* @brief
* Function for changing channel
*
* @param
*   arg - [in] Containing struct for changing channel and timer for changing channel via numbers
*
****************************************************************************/
static void changeChannelNumber(void* arg)
{
    timer_channel_changer_args* timeArgs = (timer_channel_changer_args*) arg;
    printf("changeChannelNumber Full Channel %d\n", timeArgs->changeChannelArgs->channelNumber);
    changeChannel(timeArgs->changeChannelArgs, timeArgs->removeChannelNumberTypedTimer);
}

/****************************************************************************
*
* @brief
* Function for setuping timer
*
* @param
*   timer_struct - [in] Structure containng all the timer inforamtion
*   timerCallback - [in] Function to be called on timer execution
*   timerArgs - [in] Arguments to forward to timer
*   seconds - [in] Time until the execution of timer
*
****************************************************************************/
static void setupTimer(timer_struct* timer, void (*timerCallback)(void*), void* timerArgs, int32_t seconds)
{
    timer->timerFlags = 0;
    struct sigevent signalEvent;
	signalEvent.sigev_notify = SIGEV_THREAD;
	signalEvent.sigev_notify_function = timerCallback;
	signalEvent.sigev_value.sival_ptr = timerArgs;
	signalEvent.sigev_notify_attributes = NULL;
	timer_create(CLOCK_REALTIME, &signalEvent, &timer->timerId);
    memset(&timer->timerSpec, 0, sizeof(timer->timerSpec));

    timer->timerSpec.it_value.tv_sec = seconds;
    timer->timerSpec.it_value.tv_nsec = 0;
}

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
void* checkForTDTData(void* args)
{
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    back_proc_args* backgroundProc = (back_proc_args*) args;
    int32_t isReminderTimeDetected = FALSE;
    tdt_table* tdtTable = getTDTTable();
    printf("reminder TDT time %d:%d\n", backgroundProc->reminderHead->time.hours, backgroundProc->reminderHead->time.minutes);
    while (TRUE)
    {
        waitForTableToParse();
        printf("checkForTDTData Arrived %d:%d:%d\n", tdtTable->dateTimeUTC.time.hours, tdtTable->dateTimeUTC.time.minutes, tdtTable->dateTimeUTC.time.seconds);
        activeReminder = isThereTime((reminder*) backgroundProc->reminderHead, tdtTable->dateTimeUTC.time);
        printf("IS there Active reminder %d\n", activeReminder == NULL ? 0 : 1);
        if (!isReminderTimeDetected && activeReminder != NULL)
        {
            // activeReminder = activeReminder;
            // drawReminder(backgroundProc->graphicsStruct, activeReminder->channel_index, 1);
            printf("MATCHING REMINDER %d\n", activeReminder->channel_index);
            backgroundProc->graphicsStruct->graphicsElements.reminderChannelNumber = activeReminder->channel_index;
            backgroundProc->graphicsStruct->graphicsElements.chosenButton = 1;
            drawGraphicsFeatures(backgroundProc->graphicsStruct, G_FEATURE_REMINDER);
        }
        if (activeReminder != NULL)
        {
            isReminderTimeDetected = TRUE;
        }
        else
        {
            if (isReminderTimeDetected)
            {
                // clearScreen(backgroundProc->graphicsStruct);
                clearGraphicsFeatures(backgroundProc->graphicsStruct, G_FEATURE_REMINDER);
            }
            isReminderTimeDetected = FALSE;
        }
    }

    return NO_ERROR;
}


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
void* initRemoteLoop(void* args)
{
    uint32_t soundVolume;
    uint8_t mute = FALSE;
    int32_t exitRemote = FALSE;
    uint32_t i;
    uint32_t eventCnt;
    uint8_t chosenButton = 1;
    timer_struct channelRemoveInfoTimer, soundRemoveTimer, timeRemoveTimer;
    remote_loop_args* remoteArgs = (remote_loop_args*) args;
    timer_remove_graphics_feature removeFeatureTimerChannel = {.graphicsStruct = remoteArgs->graphicsStruct, .featureToRemove = G_FEATURE_CHANNEL_INFO};
    timer_remove_graphics_feature removeFeatureTimerSound = {.graphicsStruct = remoteArgs->graphicsStruct, .featureToRemove = G_FEATURE_VOLUME};
    timer_remove_graphics_feature removeFeatureTimerTime = {.graphicsStruct = remoteArgs->graphicsStruct, .featureToRemove = G_FEATURE_TIME};
    timer_remove_graphics_feature removeFeatureTypedNumbers = 
        {.graphicsStruct = remoteArgs->graphicsStruct, .featureToRemove = G_FEATURE_CHANNEL_NUMBER | G_FEATURE_CHANNEL_INFO};
    Player_Volume_Get(remoteArgs->handles->playerHandle, &soundVolume);

    change_channel_args changeChannelArgs;
    changeChannelArgs.currentChannel = 1;
    changeChannelArgs.channelNumber = 0;
    changeChannelArgs.handles = remoteArgs->handles;
    changeChannelArgs.graphicsStruct = remoteArgs->graphicsStruct;
    changeChannelArgs.numberOfPrograms = getPATTable()->number_of_programs;

    timer_channel_changer_args timeArgs;
    timeArgs.changeChannelArgs = &changeChannelArgs;
    timeArgs.removeChannelNumberTypedTimer = (timer_struct*) malloc(sizeof(timer_struct));

    // Timer when when chaging channel via numbers
    setupTimer(&timeArgs.channelChangerTimer, changeChannelNumber, (void*) &timeArgs, 2);
    // Timer when when chaging channel time has elapsed for removing numbers
    setupTimer(timeArgs.removeChannelNumberTypedTimer, removeGraphicsFeature, &removeFeatureTypedNumbers, 2);

    // Timer to remove channelInfo graphics
    setupTimer(&channelRemoveInfoTimer, removeGraphicsFeature, &removeFeatureTimerChannel, 3);
    // Timer to remove soundInfo graphics
    setupTimer(&soundRemoveTimer, removeGraphicsFeature, &removeFeatureTimerSound, 3);
    // Timer to remove time graphics
    setupTimer(&timeRemoveTimer, removeGraphicsFeature, &removeFeatureTimerTime, 3);

    while(TRUE)
    {
        if (getKeys(NUM_EVENTS, (uint8_t*)remoteArgs->eventBuf, &eventCnt, remoteArgs->inputFileDesc))
        {
			printf("Error while reading input events !");
			return (void*) ERROR;
		}

        for (i = 0; i < eventCnt; i++)
        {
			if (remoteArgs->eventBuf[i].value == 1) 
            {
				switch (remoteArgs->eventBuf[i].code) 
                {
                    case REM_CHANNEL_0:
                    case REM_CHANNEL_1:
                    case REM_CHANNEL_2:
                    case REM_CHANNEL_3:
                    case REM_CHANNEL_4:
                    case REM_CHANNEL_5:
                    case REM_CHANNEL_6:
                    case REM_CHANNEL_7:
                    case REM_CHANNEL_8:
                    case REM_CHANNEL_9:
                    {
                        printf("Before calling timer %d\n", timeArgs.changeChannelArgs->channelNumber);
                        if ((changeChannelArgs.channelNumber * 10 + remoteArgs->eventBuf[i].code - 1) < 999)
                        {
                            if (remoteArgs->eventBuf[i].code == REM_CHANNEL_0)
                            {
                                changeChannelArgs.channelNumber = changeChannelArgs.channelNumber * 10;
                            }
                            else
                            {
                                changeChannelArgs.channelNumber = changeChannelArgs.channelNumber * 10 + remoteArgs->eventBuf[i].code - 1;
                            }
                            
                            printf("calling timer %d\n", changeChannelArgs.channelNumber);
                            remoteArgs->graphicsStruct->graphicsElements.channelNumberTyped = changeChannelArgs.channelNumber;
                            drawGraphicsFeatures(remoteArgs->graphicsStruct, G_FEATURE_CHANNEL_NUMBER);
                            timer_settime(timeArgs.channelChangerTimer.timerId, timeArgs.channelChangerTimer.timerFlags, 
                                &timeArgs.channelChangerTimer.timerSpec, &timeArgs.channelChangerTimer.timerSpecOld);
                        }
                        break;
                    }	
					case REM_CHANNEL_UP:
                    {
						changeChannelArgs.channelNumber = changeChannelArgs.currentChannel + 1;
                        changeChannel(&changeChannelArgs, &channelRemoveInfoTimer);
						break;
                    }
					case REM_CHANNEL_DOWN:
                    {
						changeChannelArgs.channelNumber = changeChannelArgs.currentChannel - 1;
                        changeChannel(&changeChannelArgs, &channelRemoveInfoTimer);
						break;
                    }
                    case REM_VOLUME_UP:
                    {
                        //volume up
                        if ((soundVolume + INT32_MAX * 0.1) <= INT32_MAX)
                        {
                            soundVolume += INT32_MAX * 0.1;
                            Player_Volume_Set(remoteArgs->handles->playerHandle, soundVolume);
                        }
                        // drawSoundInfo(remoteArgs->graphicsStruct, soundVolume);
                        remoteArgs->graphicsStruct->graphicsElements.soundVolume = soundVolume;
                        drawGraphicsFeatures(remoteArgs->graphicsStruct, G_FEATURE_VOLUME);
                        timer_settime(soundRemoveTimer.timerId, soundRemoveTimer.timerFlags, &soundRemoveTimer.timerSpec,
                            &soundRemoveTimer.timerSpecOld);
                        mute = FALSE;
                        break;
                    }
                    case REM_VOLUME_DOWN:
                    {
                        //volume down
                        if (soundVolume != 0)
                        {
                            if ((soundVolume - INT32_MAX * 0.1) <= 0)
                            {
                                soundVolume = 0;
                            }
                            else
                            {
                                soundVolume -= INT32_MAX * 0.1;
                            }
                            Player_Volume_Set(remoteArgs->handles->playerHandle, soundVolume);
                        }
                        remoteArgs->graphicsStruct->graphicsElements.soundVolume = soundVolume;
                        drawGraphicsFeatures(remoteArgs->graphicsStruct, G_FEATURE_VOLUME);
                        timer_settime(soundRemoveTimer.timerId, soundRemoveTimer.timerFlags, &soundRemoveTimer.timerSpec,
                            &soundRemoveTimer.timerSpecOld);
                        mute = FALSE;                        
                        break;
                    }
                    case REM_MUTE:
                    {
                        //mute
                        if (mute)
                        {
                            Player_Volume_Set(remoteArgs->handles->playerHandle, soundVolume);
                            mute = FALSE;
                        }
                        else
                        {
                            Player_Volume_Set(remoteArgs->handles->playerHandle, 0);
                            mute = TRUE;
                        }
                        break;
                    }
                    case REM_ARROW_LEFT:
                    {
                        printf("TRING LEFT\n");
                        if (activeReminder != NULL)
                        {
                            printf("LEFT\n");
                            chosenButton = 1;
                            // drawReminder(remoteArgs->graphicsStruct, activeReminder->channel_index, chosenButton);
                            remoteArgs->graphicsStruct->graphicsElements.reminderChannelNumber = activeReminder->channel_index;
                            remoteArgs->graphicsStruct->graphicsElements.chosenButton = chosenButton;
                            drawGraphicsFeatures(remoteArgs->graphicsStruct, G_FEATURE_REMINDER);
                        }
                        break;
                    }
                    case REM_ARROW_RIGHT:
                    {
                        printf("TRYING RIGHT\n");
                        if (activeReminder != NULL)
                        {
                            chosenButton = 2;
                            remoteArgs->graphicsStruct->graphicsElements.reminderChannelNumber = activeReminder->channel_index;
                            remoteArgs->graphicsStruct->graphicsElements.chosenButton = chosenButton;
                            drawGraphicsFeatures(remoteArgs->graphicsStruct, G_FEATURE_REMINDER);
                        }
                        break;
                    }
                    case REM_OK:
                    {
                        if (activeReminder != NULL)
                        {
                            if (chosenButton == 1)
                            {
                                changeChannelArgs.channelNumber = activeReminder->channel_index;
                                changeChannel(&changeChannelArgs, &channelRemoveInfoTimer);
                            }
                            clearGraphicsFeatures(remoteArgs->graphicsStruct, G_FEATURE_REMINDER);
                            activeReminder = NULL;                            
                        }
                        break;
                    }
					case REM_EXIT:
                    {
                        //exit
                        exitRemote = TRUE;
						break;
                    }
                    case KEY_INFO:
                    {
                        //info
                        showChannelInfo(remoteArgs->graphicsStruct, changeChannelArgs.currentChannel, &channelRemoveInfoTimer);
                        break;
                    }
                    case REM_RED_BUTTON:
                     {
                        tdt_table* tdtTable = getTDTTable();
                        printf("Draw time %d %d\n", tdtTable->dateTimeUTC.time.hours, tdtTable->dateTimeUTC.time.minutes);
                        //read button for time
                        // drawTime(remoteArgs->graphicsStruct, tdtTable->dateTimeUTC.time);
                        remoteArgs->graphicsStruct->graphicsElements.timeUtc = tdtTable->dateTimeUTC.time;
                        drawGraphicsFeatures(remoteArgs->graphicsStruct, G_FEATURE_TIME);
                        timer_settime(timeRemoveTimer.timerId, timeRemoveTimer.timerFlags, &timeRemoveTimer.timerSpec, &timeRemoveTimer.timerSpecOld);
                        break;
                     }
				}
			}	
        }
        if (exitRemote)
        {
            //stop
            break;
        }
    }
    timer_delete(channelRemoveInfoTimer.timerId);
    timer_delete(soundRemoveTimer.timerId);
    timer_delete(timeArgs.channelChangerTimer.timerId);
    // timer_delete(timeArgs.removeChannelNumberTypedTime->timerId);
    free(timeArgs.removeChannelNumberTypedTimer);
    return (void*) NO_ERROR;
}