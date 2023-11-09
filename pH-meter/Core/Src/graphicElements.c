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


ELEMENT createButton(uint8_t btnX, uint8_t btnY, const char *text, uint16_t bgdColour, uint16_t bgdColour2, void (*btnAction)(), uint16_t arg)
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
	btnElement.btn.btnX = btnX;
	btnElement.btn.btnY = btnY;
	btnElement.btn.btnText = text;
	btnElement.btn.btnColour = bgdColour;
	btnElement.btn.btnColour2 = bgdColour2;

	btnElement.btn.btnAction = btnAction; //buttonChangeColour;
	btnElement.btn.btnArg = arg;

	while(text[textLen] != 0)
		textLen++;


	dimW = ( textLen * 7 ) + 5;
	dimH = ( 10  + 5 );

	fillRoundRect(btnX, btnY, dimW, dimH, 2, bgdColour);
	ST7735_WriteString(btnX+3, btnY+3, text, Font_7x10, WHITE, bgdColour);

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

	char numToChar[2];

	nBoxElement.elemtentType = EDITNUMBOX;
	nBoxElement.nBox.textX = textX;
	nBoxElement.nBox.textY = textY;
	nBoxElement.nBox.textColour = bgdTextColor;
	nBoxElement.nBox.boxText = text;
	nBoxElement.nBox.boxNum = num;
	nBoxElement.nBox.boxColor = bgdNumcolor;


	if(num > 9)
	{
		numToChar[0] = ( num / 10 ) + 0x30;
		numToChar[1] = ( num - ( (num / 10) * 10 ) ) + 0x30;
	}
	else
	{

		numToChar[0] = '0';
		numToChar[1] = num + 0x30;

	}

	numToChar[2] = 0;

	ST7735_WriteString(textX, textY, text, Font_11x18, WHITE, bgdTextColor);
    ST7735_WriteString(textX+32, textY+22, numToChar, Font_11x18, WHITE, bgdNumcolor);
    drawRoundRect(textX+27, textY+20, 30, 20, 2, WHITE);

    return nBoxElement;

}


void modifyNumBox(NBOX nBox)
{

	char numToChar[3] = {0};


	if(nBox.boxNum > 9)
	{
		numToChar[0] = ( nBox.boxNum / 10 ) + 0x30;
		numToChar[1] = ( nBox.boxNum - ( (nBox.boxNum / 10) * 10 ) ) + 0x30;
	}
	else
	{

		numToChar[0] = '0';
		numToChar[1] = nBox.boxNum + 0x30;;

	}

	ST7735_WriteString(nBox.textX, nBox.textY, nBox.boxText, Font_11x18, WHITE, nBox.textColour);
	ST7735_WriteString(nBox.textX+32, nBox.textY+22, numToChar, Font_11x18, WHITE, nBox.boxColor);
	drawRoundRect(nBox.textX+27, nBox.textY+20, 30, 20, 2, WHITE);

}


void buttonChangeColour(BTN *btn)
{


	uint16_t dimW;
	uint8_t dimH;

	uint8_t textLen = 0;
	while(btn->btnText[textLen] != 0)
		textLen++;

	dimW = ( textLen * 7 ) + 5;
	dimH = ( 10  + 5 );

	fillRoundRect(btn->btnX, btn->btnY, dimW, dimH, 2, btn->btnColour2);
	ST7735_WriteString(btn->btnX+3, btn->btnY+3, btn->btnText, Font_7x10, WHITE, btn->btnColour2);

	uint16_t tmp = btn->btnColour;
	btn->btnColour = btn->btnColour2;
	btn->btnColour2 = tmp;

}


/*
 *	@ Function; targetElement
 * 	@ Description; creates an arrow near the element
 *  @ Par1; ELEMENT
 */
void targetElement(ELEMENT element)
{

	static ELEMENT previousArrow;

	uint8_t arrowPosX;
	uint8_t arrowPosY;
	uint8_t arrowCenter;


	if(previousArrow.elemtentType == BUTTON)
	{

		// Clear previous position
		arrowPosX = previousArrow.btn.btnX-11;
		arrowPosY = previousArrow.btn.btnY;
		arrowCenter = (arrowPosY + 7);
		fillTriangle(arrowPosX, arrowPosY, arrowPosX+10, arrowCenter, arrowPosX, arrowPosY+14, background);

	}
	else if(previousArrow.elemtentType == EDITNUMBOX)
	{

		// Clear previous position
		arrowPosX = previousArrow.nBox.textX-11;
		arrowPosY = previousArrow.nBox.textY;
		arrowCenter = (arrowPosY + 7);
		fillTriangle(arrowPosX, arrowPosY, arrowPosX+10, arrowCenter, arrowPosX, arrowPosY+14, background);

	}

	if(element.elemtentType == BUTTON)
	{

		arrowPosX = element.btn.btnX-11;
		arrowPosY = element.btn.btnY;
		arrowCenter = (arrowPosY + 7);
		fillTriangle(arrowPosX, arrowPosY, arrowPosX+10, arrowCenter, arrowPosX, arrowPosY+14, WHITE);

		previousArrow = element;

	}
	else if(element.elemtentType == EDITNUMBOX)
	{

		arrowPosX = element.nBox.textX-11;
		arrowPosY = element.nBox.textY;
		arrowCenter = (arrowPosY + 7);
		fillTriangle(arrowPosX, arrowPosY, arrowPosX+10, arrowCenter, arrowPosX, arrowPosY+14, WHITE);

		previousArrow = element;

	}

}

