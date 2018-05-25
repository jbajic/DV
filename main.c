#include "main.h"

int32_t main(int argc, char** argv)
{
    thread_args threadArguments = {.mutex = PTHREAD_MUTEX_INITIALIZER, .condition = PTHREAD_COND_INITIALIZER};
    //contains palyer handles and remidner head
    player_handles_reminders playerHandlesReminders;
    playerHandlesReminders.handles = (player_handles*) malloc(sizeof(player_handles));

    config_parameters* config = (config_parameters*) malloc(sizeof(config_parameters));
    graphics* graphicsStruct = (graphics*) malloc(sizeof(graphics));
    if (argc <= 1) 
    {
        printf("No file path given!\n");
        return ERROR;
    }
    loadFile(&argv[1], config);
    testConfigPrintf(config);
    playerHandlesReminders.channelReminders = config->headReminder;
    tunerInitialization(config);

    startPlayer(playerHandlesReminders.handles);
    
    initGraphics(graphicsStruct);

    createStream(playerHandlesReminders.handles, config);

    setupData(playerHandlesReminders.handles, &threadArguments);
    startRemote(playerHandlesReminders.handles, graphicsStruct, config->headReminder);

    removeStream(playerHandlesReminders.handles);
    printf("Condition\n");
    pthread_cond_signal(&threadArguments.condition);

    stopPlayer(playerHandlesReminders.handles);
    deinitGraphics(graphicsStruct);
    tunerDeinitialization();

    free(config);
    free(playerHandlesReminders.handles);
    free(graphicsStruct);
    
    return NO_ERROR;
}
