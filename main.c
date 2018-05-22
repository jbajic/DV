#include "main.h"

int32_t main(int argc, char** argv)
{
    pthread_t backgroundProcesses;
    player_handles* handles = (player_handles*) malloc(sizeof(player_handles));
    config_parameters* config = (config_parameters*) malloc(sizeof(config_parameters));
    graphics* graphicsStruct = (graphics*) malloc(sizeof(graphics));
    if (argc <= 1) 
    {
        printf("No file path given!\n");
        return ERROR;
    }
    loadFile(&argv[1], config);
    tunerInitialization(config);

    startPlayer(handles);
    
    initGraphics(graphicsStruct);

    createStream(handles, config);

    setupData(handles, &backgroundProcesses);

    startRemote(handles, graphicsStruct);
    pthread_join(backgroundProcesses, NULL);

    removeStream(handles);
    stopPlayer(handles);
    deinitGraphics(graphicsStruct);
    tunerDeinitialization();

    free(config);
    free(handles);
    free(graphicsStruct);
    
    return NO_ERROR;
}
