/*
 * pH_meter.c
 *
 *  Created on: Sep 26, 2023
 *      Author: Fato
 */

#include <stdio.h>

#include "main.h"

#include "user_sysfunc.h"
#include "ST7735.h"
#include "GFX_FUNCTIONS.h"
#include "v_eeprom.h"

#include "pH_meter.h"


static uint8_t processStatus_update(void);
static void pHStatus_update(void);

static void joysticHandler(ELEMENT *element);
static void pageTitle_graphics(const char *title, uint8_t page);
static void mainPage_graphics(void);
static void circuitCalibPage_graphics(void);
static void softwareCalibPage_graphics(void);
static void colorBar_graphics(void);
static void orizzIndicators_graphics(uint16_t color);


uint8_t first = 1;

JOYSTICK_DIR joystick = REST;

ELEMENT *elements;

ELEMENT swCalibElement[SWCALIB_NUM_OF_ELEMENTS];
ELEMENT mainElement[MAIN_NUM_OF_ELEMENTS];

uint8_t elementIndex = 0;
uint8_t elementIndexLim = 0;

uint32_t pHTimeStamp = 0;
float pH_current = 7.0;
PROCESS_STATUS process_status = MAIN_PROCESS;
PH_STATUS pH_status = NEUTRAL;

uint16_t background = BLACK;

uint16_t newPageFlags = MAIN_PROCESS;

uint8_t timerOn = 0;

float twoPointsCalib_pH[2] = {7.0, 4.0};
float twoPointsCalib_mV[2] = {2.0, 2.6};


/************** pH **************/

/*
 *  @ Function; pHMeter_process()
 *  @ Description; handles the screen views
 */
void pHMeter_process(void)
{

	uint8_t pageChanged = 0;

	if(first)
	{

		idlePage();

		ee_init();

		if(!HAL_GPIO_ReadPin(SEL_GPIO_Port, SEL_Pin))
			newPageFlags = SW_PROCESS;
		else
			writeAppDataToEE();

	}

	// Get a new pH value
	pH_read();

    // New pH status evaluation
	pHStatus_update();

	// New process status evaluation
	pageChanged = processStatus_update();

	switch(process_status)
	{

		default:
		case MAIN_PROCESS:

			mainPage(pageChanged);

			break;

		case SW_PROCESS:

			softwareCalibPage(pageChanged);

			break;

		case HW_PROCESS:

			circuitCalibPage(pageChanged);

			break;

	}


	if(first == 1)
		first = 0;

	joysticHandler(elements);

}


/*
 *  @ Function; pH_read()
 *  @ Description; read a new pH value from the probe
 *  @ every PH_PROCESS_DELAY seconds
 */
void pH_read(void)
{

	uint32_t rawAdcData;
	float rawAdcmV;
	float pHValue = 0.0;

	static float buff[6] = {0};
	static uint32_t count = 0;

	if(getDelayMs(pHTimeStamp) < PH_PROCESS_DELAY)
		return;

	pHTimeStamp = getTimeMs();

	rawAdcData = HAL_ADC_GetValue(&hadc);

	rawAdcmV = ( rawAdcData * ( 3.3 / 4096.0 ) );

	buff[count%6] = ( ( PH_V_SLOPE() * ( rawAdcmV - V_OFFSET() ) ) + PH_OFFSET() );

	pHValue = buff[count%6];

	count++;

	if(count >= 6)
	{

		pHValue = 0.0;
		for(int i = 0 ; i < 6 ; i++)
			pHValue += buff[i];

		pHValue = ( pHValue / 6.0 );

	}

	if(( pHValue < 0 ) || ( pHValue > 14 ))
		pHValue = 9.9;

	pH_current = pHValue;

	HAL_ADC_Start(&hadc);

}

/*********** DISPLAY ************/

/*
 * 	@ Function: st7735_hallScreen
 * 	@ Description: Presentation screen
 */
