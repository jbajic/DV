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