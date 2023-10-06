/*
 * graphicElements.h
 *
 *  Created on: Oct 5, 2023
 *      Author: Fato
 */

#ifndef INC_GRAPHICELEMENTS_H_
#define INC_GRAPHICELEMENTS_H_


#define BUTTON					0
#define EDITNUMBOX			    1


typedef struct
{

	uint8_t btnX;
	uint8_t btnY;
	uint16_t btnColour;
	const char * btnText;

	void *btnAction;

}BTN;

typedef struct
{

	uint8_t textX;
	uint8_t textY;
	uint16_t textColour;
	const char *boxText;
	uint8_t boxNum;
	uint8_t boxColor;

	void *boxAction;

}NBOX;

typedef struct
{

	uint8_t elemtentType;

	BTN btn;
	NBOX nBox;

}ELEMENT;

ELEMENT createButton(uint8_t btnX, uint8_t btnY, const char *text, uint16_t color);
ELEMENT createEditNumBox(uint8_t textX, uint8_t textY, const char *text, uint16_t bgdTextColor, uint8_t num, uint16_t bgdNumcolor);


#endif /* INC_GRAPHICELEMENTS_H_ */
