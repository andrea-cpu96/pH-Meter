/*
 * pH_meter.c
 *
 *  Created on: Sep 26, 2023
 *      Author: Fato
 */

#include <stdio.h>

#include "main.h"
#include "ST7735.h"
#include "GFX_FUNCTIONS.h"
#include "pH_meter.h"



static void pH_read(void);
static void pageTitle_graphics(const char *title);
static void colorBar_graphics(void);

float pH_current = 7.0;
uint8_t pH_status = NEUTRAL;


/************** pH **************/

void pHMeter_process(void)
{

	uint8_t processState;
	static uint16_t color = GREEN;
	static uint8_t first = 1;

	pH_read();

	//pH_current = 7.0;

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

	if(( first == 1 ) || ( pH_status != processState ))
	{

		switch(processState)
		{

			default:
			case NEUTRAL:

				pH_status = NEUTRAL;

				fillScreen(GREEN);
				st7735_mainScreen();

				color = GREEN;

				break;

			case STRONG_ACID:

				pH_status = STRONG_ACID;

				fillScreen(RED);
				st7735_mainScreen();

				color = RED;

				break;

			case ACID:

				pH_status = ACID;

				fillScreen(YELLOW);
				st7735_mainScreen();

				color = YELLOW;

				break;

			case BASE:

				pH_status = BASE;

				fillScreen(BLUE);
				st7735_mainScreen();

				color = BLUE;

				break;

			case STRONG_BASE:

				pH_status = STRONG_BASE;

				fillScreen(DARK_BLUE);
				st7735_mainScreen();

				color = DARK_BLUE;

				break;

		}

	}


	uint16_t upH = (uint16_t)( pH_current * 100 );

	char pHstring[6];

	if(upH >= 1000)
	{

		pHstring[5] = 0;

		pHstring[0] = (uint8_t)( upH / 1000 ) + 0x30;
		pHstring[1] = (uint8_t)( ( upH - ( ( upH / 1000 ) * 1000 ) ) / 100 ) + 0x30;
		pHstring[2] = '.';
		pHstring[3] = (uint8_t)( ( upH - ( ( upH / 100 ) * 100 ) ) / 10 ) + 0x30;
		pHstring[4] = (uint8_t)( upH - ( ( upH / 10 ) * 10 ) ) + 0x30;

	}
	else
	{

		pHstring[4] = 0;

		pHstring[0] = (uint8_t)( upH / 100 ) + 0x30;
		pHstring[1] = '.';
		pHstring[2] = (uint8_t)( ( upH - ( ( upH / 100 ) * 100 ) ) / 10 ) + 0x30;
		pHstring[3] = (uint8_t)( upH - ( ( upH / 10 ) * 10 ) ) + 0x30;

	}

	ST7735_SetRotation(1);
	ST7735_WriteString(40, 50, pHstring, Font_16x26, WHITE, color);

	if(first == 1)
		first = 0;

}


/*********** DISPLAY ************/

/*
 * 	@ Function: st7735_hallScreen
 * 	@ Description: Presentation screen
 */
void st7735_hallScreen(void)
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
	float pHValue = 0.0;

	static float buff[6] = {0};
	static uint32_t count = 0;

	rawAdcData = HAL_ADC_GetValue(&hadc);

	rawAdcmV = ( rawAdcData * ( 3.3 / 4096.0 ) );

	buff[count%10] = ( ( PH_V_SLOPE * ( rawAdcmV - V_OFFSET ) ) + PH_OFFSET );

	pHValue = buff[count];

	count++;

	if(count > 9)
	{

		pHValue = 0.0;
		for(int i = 0 ; i < 10 ; i++)
			pHValue += buff[i];

		pHValue = ( pHValue / 10 );

	}

	pH_current = pHValue;

	HAL_ADC_Start(&hadc);




}


static void pageTitle_graphics(const char *title)
{

	/*
	 *  \---------------------------/
	 *   \						   /
	 *	  \-----------------------/
	 */

	ST7735_SetRotation(1);

	fillTriangle(30, 0, 50, 0, 50, 20, MAIN_COLOR);
	fillTriangle(110, 0, 130, 0, 110, 20, MAIN_COLOR);
    fillRect(50, 0, 60, 21, MAIN_COLOR);

    ST7735_WriteString(45, 3, title, Font_7x10, WHITE, MAIN_COLOR);

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

/*****************************************************/
