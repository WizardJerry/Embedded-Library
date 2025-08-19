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
*\*\file mps_crc.c
*\*\author Nations 
*\*\version v1.0.0 
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#include "mps_crc.h"
#include "n32h47x_48x_crc.h"  // 添加CRC硬件功能相关的头文件
#include "n32h47x_48x_rcc.h"  // 添加RCC时钟控制相关的头文件

/**
*\*\name    Calculate_CRC32.
*\*\fun     Calculate CRC32 for data with register address.
*\*\param   reg_addr - register address
*\*\param   length - data length
*\*\param   data - data pointer
*\*\return  CRC32 value
**/
uint32_t Calculate_CRC32(uint16_t reg_addr, uint8_t length, uint8_t* data)
{
    short i;
    unsigned long crc = 0xffffffff;
    unsigned char dataTemp[4];
    for (i=-1; i<length; i++){
        if(i==-1) {
            dataTemp[0]=length;
            dataTemp[1]=reg_addr&0x00FF;
            dataTemp[2]=(reg_addr&0xFF00)>>8;
            dataTemp[3]=0;
        }
        else dataTemp[i%4]=data[i];
        if((i%4)==3 || i == length-1 || i == -1) {
            for (char j=0; j< 4; j++) {
                crc ^= dataTemp[3-j] << 24;
                for (char k = 0; k < 8; ++k) {
                    if ((crc & 0x80000000) != 0)
                        crc = (crc << 1) ^ 0x04C11DB7;
                    else
                        crc <<= 1;
                }
            }
            dataTemp[0]=0;
            dataTemp[1]=0;
            dataTemp[2]=0;
            dataTemp[3]=0;
        }
    }
    return crc;
}

/* Calculate_CRC32_Simple 函数已删除 - 未被使用 */

/* Calculate_CRC32_WithHeader 函数已删除 - 不再需要 */

/**
*\*\name    Calculate_CRC16.
*\*\fun     Calculate CRC16 for data with register address.
*\*\param   reg_addr - register address
*\*\param   length - data length
*\*\param   data - data pointer
*\*\return  CRC16 value
**/
uint16_t Calculate_CRC16(uint16_t reg_addr, uint8_t length, uint8_t* data)
{
    uint16_t crc = CRC16_INITIAL_VALUE;
    uint16_t i;
    uint8_t j;
    uint8_t header[3];
    
    // Process register address and length first
    header[0] = length;
    header[1] = reg_addr & 0xFF;
    header[2] = (reg_addr & 0xFF00) >> 8;
    
    // Calculate CRC for header
    for (i = 0; i < 3; i++) {
        crc ^= (uint16_t)header[i] << 8;
        for (j = 0; j < 8; j++) {
            if ((crc & 0x8000) != 0) {
                crc = (crc << 1) ^ CRC16_POLYNOMIAL;
            } else {
                crc <<= 1;
            }
        }
    }
    
    // Calculate CRC for data
    for (i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (j = 0; j < 8; j++) {
            if ((crc & 0x8000) != 0) {
                crc = (crc << 1) ^ CRC16_POLYNOMIAL;
            } else {
                crc <<= 1;
            }
        }
    }
    
    return crc;
}

/* Calculate_CRC16_WithHeader 函数已删除 - 不再需要 */

/**
*\*\name    Calculate_CRC8.
*\*\fun     Calculate CRC8 for data with register address.
*\*\param   reg_addr - register address
*\*\param   length - data length
*\*\param   data - data pointer
*\*\return  CRC8 value
**/
uint8_t Calculate_CRC8(uint16_t reg_addr, uint8_t length, uint8_t* data)
{
    uint8_t crc = CRC8_INITIAL_VALUE;
    uint16_t i;
    uint8_t j;
    uint8_t header[3];
    
    // Process register address and length first
    header[0] = length;
    header[1] = reg_addr & 0xFF;
    header[2] = (reg_addr & 0xFF00) >> 8;
    
    // Calculate CRC for header
    for (i = 0; i < 3; i++) {
        crc ^= header[i];
        for (j = 0; j < 8; j++) {
            if ((crc & 0x80) != 0) {
                crc = (crc << 1) ^ CRC8_POLYNOMIAL;
            } else {
                crc <<= 1;
            }
        }
    }
    
    // Calculate CRC for data
    for (i = 0; i < length; i++) {
        crc ^= data[i];
        for (j = 0; j < 8; j++) {
            if ((crc & 0x80) != 0) {
                crc = (crc << 1) ^ CRC8_POLYNOMIAL;
            } else {
                crc <<= 1;
            }
        }
    }
    
    return crc;
}



/**
*\*\name    Calculate_CRC32_HW.
*\*\fun     Calculate CRC32 using hardware CRC unit.
*\*\param   reg_addr - register address
*\*\param   length - data length
*\*\param   data - data pointer
*\*\return  CRC32 value
**/
uint32_t Calculate_CRC32_HW(uint16_t reg_addr, uint8_t length, uint8_t* data)
{
    uint8_t dataBuffer[100]; // Ensure sufficient size, max 82 bytes data + 4 bytes header + padding
    uint32_t index = 0;
    uint32_t crc32Value = 0;
    
    /* Organize data according to protocol */
    dataBuffer[index++] = length;                 // Data length
    dataBuffer[index++] = reg_addr & 0xFF;        // Register address low byte
    dataBuffer[index++] = (reg_addr >> 8) & 0xFF; // Register address high byte
    dataBuffer[index++] = 0;                      // Padding byte
    
    /* Copy actual data */
    for (uint8_t i = 0; i < length; i++) {
        dataBuffer[index++] = data[i];
    }
    
    /* Add necessary padding to ensure total length is a multiple of 4 */
    while (index % 4 != 0) {
        dataBuffer[index++] = 0;
    }
    
    /* Enable CRC clock */
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_CRC, ENABLE);
    
    /* Reset CRC calculation unit */
    CRC32_ResetCrc();
    
    /* Calculate CRC32, directly convert byte array to 32-bit word array */
    crc32Value = CRC32_CalcBufCrc((uint32_t*)dataBuffer, index / 4);
    
    return crc32Value;
}

