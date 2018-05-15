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

static void showChannelInfo(graphics* graphicsStruct, int32_t currentChannel)
{
    #ifdef DEBUG
        printf("CHANNEL INFO\n\n\n");
    #endif
    int8_t isThereTeletext = FALSE;
    timer_struct clearGraphicsTimer;
    clearGraphicsTimer.timerFlags = 0;
    struct sigevent signalEvent;
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
    signalEvent.sigev_notify = SIGEV_THREAD;
	signalEvent.sigev_notify_function = clearScreen;
	signalEvent.sigev_value.sival_ptr = (void*) graphicsStruct;
	signalEvent.sigev_notify_attributes = NULL;
	timer_create(CLOCK_REALTIME, &signalEvent, &clearGraphicsTimer.timerId);

    memset(&clearGraphicsTimer.timerSpec, 0, sizeof(clearGraphicsTimer.timerSpec));
    clearGraphicsTimer.timerSpec.it_value.tv_sec = 3;
    clearGraphicsTimer.timerSpec.it_value.tv_nsec = 0;
    timer_settime(clearGraphicsTimer.timerId, clearGraphicsTimer.timerFlags, &clearGraphicsTimer.timerSpec, &clearGraphicsTimer.timerSpecOld);
}

static void changeChannel(player_handles* handles, int32_t* currentChannel, int32_t numberOfPrograms, graphics* graphicsStruct)
{
    if (*currentChannel <= 0)
    {
        *currentChannel = numberOfPrograms - 1;
    }
    if (*currentChannel > numberOfPrograms - 1)
    {
        *currentChannel = 1;
    }
    
    changeStream(handles, *currentChannel);
    showChannelInfo(graphicsStruct, *currentChannel);
}

static void changeChannelNumber(void* arg)
{
    #ifdef DEBUG
    printf("Ladida\n");
    #endif
    timer_channel_changer_args* timeArgs = (timer_channel_changer_args*) arg;
    printf("Full CHannel %d\n", timeArgs->channelNumber);
    changeChannel(timeArgs->handles, &timeArgs->channelNumber, timeArgs->numberOfPrograms, timeArgs->graphicsStruct);
}

void* initRemoteLoop(void* args)
{
    int32_t currentChannel = 1;
    uint32_t soundVolume;
    uint8_t mute = FALSE;
    int32_t numberOfPrograms = getPATTable()->number_of_programs;
    int32_t exitRemote = FALSE;
    uint32_t i;
    uint32_t eventCnt;
    remote_loop_args* remoteArgs = (remote_loop_args*) args;
    Player_Volume_Get(remoteArgs->handles->playerHandle, &soundVolume);

    timer_channel_changer_args timeArgs;
    timeArgs.channelNumber = 1;
    timeArgs.numberOfPrograms = numberOfPrograms;
    timeArgs.handles = remoteArgs->handles;
    timeArgs.graphicsStruct = remoteArgs->graphicsStruct;

    int32_t timerFlags = 0;
    struct sigevent signalEvent;
	signalEvent.sigev_notify = SIGEV_THREAD;
	signalEvent.sigev_notify_function = changeChannelNumber;
	signalEvent.sigev_value.sival_ptr = (void*) &timeArgs;
	signalEvent.sigev_notify_attributes = NULL;
	timer_create(CLOCK_REALTIME, &signalEvent, &timeArgs.channelChangerTimer.timerId);

    memset(&timeArgs.channelChangerTimer.timerSpec, 0, sizeof(timeArgs.channelChangerTimer.timerSpec));
    timeArgs.channelChangerTimer.timerSpec.it_value.tv_sec = 2;
    timeArgs.channelChangerTimer.timerSpec.it_value.tv_nsec = 0;

    while(TRUE)
    {
        /* read input eventS */
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
                        printf("Before calling timer1 %d\n", timeArgs.channelNumber);
                        if (timeArgs.channelNumber < 999)
                        {
                            timeArgs.channelNumber = timeArgs.channelNumber * 10 + remoteArgs->eventBuf[i].code - 1;
                            printf(" calling timer1 %d\n", timeArgs.channelNumber);
                            timer_settime(timeArgs.channelChangerTimer.timerId, timerFlags, &timeArgs.channelChangerTimer.timerSpec,
                                 &timeArgs.channelChangerTimer.timerSpecOld);
                        }
                        break;
                    }	
					case 62:
                    {
						currentChannel++;
                        changeChannel(remoteArgs->handles, &currentChannel, numberOfPrograms, remoteArgs->graphicsStruct);
						break;
                    }
					case 61:
                    {
						currentChannel--;
                        changeChannel(remoteArgs->handles, &currentChannel, numberOfPrograms, remoteArgs->graphicsStruct);
						break;
                    }
                    case 63:
                    {
                        //volume up
                        if (soundVolume < INT32_MAX)
                        {
                            soundVolume += INT32_MAX * 0.1;
                            Player_Volume_Set(remoteArgs->handles->playerHandle, soundVolume);
                        }
                        mute = FALSE;
                        break;
                    }
                    case 64:
                    {
                        //volume down
                        if (soundVolume > 0)
                        {
                            soundVolume -= INT32_MAX * 0.1;
                            Player_Volume_Set(remoteArgs->handles->playerHandle, soundVolume);
                        }
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
                        showChannelInfo(remoteArgs->graphicsStruct, currentChannel);
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
    return NULL;
}