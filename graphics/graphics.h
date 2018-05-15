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

int32_t deinitGraphics(graphics*);

int32_t clearGraphics(graphics*);

#endif