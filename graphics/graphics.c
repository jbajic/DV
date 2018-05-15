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

    // IDirectFBFont *fontInterface = NULL;
	// DFBFontDescription fontDesc;
	
    // /* specify the height of the font by raising the appropriate flag and setting the height value */
	// fontDesc.flags = DFDESC_HEIGHT;
	// fontDesc.height = 48;
	// printf("xaxax5\n");
    // /* create the font and set the created font for primary surface text drawing */
	// DFBCHECK(graphicsStruct->dfbInterface->CreateFont(graphicsStruct->dfbInterface, "/home/galois/fonts/DejaVuSans.ttf", &fontDesc, &fontInterface));
	// printf("xaxax6\n");
	// DFBCHECK(graphicsStruct->primary->SetFont(graphicsStruct->primary, fontInterface));   
	// printf("xaxax7\n");
    return NO_ERROR;
}

int32_t drawChannelInfo(graphics* graphicsStruct, int32_t channelNumber, int8_t isThereTeletext)
{
	IDirectFBImageProvider *provider;
	IDirectFBSurface *logoSurface = NULL;
	int32_t logoHeight, logoWidth;
	char tekst[10];
	sprintf(tekst, "Channel %d", channelNumber);

	DFBCHECK(graphicsStruct->dfbInterface->CreateImageProvider(graphicsStruct->dfbInterface, "blackCircle.png", &provider));
	DFBCHECK(provider->GetSurfaceDescription(provider, &graphicsStruct->surfaceDesc));
	DFBCHECK(graphicsStruct->dfbInterface->CreateSurface(graphicsStruct->dfbInterface, &graphicsStruct->surfaceDesc, &logoSurface));
	DFBCHECK(provider->RenderTo(provider, logoSurface, NULL));
	provider->Release(provider);
    /* fetch the logo size and add (blit) it to the screen */
	DFBCHECK(logoSurface->GetSize(logoSurface, &logoWidth, &logoHeight));
	DFBCHECK(graphicsStruct->primary->Blit(graphicsStruct->primary, logoSurface, NULL, 0, 0));
    

	IDirectFBFont *fontInterface = NULL;
	DFBFontDescription fontDesc;
	
	fontDesc.flags = DFDESC_HEIGHT;
	fontDesc.height = 48;
	
	DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0xFF, 0xff));
	DFBCHECK(graphicsStruct->dfbInterface->CreateFont(graphicsStruct->dfbInterface, "/home/galois/fonts/DejaVuSans.ttf", &fontDesc, &fontInterface));
	DFBCHECK(graphicsStruct->primary->SetFont(graphicsStruct->primary, fontInterface));
    
	DFBCHECK(graphicsStruct->primary->DrawString(graphicsStruct->primary, tekst, -1,  200, 200, DSTF_LEFT));
	if (isThereTeletext)
	{
		DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0xFF, 0x55));
		DFBCHECK(graphicsStruct->primary->DrawString(graphicsStruct->primary, "TTX", -1,  250, 250, DSTF_LEFT));
	}
    
	DFBCHECK(graphicsStruct->primary->Flip(graphicsStruct->primary, NULL, 0));
    
    return NO_ERROR;
}

int32_t clearGraphics(graphics* graphicsStruct)
{
	printf("Clear screen\n");
	DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0x00, 0x00));
	DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary, 0, 0, graphicsStruct->screenWidth, graphicsStruct->screenHeight));
	DFBCHECK(graphicsStruct->primary->Flip(graphicsStruct->primary, NULL, 0));
    return NO_ERROR;
}

int32_t deinitGraphics(graphics* graphicsStruct)
{
	graphicsStruct->primary->Release(graphicsStruct->primary);
	graphicsStruct->dfbInterface->Release(graphicsStruct->dfbInterface);

	return NO_ERROR;
}