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
* \base.h
* \brief
* Datoteka s osnovnim funkcijama i makroima dostupni svim modulima
* Made on 08.05.2018.
*
* @Author Jure Bajic
*****************************************************************************/
#ifndef BASE_H
#define BASE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>

#ifdef DEBUG
#undef DEBUG
#endif
#define DEBUG 0

#define ERROR 1
#define NO_ERROR 0

#define TRUE 1
#define FALSE 0

typedef struct _timer_struct
{
    timer_t timerId;
    struct itimerspec timerSpec;
    struct itimerspec timerSpecOld;
    int32_t timerFlags;
} timer_struct;

#endif
