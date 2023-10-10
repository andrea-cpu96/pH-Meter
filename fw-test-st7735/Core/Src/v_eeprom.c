/*
 * v_eeprom.c
 *
 *  Created on: Oct 10, 2023
 *      Author: Fato
 */


#include "main.h"

#include "user_sysfunc.h"
#include "pH_meter.h"

#include "v_eeprom.h"


static EEPROM_STATUS getByteEe(uint32_t eeadr, uint8_t *byte);
static EEPROM_STATUS setHalfWorldEe(uint32_t eeadr, uint8_t *halfW);

static bool ee_format(bool keepRamData);
static bool ee_read(uint32_t startVirtualAddress, uint32_t len, uint8_t* data);
static bool ee_write(uint32_t startVirtualAddress, uint32_t len, uint8_t *data);
#if(_EE_USE_RAM_BYTE > 0)
static bool ee_writeToRam(uint32_t startVirtualAddress, uint32_t len, uint8_t* data);
static bool  ee_commit(void);
static uint32_t  ee_maxVirtualAddress(void);
#endif


#define   _EE_SIZE              2048
#define   _EE_ADDR_INUSE        (((uint32_t)0x08000000) | (_EE_SIZE * _EE_USE_FLASH_PAGE_OR_SECTOR)) // Page 31
#define   _EE_PAGE_OR_SECTOR    PAGE
#if (_EE_USE_FLASH_PAGE_OR_SECTOR > 127)
#error  "Please Enter correct address, maximum is (127)"
#endif

#if (_EE_USE_RAM_BYTE > 0)
uint8_t ee_ram[_EE_USE_RAM_BYTE]; // RAM buffer
#endif


bool ee_init(void)
{

	  readAppDataFromEE();

	  // Erase FLASH memory page
	  if(ee_format(0) == false)
		  return false;

	  return true;

}


void readAppDataFromEE(void)
{

	float pH_pt1 = readFloatFromEE(EE_SWCALIB_PH_PT1);
	float mV_pt1 = readFloatFromEE(EE_SWCALIB_MV_PT1);
	float pH_pt2 = readFloatFromEE(EE_SWCALIB_PH_PT2);
	float mV_pt2 = readFloatFromEE(EE_SWCALIB_MV_PT2);


	if(( pH_pt1  > 14 ) || ( pH_pt2 > 14 ) || ( pH_pt1 == pH_pt2 ))
	{

		twoPointsCalib_pH[0] = 7.0;
		twoPointsCalib_pH[1] = 4.0;

	}
	else
	{

		twoPointsCalib_pH[0] = pH_pt1;
		twoPointsCalib_pH[1] = pH_pt2;

	}

	if(( mV_pt1  > 3.6 ) || ( mV_pt2 > 3.6 ) || ( mV_pt1 == mV_pt2 ))
	{

		twoPointsCalib_mV[0] = 2.0;
		twoPointsCalib_mV[1] = 2.6;

	}
	else
	{

		twoPointsCalib_mV[0] = mV_pt1;
		twoPointsCalib_mV[1] = mV_pt2;

	}


}


void writeAppDataToEE(void)
{

	writeFloatToEE(EE_SWCALIB_PH_PT1, &twoPointsCalib_pH[0]);
	writeFloatToEE(EE_SWCALIB_MV_PT1, &twoPointsCalib_mV[0]);
	writeFloatToEE(EE_SWCALIB_PH_PT2, &twoPointsCalib_pH[1]);
	writeFloatToEE(EE_SWCALIB_MV_PT2, &twoPointsCalib_mV[1]);

}


float readFloatFromEE(uint32_t eeadr)
{

	UNION4_CONVERSION uconv;


	if(getByteEe(eeadr+0, &uconv.un4_uchar[0]) == EE_INVALID)
		return 0.0;
	if(getByteEe(eeadr+1, &uconv.un4_uchar[1]) == EE_INVALID)
		return 0.0;
	if(getByteEe(eeadr+2, &uconv.un4_uchar[2]) == EE_INVALID)
		return 0.0;
	if(getByteEe(eeadr+3, &uconv.un4_uchar[3]) == EE_INVALID)
		return 0.0;

	return uconv.un4_float;

}



