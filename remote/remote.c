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

static int32_t getKeys(int32_t count, uint8_t* buf, int32_t* eventsRead, int32_t* inputFileDesc)
{
    int32_t ret = 0;
    
    /* read input events and put them in buffer */
    printf("buff5: %d\n inputfile5 %d\n", buf, *inputFileDesc);
    ret = read((*inputFileDesc), buf, (size_t)(count * (int) sizeof(struct input_event)));
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
    printf("START REMOTE\n");
    struct input_event* eventBuf;
    int32_t inputFileDesc;
    pthread_t remoteLoopId;
    printf("START REMOTE1\n");
    remote_loop_args* args = (remote_loop_args*) malloc(sizeof(remote_loop_args));
    printf("START REMOTE2\n");
    initRemote(&eventBuf, &inputFileDesc);
    // printf("buff1: %d\n inputfile1 %d\n", eventBuf, inputFileDesc);
    args->eventBuf = &eventBuf;
    args->inputFileDesc = &inputFileDesc;
    args->handles = handles;

    pthread_create(&remoteLoopId, NULL, initRemoteLoop, args);
    pthread_join(remoteLoopId, NULL);
    return NO_ERROR;
}

int32_t initRemote(struct input_event** eventBuf, int32_t* inputFileDesc)
{
    printf("INIT REMOTE\n");
    const char* dev = "/dev/input/event0";
    char deviceName[20];
    uint32_t i;
    int32_t program = 0;

    *(inputFileDesc) = open(dev, O_RDWR);
    if (*(inputFileDesc) == -1)
    {
        printf("Error while opening device (%s) !", strerror(errno));
	    return ERROR;
    }
    
    ioctl(*(inputFileDesc), EVIOCGNAME(sizeof(deviceName)), deviceName);
	printf("RC device opened succesfully [%s]\n", deviceName);
    
    *(eventBuf) = malloc(NUM_EVENTS * sizeof(struct input_event));
    if (!(*eventBuf))
    {
        printf("Error allocating memory !");
        return ERROR;
    }
    printf("REMOTE INITIALIZED\n");
    // printf("buff3: %d\n inputfile3 %d\n", *(eventBuf), (*inputFileDesc));
    return NO_ERROR;
}

void *initRemoteLoop(void* args)
{
    int32_t i;
    uint8_t exitRemoteLoop = FALSE;
    uint32_t eventCnt;
    int32_t currentChannel = 1;
    int32_t numberOfPrograms = getPATTable()->number_of_programs;
    remote_loop_args* remoteLoopArgs = (remote_loop_args*) args;
    // printf("buff2: %d\n inputfile2: %d\n", (*(remoteLoopArgs->eventBuf)), *(remoteLoopArgs->inputFileDesc));
    while (TRUE)
    {
        /* read input eventS */
        if (getKeys(NUM_EVENTS, (uint8_t*) (*(remoteLoopArgs->eventBuf)), &eventCnt, remoteLoopArgs->inputFileDesc))
        {
			printf("Error while reading input events !\n");
			return (void*) ERROR;
		}

        for (i = 0; i < eventCnt; i++)
        {
	
			if ((*(remoteLoopArgs->eventBuf + i))->value == 1) {
				switch ((*(remoteLoopArgs->eventBuf + i))->code) 
                {
					case 62:
                    {
						currentChannel++;
						if (currentChannel < 0)
                        {
                            currentChannel = numberOfPrograms - 2;
                        }
						if (currentChannel >= numberOfPrograms)
                        {
                            currentChannel = 0;
                        }
						changeStream(remoteLoopArgs->handles, currentChannel);	
						break;
                    }
					case 61:
                    {
						currentChannel--;
						if(currentChannel < 0) currentChannel = numberOfPrograms - 2;
						if(currentChannel >= numberOfPrograms) currentChannel = 0;
						changeStream(remoteLoopArgs->handles, currentChannel);	
						break;
                    }
					case 102:
                    {
                        exitRemoteLoop = TRUE;
						break;
                    }
				}
                if (exitRemoteLoop)
                {
                    break;
                }
			}	
        }
    }
    free(remoteLoopArgs->eventBuf);
    free(remoteLoopArgs);
}