void idlePage(void)
{

	  ST7735_SetRotation(3);
	  ST7735_WriteString(0, 0, "UnNatural presents", Font_7x10, WHITE,BLACK);
	  HAL_Delay(1000);
	  fillScreen(BLACK);

	  ST7735_SetRotation(3);
	  ST7735_WriteString(20, 50, "pH-METER V0", Font_11x18, GREEN,BLACK);
	  HAL_Delay(1000);
	  fillScreen(BLACK);

	  ST7735_SetRotation(3);
	  ST7735_WriteString(0, 0, "A product developed by", Font_7x10, WHITE,BLACK);
	  HAL_Delay(1000);
	  ST7735_WriteString(20, 50, "Eng. Andrea", Font_11x18, GREEN,BLACK);
	  HAL_Delay(1000);
	  fillScreen(BLACK);

}


/*
 *  @ Function; mainPage
 *  @ Description; This is the main page where the current
 *  @ value of pH is shown
 */
void mainPage(uint8_t updatePage)
{

	static PH_STATUS main_pH_status = IDLE;
	static uint16_t mainColor = GREEN;


	if( ( updatePage == 1 ) || ( pH_status != main_pH_status ) )
	{

		switch(pH_status)
		{

			default:
			case NEUTRAL:

				main_pH_status = NEUTRAL;

				background = GREEN;

				fillScreen(GREEN);
				mainPage_graphics();

				mainColor = GREEN;

				break;

			case STRONG_ACID:

				main_pH_status = STRONG_ACID;

				background = RED;

				fillScreen(RED);
				mainPage_graphics();

				mainColor = RED;

				break;

			case ACID:

				main_pH_status = ACID;

				background = YELLOW;

				fillScreen(YELLOW);
				mainPage_graphics();

				mainColor = YELLOW;

				break;

			case BASE:

				main_pH_status = BASE;

				background = BLUE;

				fillScreen(BLUE);
				mainPage_graphics();

				mainColor = BLUE;

				break;

			case STRONG_BASE:

				main_pH_status = STRONG_BASE;

				background = DARK_BLUE;

				fillScreen(DARK_BLUE);
				mainPage_graphics();

				mainColor = DARK_BLUE;

				break;

		}

	}

	if(updatePage == 1)
	{

		elements = mainElement;
		elementIndexLim = MAIN_NUM_OF_ELEMENTS;
		background = mainColor;

	}

	char pHstring[6];

	floatToString(pH_current, pHstring);

	ST7735_SetRotation(3);
	ST7735_WriteString(MAIN_PH_VAL_POSX, MAIN_PH_VAL_POSY, pHstring, Font_16x26, WHITE, mainColor);

}


/*
 *  @ Function; circuitCalibPage
 *  @ Description; When user push left button, this page
 *  @ indicates the OPAMP calibration
 */
void circuitCalibPage(uint8_t updatePage)
{

	static float hLineBuff[6] = {0};
	static uint32_t hLineCount = 0;
	static uint32_t hLine_old = 128;

	uint16_t indicatorsColor = RED;

	uint16_t rawAdcData = 128;
	uint32_t hLine = 128;
	float rawAdcmV = 0.0;


	if(updatePage == 1)
	{

		fillScreen(BLACK);
		circuitCalibPage_graphics();
		background = BLACK;

		HAL_ADC_Start(&hadc);

		return;

	}

	rawAdcData = HAL_ADC_GetValue(&hadc);
	rawAdcmV = ( rawAdcData * ( 3.3 / 4096.0 ) );
	hLineBuff[hLineCount%6] = 128.0 - ( rawAdcmV * 32.72); // ( 128 pixels / 3.3 V ) = 38.79

	hLine = hLineBuff[hLineCount%6];

	hLineCount++;

	if(hLineCount >= 6)
	{

		hLine = 0.0;
		for(int i = 0 ; i < 6 ; i++)
			hLine += hLineBuff[i];

		hLine = ( hLine / 6.0 );

	}

	HAL_ADC_Start(&hadc);

	// Draw a line indicator

	if( (uint8_t)(hLine_old*100) != (uint8_t)(hLine*100) )
		drawLine(0, hLine_old, 160, hLine_old, BLACK);

	drawLine(0, hLine, 160, hLine, WHITE);

	hLine_old = hLine;

	if(( hLine >= ( 128 - 54 - 1 ) ) && ( hLine <= ( 128 - 54 + 1 ) ))
		indicatorsColor = GREEN;
	else
		indicatorsColor = RED;

	orizzIndicators_graphics(indicatorsColor);

}


