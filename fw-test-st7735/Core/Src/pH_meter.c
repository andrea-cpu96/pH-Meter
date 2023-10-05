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
#include "pH_meter.h"


static uint8_t processStatus_update(void);
static void pHStatus_update(void);

static void pageTitle_graphics(const char *title, uint8_t page);
static void mainPage_graphics(void);
static void circuitCalibPage_graphics(void);
static void softwareCalibPage_graphics(void);
static void colorBar_graphics(void);
static void orizzIndicators_graphics(uint16_t color);


uint8_t first = 1;

BTN swCalib_btn[3];

uint32_t pHTimeStamp = 0;
float pH_current = 7.0;
PROCESS_STATUS process_status = MAIN_PROCESS;
PH_STATUS pH_status = NEUTRAL;


/************** pH **************/

/*
 *  @ Function; pHMeter_process()
 *  @ Description; handles the screen views
 */
void pHMeter_process(void)
{

	uint8_t pageChanged = 0;

	if(first)
		idlePage();

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

		case CIRCUIT_CALIBRATION:

			circuitCalibPage(pageChanged);

			break;

	}


	if(first == 1)
		first = 0;

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

	buff[count%6] = ( ( PH_V_SLOPE * ( rawAdcmV - V_OFFSET ) ) + PH_OFFSET );

	pHValue = buff[count%6];

	count++;

	if(count >= 6)
	{

		pHValue = 0.0;
		for(int i = 0 ; i < 6 ; i++)
			pHValue += buff[i];

		pHValue = ( pHValue / 6.0 );

	}

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

	  ST7735_SetRotation(1);
	  ST7735_WriteString(0, 0, "UnNatural presents", Font_7x10, WHITE,BLACK);
	  HAL_Delay(1000);
	  fillScreen(BLACK);

	  ST7735_SetRotation(1);
	  ST7735_WriteString(20, 50, "pH-METER V0", Font_11x18, GREEN,BLACK);
	  HAL_Delay(1000);
	  fillScreen(BLACK);

	  ST7735_SetRotation(1);
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

				fillScreen(GREEN);
				mainPage_graphics();

				mainColor = GREEN;

				break;

			case STRONG_ACID:

				main_pH_status = STRONG_ACID;

				fillScreen(RED);
				mainPage_graphics();

				mainColor = RED;

				break;

			case ACID:

				main_pH_status = ACID;

				fillScreen(YELLOW);
				mainPage_graphics();

				mainColor = YELLOW;

				break;

			case BASE:

				main_pH_status = BASE;

				fillScreen(BLUE);
				mainPage_graphics();

				mainColor = BLUE;

				break;

			case STRONG_BASE:

				main_pH_status = STRONG_BASE;

				fillScreen(DARK_BLUE);
				mainPage_graphics();

				mainColor = DARK_BLUE;

				break;

		}

	}

	char pHstring[6];

	floatToString(pH_current, pHstring);

	ST7735_SetRotation(1);
	ST7735_WriteString(40, 50, pHstring, Font_16x26, WHITE, mainColor);

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
static void softwareCalibPage(uint8_t updatePage)
{


	if(updatePage == 1)
	{

		fillScreen(BLACK);
		softwareCalibPage_graphics();

		return;

	}

}



/********************************/





/**************** PRIVATE FUNCTIONS ******************/

static uint8_t processStatus_update(void)
{

	static PROCESS_STATUS local_process_status = IDLE_PROCESS;


	if(HAL_GPIO_ReadPin(OPAMP_CALIB_GPIO_Port, OPAMP_CALIB_Pin) == 0)
		process_status = CIRCUIT_CALIBRATION;
	else
		process_status = MAIN_PROCESS;

	if(local_process_status != process_status)
	{

		local_process_status = process_status;
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
	char sTimer[] = {'3', '0'};
	ST7735_WriteString(30, 54, "Timer", Font_11x18, WHITE, BLACK);
	ST7735_WriteString(30, 64, sTimer, Font_11x18, WHITE, BLACK);

	// 3. Point number
	ST7735_WriteString(50, 64, "PT. 1", Font_11x18, WHITE, BLACK);

	// 4. pH buffer
	editNumBox(PH_BUFFER_TEXT_POSX, PH_BUFFER_TEXT_POSY, "pH buffer", 7);

	// 5. Buttons
	swCalib_btn[0] = createButton(PH_BUFFER_START_BTN_POSX, PH_BUFFER_START_BTN_POSY, "START", GRAY);
	swCalib_btn[1] = createButton(PH_BUFFER_STOP_BTN_POSX, PH_BUFFER_STOP_BTN_POSY, "STOP", GRAY);

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
		offset = 20;
	else if(page == SOFTWARE_CALIBRATION_PAGE)
		offset = 20;
	else
		offset = 0;

	ST7735_SetRotation(1);

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


	ST7735_SetRotation(1);

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
