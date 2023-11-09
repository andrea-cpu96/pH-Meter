/*
 * pH_meter.h
 *
 *  Created on: Sep 26, 2023
 *      Author: Fato
 */

#ifndef INC_PH_METER_H_
#define INC_PH_METER_H_

#include "graphicElements.h"


#define NONE									0
#define SELECT									1

#define PLUS									1
#define MINUS									-1

#define PH_PROCESS_DELAY						1000  // 1 sec

#define STRONG_BASE_THR							10.5
#define BASE_THR							    8.5
#define ACID_THR								5.5
#define STRONG_ACID_THR							3.5

#define PH_V_SLOPE()							( ( twoPointsCalib_pH[0] - twoPointsCalib_pH[1] ) / ( twoPointsCalib_mV[0] - twoPointsCalib_mV[1] ) ) //-15.0
#define V_OFFSET()								( twoPointsCalib_mV[0] ) // 1.25
#define PH_OFFSET()								( twoPointsCalib_pH[0]) // 7.0

#define MAIN_PAGE								2
#define SOFTWARE_CALIBRATION_PAGE				4
#define CIRCUIT_CALIBRATION_PAGE				8

// Macros for element positions

#define MAIN_PH_VAL_POSX						20
#define MAIN_PH_VAL_POSY						56

#define MAIN_TO_SWCALIB_BTN_POSX				50
#define MAIN_TO_SWCALIB_BTN_POSY				104

#define HOME_BTN_POSX							12
#define HOME_BTN_POSY							104

#define PH_BUFFER_TEXT_POSX						80
#define PH_BUFFER_TEXT_POSY						35
#define PH_BUFFER_START_BTN_POSX				PH_BUFFER_TEXT_POSX
#define PH_BUFFER_START_BTN_POSY    			( PH_BUFFER_TEXT_POSY + 45 )

#define SWCALIB_TIMER_POSX						20
#define SWCALIB_TIMER_POSY						55

#define SWCALIB_PT_NUM_POSX						60
#define SWCALIB_PT_NUM_POSY						104

typedef enum
{

	IDLE_PROCESS = 1,

	MAIN_PROCESS = MAIN_PAGE,
	SW_PROCESS = SOFTWARE_CALIBRATION_PAGE,
	HW_PROCESS = CIRCUIT_CALIBRATION_PAGE,

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


typedef enum
{

	REST,

	UP,
	DOWN,
	LEFT,
	RIGHT,
	CENTER,

}JOYSTICK_DIR;


extern JOYSTICK_DIR joystick;

extern ELEMENT *elements; // It points to the elements of a specific page
extern uint8_t elementIndex;
extern uint8_t elementIndexLim;

#define SWCALIB_NUM_OF_ELEMENTS				4
extern ELEMENT swCalibElement[SWCALIB_NUM_OF_ELEMENTS];

#define MAIN_NUM_OF_ELEMENTS				1 //2
extern ELEMENT mainElement[MAIN_NUM_OF_ELEMENTS];

extern uint16_t newPageFlags;

extern uint8_t timerOn;

extern float twoPointsCalib_pH[2];
extern float twoPointsCalib_mV[2];

/************** pH ***************/

void pHMeter_process(void);
void pH_read(void);

/*********** DISPLAY ************/

void idlePage(void);
void mainPage(uint8_t updatePage);
void circuitCalibPage(uint8_t updatePage);
void softwareCalibPage(uint8_t updatePage);

void btnSwitchPage(BTN *btn);
void btnStartTimer(BTN *btn);

/********************************/

extern float pH_current;
extern uint8_t pH_status;


#endif /* INC_PH_METER_H_ */
