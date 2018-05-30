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
#include <math.h>

/**
 * Graphics feature
*/
#define NUMBER_OF_GRAPHICS_FEATURES 5

/**
 * Sound dimensions 
**/
#define MAX_VOLUME_LINES 10
#define BIG_VOLUME 8
#define SOUND_SMALL_LINE_WIDTH 45
#define SOUND_BIG_LINE_WIDTH 70

/**
 * Reminder dimensions  and strings
**/
#define CONFIRM_BUTTON "YES"
#define DECLINE_BUTTON "NO"

/**
 * Clock
 **/
#define NUMBER_OF_CLOCK_LINES 7
#define NUMBER_OF_DIGITS 4

#define degreesToRadians(angleDegrees) ((angleDegrees) * M_PI / 180.0)
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

enum graphics_features
{
    G_FEATURE_CHANNEL_INFO = 0b00000001,
    G_FEATURE_VOLUME = 0b00000010,
    G_FEATURE_REMINDER = 0b00000100,
    G_FEATURE_TIME = 0b00001000,
    G_FEATURE_CHANNEL_NUMBER = 0b00010000
};

typedef struct _graphics_elements
{
    int32_t channelNumber;
    int32_t channelNumberTyped;
    uint8_t isThereTTX;
    int32_t soundVolume;
    int32_t reminderChannelNumber;
    uint8_t chosenButton;
    time_utc timeUtc;
} graphics_elements;

typedef struct _graphics
{
    IDirectFBSurface *primary;
    IDirectFB *dfbInterface;
    int32_t screenWidth;
    int32_t screenHeight;
    DFBSurfaceDescription surfaceDesc;
    graphics_elements graphicsElements;
} graphics;

typedef struct _graphics_features_drawing
{
    enum graphics_features feature;
    int32_t (*drawingFunction) (graphics*);
} graphics_features_drawing;

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
* Function for drawing certainElements
* @param
*       graphicsStruct - [in] Graphics structure in which all graphics will be found
*       flags - [in] Flag containing all elements to remove
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t drawGraphicsFeatures(graphics* graphicsStruct, uint8_t flags);

/****************************************************************************
*
* @brief
* Function for clearing certain graphics elements
*
* @param
*       graphicsStruct - [in] Graphics structure in which all graphics will be found
*       flags - [in] Flag containing all elements to remove
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t clearGraphicsFeatures(graphics* graphicsStruct, uint8_t flags);

/****************************************************************************
*
* @brief
* Function for drawing channel information
*
* @param
*       graphicsStruct - [in] Graphics structure in which all graphics will be found
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t drawChannelInfo(graphics* graphicsStruct);

/****************************************************************************
*
* @brief
* Function for drawing sound information
*
* @param
*       graphicsStruct - [in] Graphics structure in which all graphics will be found
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t drawSoundInfo(graphics* graphicsStruct);

/****************************************************************************
*
* @brief
* Function for drawing remidner dialog
*
* @param
*       graphicsStruct - [in] Graphics structure in which all graphics will be found
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t drawReminder(graphics* graphicsStruct);

/****************************************************************************
*
* @brief
* Function for chanign color when drawing clock
*
* @param
*       graphicsStruct - [in] Graphics structure in which all graphics will be found
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t drawTime(graphics* graphicsStruct);

/****************************************************************************
*
* @brief
* Function for drawing channel number
*
* @param
*       graphicsStruct - [in] Graphics structure in which all graphics will be found
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t drawChannelNumber(graphics* graphicsStruct);

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