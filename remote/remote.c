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
* Datoteka definra funckije potrebne za rad danjiljskog upravljaca
* Made on 09.05.2018.
*
* @Author Jure Bajic
*****************************************************************************/
#include "remote.h"

static int32_t getKeys(int32_t count, uint8_t* buf, uint32_t* eventsRead, int32_t inputFileDesc)
{
    int32_t ret = 0;
    
    // printf("buff5: %u\n inputfile5 %d\n", *buf, inputFileDesc);
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

int32_t startRemote(player_handles* handles, graphics* graphicsStruct)
{
    pthread_t remoteThreadId;
    const char* dev = "/dev/input/event0";
    char deviceName[20];
    remote_loop_args* args = (remote_loop_args*) malloc(sizeof(remote_loop_args));
    args->handles = handles;
    args->inputFileDesc = open(dev, O_RDWR);
    args->graphicsStruct = graphicsStruct;

    if(args->inputFileDesc == -1)
    {
        printf("Error while opening device (%s) !", strerror(errno));
	    //return ERROR;
    }
    
    ioctl(args->inputFileDesc, EVIOCGNAME(sizeof(deviceName)), deviceName);
	printf("RC device opened succesfully [%s]\n", deviceName);
    
    args->eventBuf = (struct input_event*) malloc(NUM_EVENTS * sizeof(struct input_event));
    if(!args->eventBuf)
    {
        printf("Error allocating memory !");
        //return ERROR;
    }
    pthread_create(&remoteThreadId, NULL, initRemoteLoop, args);
    pthread_join(remoteThreadId, NULL);

    free(args->eventBuf);
    free(args);
    return NO_ERROR;
}

static void clearScreen(void* args)
{
    printf("Clear screen\n");
    clearGraphics((graphics*) args);
}

static void showChannelInfo(graphics* graphicsStruct, int32_t currentChannel, timer_struct* removeChannelInfo)
{
    printf("CHANNEL INFO\n\n\n");
    int8_t isThereTeletext = FALSE;
    int32_t i;
    pmt_table* currentPmtTable = getPMTTable((currentChannel) - 1);

    for (i = 0; i < currentPmtTable->numberOfStreams; i++)
    {
        printf("Checking streams\n");
        if (currentPmtTable->streams[i].descriptor == 0x56)
        {
            isThereTeletext = TRUE;
            break;
        }
    }
    drawChannelInfo(graphicsStruct, currentChannel, isThereTeletext);
    timer_settime(removeChannelInfo->timerId, removeChannelInfo->timerFlags, &removeChannelInfo->timerSpec, &removeChannelInfo->timerSpecOld);
}

static void changeChannel(change_channel_args* changeChannelArgs, timer_struct* removeChannelInfoTimer)
{
    if (changeChannelArgs->channelNumber <= 0)
    {
        printf("COndition 1\n");
        changeChannelArgs->currentChannel = changeChannelArgs->numberOfPrograms - 1;
    }
    else if (changeChannelArgs->channelNumber > changeChannelArgs->numberOfPrograms - 1)
    {
        printf("COndition 2\n");
        changeChannelArgs->currentChannel = 1;
    }
    else
    {
        printf("COndition 3\n");
        changeChannelArgs->currentChannel = changeChannelArgs->channelNumber;
    }
    printf("numberOfChannels %d\nchannel current %d\nchannelNumber %d\n", changeChannelArgs->numberOfPrograms, 
        changeChannelArgs->currentChannel, changeChannelArgs->channelNumber);
    changeStream(changeChannelArgs->handles, changeChannelArgs->currentChannel);
    changeChannelArgs->channelNumber = 0;
    showChannelInfo(changeChannelArgs->graphicsStruct, changeChannelArgs->currentChannel, removeChannelInfoTimer);
}

static void changeChannelNumber(void* arg)
{
    timer_channel_changer_args* timeArgs = (timer_channel_changer_args*) arg;
    printf("changeChannelNumber Full Channel %d\n", timeArgs->changeChannelArgs->channelNumber);
    changeChannel(timeArgs->changeChannelArgs, timeArgs->removeChannelInfoTimer);
    // memset(&timeArgs->channelChangerTimer.timerSpec, 0, sizeof(timeArgs->channelChangerTimer.timerSpec));
    // timer_settime(timeArgs->channelChangerTimer.timerId, 0, &timeArgs->channelChangerTimer.timerSpec,
    //                              &timeArgs->channelChangerTimer.timerSpecOld);
}

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

void* initRemoteLoop(void* args)
{
    uint32_t soundVolume;
    uint8_t mute = FALSE;
    int32_t exitRemote = FALSE;
    uint32_t i;
    uint32_t eventCnt;
    remote_loop_args* remoteArgs = (remote_loop_args*) args;
    Player_Volume_Get(remoteArgs->handles->playerHandle, &soundVolume);

    change_channel_args changeChannelArgs;
    changeChannelArgs.currentChannel = 1;
    changeChannelArgs.channelNumber = 0;
    changeChannelArgs.handles = remoteArgs->handles;
    changeChannelArgs.graphicsStruct = remoteArgs->graphicsStruct;
    changeChannelArgs.numberOfPrograms = getPATTable()->number_of_programs;

    timer_struct channelRemoveInfoTimer, soundRemoveTimer;
    channelRemoveInfoTimer.timerFlags = 0;

    timer_channel_changer_args timeArgs;
    timeArgs.changeChannelArgs = &changeChannelArgs;
    timeArgs.channelChangerTimer.timerFlags = 0;
    timeArgs.removeChannelInfoTimer = &channelRemoveInfoTimer;

    setupTimer(&timeArgs.channelChangerTimer, changeChannelNumber, (void*) &timeArgs, 2);
    setupTimer(&channelRemoveInfoTimer, clearScreen, (void*) remoteArgs->graphicsStruct, 3);
    setupTimer(&soundRemoveTimer, clearScreen, (void*) remoteArgs->graphicsStruct, 3);

    while(TRUE)
    {
        if (getKeys(NUM_EVENTS, (uint8_t*)remoteArgs->eventBuf, &eventCnt, remoteArgs->inputFileDesc))
        {
			printf("Error while reading input events !");
			//return ERROR;
		}

        for (i = 0; i < eventCnt; i++)
        {
			if (remoteArgs->eventBuf[i].value == 1) 
            {
				switch (remoteArgs->eventBuf[i].code) 
                {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    case 8:
                    case 9:
                    case 10:
                    {
                        printf("Before calling timer %d\n", timeArgs.changeChannelArgs->channelNumber);
                        if (timeArgs.changeChannelArgs->channelNumber < 999)
                        {
                            changeChannelArgs.channelNumber = changeChannelArgs.channelNumber * 10 + remoteArgs->eventBuf[i].code - 1;
                            printf("calling timer %d\n", changeChannelArgs.channelNumber);
                            timer_settime(timeArgs.channelChangerTimer.timerId, timeArgs.channelChangerTimer.timerFlags, 
                                &timeArgs.channelChangerTimer.timerSpec, &timeArgs.channelChangerTimer.timerSpecOld);
                        }
                        break;
                    }	
					case 62:
                    {
						changeChannelArgs.channelNumber = changeChannelArgs.currentChannel + 1;
                        changeChannel(&changeChannelArgs, &channelRemoveInfoTimer);
						break;
                    }
					case 61:
                    {
						changeChannelArgs.channelNumber = changeChannelArgs.currentChannel - 1;
                        changeChannel(&changeChannelArgs, &channelRemoveInfoTimer);
						break;
                    }
                    case 63:
                    {
                        //volume up
                        if ((soundVolume + INT32_MAX * 0.1 ) <= INT32_MAX)
                        {
                            printf("SOUND UP\n");
                            soundVolume += INT32_MAX * 0.1;
                            Player_Volume_Set(remoteArgs->handles->playerHandle, soundVolume);
                            drawSoundInfo(remoteArgs->graphicsStruct, soundVolume);
                        }
                        // timer_settime(soundRemoveTimer.timerId, soundRemoveTimer.timerFlags, &soundRemoveTimer.timerSpec,
                        //     &soundRemoveTimer.timerSpecOld);
                        mute = FALSE;
                        break;
                    }
                    case 64:
                    {
                        //volume down
                        if (soundVolume != 0)
                        {
                            printf("SOUND DOWN\n");
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
                        drawSoundInfo(remoteArgs->graphicsStruct, soundVolume);
                        // timer_settime(soundRemoveTimer.timerId, soundRemoveTimer.timerFlags, &soundRemoveTimer.timerSpec,
                        //     &soundRemoveTimer.timerSpecOld);
                        mute = FALSE;                        
                        break;
                    }
                    case 60:
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
					case 102:
                    {
                        //exit
                        exitRemote = TRUE;
						break;
                    }
                    case 358:
                    {
                        //info
                        showChannelInfo(remoteArgs->graphicsStruct, changeChannelArgs.currentChannel, &channelRemoveInfoTimer);
                        break;
                    }
				}
			}	
        }
        if (exitRemote)
        {
            break;
        }
    }
    timer_delete(channelRemoveInfoTimer.timerId);
    timer_delete(soundRemoveTimer.timerId);
    timer_delete(timeArgs.channelChangerTimer.timerId);
    return NULL;
}