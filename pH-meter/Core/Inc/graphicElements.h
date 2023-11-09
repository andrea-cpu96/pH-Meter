/*
 * graphicElements.h
 *
 *  Created on: Oct 5, 2023
 *      Author: Fato
 */

#ifndef INC_GRAPHICELEMENTS_H_
#define INC_GRAPHICELEMENTS_H_


#define NONE					0
#define BUTTON					1
#define EDITNUMBOX			    2


typedef struct
{

	uint8_t btnX;
	uint8_t btnY;
	uint16_t btnColour;
	uint16_t btnColour2;
	const char * btnText;

	void (*btnAction)();
	uint16_t btnArg;

}BTN, *pBTN;

typedef struct
{

	uint8_t textX;
	uint8_t textY;
	uint16_t textColour;
	const char *boxText;
	uint8_t boxNum;
	uint8_t boxColor;

}NBOX;

typedef struct
{

	uint8_t elemtentType;

	BTN btn;
	NBOX nBox;

}ELEMENT;


extern uint16_t background;


ELEMENT createButton(uint8_t btnX, uint8_t btnY, const char *text, uint16_t bgdColour, uint16_t bgdColour2, void (*btnAction)(), uint16_t arg);
ELEMENT createEditNumBox(uint8_t textX, uint8_t textY, const char *text, uint16_t bgdTextColor, uint8_t num, uint16_t bgdNumcolor);

void buttonChangeColour(BTN *btn);
void targetElement(ELEMENT element);

void modifyNumBox(NBOX nBox);

#endif /* INC_GRAPHICELEMENTS_H_ */
