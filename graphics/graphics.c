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
* File defines all the needed function for starting graphics modules, and needed drawing
* Made on 15.05.2018.
*
* @Author Jure Bajic
*****************************************************************************/
#include "graphics.h"

static pthread_mutex_t graphicsMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t graphicsCondition = PTHREAD_COND_INITIALIZER;
static pthread_t drawThread;

static int32_t numberToDigitalClock[10][7] =
{
    {CLOCK_TOP_LINE, CLOCK_RIGHT_UPPER_LINE, CLOCK_RIGHT_LOWER_LINE, CLOCK_DOWN_LINE, CLOCK_LEFT_UPPER_LINE, CLOCK_LEFT_LOWER_LINE},//0
    {CLOCK_RIGHT_UPPER_LINE, CLOCK_RIGHT_LOWER_LINE},//1
    {CLOCK_TOP_LINE, CLOCK_RIGHT_UPPER_LINE, CLOCK_MIDDLE_LINE, CLOCK_LEFT_LOWER_LINE, CLOCK_DOWN_LINE},//2
    {CLOCK_TOP_LINE, CLOCK_RIGHT_UPPER_LINE, CLOCK_MIDDLE_LINE, CLOCK_RIGHT_LOWER_LINE, CLOCK_DOWN_LINE},//3
    {CLOCK_RIGHT_UPPER_LINE, CLOCK_RIGHT_LOWER_LINE, CLOCK_MIDDLE_LINE, CLOCK_LEFT_UPPER_LINE},//4
    {CLOCK_TOP_LINE, CLOCK_LEFT_UPPER_LINE, CLOCK_MIDDLE_LINE, CLOCK_RIGHT_LOWER_LINE, CLOCK_DOWN_LINE},//5
    {CLOCK_TOP_LINE, CLOCK_LEFT_UPPER_LINE, CLOCK_LEFT_LOWER_LINE, CLOCK_DOWN_LINE, CLOCK_RIGHT_LOWER_LINE, CLOCK_MIDDLE_LINE},//6
    {CLOCK_TOP_LINE, CLOCK_RIGHT_UPPER_LINE, CLOCK_RIGHT_LOWER_LINE},//7
    {CLOCK_TOP_LINE, CLOCK_RIGHT_UPPER_LINE, CLOCK_RIGHT_LOWER_LINE, CLOCK_DOWN_LINE, CLOCK_LEFT_UPPER_LINE, CLOCK_LEFT_LOWER_LINE, CLOCK_MIDDLE_LINE},//8
	{CLOCK_TOP_LINE, CLOCK_RIGHT_UPPER_LINE, CLOCK_LEFT_UPPER_LINE, CLOCK_MIDDLE_LINE, CLOCK_RIGHT_LOWER_LINE}//9
};

static graphics_features_drawing graphicsFeaturesArray[NUMBER_OF_GRAPHICS_FEATURES] = {
	{.feature = G_FEATURE_CHANNEL_INFO, .drawingFunction = drawChannelInfo},
	{.feature = G_FEATURE_VOLUME, .drawingFunction = drawSoundInfo},
	{.feature = G_FEATURE_REMINDER, .drawingFunction = drawReminder},
	{.feature = G_FEATURE_TIME, .drawingFunction = drawTime},
	{.feature = G_FEATURE_CHANNEL_NUMBER, .drawingFunction = drawChannelNumber}
};

uint8_t featureFlags = 0b00000000;


