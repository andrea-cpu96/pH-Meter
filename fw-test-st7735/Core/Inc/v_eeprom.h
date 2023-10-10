/*
 * v_eeprom.h
 *
 *  Created on: Oct 10, 2023
 *      Author: Fato
 */

#ifndef INC_V_EEPROM_H_
#define INC_V_EEPROM_H_


#include <stdbool.h>


#define PAGE                  						0
#define SECTOR                						1
#define PAGE_NUM              						2
#define _EE_INIZIALIZED 							0xAAAA
#define	EE_FLAG_ADDR								(uint32_t)( 0x00000000 + 2040 )


#define _EE_USE_FLASH_PAGE_OR_SECTOR              	( 31 ) // Not 32 because addresses start from 0 to 0fff (16 bits addresses)
#define _EE_USE_RAM_BYTE                          	( 0 ) // If 0 the RAM buffer is not used

#define EEPROM_START_ADDR							(uint32_t)0x00000000 // Virtual EEPROM address

#define EE_SWCALIB_PH_PT1							(uint32_t)EEPROM_START_ADDR
#define EE_SWCALIB_MV_PT1							(uint32_t)( EE_SWCALIB_PH_PT1 + 4 )
#define EE_SWCALIB_PH_PT2							(uint32_t)( EE_SWCALIB_MV_PT1 + 4 )
#define EE_SWCALIB_MV_PT2							(uint32_t)( EE_SWCALIB_PH_PT2 + 4 )


typedef enum
{

	EE_INVALID,
	EE_VALID,

}EEPROM_STATUS;


bool ee_init(void);

void readAppDataFromEE(void);

float readFloatFromEE(uint32_t eeadr);
uint8_t readByteFromEE(uint32_t eeadr);
void writeFloatToEE(uint32_t eeadr, float fvalue);


#endif /* INC_V_EEPROM_H_ */