/*
 *  @ Function; softwareCalibPage
 *  @ Description; user reach this page from
 *  @ the main page. Here it is performed the
 *  @ two points probe calibration
 */
void softwareCalibPage(uint8_t updatePage)
{

	static uint8_t swCalibTimer = 9;
	static uint32_t swCalibTimeStamp = 0;
	static uint8_t pointIndex = 0;

	char sTimer[2] = {0};

	static float mVBuffCalib[6] = {0};
	float rawAdcmV;
	uint16_t rawAdcData;

	static uint8_t count = 0;


	if(updatePage == 1)
	{

		fillScreen(BLACK);
		softwareCalibPage_graphics();
		elements = swCalibElement;
		elementIndexLim = SWCALIB_NUM_OF_ELEMENTS;
		background = BLACK;

		HAL_ADC_Start(&hadc);

		return;

	}

	if(timerOn)
	{

		if(getDelayMs(swCalibTimeStamp) >= 1000)
		{

			rawAdcData = HAL_ADC_GetValue(&hadc);
			rawAdcmV = ( rawAdcData * ( 3.3 / 4096.0 ) );
			mVBuffCalib[count%6] = rawAdcmV;

			count++;

			if(count >= 6)
			{

				rawAdcmV = 0.0;
				for(int i = 0 ; i < 6 ; i++)
					rawAdcmV += mVBuffCalib[i];

				rawAdcmV = ( rawAdcmV / 6.0 );

			}

			HAL_ADC_Start(&hadc);

			swCalibTimer--;

			if(swCalibTimer <= 0)
			{

				swCalibTimer = 9;
				timerOn = 0;

				count = 0;

				ST7735_WriteString(SWCALIB_PT_NUM_POSX, SWCALIB_PT_NUM_POSY, "PT.2", Font_11x18, WHITE, background);

				buttonChangeColour(&swCalibElement[3].btn);

				twoPointsCalib_mV[pointIndex] = rawAdcmV;
				twoPointsCalib_pH[pointIndex] = swCalibElement[2].nBox.boxNum;

				pointIndex++;

				if(pointIndex >= 2)
				{

					writeFloatToEE(EE_SWCALIB_PH_PT1, &twoPointsCalib_pH[0]);
					writeFloatToEE(EE_SWCALIB_MV_PT1, &twoPointsCalib_mV[0]);
					writeFloatToEE(EE_SWCALIB_PH_PT2, &twoPointsCalib_pH[1]);
					writeFloatToEE(EE_SWCALIB_MV_PT2, &twoPointsCalib_mV[1]);

					pointIndex = 0;
					newPageFlags = MAIN_PROCESS;

				}

			}

			sTimer[0] = ( swCalibTimer + 0x30 );
			sTimer[1] = 0;
			ST7735_WriteString(SWCALIB_TIMER_POSX, SWCALIB_TIMER_POSY,sTimer ,Font_11x18, WHITE, background);

			swCalibTimeStamp = getTimeMs();

		}

	}
	else if(swCalibTimer != 9)
	{

		swCalibTimer = 9;
		swCalibTimeStamp = 0;
		sTimer[0] = ( swCalibTimer + 0x30 );
		sTimer[1] = 0;
		ST7735_WriteString(SWCALIB_TIMER_POSX, SWCALIB_TIMER_POSY,sTimer ,Font_11x18, WHITE, background);

	}

}


void btnSwitchPage(BTN *btn)
{

	buttonChangeColour(btn);
	newPageFlags = btn->btnArg;

}


void btnStartTimer(BTN *btn)
{

	buttonChangeColour(btn);
	timerOn = !timerOn;

}


/********************************/





/**************** PRIVATE FUNCTIONS ******************/