/****************************************************************************
*
* @brief
* Function for showing drawn graphics elements
*
* @param
*       graphicsStruct - [in] Graphics structure in which all graphics will be found
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
static int32_t showGraphicsDraw(graphics* graphicsStruct)
{
	DFBCHECK(graphicsStruct->primary->Flip(graphicsStruct->primary, NULL, 0));

	return NO_ERROR;
}

/****************************************************************************
*
* @brief
* Function for drawing all graphics elements
*
* @param
*       graphicsStruct - [in] Graphics structure in which all graphics will be found
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
static void* drawAllGraphicsElements(void* args)
{
	graphics* graphicsStruct = (graphics*) args;
	uint8_t i;
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	while(TRUE)
	{
		pthread_mutex_lock(&graphicsMutex);
		pthread_cond_wait(&graphicsCondition, &graphicsMutex);
		pthread_mutex_unlock(&graphicsMutex);

		clearGraphics(graphicsStruct);
		for (i = 0; i < NUMBER_OF_GRAPHICS_FEATURES; ++i)
		{
			if (featureFlags & graphicsFeaturesArray[i].feature)
			{
				graphicsFeaturesArray[i].drawingFunction(graphicsStruct);
			}
		}
		showGraphicsDraw(graphicsStruct);
	}
	return NO_ERROR;
}

/****************************************************************************
*
* @brief
* Function for initializing graphics structure
*
* @param
*       graphicsStruct - [in] Graphics structure in which all graphics will be found
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
int32_t initGraphics(graphics* graphicsStruct)
{
	pthread_mutex_lock(&graphicsMutex);
    graphicsStruct->primary = NULL;
    graphicsStruct->dfbInterface = NULL;
    graphicsStruct->screenWidth = 0;
    graphicsStruct->screenHeight = 0;

	DFBCHECK(DirectFBInit(NULL, NULL));
    /* fetch the DirectFB interface */
	DFBCHECK(DirectFBCreate(&graphicsStruct->dfbInterface));
    /* tell the DirectFB to take the full screen for this application */
	DFBCHECK(graphicsStruct->dfbInterface->SetCooperativeLevel(graphicsStruct->dfbInterface, DFSCL_FULLSCREEN));
	
    /* create primary surface with double buffering enabled */
	graphicsStruct->surfaceDesc.flags = DSDESC_CAPS;
	graphicsStruct->surfaceDesc.caps = DSCAPS_PRIMARY | DSCAPS_FLIPPING;
	DFBCHECK (graphicsStruct->dfbInterface->CreateSurface(graphicsStruct->dfbInterface, &graphicsStruct->surfaceDesc, &graphicsStruct->primary));

    /* fetch the screen size */
    DFBCHECK (graphicsStruct->primary->GetSize(graphicsStruct->primary, &graphicsStruct->screenWidth, &graphicsStruct->screenHeight));
	pthread_mutex_unlock(&graphicsMutex);
	pthread_create(&drawThread, NULL, drawAllGraphicsElements, graphicsStruct);
    return NO_ERROR;
}

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
int32_t clearGraphicsFeatures(graphics* graphicsStruct, uint8_t flags)
{
	printf("clearGraphicsFeatures\nFLAGS: %d\n", flags);
	featureFlags = featureFlags & (~flags);
	// drawAllGraphicsElements(graphicsStruct);
	pthread_mutex_lock(&graphicsMutex);
	pthread_cond_signal(&graphicsCondition);
	pthread_mutex_unlock(&graphicsMutex);
	return NO_ERROR;
}

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
int32_t drawGraphicsFeatures(graphics* graphicsStruct, uint8_t flags)
{
	printf("drawGraphicsFeatures\nFLAGS: %d\n", flags);
	featureFlags = featureFlags | flags;
	// drawAllGraphicsElements(graphicsStruct);
	pthread_mutex_lock(&graphicsMutex);
	pthread_cond_signal(&graphicsCondition);
	pthread_mutex_unlock(&graphicsMutex);
	return NO_ERROR;
}

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
int32_t drawChannelInfo(graphics* graphicsStruct)
{
	pthread_mutex_lock(&graphicsMutex);
	int32_t i;
	char tekst[10];
	sprintf(tekst, "Channel %d", graphicsStruct->graphicsElements.channelNumber);
	float circleRadius, circleWidthDifference, circleOffsetX, circleOffsetY;
	circleRadius = 200;
	circleWidthDifference = 40;
	circleOffsetX = 300;
	circleOffsetY = 250;

	// //clear surface
	// DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0x00, 0x00));
	// DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary, 0, 0, graphicsStruct->screenWidth, graphicsStruct->screenHeight));

	DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0x00, 0xAA));
	for (i = 0; i <= 360; i++)
	{
		DFBCHECK(graphicsStruct->primary->FillTriangle(graphicsStruct->primary, circleRadius, circleRadius,
		(circleRadius + circleWidthDifference) * cos(degreesToRadians(i)) + circleOffsetX, circleRadius * sin(degreesToRadians(i)) + circleOffsetY,
		(circleRadius + circleWidthDifference) * cos(degreesToRadians(i + 1)) + circleOffsetX, circleRadius * sin(degreesToRadians(i + 1)) + circleOffsetY));
	}

	IDirectFBFont *fontInterface = NULL;
	DFBFontDescription fontDesc;
	
	fontDesc.flags = DFDESC_HEIGHT;
	fontDesc.height = 60;
	fontDesc.width = 150;
	
	DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0xFF, 0xff));
	DFBCHECK(graphicsStruct->dfbInterface->CreateFont(graphicsStruct->dfbInterface, "/home/galois/fonts/DejaVuSans.ttf", &fontDesc, &fontInterface));
	DFBCHECK(graphicsStruct->primary->SetFont(graphicsStruct->primary, fontInterface));
    
	DFBCHECK(graphicsStruct->primary->DrawString(graphicsStruct->primary, tekst, -1,  circleRadius - 30, 230, DSTF_LEFT));
	if (graphicsStruct->graphicsElements.isThereTTX)
	{
		DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0xFF, 0x11));
		DFBCHECK(graphicsStruct->primary->DrawString(graphicsStruct->primary, "TTX", -1,  250, 300, DSTF_LEFT));
	}
    
	// DFBCHECK(graphicsStruct->primary->Flip(graphicsStruct->primary, NULL, 0));
	fontInterface->Release(fontInterface);
    pthread_mutex_unlock(&graphicsMutex);
    return NO_ERROR;
}

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
int32_t drawSoundInfo(graphics* graphicsStruct)
{
	pthread_mutex_lock(&graphicsMutex);
	int32_t volumePercent = roundfunc(((float) graphicsStruct->graphicsElements.soundVolume / INT32_MAX) * 100);
	int32_t i;
	char tekst[10];
	sprintf(tekst, "%d %%", volumePercent);

	// //clear surface
	// DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0x00, 0x00));
	// DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary, 0, 0, graphicsStruct->screenWidth, graphicsStruct->screenHeight));

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
	// DFBCHECK(graphicsStruct->primary->Flip(graphicsStruct->primary, NULL, 0));
    
	fontInterface->Release(fontInterface);
	pthread_mutex_unlock(&graphicsMutex);
    return NO_ERROR;
}

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
int32_t drawReminder(graphics* graphicsStruct)
{
	pthread_mutex_lock(&graphicsMutex);
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
	sprintf(tekst2, " channel %d?", graphicsStruct->graphicsElements.reminderChannelNumber);

	DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0x00, 0xAA));
    DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary, boxX, boxY, boxWidth, boxHeight));

	DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0x00, 0xFF));
	if (graphicsStruct->graphicsElements.chosenButton == 1)
	{
		DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary, buttonTwoX, buttonTwoY, buttonWidth, buttonHeight));
		DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0xFF, 0xFF));
		DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary, buttonOneX, buttonOneY, buttonWidth, buttonHeight));
	}
	else
	{
		DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary, buttonOneX, buttonOneY, buttonWidth, buttonHeight));
		DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0xFF, 0xFF));
		DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary, buttonTwoX, buttonTwoY, buttonWidth, buttonHeight));
	}

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

	DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0x00, 0xFF));
	//OK
	DFBCHECK(graphicsStruct->primary->DrawString(graphicsStruct->primary, CONFIRM_BUTTON, -1,
		boxX + boxPadding + buttonWidth * 0.3,
		boxY + boxHeight - boxPadding - textPadding, DSTF_LEFT));

	//CANCEL
	DFBCHECK(graphicsStruct->primary->DrawString(graphicsStruct->primary, DECLINE_BUTTON, -1,
		boxX + boxWidth * 0.5 + 0.5 * boxPadding + buttonWidth * 0.3,
		boxY + boxHeight - boxPadding - textPadding, DSTF_LEFT));

	// DFBCHECK(graphicsStruct->primary->Flip(graphicsStruct->primary, NULL, 0));
	fontInterface->Release(fontInterface);
	pthread_mutex_unlock(&graphicsMutex);
	return NO_ERROR;
}

