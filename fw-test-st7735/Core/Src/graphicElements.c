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


ELEMENT createButton(uint8_t btnX, uint8_t btnY, const char *text, uint16_t bgdColor)
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

	ELEMENT btnElement;

	btnElement.elemtentType = BUTTON;
	btnElement.btn.btnColour = bgdColor;
	btnElement.btn.btnX = btnX;
	btnElement.btn.btnY = btnY;
	btnElement.btn.btnText = text;

	btnElement.btn.btnAction =

	while(text[textLen] != 0)
		textLen++;


	dimW = ( textLen * 7 ) + 5;
	dimH = ( 10  + 5 );

	fillRoundRect(btnX, btnY, dimW, dimH, 2, bgdColor);
	ST7735_WriteString(btnX+3, btnY+3, text, Font_7x10, WHITE, bgdColor);

    return btnElement;

}


ELEMENT createEditNumBox(uint8_t textX, uint8_t textY, const char *text, uint16_t bgdTextColor, uint8_t num, uint16_t bgdNumcolor)
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

	ELEMENT nBoxElement;

	char numToChar[1];

	nBoxElement.elemtentType = EDITNUMBOX;
	nBoxElement.nBox.textX = textX;
	nBoxElement.nBox.textY = textY;
	nBoxElement.nBox.textColour = bgdTextColor;
	nBoxElement.nBox.boxText = text;
	nBoxElement.nBox.boxNum = num;
	nBoxElement.nBox.boxColor = bgdNumcolor;

	nBoxElement.nBox.boxAction =

	numToChar[0] = num + 0x30;

	ST7735_WriteString(textX, textY, text, Font_11x18, WHITE, bgdTextColor);
    ST7735_WriteString(textX+32, textY+22, numToChar, Font_11x18, WHITE, bgdNumcolor);
    drawRoundRect(textX+27, textY+20, 20, 20, 2, WHITE);

    return nBoxElement;

}
