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

/****************************************************************************
*
* @brief
* Function for adding reminder in reminder linked list
*
* @param
*       hours - [in] Hours of new reminder
*       minutes - [in] Minutes of new reminder
*       reminderHead - [in] Head of reminder list
*
****************************************************************************/
void addReminderTime(int32_t hours, int32_t minutes, reminder** reminderHead);

/****************************************************************************
*
* @brief
* Function for adding channel index in newly created reminder (last)
*
* @param
*       index - [in] Index of channel to put in reminder
*       reminderHead - [in] Head of reminder list
*
****************************************************************************/
void addReminderChannelIndex(int32_t index, reminder** reminderHead);

/****************************************************************************
*
* @brief
* Function for checking is there reminder for current time
*
* @param
*       utcTime - [in] UTC time of current time
*       reminderHead - [in] Head of reminder list
*
* @return
*       reminder* - pointer to found reminder or NULL of non exist
*
****************************************************************************/
reminder* isThereTime(reminder* reminderHead, time_utc utcTime);

#endif