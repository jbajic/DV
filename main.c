#include "main.h"

uint32_t main(int argc, char** argv)
{
    player_handles* handles = (player_handles*) malloc(sizeof(player_handles));
    if (argc <= 1) 
    {
        printf("No file path given!\n");
        return ERROR;
    }
	config_parameters* config = loadFile(&argv[1]);
    initTuner(config);

    startPlayer(handles);

    createStream(handles, config);

    setupData(handles);

    startRemote(handles);

    fflush(stdin);
    getchar();
    // testConfigPrintf(config);
    removeStream(handles);

    stopPlayer(handles);

    tunerDeinitialization();

    free(handles);
    
    freeConfig(config);
    
    return NO_ERROR;
}
