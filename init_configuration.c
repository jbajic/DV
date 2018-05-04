#include "init_configuration.h"

config_parameters* loadFile(char*** file_path)
{
    FILE* file_pointer;
    //config_parameters config = (config_parameters*) malloc(sizeof(config_parameters));
    
    //file_pointer = fopen(*file_path, "r");
    
    
    //fclose(file_pointer);
    printf("%s", *(file_path));
    return NULL;
}

uint8_t freeConfig(struct config_parameters* config)
{
    free(config);
}
