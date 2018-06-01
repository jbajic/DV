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
* File with basic functions and macrois available throughtout all modules
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
#include <signal.h>
#include <time.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include "tdp_api.h"

#ifndef TDPAPI_H_
    #define ERROR -1
    #define NO_ERROR 0
#endif

#define TRUE 1
#define FALSE 0


#define ASSERT_TDP_RESULT(x,y)  if(NO_ERROR == x) \
                                    printf("%s success\n", y); \
                                else{ \
                                    printf("%s fail\n", y); \
                                    return -1; \
                                }

typedef struct _time_utc
{
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} time_utc;

typedef struct _date_tdt
{
    uint8_t dayInMonth;
    uint8_t month;
    uint32_t year;
} date_tdt;

typedef struct _datetime
{
    date_tdt date;
    time_utc time;
} datetime;

typedef struct _timer_struct
{
    timer_t timerId;
    struct itimerspec timerSpec;
    struct itimerspec timerSpecOld;
    int32_t timerFlags;
} timer_struct;

typedef struct thread_args
{
    pthread_mutex_t mutex;
    pthread_cond_t condition;
} thread_args;

typedef struct _player_handles {
    uint32_t playerHandle;
    uint32_t sourceHandle;
    uint32_t videoStreamHandle;
    uint32_t audioStreamHandle;
    uint32_t filterHandle;
} player_handles;

#endif
