/*
 * graphicElements.c
 *
 *  Created on: Oct 5, 2023
 *      Author: Fato
 */

#include <stdio.h>

#include "main.h"

#include "user_sysfunc.h"
#include "ST7735.h"
#include "GFX_FUNCTIONS.h"
#include "graphicElements.h"


BTN createButton(uint8_t btnX, uint8_t btnY, const char *text, uint16_t color)
{

	/*
	 *   ________
	 *  |        |
	 *  |  TEXT	 |
	 *  |________|
	 *
	 */

	uint8_t textLen = 0;
	uint16_t dimW = 1;
	uint8_t dimH = 1;

	BTN btn;

	btn.btnColour = color;
	btn.btnX = btnX;
	btn.btnY = btnY;

	btn.btnText = text;

	while(text[textLen] != 0)
		textLen++;


	dimW = ( textLen * 7 ) + 5;
	dimH = ( 10  + 5 );

	fillRoundRect(btnX, btnY, dimW, dimH, 2, color);
	ST7735_WriteString(btnX+3, btnY+3, text, Font_7x10, WHITE, color);

    return btn;

}


void editNumBox(uint8_t textX, uint8_t textY, const char *text, uint8_t num)
{

	/*
	 *
	 *    TEXT
	 *   _______
	 *  |       |
	 *  |	-	|
	 *  |_______|
	 *
	 */


	char numToChar[1];


	numToChar[0] = num + 0x30;

	ST7735_WriteString(textX, textY, text, Font_11x18, WHITE, BLACK);
    ST7735_WriteString(textX+32, textY+22, numToChar, Font_11x18, WHITE, BLACK);
    drawRoundRect(textX+27, textY+20, 20, 20, 2, WHITE);

}
