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
* \player_tuner.h
* \brief
* Datoteka deklarira funkcije potrebne za danjiljskog upravljaca
* Made on 08.05.2018.
*
* @Author Jure Bajic
*****************************************************************************/
#ifndef REMOTE_H
#define REMOTE_H

#include "./../player_tuner/player_tuner.h"
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>

#define NUM_EVENTS  5

typedef struct _remote_loop_args {
    struct input_event* eventBuf;
    int32_t inputFileDesc;
    player_handles* handles;
} remote_loop_args;

int32_t startRemote(player_handles*);

int32_t initRemote(struct input_event**, int32_t*);

void *initRemoteLoop(void*);

#endif