static void joysticHandler(ELEMENT *element)
{

	static uint8_t joystick_old = REST;


	if(!HAL_GPIO_ReadPin(UP_GPIO_Port, UP_Pin))
	{

		joystick = UP;

		if(joystick_old == REST)
		{

			if(( element[elementIndex].elemtentType == EDITNUMBOX ) && ( element[elementIndex].nBox.boxNum < 14 ))
			{

				element[elementIndex].nBox.boxNum++;
				modifyNumBox(element[elementIndex].nBox);

			}
			else
			{

				elementIndex--;

				if(( elementIndex >= elementIndexLim ) || ( elementIndex < 0 ))
					elementIndex = 0;

				targetElement(element[elementIndex]);

			}

		}

		HAL_Delay(200);

	}
	else if(!HAL_GPIO_ReadPin(DOWN_GPIO_Port, DOWN_Pin))
	{

		joystick = DOWN;

		if(joystick_old == REST)
		{

			if(( element[elementIndex].elemtentType == EDITNUMBOX )  && ( element[elementIndex].nBox.boxNum > 0 ))
			{

				element[elementIndex].nBox.boxNum--;
				modifyNumBox(element[elementIndex].nBox);

			}
			else
			{

				elementIndex++;

				if(( elementIndex >= elementIndexLim ) || ( elementIndex < 0 ))
					elementIndex = ( elementIndexLim - 1 );

				targetElement(element[elementIndex]);

			}

		}

		HAL_Delay(200);

	}
	else if(!HAL_GPIO_ReadPin(LEFT_GPIO_Port, LEFT_Pin))
	{

		joystick = LEFT;

		if(joystick_old == REST)
		{

			elementIndex--;

			if(( elementIndex >= elementIndexLim ) || ( elementIndex < 0 ))
				elementIndex = ( elementIndexLim - 1 );

			targetElement(element[elementIndex]);

		}

		HAL_Delay(200);

	}
	else if(!HAL_GPIO_ReadPin(RIGHT_GPIO_Port, RIGHT_Pin))
	{

		joystick = RIGHT;

		if(joystick_old == REST)
		{

			elementIndex++;

			if(( elementIndex >= elementIndexLim ) || ( elementIndex < 0 ))
				elementIndex = 0;

			targetElement(element[elementIndex]);

		}

		HAL_Delay(200);

	}
	else if(!HAL_GPIO_ReadPin(SEL_GPIO_Port, SEL_Pin))
	{

		joystick = CENTER;

		if(joystick_old == REST)
		{

			if(element[elementIndex].elemtentType == BUTTON)
				element[elementIndex].btn.btnAction(&element[elementIndex].btn);

		}

		HAL_Delay(200);

	}
	else
	{

		joystick = REST;

	}

	joystick_old = joystick;

}


static uint8_t processStatus_update(void)
{

	static PROCESS_STATUS local_process_status = IDLE_PROCESS;


	if(HAL_GPIO_ReadPin(OPAMP_CALIB_GPIO_Port, OPAMP_CALIB_Pin) == 0)
		process_status = HW_PROCESS;
	else
		process_status = newPageFlags;

	if(local_process_status != process_status)
	{

		local_process_status = process_status;
		elementIndex = 0;
		timerOn = 0;

		return 1;

	}
	else
	{

		local_process_status = process_status;
		return 0;

	}

}


static void pHStatus_update(void)
{

	if(pH_current >= STRONG_BASE_THR)
		pH_status = STRONG_BASE;
	else if(( pH_current > BASE_THR ) && ( pH_current < STRONG_BASE_THR ))
		pH_status = BASE;
	else if(( pH_current > STRONG_ACID_THR ) && ( pH_current < ACID_THR ))
		pH_status = ACID;
	else if(pH_current <= STRONG_ACID_THR)
		pH_status = STRONG_ACID;
	else
		pH_status = NEUTRAL;

}


static void mainPage_graphics(void)
{

	// 1. Title
	pageTitle_graphics("Current pH", MAIN_PAGE);

	// 2. Color scale
	colorBar_graphics();

	// 3. Buttons
	mainElement[0].elemtentType = NONE;
	//mainElement[1] = createButton(MAIN_TO_SWCALIB_BTN_POSX, MAIN_TO_SWCALIB_BTN_POSY, "CAL", GRAY, BLACK, btnSwitchPage, SW_PROCESS);

}


