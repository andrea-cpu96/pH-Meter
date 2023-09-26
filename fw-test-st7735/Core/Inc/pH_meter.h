/*
 * pH_meter.h
 *
 *  Created on: Sep 26, 2023
 *      Author: Fato
 */

#ifndef INC_PH_METER_H_
#define INC_PH_METER_H_

#define PH_V_SLOPE			1.0
#define V_OFFSET			0.0
#define PH_OFFSET			0.0

typedef enum
{

	STRONG_BASE,
	BASE,
	NEUTRAL,
	ACID,
	STRONG_ACID,

}PH_STATUS;


/************** pH ***************/

void pHMeter_process(void);


/*********** DISPLAY ************/

void st7735_hallScreen(void);
void st7735_mainScreen(void);


/********************************/

extern float pH_current;
extern uint8_t pH_status;


#endif /* INC_PH_METER_H_ */
