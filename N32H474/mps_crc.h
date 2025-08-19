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
*\*\file mps_crc.h
*\*\author Nations 
*\*\version v1.0.0 
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#ifndef __MPS_CRC_H__
#define __MPS_CRC_H__

#include "n32h47x_48x.h"
#include <stdint.h>

/** CRC Configuration **/
/* CRC-32 Configuration */
#define CRC32_BLOCK_SIZE    4       /* CRC block size for CRC-32 (4 bytes) */
#define CRC32_LENGTH        4       /* CRC-32 result length (4 bytes) */
#define CRC32_INITIAL_VALUE 0xFFFFFFFF  /* CRC-32 initial value */
#define CRC32_POLYNOMIAL    0x04C11DB7  /* CRC-32 polynomial */

/* CRC-16 Configuration */
#define CRC16_BLOCK_SIZE    2       /* CRC block size for CRC-16 (2 bytes) */
#define CRC16_LENGTH        2       /* CRC-16 result length (2 bytes) */
#define CRC16_INITIAL_VALUE 0xFFFF  /* CRC-16 initial value */
#define CRC16_POLYNOMIAL    0x1021  /* CRC-16 polynomial (CCITT) */

/* CRC-8 Configuration */
#define CRC8_BLOCK_SIZE     1       /* CRC block size for CRC-8 (1 byte) */
#define CRC8_LENGTH         1       /* CRC-8 result length (1 byte) */
#define CRC8_INITIAL_VALUE  0xFF    /* CRC-8 initial value */
#define CRC8_POLYNOMIAL     0x07    /* CRC-8 polynomial */

/* CRC Function Declarations */
uint32_t Calculate_CRC32(uint16_t reg_addr, uint8_t length, uint8_t* data);
uint32_t Calculate_CRC32_HW(uint16_t reg_addr, uint8_t length, uint8_t* data);
uint16_t Calculate_CRC16(uint16_t reg_addr, uint8_t length, uint8_t* data);
uint8_t Calculate_CRC8(uint16_t reg_addr, uint8_t length, uint8_t* data);


/* 辅助函数已删除，可以在需要时直接在代码中实现 */

#endif /* __MPS_CRC_H__ */ 