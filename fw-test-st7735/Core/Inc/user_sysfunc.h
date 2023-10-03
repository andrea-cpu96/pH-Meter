/*
 * user_sysfunc.h
 *
 *  Created on: Oct 3, 2023
 *      Author: Fato
 */

#ifndef INC_USER_SYSFUNC_H_
#define INC_USER_SYSFUNC_H_

uint32_t getTimeMs(void);
uint32_t getDelayMs(uint32_t timeStamp);

void floatToString(float f, char *s);

#endif /* INC_USER_SYSFUNC_H_ */
