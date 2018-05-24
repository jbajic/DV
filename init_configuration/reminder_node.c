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
* \reminder_node.c
* \brief
* Datoteka definira funkcije za dodavanje i izmjenu liste remindera
* Made on 07.05.2018.
*
* @Author Jure Bajic
*****************************************************************************/
#include "reminder_node.h"

void addReminderTime(int32_t hours, int32_t minutes, reminder** reminderHead)
{
    if (*(reminderHead) == NULL)
    {
        (*reminderHead) = (reminder*) malloc(sizeof (reminder));
        (*reminderHead)->time.hours = hours;
        (*reminderHead)->time.minutes = minutes;
        (*reminderHead)->next = NULL;
    }
    else
    {
        reminder* newReminder = (*reminderHead);
        while (newReminder->next != NULL)
        {
            newReminder = newReminder->next;
        }
        newReminder->next = (reminder*) malloc(sizeof (reminder));
        newReminder->next->time.hours = hours;
        newReminder->next->time.minutes = minutes;
        newReminder->next->next = NULL;
    }
}

void addReminderChannelIndex(int32_t index, reminder** reminderHead)
{
    reminder* newReminder = *(reminderHead);
    while (newReminder->next != NULL)
    {
        newReminder = newReminder->next;
    }
    newReminder->channel_index = index;
}

reminder* isThereTime(reminder* reminderHead, time_utc utcTime)
{
    reminder* newReminder = reminderHead;
    while (newReminder->next != NULL)
    {
        // printf("%d:%d == %d:%d\n", newReminder->time.hours, newReminder->time.minutes, utcTime.hours, utcTime.minutes);
        if (newReminder->time.hours == utcTime.hours && newReminder->time.minutes == utcTime.minutes)
        {
            return newReminder;
        }
        newReminder = newReminder->next;
    }
    
    return NULL;
}