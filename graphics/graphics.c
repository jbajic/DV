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

int32_t drawSoundInfo(graphics* graphicsStruct, uint32_t volume)
{
	int32_t volumePercent = roundfunc(((float)volume / INT32_MAX) * 100);
	int32_t i;
	char tekst[10];
	sprintf(tekst, "%d %%", volumePercent);

	//clear surface
	DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0x00, 0x00));
	DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary, 0, 0, graphicsStruct->screenWidth, graphicsStruct->screenHeight));

	DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0x00, 0xAA));
    DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary, 
		11 * (graphicsStruct->screenWidth / 12) , 200,
		12 * (graphicsStruct->screenWidth / 12), graphicsStruct->screenHeight - 300));
    
	DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0xFF, 0xFF));
	for (i = 1; i <= MAX_VOLUME_LINES; i++)
	{
		if ((i * 10) > volumePercent)
		{
			DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0xFF, 0x44));
		}
		if (i >= BIG_VOLUME)
		{
			DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary, 
				graphicsStruct->screenWidth - 70 -  SOUND_BIG_LINE_WIDTH + SOUND_SMALL_LINE_WIDTH, graphicsStruct->screenHeight - 250 - i * 50,
				SOUND_BIG_LINE_WIDTH, 15));
		}
		else
		{
			DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary, 
				graphicsStruct->screenWidth - 70 , graphicsStruct->screenHeight - 250 - i * 50,
				SOUND_SMALL_LINE_WIDTH, 15));
		}	
	}
	IDirectFBFont *fontInterface = NULL;
	DFBFontDescription fontDesc;
	
	fontDesc.flags = DFDESC_HEIGHT;
	fontDesc.height = 50;
	
	DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0x00, 0xff));
	DFBCHECK(graphicsStruct->dfbInterface->CreateFont(graphicsStruct->dfbInterface, "/home/galois/fonts/DejaVuSans.ttf", &fontDesc, &fontInterface));
	DFBCHECK(graphicsStruct->primary->SetFont(graphicsStruct->primary, fontInterface));
	DFBCHECK(graphicsStruct->primary->DrawString(graphicsStruct->primary, tekst, -1,
		graphicsStruct->screenWidth - 150,  graphicsStruct->screenHeight - 150, DSTF_LEFT));
	DFBCHECK(graphicsStruct->primary->Flip(graphicsStruct->primary, NULL, 0));
    
    return NO_ERROR;
}

int32_t showReminder(graphics* graphicsStruct, int32_t channelNumber, uint8_t defaultMarkedButton)
{
	printf("Show reminder\n");
	char tekst1[] = "Reminder activated. Switch to";
	char tekst2[50];
	float boxX, boxY, buttonWidth, buttonHeight, boxWidth, boxHeight, buttonOneX, buttonOneY, buttonTwoX, buttonTwoY, textPadding, boxPadding;
	textPadding = 20;
	boxPadding = 20;

	boxX = graphicsStruct->screenWidth * 0.25;
	boxY = graphicsStruct->screenHeight * 0.25;
	boxWidth = graphicsStruct->screenWidth * 0.5;
	boxHeight = graphicsStruct->screenHeight * 0.5;

	buttonWidth = boxX - 1.5 * boxPadding;
	buttonHeight = 70;
	buttonOneX = boxX + boxPadding;
	buttonOneY = boxY + boxHeight - boxPadding - buttonHeight;

	buttonTwoX = graphicsStruct->screenWidth * 0.5 + 0.5 * boxPadding;
	buttonTwoY = boxY + boxHeight - boxPadding - buttonHeight;
	sprintf(tekst2, " channel %d?", channelNumber);
	//clear surface
	DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0x00, 0x00));
	DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary, 0, 0, graphicsStruct->screenWidth, graphicsStruct->screenHeight));

	DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0x00, 0xAA));
    DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary, boxX, boxY, boxWidth, boxHeight));

	DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0x00, 0xFF));
	DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary, buttonOneX, buttonOneY, buttonWidth, buttonHeight));
	DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary, buttonTwoX, buttonTwoY, buttonWidth, buttonHeight));

	IDirectFBFont *fontInterface = NULL;
	DFBFontDescription fontDesc;
	
	fontDesc.flags = DFDESC_HEIGHT;
	fontDesc.height = 50;
	DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0xFF, 0xFF));
	
	// DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0x00, 0xff));
	DFBCHECK(graphicsStruct->dfbInterface->CreateFont(graphicsStruct->dfbInterface, "/home/galois/fonts/DejaVuSans.ttf", &fontDesc, &fontInterface));
	DFBCHECK(graphicsStruct->primary->SetFont(graphicsStruct->primary, fontInterface));

	DFBCHECK(graphicsStruct->primary->DrawString(graphicsStruct->primary, tekst1, -1,
		boxX + boxPadding,  
		boxY + boxPadding + fontDesc.height, DSTF_LEFT));

	DFBCHECK(graphicsStruct->primary->DrawString(graphicsStruct->primary, tekst2, -1,
		boxX + boxPadding,  
		boxY + boxPadding + 50 + fontDesc.height, DSTF_LEFT));

	//OK
	DFBCHECK(graphicsStruct->primary->DrawString(graphicsStruct->primary, CONFIRM_BUTTON, -1,
		boxX + boxPadding + buttonWidth * 0.3,
		boxY + boxHeight - boxPadding - textPadding, DSTF_LEFT));

	//CANCEL
	DFBCHECK(graphicsStruct->primary->DrawString(graphicsStruct->primary, DECLINE_BUTTON, -1,
		boxX + boxWidth * 0.5 + 0.5 * boxPadding + buttonWidth * 0.3,
		boxY + boxHeight - boxPadding - textPadding, DSTF_LEFT));

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