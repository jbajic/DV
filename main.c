#include "main.h"

uint32_t main(int argc, char** argv)
{
	config_parameters* config = loadFile(&argv[1]);


    //testConfigPrintf(config);
    freeConfig(config);
    return NO_ERROR;
}
