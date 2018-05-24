// #include "background_processes.h"

// void* checkForTDTData(void* args)
// {
//     back_proc_args* backgroundProc = (back_proc_args*) args;
//     uint8_t isReminderTimeDetected = FALSE;
//     uint8_t isItTimeForReminder = FALSE;
//     tdt_table* tdtTable = getTDTTable();
//     pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
//     while (TRUE)
//     {
//         if (isTDTTableParsed())
//         {
//             printf("Time Arrived\n");
//             reminder* matchingReminder = isThereTime((reminder*) args, tdtTable->dateTimeUTC.time);
//             if (isItTimeForReminder)
//             {
//                 isReminderTimeDetected = TRUE;
//             }
//             else
//             {
//                 isReminderTimeDetected = FALSE;
//             }
//             if (!isReminderTimeDetected && isItTimeForReminder)
//             {
//                 showReminder(backgroundProc->graphicsStruct, matchingReminder->channel_index);
//             }
//             setTDTTableNotParsed();
//         }
//     }

//     return NO_ERROR;
// }