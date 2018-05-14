#include "main.h"

int32_t main(int argc, char** argv)
{
    player_handles* handles = (player_handles*) malloc(sizeof(player_handles));
    if (argc <= 1) 
    {
        printf("No file path given!\n");
        return ERROR;
    }
	config_parameters* config = loadFile(&argv[1]);
    tunerInitialization(config);

    startPlayer(handles);
    
    createStream(handles, config);

    setupData(handles);

    startRemote(handles);

    removeStream(handles);
    stopPlayer(handles);
    tunerDeinitialization();
    freeConfig(config);
    
    return NO_ERROR;
}
