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
*\*\file mps_spi.h
*\*\author Nations 
*\*\version v1.0.0 
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#ifndef __MPS_SPI_H__
#define __MPS_SPI_H__

#include "n32h47x_48x.h"
#include "n32h47x_48x_spi.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_rcc.h"

/** SPI1 Configuration **/
#define SPI1_PORT               SPI1
#define SPI1_RCC                RCC_APB2_PERIPH_SPI1
#define SPI1_SCK_PIN            GPIO_PIN_5
#define SPI1_MOSI_PIN           GPIO_PIN_7
#define SPI1_MISO_PIN           GPIO_PIN_6
#define SPI1_CS_PIN             GPIO_PIN_4
#define SPI1_SCK_GPIO           GPIOA
#define SPI1_MOSI_GPIO          GPIOA
#define SPI1_MISO_GPIO          GPIOA
#define SPI1_CS_GPIO            GPIOA
#define SPI1_SCK_AF             GPIO_AF1
#define SPI1_MOSI_AF            GPIO_AF1
#define SPI1_MISO_AF            GPIO_AF1
#define SPI1_CLK_EN             RCC_AHB_PERIPHEN_GPIOA

/** SPI2 Configuration **/
#define SPI2_PORT               SPI2
#define SPI2_RCC                RCC_APB1_PERIPH_SPI2
#define SPI2_SCK_PIN            GPIO_PIN_7
#define SPI2_MOSI_PIN           GPIO_PIN_2
#define SPI2_MISO_PIN           GPIO_PIN_2
#define SPI2_CS_PIN             GPIO_PIN_12
#define SPI2_SCK_GPIO           GPIOC
#define SPI2_MOSI_GPIO          GPIOD
#define SPI2_MISO_GPIO          GPIOC
#define SPI2_CS_GPIO            GPIOB
#define SPI2_SCK_AF             GPIO_AF13
#define SPI2_MOSI_AF            GPIO_AF11
#define SPI2_MISO_AF            GPIO_AF1
#define SPI2_CLK_EN             (RCC_AHB_PERIPHEN_GPIOB|RCC_AHB_PERIPHEN_GPIOC|RCC_AHB_PERIPHEN_GPIOD)

/** SPI3 Configuration **/
#define SPI3_PORT               SPI3
#define SPI3_RCC                RCC_APB1_PERIPH_SPI3
#define SPI3_SCK_PIN            GPIO_PIN_3
#define SPI3_MOSI_PIN           GPIO_PIN_1
#define SPI3_MISO_PIN           GPIO_PIN_11
#define SPI3_CS_PIN             GPIO_PIN_4
#define SPI3_SCK_GPIO           GPIOC
#define SPI3_MOSI_GPIO          GPIOC
#define SPI3_MISO_GPIO          GPIOD
#define SPI3_CS_GPIO            GPIOC
#define SPI3_SCK_AF             GPIO_AF2
#define SPI3_MOSI_AF            GPIO_AF1
#define SPI3_MISO_AF            GPIO_AF1
#define SPI3_CLK_EN             (RCC_AHB_PERIPHEN_GPIOC|RCC_AHB_PERIPHEN_GPIOD)

/** SPI4 Configuration **/
#define SPI4_PORT               SPI4
#define SPI4_RCC                RCC_APB2_PERIPH_SPI4
#define SPI4_SCK_PIN            GPIO_PIN_2
#define SPI4_MOSI_PIN           GPIO_PIN_6
#define SPI4_MISO_PIN           GPIO_PIN_5
#define SPI4_CS_PIN             GPIO_PIN_4
#define SPI4_SCK_GPIO           GPIOE
#define SPI4_MOSI_GPIO          GPIOE
#define SPI4_MISO_GPIO          GPIOE
#define SPI4_CS_GPIO            GPIOE
#define SPI4_SCK_AF             GPIO_AF1
#define SPI4_MOSI_AF            GPIO_AF1
#define SPI4_MISO_AF            GPIO_AF1
#define SPI4_CLK_EN             RCC_AHB_PERIPHEN_GPIOE

/** SPI5 Configuration **/
#define SPI5_PORT               SPI5
#define SPI5_RCC                RCC_APB2_PERIPH_SPI5
#define SPI5_SCK_PIN            GPIO_PIN_7
#define SPI5_MOSI_PIN           GPIO_PIN_9
#define SPI5_MISO_PIN           GPIO_PIN_8
#define SPI5_CS_PIN             GPIO_PIN_6
#define SPI5_SCK_GPIO           GPIOF
#define SPI5_MOSI_GPIO          GPIOF
#define SPI5_MISO_GPIO          GPIOF
#define SPI5_CS_GPIO            GPIOF
#define SPI5_SCK_AF             GPIO_AF11
#define SPI5_MOSI_AF            GPIO_AF11
#define SPI5_MISO_AF            GPIO_AF11
#define SPI5_CLK_EN             RCC_AHB_PERIPHEN_GPIOF



/**
 * N32h47x SPI wrapper functions
 */
void N32h47x_SPI_TransmitData(SPI_Module* SPIx, uint16_t Data);
uint16_t N32h47x_SPI_ReceiveData(SPI_Module* SPIx);
FlagStatus N32h47x_SPI_GetStatus(SPI_Module* SPIx, uint16_t SPI_I2S_FLAG);
void N32h47x_SPI_Init(SPI_Module* SPIx, SPI_InitType* SPI_InitStruct);
void N32h47x_SPI_InitStruct(SPI_InitType* SPI_InitStruct);
void N32h47x_SPI_Enable(SPI_Module* SPIx, FunctionalState Cmd);
void N32h47x_SPI_EnableCrc(SPI_Module* SPIx, FunctionalState Cmd);

/**
 * SPI initialization and control functions
 */
int spi_master_init(SPI_Module* SPIx);
void spi_cs_low(SPI_Module* SPIx);
void spi_cs_high(SPI_Module* SPIx);

/**
 * SPI data read/write functions
 */
uint8_t SPI_Read(SPI_Module* SPIx, uint8_t Address, uint8_t Register, uint8_t* DataL, uint8_t* DataH, uint8_t usecrc);
uint8_t SPI_Write(SPI_Module* SPIx, uint8_t Address, uint8_t Register, uint8_t DataL, uint8_t DataH, uint8_t usecrc);

/** SPI CRC Configuration **/
#define SPI_CRC_POLYNOMIAL      7  /* Hardware CRC-7 polynomial */
#define SPI_CRC_ENABLE          1  /* Enable SPI hardware CRC */

#endif /* __MPS_SPI_H__ */ 