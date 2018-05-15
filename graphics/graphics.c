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
* Datoteka sadrzi definicije potrebnih funkcija za pokretanje grafickog sucelja
* Made on 15.05.2018.
*
* @Author Jure Bajic
*****************************************************************************/
#include "graphics.h"

int32_t initGraphics(graphics* graphicsStruct)
{
    graphicsStruct->primary = NULL;
    graphicsStruct->dfbInterface = NULL;
    graphicsStruct->screenWidth = 0;
    graphicsStruct->screenHeight = 0;

	DFBCHECK(DirectFBInit(NULL, NULL));
	printf("xaxax1\n");
    /* fetch the DirectFB interface */
	DFBCHECK(DirectFBCreate(&graphicsStruct->dfbInterface));
	printf("xaxax2\n");
    /* tell the DirectFB to take the full screen for this application */
	DFBCHECK(graphicsStruct->dfbInterface->SetCooperativeLevel(graphicsStruct->dfbInterface, DFSCL_FULLSCREEN));
	printf("xaxax3\n");
	
    /* create primary surface with double buffering enabled */
	graphicsStruct->surfaceDesc.flags = DSDESC_CAPS;
	graphicsStruct->surfaceDesc.caps = DSCAPS_PRIMARY | DSCAPS_FLIPPING;
	DFBCHECK (graphicsStruct->dfbInterface->CreateSurface(graphicsStruct->dfbInterface, &graphicsStruct->surfaceDesc, &graphicsStruct->primary));

    /* fetch the screen size */
    DFBCHECK (graphicsStruct->primary->GetSize(graphicsStruct->primary, &graphicsStruct->screenWidth, &graphicsStruct->screenHeight));
    printf("xaxax4\n");

    IDirectFBFont *fontInterface = NULL;
	DFBFontDescription fontDesc;
	
    /* specify the height of the font by raising the appropriate flag and setting the height value */
	fontDesc.flags = DFDESC_HEIGHT;
	fontDesc.height = 48;
	printf("xaxax5\n");
    /* create the font and set the created font for primary surface text drawing */
	DFBCHECK(graphicsStruct->dfbInterface->CreateFont(graphicsStruct->dfbInterface, "/home/galois/fonts/DejaVuSans.ttf", &fontDesc, &fontInterface));
	printf("xaxax6\n");
	DFBCHECK(graphicsStruct->primary->SetFont(graphicsStruct->primary, fontInterface));   
	printf("xaxax7\n");
    return NO_ERROR;
}

int32_t drawChannelInfo(graphics* graphicsStruct, int32_t channelNumber)
{

    return NO_ERROR;
}

int32_t clearGraphics(graphics* graphicsStruct)
{
    	DFBCHECK(graphicsStruct->primary->SetColor(/*surface to draw on*/ graphicsStruct->primary,
                               /*red*/ 0x00,
                               /*green*/ 0x00,
                               /*blue*/ 0x00,
                               /*alpha*/ 0x00));
	DFBCHECK(graphicsStruct->primary->FillRectangle(/*surface to draw on*/ graphicsStruct->primary,
					/*upper left x coordinate*/ 0,
					/*upper left y coordinate*/ 0,
					/*rectangle width*/ graphicsStruct->screenWidth,
					/*rectangle height*/ graphicsStruct->screenHeight));
	DFBCHECK(graphicsStruct->primary->Flip(graphicsStruct->primary,
                           /*region to be updated, NULL for the whole surface*/NULL,
                           /*flip flags*/0));   
    return NO_ERROR;
}

int32_t deinitGraphics(graphics* graphicsStruct)
{
	graphicsStruct->primary->Release(graphicsStruct->primary);
	graphicsStruct->dfbInterface->Release(graphicsStruct->dfbInterface);

	return NO_ERROR;
}