/*
 * pH_meter.c
 *
 *  Created on: Sep 26, 2023
 *      Author: Fato
 */


#include "main.h"
#include "ST7735.h"
#include "GFX_FUNCTIONS.h"
#include "pH_meter.h"



static void pageTitle_graphics(const char *title);
static void colorBar_graphics(void);

float pH_current = 7.0;
uint8_t pH_status = NEUTRAL;


/************** pH **************/

void pHMeter_process(void)
{

	uint8_t processState;

	if(pH_current >= 10.5)
		processState = STRONG_BASE;
	else if(( pH_current > 8.5 ) && ( pH_current < 10.5 ))
		processState = BASE;
	else if(( pH_current > 3.5 ) && ( pH_current < 5.5 ))
		processState = ACID;
	else if(pH_current <= 3.5)
		processState = STRONG_ACID;
	else
		processState = NEUTRAL;

	switch(processState)
	{

		default:
		case NEUTRAL:

			fillScreen(GREEN);
			st7735_mainScreen();

		break;

		case


	}




}


/*********** DISPLAY ************/

/*
 * 	@ Function: st7735_hallScreen
 * 	@ Description: Presentation screen
 */
void st7735_hallScreen(void)
{

	  ST7735_SetRotation(0);
	  ST7735_WriteString(0, 0, "UnNatural eng. presents", Font_11x18, WHITE,BLACK);
	  HAL_Delay(1000);
	  fillScreen(BLACK);

	  ST7735_SetRotation(0);
	  ST7735_WriteString(0, 0, "pH-METER V0", Font_16x26, GREEN,BLACK);
	  HAL_Delay(1000);
	  fillScreen(BLACK);

	  ST7735_SetRotation(0);
	  ST7735_WriteString(0, 0, "A product developed by", Font_11x18, WHITE,BLACK);
	  HAL_Delay(1000);
	  ST7735_WriteString(0, 20, "Andrea Fato eng.", Font_11x18, WHITE,BLACK);
	  HAL_Delay(1000);
	  fillScreen(BLACK);

}

/*
 * 	@ Function: st7735_mainScreen
 * 	@ Description: Presentation main screen for current pH measure
 */
void st7735_mainScreen(void)
{

	// 1. Title
	pageTitle_graphics("Current pH");

	// 2. Color scale
	colorBar_graphics();

}

/********************************/





/**************** PRIVATE FUNCTIONS ******************/


static void pH_read(void)
{

	uint32_t rawAdcData;
	float rawAdcmV;
	float pHValue;

	rawAdcData = HAL_ADC_GetValue(&hadc);

	rawAdcmV = ( rawAdcData * ( 5000.0 / 4096.0 ) );

	pHValue = ( ( PH_V_SLOPE * ( rawAdcmV - V_OFFSET ) ) + PH_OFFSET );

	pH_current = pHValue;


}


static void pageTitle_graphics(const char *title)
{

	/*
	 *  \---------------------------/
	 *   \						   /
	 *	  \-----------------------/
	 */

	drawLine(49, 0, 109, 0, MAIN_COLOR);
	drawLine(49, 0, 69, 20, MAIN_COLOR);
	drawLine(69, 20, 89, 20, MAIN_COLOR);
	drawLine(89, 20, 109, 0, MAIN_COLOR);

	fillTriangle(49, 0, 69, 0, 69, 20, MAIN_COLOR);
	fillTriangle(89, 0, 109, 0, 89, 20, MAIN_COLOR);
    fillRect(49, 20, 60, 20, MAIN_COLOR);

    ST7735_WriteString(59, 5, title, Font_11x18, WHITE, MAIN_COLOR);

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

	drawRoundRect(40, 110, 40, 20, 5, DARK_BLUE);
	drawRoundRect(60, 110, 40, 20, 5, BLUE);
	drawRoundRect(80, 110, 40, 20, 5, GREEN);
	drawRoundRect(100, 110, 40, 20, 5, YELLOW);
	drawRoundRect(120, 110, 40, 20, 5, RED);

}

/*****************************************************/
