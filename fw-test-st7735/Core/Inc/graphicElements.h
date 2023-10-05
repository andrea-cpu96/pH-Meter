/*
 * graphicElements.h
 *
 *  Created on: Oct 5, 2023
 *      Author: Fato
 */

#ifndef INC_GRAPHICELEMENTS_H_
#define INC_GRAPHICELEMENTS_H_

typedef struct
{

	uint8_t btnX;
	uint8_t btnY;
	uint16_t btnColour;
	const char * btnText;

}BTN;


BTN createButton(uint8_t btnX, uint8_t btnY, const char *text, uint16_t color);
void editNumBox(uint8_t textX, uint8_t textY, const char *text, uint8_t num);


#endif /* INC_GRAPHICELEMENTS_H_ */
