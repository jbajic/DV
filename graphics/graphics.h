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
#define BOX_PADDING 20

#define DEFAULT_MARKED_BUTTON 1
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

typedef struct _graphics
{
    IDirectFBSurface *primary;
    IDirectFB *dfbInterface;
    int screenWidth;
    int screenHeight;
    DFBSurfaceDescription surfaceDesc;
} graphics;

int32_t initGraphics(graphics*);

int32_t drawChannelInfo(graphics*, int32_t, int8_t);

int32_t drawSoundInfo(graphics*, uint32_t);

int32_t showReminder(graphics*, int32_t, uint8_t);

int32_t deinitGraphics(graphics*);

int32_t clearGraphics(graphics*);

#endif