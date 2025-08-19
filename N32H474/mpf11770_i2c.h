/**
*     Copyright (c) 2023, Nations Technologies Inc.
* 
*     All rights reserved.
*
*     This software is the exclusive property of Nations Technologies Inc. (Hereinafter 
* referred to as NATIONS). This software, and the product of NATIONS described herein 
* (Hereinafter referred to as the Product) are owned by NATIONS under the laws and treaties
* of the People's Republic of China and other applicable jurisdictions worldwide.
**/

/**
*\*\file mps_i2c.h
*\*\author Nations 
*\*\version v1.0.0 
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#ifndef __MPF11770_I2C_H__
#define __MPF11770_I2C_H__

#include "n32h47x_48x.h"
#include "n32h47x_48x_i2c.h"
#include "mps_crc.h"  /* For CRC functions */
#include <stdbool.h>


/* MPF11770 specific I2C functions with optional CRC */
/**
 * @brief Write data to an MPF11770 device via I2C with software CRC32
 * @param I2Cx - I2C interface to use
 * @param devAddr - 7-bit device address
 * @param regAddr - 16-bit register address
 * @param pData - pointer to data buffer
 * @param length - length of data to write (1-82 bytes as per spec)
 * @param enableCRC - set true to enable CRC32 calculation and verification
 * @return 0 if successful, error code otherwise
 */
int32_t MPF11770_I2C_master_write(I2C_Module* I2Cx, uint8_t devAddr, uint16_t regAddr, 
                                 uint8_t* pData, uint8_t length, bool enableCRC);

/**
 * @brief Read data from an MPF11770 device via I2C with software CRC32
 * @param I2Cx - I2C interface to use
 * @param devAddr - 7-bit device address
 * @param regAddr - 16-bit register address
 * @param pData - pointer to data buffer for read data
 * @param length - length of data to read (1-82 bytes as per spec)
 * @param enableCRC - set true to enable CRC32 calculation and verification
 * @return 0 if successful, error code otherwise
 */
int32_t MPF11770_I2C_master_read(I2C_Module* I2Cx, uint8_t devAddr, uint16_t regAddr, 
                                uint8_t* pData, uint8_t length, bool enableCRC);



/**
 * @brief Puts the MPF11770 device into active mode
 * @param I2Cx - I2C interface to use
 * @param devAddr - 7-bit device address
 * @param regAddr - 16-bit register address
 * @param length - Length of subsequent transaction
 * @param NoStop - Set true to skip sending STOP condition at end
 * @return 0 if successful, error code otherwise
 */
int32_t MPF11770_to_Active_Mode(I2C_Module* I2Cx, uint8_t devAddr, uint16_t regAddr, uint8_t length, bool NoStop);

/**
 * @brief  Activates the MPF11770 chip by pulling SDA low for 10ms
 * @param  I2Cx: I2C peripheral to be used
 * @return int32_t: 0 if successful, error code otherwise
 */
int32_t MPF11770_Activate(I2C_Module* I2Cx);



/**
 * @brief  callBack function for I2C communication timeout and errors
 * @param  errcode: Error code indicating the type of error
 * @return none
 */


#endif /* __MPF11770_I2C_H__ */ 