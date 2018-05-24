#include "main.h"

int32_t main(int argc, char** argv)
{
    pthread_t backgroundProcesses;
    player_handles_mutex threadArgs;
    threadArgs.handles = (player_handles*) malloc(sizeof(player_handles));

    // threadArgs.backgroundProcessesMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_init(&threadArgs.backgroundProcessesMutex, NULL);
    pthread_mutex_lock(&threadArgs.backgroundProcessesMutex);

    // player_handles* handles = (player_handles*) malloc(sizeof(player_handles));
    config_parameters* config = (config_parameters*) malloc(sizeof(config_parameters));
    graphics* graphicsStruct = (graphics*) malloc(sizeof(graphics));
    if (argc <= 1) 
    {
        printf("No file path given!\n");
        return ERROR;
    }
    loadFile(&argv[1], config);
    testConfigPrintf(config);
    threadArgs.channelReminders = config->headReminder;
    tunerInitialization(config);

    startPlayer(threadArgs.handles);
    
    initGraphics(graphicsStruct);

    createStream(threadArgs.handles, config);

    setupData(&backgroundProcesses, &threadArgs);
    startRemote(threadArgs.handles, graphicsStruct, config->headReminder);
    pthread_mutex_unlock(&threadArgs.backgroundProcessesMutex);
    pthread_join(backgroundProcesses, NULL);

    removeStream(threadArgs.handles);
    stopPlayer(threadArgs.handles);
    deinitGraphics(graphicsStruct);
    tunerDeinitialization();

    free(config);
    free(threadArgs.handles);
    free(graphicsStruct);
    
    return NO_ERROR;
}
