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

#include "./../base.h"

typedef struct reminder
{
    int32_t channel_index;
    time_utc time;
    struct reminder* next;
} reminder;

void addReminderTime(int32_t, int32_t, reminder**);

void addReminderChannelIndex(int32_t, reminder**);

reminder* isThereTime(reminder*, time_utc);

#endif