uint8_t readByteFromEE(uint32_t eeadr)
{

	uint8_t uc;


	if(getByteEe(eeadr, &uc) == EE_INVALID)
		return 0;

	return uc;

}


/*
 * @ NOTE; you can only write HALF-WORLD AT TIME
 */
void writeFloatToEE(uint32_t eeadr, float *fvalue)
{

	UNION4_CONVERSION uconv;
	uconv.un4_float = *fvalue;


	setHalfWorldEe(eeadr+0, &uconv.un4_uchar[0]);
	setHalfWorldEe(eeadr+2, &uconv.un4_uchar[2]);

}



/******************* PRIVARE FUNCTIONS ************************/


static EEPROM_STATUS getByteEe(uint32_t eeadr, uint8_t *byte)
{

	if(ee_read(eeadr, 1, byte) == false)
		return EE_INVALID;

	return EE_VALID;

}


static EEPROM_STATUS setHalfWorldEe(uint32_t eeadr, uint8_t *halfW)
{

	if(ee_write(eeadr, 2, halfW) == false)
		return EE_INVALID;

	return EE_VALID;

}



static bool ee_format(bool keepRamData)
{

  uint32_t error;
  FLASH_EraseInitTypeDef flashErase;


  HAL_FLASH_Unlock();

  // Erase page 32
  flashErase.NbPages = 1;
  flashErase.PageAddress = _EE_ADDR_INUSE;
  flashErase.TypeErase = FLASH_TYPEERASE_PAGES;

  if (HAL_FLASHEx_Erase(&flashErase, &error) == HAL_OK)
  {

    HAL_FLASH_Lock();
    if (error != 0xFFFFFFFF)
    {

    	return false;

    }
    else
    {

#if (_EE_USE_RAM_BYTE > 0)
      if (keepRamData == false)
        memset(ee_ram, 0xFF, _EE_USE_RAM_BYTE);
#endif

      return true;

    }

  }

  return false;

}


static bool ee_read(uint32_t startVirtualAddress, uint32_t len, uint8_t* data)
{

  if(( startVirtualAddress + len ) > _EE_SIZE)
	  // Max size is one page ( 2kB )
	  return false;

  for(uint32_t i = startVirtualAddress; i < len + startVirtualAddress; i++)
  {

    if(data != NULL)
    {

      *data = (*(__IO uint8_t*) (i + _EE_ADDR_INUSE)); // __IO is volatile type, which allows to read and write memory
      data++;

    }

#if(_EE_USE_RAM_BYTE > 0)
    if (i < _EE_USE_RAM_BYTE)
      ee_ram[i] = (*(__IO uint8_t*) (i + _EE_ADDR_INUSE));
#endif

  }

  return true;

}


static bool ee_write(uint32_t startVirtualAddress, uint32_t len, uint8_t *data)
{

  if(( startVirtualAddress + len ) > _EE_SIZE)
    return false;

  if(data == NULL)
    return false;

  HAL_FLASH_Unlock();

  // Write half-worlds
  for(uint32_t i = 0; i < len ; i+=2)
  {

    if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, ((i + startVirtualAddress)) + _EE_ADDR_INUSE, (uint64_t)(data[i] | (data[i+1] << 8))) != HAL_OK)
    {

      HAL_FLASH_Lock();

      return false;

    }

  }

  HAL_FLASH_Lock();

  return true;

}

#if(_EE_USE_RAM_BYTE > 0)

static bool ee_writeToRam(uint32_t startVirtualAddress, uint32_t len, uint8_t* data)
{

#if (_EE_USE_RAM_BYTE > 0)

  if((startVirtualAddress + len) > _EE_USE_RAM_BYTE)
    return false;

  if (data == NULL)
    return false;

  memcpy(&ee_ram[startVirtualAddress], data, len);

  return true;

#else

  return false;

#endif

}


static bool ee_commit(void)
{

#if (_EE_USE_RAM_BYTE > 0)

  if (ee_format(true) == false)
    return false;

  return ee_write(0, _EE_USE_RAM_BYTE, ee_ram);

#else

  return false;

#endif

}


static uint32_t ee_maxVirtualAddress(void)
{

  return (_EE_SIZE);

}

#endif