static void circuitCalibPage_graphics(void)
{

	// 1. Title
	pageTitle_graphics("HW calibration", CIRCUIT_CALIBRATION_PAGE);

	// 2. Offset indicators
	orizzIndicators_graphics(RED);

}


static void softwareCalibPage_graphics(void)
{

	// 1. Title
	pageTitle_graphics("SW calibration", SOFTWARE_CALIBRATION_PAGE);

	// 2. Timer
	char sTimer[] = {'9', 0};
	ST7735_WriteString(5, 35, "TIMER", Font_11x18, WHITE, BLACK);
	ST7735_WriteString(SWCALIB_TIMER_POSX, SWCALIB_TIMER_POSY, sTimer, Font_11x18, WHITE, BLACK);

	// 3. Point number
	ST7735_WriteString(SWCALIB_PT_NUM_POSX, SWCALIB_PT_NUM_POSY, "PT.1", Font_11x18, WHITE, BLACK);

	// 4. First element always NULL
	swCalibElement[0].elemtentType = NONE;

	// 5. Home button
	swCalibElement[1] = createButton(HOME_BTN_POSX, HOME_BTN_POSY, "<-", GRAY, GRAY, btnSwitchPage, 2);

	// 6. pH buffer
	swCalibElement[2] = createEditNumBox(PH_BUFFER_TEXT_POSX, PH_BUFFER_TEXT_POSY, "pH BUFF", BLACK, 0, BLACK);

	// 7. Buttons
	swCalibElement[3] = createButton(PH_BUFFER_START_BTN_POSX, PH_BUFFER_START_BTN_POSY, "START/STOP", RED, GREEN, btnStartTimer, 0);

}

static void pageTitle_graphics(const char *title, uint8_t page)
{

	/*
	 *  \---------------------------/
	 *   \						   /
	 *	  \-----------------------/
	 */


	uint8_t offset = 0;


	if(page == MAIN_PAGE)
		offset = 0;
	else if(page == CIRCUIT_CALIBRATION_PAGE)
		offset = 13;
	else if(page == SOFTWARE_CALIBRATION_PAGE)
		offset = 13;
	else
		offset = 0;

	ST7735_SetRotation(3);

	fillTriangle(30-offset, 0, 50-offset, 0, 50-offset, 20, MAIN_COLOR);
	fillTriangle(110+offset, 0, 130+offset, 0, 110+offset, 20, MAIN_COLOR);
    fillRect(50-offset, 0, 60+(2*offset), 21, MAIN_COLOR);

    ST7735_WriteString(45-offset, 3, title, Font_7x10, WHITE, MAIN_COLOR);

}

static void colorBar_graphics(void)
{

	/*
	 *  ________
	 * |________|    14
	 * |________|
	 * |________|
	 * |________|
	 * |________|
	 * |________|
	 * |________|	0
	 *
	 */


	ST7735_SetRotation(3);

	uint8_t x = 120;
	uint8_t y = 100;

	drawRect(x, y-80, 40, 20, BLACK);
	drawRect(x, y-60, 40, 20, BLACK);
	drawRect(x, y-40, 40, 20, BLACK);
	drawRect(x, y-20, 40, 20, BLACK);
	drawRect(x, y, 40, 20, BLACK);

	fillRect(x+1, y-80, 39, 19, DARK_BLUE);
	fillRect(x+1, y-60, 39, 19, BLUE);
	fillRect(x+1, y-40, 39, 19, GREEN);
	fillRect(x+1, y-20, 39, 19, YELLOW);
	fillRect(x+1, y, 39, 19, RED);

}


static void orizzIndicators_graphics(uint16_t color)
{

	/*
	 *
	 * |\                              /|
	 * | \                            / |
	 * | /                            \ |
	 * |/                              \|
	 *
	 */


	fillTriangle(0, 128-34, 20, 128-54, 0, 128-74, color);
	fillTriangle(160, 128-34, 140, 128-54, 160, 128-74, color);

}


/*****************************************************/
