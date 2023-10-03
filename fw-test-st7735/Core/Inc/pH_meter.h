/*
 * pH_meter.h
 *
 *  Created on: Sep 26, 2023
 *      Author: Fato
 */

#ifndef INC_PH_METER_H_
#define INC_PH_METER_H_

#define PH_PROCESS_DELAY	1000  // 1 sec

#define PH_V_SLOPE			-15.0
#define V_OFFSET			1.25
#define PH_OFFSET			7.0

typedef enum
{

	IDLE_PROCESS,

	MAIN_PROCESS,
	CIRCUIT_CALIBRATION,

}PROCESS_STATUS;


typedef enum
{

	IDLE,

	STRONG_BASE,
	BASE,
	NEUTRAL,
	ACID,
	STRONG_ACID,

}PH_STATUS;


/************** pH ***************/

void pHMeter_process(void);
void pH_read(void);

/*********** DISPLAY ************/

void idlePage(void);
void mainPage(uint8_t updatePage);
void circuitCalibPage(void);

/********************************/

extern float pH_current;
extern uint8_t pH_status;


#endif /* INC_PH_METER_H_ */
