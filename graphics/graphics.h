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
* \graphics.h
* \brief
* Datoteka sadrzi deklaracije potrebnih funkcija i struktura za pokretanje grafickog sucelja
* Made on 15.05.2018.
*
* @Author Jure Bajic
*****************************************************************************/
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <directfb.h>
#include "./../base.h"

/**
 * Sound dimensions 
**/
#define MAX_VOLUME_LINES 10
#define BIG_VOLUME 8
#define SOUND_SMALL_LINE_WIDTH 45
#define SOUND_BIG_LINE_WIDTH 70

/**
 * Reminder dimensions 
**/
#define CONFIRM_BUTTON "OK"
#define DECLINE_BUTTON "NO"

/**
 * Clock
 **/
#define NUMBER_OF_CLOCK_LINES 7
#define NUMBER_OF_DIGITS 4

#define roundfunc(x) ((int) ((x) > 0.0 ? (x) + 0.5 : (x) - 0.5))
#define DFBCHECK(x...)                                      \
{                                                           \
DFBResult err = x;                                          \
                                                            \
if (err != DFB_OK)                                          \
  {                                                         \
    fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ );  \
    DirectFBErrorFatal( #x, err );                          \
  }                                                         \
}

enum digital_clock_lines
{
    CLOCK_TOP_LINE = 1,
    CLOCK_RIGHT_UPPER_LINE = 2,
    CLOCK_RIGHT_LOWER_LINE = 3,
    CLOCK_DOWN_LINE = 4,
    CLOCK_LEFT_UPPER_LINE = 5,
    CLOCK_LEFT_LOWER_LINE = 6,
    CLOCK_MIDDLE_LINE = 7
};

typedef struct _graphics
{
    IDirectFBSurface *primary;
    IDirectFB *dfbInterface;
    int screenWidth;
    int screenHeight;
    DFBSurfaceDescription surfaceDesc;
} graphics;

/****************************************************************************
*
* @brief
* Function for initializin graphics structure
*
* @param
*       graphicsStruct - [in] Graphics structure in which all graphics will be found
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t initGraphics(graphics* graphicsStruct);

/****************************************************************************
*
* @brief
* Function for drawing channel information
*
* @param
*       graphicsStruct - [in] Graphics structure in which all graphics will be found
*       channelNumber - [in] Number of channel
*       isThereTeletext - [in] Integer indicating if the programm has teletext
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t drawChannelInfo(graphics* graphicsStruct, int32_t channelNumber, int8_t isThereTeletext);

/****************************************************************************
*
* @brief
* Function for drawing sound information
*
* @param
*       graphicsStruct - [in] Graphics structure in which all graphics will be found
*       volume - [in] Volume
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t drawSoundInfo(graphics* graphicsStruct, uint32_t volume);

/****************************************************************************
*
* @brief
* Function for drawing remidner dialog
*
* @param
*       graphicsStruct - [in] Graphics structure in which all graphics will be found
*       channelNumber - [in] Number of channel to switch
*       chosenButton - [in] Chosen button (left or right)
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t drawReminder(graphics* graphicsStruct, int32_t channelNumber, uint8_t chosenButton);

/****************************************************************************
*
* @brief
* Function for chanign color when drawing clock
*
* @param
*       graphicsStruct - [in] Graphics structure in which all graphics will be found
*       timeUtc - [in] Time to show on digital clock
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t drawTime(graphics* graphicsStruct, time_utc timeUtc);

/****************************************************************************
*
* @brief
* Function for clearing screen
*
* @param
*       graphicsStruct - [in] Graphics structure in which all graphics will be found
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t clearGraphics(graphics* graphicsStruct);

/****************************************************************************
*
* @brief
* Function for releasing graphics structure
*
* @param
*       graphicsStruct - [in] Graphics structure in which all graphics will be found
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t deinitGraphics(graphics* graphicsStruct);

#endif