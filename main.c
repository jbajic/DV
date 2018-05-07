#include "main.h"

uint32_t main(int argc, char** argv)
{
	config_parameters* config = loadFile(&argv[1]);
 
    return NO_ERROR;
}
