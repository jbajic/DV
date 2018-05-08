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

void addReminderTime(char** time, reminder** reminderHead)
{
    if (*(reminderHead) == NULL)
    {
        (*reminderHead) = (reminder*) malloc(sizeof (reminder));
        (*reminderHead)->time = *time;
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
        newReminder->next->time = *(time);
        newReminder->next->next = NULL;
    }
}

void addReminderChannelIndex(uint16_t index, reminder** reminderHead)
{
    reminder* newReminder = *(reminderHead);
    while (newReminder->next != NULL)
    {
        newReminder = newReminder->next;
    }
    newReminder->channel_index = index;
}