/****************************************************************************
*
* @brief
* Function for chanign color when drawing clock
*
* @param
*       graphicsStruct - [in] Graphics structure in which all graphics will be found
*       number - [in] Number of channel to check
*       clockLine - [in] Enum of digital clock line
*
* @return
*   ERROR, if there is error
*   NO_ERROR, if there is no error
****************************************************************************/
static int32_t changeColorIfNeeded(graphics* graphicsStruct, uint8_t number, enum digital_clock_lines clockLine)
{
	int32_t i;
	int32_t numberOfClockLines = NUMBER_OF_CLOCK_LINES;
	for (i = 0; i < numberOfClockLines; ++i)
	{
		if (numberToDigitalClock[number][i] == clockLine)
		{
			DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0xFF, 0xFF));
			return NO_ERROR;
		}
		else
		{
			DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0x00, 0x55));
		}
	}

	return NO_ERROR;
}

/****************************************************************************
*
* @brief
* Function for chanign color when drawing clock
*
* @param
*       timeUtc - [in] UTC time to show
*       index - [in] Index which to return [0][1]:[2][3]
*
* @return
*   ERROR, if there is error
*   DIGIT, from 0 to 9
****************************************************************************/
static int32_t getNumberFromTime(time_utc timeUtc, int32_t index)
{
	switch(index)
	{
		case 0:
		{
			return timeUtc.hours / 10;
		}
		case 1:
		{
			return timeUtc.hours % 10;
		}
		case 2:
		{
			return timeUtc.minutes / 10;
		}
		case 3:
		{
			return timeUtc.minutes % 10;
		}
		default:
			return ERROR;
	}
}

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
int32_t drawTime(graphics* graphicsStruct)
{
	pthread_mutex_lock(&graphicsMutex);
	float boxX, boxY, boxHeight, boxWidth, boxPadding, lineLength, lineWidness, linePadding, numberOffset, minutesOffset;
	int32_t i, number;

	lineLength = 70;
	lineWidness = 5;
	linePadding = 8;
	boxPadding = 40;
	boxX = graphicsStruct->screenWidth - graphicsStruct->screenWidth * 0.3;
	boxY = 20;
	boxWidth = graphicsStruct->screenWidth * 0.3 - 20;
	boxHeight = graphicsStruct->screenHeight * 0.25;
	numberOffset = linePadding + lineLength + 30;
	minutesOffset = 0;

	DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0x00, 0xFF));
    DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary, boxX, boxY, boxWidth, boxHeight));

	DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0xFF, 0xFF));
	//Draw dots
	DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary,
			boxX + boxPadding  + numberOffset * 2 + lineLength * 0.3, boxPadding + lineLength + 10,
			7, 7));
	DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary,
			boxX + boxPadding + numberOffset * 2 + lineLength * 0.3, boxPadding + lineLength + 30,
			7, 7));


	for (i = 0; i < NUMBER_OF_DIGITS; ++i)
	{
		number = getNumberFromTime(graphicsStruct->graphicsElements.timeUtc, i);
		if (i >= 2)
		{
			minutesOffset = lineLength;
		}
		//top && bottom
		changeColorIfNeeded(graphicsStruct, number, CLOCK_TOP_LINE);
		DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary,
			boxX + boxPadding + 10 + minutesOffset + numberOffset * i, boxPadding + 15,
			lineLength - 20, lineWidness));
		changeColorIfNeeded(graphicsStruct, number, CLOCK_DOWN_LINE);
		DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary,
			boxX + boxPadding + 10 + minutesOffset + numberOffset * i, boxPadding + lineLength * 2 + linePadding + 25,
			lineLength - 20, lineWidness));

		//middle
		changeColorIfNeeded(graphicsStruct, number, CLOCK_MIDDLE_LINE);
		DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary,
		boxX + boxPadding + 10 + minutesOffset + numberOffset * i, boxPadding + lineLength + 25,
		lineLength - 20, lineWidness));

		//left
		changeColorIfNeeded(graphicsStruct, number, CLOCK_LEFT_UPPER_LINE);
		DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary,
		boxX + boxPadding + minutesOffset + numberOffset * i, boxPadding + 20,
		lineWidness, lineLength));
		changeColorIfNeeded(graphicsStruct, number, CLOCK_LEFT_LOWER_LINE);
		DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary,
		boxX + boxPadding + minutesOffset + numberOffset * i, boxPadding + lineLength + linePadding + 20,
		lineWidness, lineLength));

		//right
		changeColorIfNeeded(graphicsStruct, number, CLOCK_RIGHT_UPPER_LINE);
		DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary,
		boxX + boxPadding + lineLength + minutesOffset + numberOffset * i, boxPadding + 20,
		lineWidness, lineLength));
		changeColorIfNeeded(graphicsStruct, number, CLOCK_RIGHT_LOWER_LINE);
		DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary,
		boxX + boxPadding + lineLength + minutesOffset + numberOffset * i, boxPadding + lineLength + linePadding + 20,
		lineWidness, lineLength));
	}

	// DFBCHECK(graphicsStruct->primary->Flip(graphicsStruct->primary, NULL, 0));
	
	pthread_mutex_unlock(&graphicsMutex);
	return NO_ERROR;
}

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
int32_t drawChannelNumber(graphics* graphicsStruct)
{
	pthread_mutex_lock(&graphicsMutex);
	char tekst[3];
	sprintf(tekst, "%d", graphicsStruct->graphicsElements.channelNumberTyped);
	IDirectFBFont *fontInterface = NULL;
	DFBFontDescription fontDesc;
	
	fontDesc.flags = DFDESC_HEIGHT;
	fontDesc.height = 50;

	DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0x00, 0xFF));
	DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary,
		graphicsStruct->screenWidth / 2 - 10, 10,
		120, 50));

	DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0xFF, 0xFF));
	DFBCHECK(graphicsStruct->dfbInterface->CreateFont(graphicsStruct->dfbInterface, "/home/galois/fonts/DejaVuSans.ttf", &fontDesc, &fontInterface));
	DFBCHECK(graphicsStruct->primary->SetFont(graphicsStruct->primary, fontInterface));

	DFBCHECK(graphicsStruct->primary->DrawString(graphicsStruct->primary, tekst, -1,
		graphicsStruct->screenWidth / 2,  
		50, DSTF_LEFT));
	
	fontInterface->Release(fontInterface);
	
	pthread_mutex_unlock(&graphicsMutex);
	return NO_ERROR;
}

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
int32_t clearGraphics(graphics* graphicsStruct)
{
	pthread_mutex_lock(&graphicsMutex);
	DFBCHECK(graphicsStruct->primary->SetColor(graphicsStruct->primary, 0x00, 0x00, 0x00, 0x00));
	DFBCHECK(graphicsStruct->primary->FillRectangle(graphicsStruct->primary, 0, 0, graphicsStruct->screenWidth, graphicsStruct->screenHeight));
	// DFBCHECK(graphicsStruct->primary->Flip(graphicsStruct->primary, NULL, 0));
	pthread_mutex_unlock(&graphicsMutex);
    return NO_ERROR;
}

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
int32_t deinitGraphics(graphics* graphicsStruct)
{
	pthread_mutex_lock(&graphicsMutex);
	pthread_cancel(drawThread);
	graphicsStruct->primary->Release(graphicsStruct->primary);
	graphicsStruct->dfbInterface->Release(graphicsStruct->dfbInterface);
	
	pthread_mutex_unlock(&graphicsMutex);
	return NO_ERROR;
}