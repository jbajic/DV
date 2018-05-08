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
* \reminder_node.h
* \brief
* Datoteka deklarira funkcije za dodavanje i izmjenu liste remindera, te definira i strukturu remindera
* Made on 07.05.2018.
*
* @Author Jure Bajic
*****************************************************************************/
#ifndef REMINDER_NODE_H
#define REMINDER_NODE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct reminder
{
    uint16_t channel_index;
    char* time;
    struct reminder* next;
} reminder;

void addReminderTime(char**, reminder**);

void addReminderChannelIndex(uint16_t, reminder**);

#endif
