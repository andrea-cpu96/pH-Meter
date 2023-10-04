/*
 * user_sysfunc.c
 *
 *  Created on: Oct 3, 2023
 *      Author: Fato
 */

#include "main.h"
#include "user_sysfunc.h"


/*
 *	@ Function; getDelayms
 *  @ Return; uint32_t -> current time in ms
 */
uint32_t getTimeMs(void)
{

	// Tick frequency is by default 1kHz (1ms period)
	return ( HAL_GetTick() * HAL_GetTickFreq() );

}


/*
 *	@ Function; getDelayms
 * 	@ Par1; uint32_t timeStamp -> captured time
 *  @ Return; uint32_t tDiff -> time elapsed since timeStamp in ms
 */
uint32_t getDelayMs(uint32_t timeStamp)
{

	uint32_t tDiff;
	uint32_t currentTime;

	currentTime = ( getTimeMs() * HAL_GetTickFreq() );

	if(currentTime >= timeStamp)
		tDiff = currentTime - timeStamp;
	else
		tDiff = timeStamp - currentTime;

	return tDiff;

}


/*
 *	@ Function; floatToString
 *	@ Description; converts a float number into a string
 * 	@ Par1; float f -> float number
 * 	@ Par2; char *s -> string
 */
void floatToString(float f, char *s)
{

	uint16_t ui16Temp = (uint16_t)( f * 100 );

	if(ui16Temp >= 1000)
	{

		s[5] = 0;

		s[0] = (uint8_t)( ui16Temp / 1000 ) + 0x30;
		s[1] = (uint8_t)( ( ui16Temp - ( ( ui16Temp / 1000 ) * 1000 ) ) / 100 ) + 0x30;
		s[2] = '.';
		s[3] = (uint8_t)( ( ui16Temp - ( ( ui16Temp / 100 ) * 100 ) ) / 10 ) + 0x30;
		s[4] = (uint8_t)( ui16Temp - ( ( ui16Temp / 10 ) * 10 ) ) + 0x30;

	}
	else
	{

		s[5] = 0;
		s[4] = 0;

		s[0] = (uint8_t)( ui16Temp / 100 ) + 0x30;
		s[1] = '.';
		s[2] = (uint8_t)( ( ui16Temp - ( ( ui16Temp / 100 ) * 100 ) ) / 10 ) + 0x30;
		s[3] = (uint8_t)( ui16Temp - ( ( ui16Temp / 10 ) * 10 ) ) + 0x30;

	}

}
