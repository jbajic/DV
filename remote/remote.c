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
    
    /* read input events and put them in buffer */
    printf("buff5: %u\n inputfile5 %d\n", *buf, inputFileDesc);
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

int32_t startRemote(player_handles* handles)
{
    pthread_t remoteThreadId;
    const char* dev = "/dev/input/event0";
    char deviceName[20];
    remote_loop_args* args = (remote_loop_args*) malloc(sizeof(remote_loop_args));
    args->handles = handles;
    args->inputFileDesc = open(dev, O_RDWR);

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

void* initRemoteLoop(void* args)
{
    int32_t currentChannel = 0;
    uint32_t soundVolume;
    uint8_t mute = FALSE;
    int32_t numberOfPrograms = getPATTable()->number_of_programs;
    int32_t exitRemote = FALSE;
    uint32_t i;
    uint32_t eventCnt;
    remote_loop_args* remoteArgs = (remote_loop_args*) args;
    Player_Volume_Get(remoteArgs->handles->playerHandle, &soundVolume);

    while(TRUE)
    {
        /* read input eventS */
        if(getKeys(NUM_EVENTS, (uint8_t*)remoteArgs->eventBuf, &eventCnt, remoteArgs->inputFileDesc))
        {
			printf("Error while reading input events !");
			//return ERROR;
		}

        for(i = 0; i < eventCnt; i++)
        {
	
			if(remoteArgs->eventBuf[i].value == 1) {
				switch(remoteArgs->eventBuf[i].code) {
					case 62:
						currentChannel++;
						if (currentChannel < 0)
                        {
                            currentChannel = numberOfPrograms - 2;
                        }
						if (currentChannel >= numberOfPrograms)
                        {
                            currentChannel = 0;
                        }
						changeStream(remoteArgs->handles, currentChannel);	
						break;
					case 61:
						currentChannel--;
						if (currentChannel < 0)
                        {
                            currentChannel = numberOfPrograms - 2;
                        }
						if (currentChannel >= numberOfPrograms)
                        {
                            currentChannel = 0;
                        }
						changeStream(remoteArgs->handles, currentChannel);	
						break;
                        //volume up
                    case 63:
                        if (soundVolume < INT32_MAX)
                        {
                            soundVolume += INT32_MAX * 0.1;
                            Player_Volume_Set(remoteArgs->handles->playerHandle, soundVolume);
                        }
                        mute = FALSE;
                        break;
                    //volume down                        
                    case 64:
                    if (soundVolume > 0)
                        {
                            soundVolume -= INT32_MAX * 0.1;
                            Player_Volume_Set(remoteArgs->handles->playerHandle, soundVolume);
                        }
                        mute = FALSE;                        
                        break;
                    //mute
                    case 60:
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
					case 102:
                        exitRemote = TRUE;
						break;
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