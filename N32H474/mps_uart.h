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
*\*\file mps_uart.h
*\*\author Nations 
*\*\version v1.0.0 
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#ifndef __MPS_UART_H__
#define __MPS_UART_H__

#include "n32h47x_48x.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_dma.h"
#include "n32h47x_48x_usart.h"
#include "mps_crc.h"

/** UART Error Codes **/
typedef enum
{
    UART_OK = 0,
    UART_ERROR_TIMEOUT,
    UART_ERROR_BUSY,
    UART_ERROR_CONFIG,
    UART_ERROR_BUFFER_OVERFLOW,
    UART_ERROR_CRC,
    UART_ERROR_UNKNOWN
} UART_ErrorCode_t;


/* DMA Remap Definitions (if not defined elsewhere) */
#ifndef DMA_REMAP_UART4_TX
#define DMA_REMAP_UART4_TX       0x00000000U  // Please update with correct value from reference code
#endif

#ifndef DMA_REMAP_UART4_RX
#define DMA_REMAP_UART4_RX       0x00000000U  // Please update with correct value from reference code
#endif
/* Module aliases to match the peripheral naming in the device header */
#define USART1_MODULE             USART1
#define USART2_MODULE             USART2
#define USART3_MODULE             USART3

/* USART/UART Configuration Definitions */
/* USART1 Configuration */
#define USART1_CLK               RCC_APB2_PERIPH_USART1
#define USART1_TX_PIN            GPIO_PIN_9
#define USART1_RX_PIN            GPIO_PIN_10
#define USART1_GPIO              GPIOA
#define USART1_GPIO_CLK          RCC_AHB_PERIPHEN_GPIOA
#define USART1_TX_AF             GPIO_AF5
#define USART1_RX_AF             GPIO_AF5
#define USART1_DAT_BASE          (USART1_BASE + 0x10)
#define USART1_TX_DMA_CH         DMA1_CH4
#define USART1_RX_DMA_CH         DMA1_CH5
#define USART1_TX_DMA_FLAG       DMA_FLAG_TC4
#define USART1_RX_DMA_FLAG       DMA_FLAG_TC5
#define USART1_TX_DMA_REMAP      DMA_REMAP_USART1_TX
#define USART1_RX_DMA_REMAP      DMA_REMAP_USART1_RX
/* USART1_IRQn is already defined in system headers */

/* USART2 Configuration */
#define USART2_CLK               RCC_APB1_PERIPH_USART2
#define USART2_TX_PIN            GPIO_PIN_2
#define USART2_RX_PIN            GPIO_PIN_3
#define USART2_GPIO              GPIOA
#define USART2_GPIO_CLK          RCC_AHB_PERIPHEN_GPIOA
#define USART2_TX_AF             GPIO_AF1
#define USART2_RX_AF             GPIO_AF1
#define USART2_DAT_BASE          (USART2_BASE + 0x10)
#define USART2_TX_DMA_CH         DMA1_CH7
#define USART2_RX_DMA_CH         DMA1_CH6
#define USART2_TX_DMA_FLAG       DMA_FLAG_TC7
#define USART2_RX_DMA_FLAG       DMA_FLAG_TC6
#define USART2_TX_DMA_REMAP      DMA_REMAP_USART2_TX
#define USART2_RX_DMA_REMAP      DMA_REMAP_USART2_RX
/* USART2_IRQn is already defined in system headers */

/* USART3 Configuration */
#define USART3_CLK               RCC_APB1_PERIPH_USART3
#define USART3_TX_PIN            GPIO_PIN_10
#define USART3_RX_PIN            GPIO_PIN_11
#define USART3_GPIO              GPIOC
#define USART3_GPIO_CLK          RCC_AHB_PERIPHEN_GPIOC
#define USART3_TX_AF             GPIO_AF18
#define USART3_RX_AF             GPIO_AF18
#define USART3_DAT_BASE          (USART3_BASE + 0x10)
#define USART3_TX_DMA_CH         DMA1_CH2
#define USART3_RX_DMA_CH         DMA1_CH3
#define USART3_TX_DMA_FLAG       DMA_FLAG_TC2
#define USART3_RX_DMA_FLAG       DMA_FLAG_TC3
#define USART3_TX_DMA_REMAP      DMA_REMAP_USART3_TX
#define USART3_RX_DMA_REMAP      DMA_REMAP_USART3_RX
/* USART3_IRQn is already defined in system headers */
/** DMA with Interrupt Function Declarations **/
int uart_dma_send_data(USART_Module* USARTx, DMA_ChannelType* txDmaChannel, uint8_t* data, uint16_t size);
int uart_dma_receive_data(USART_Module* USARTx, DMA_ChannelType* rxDmaChannel, uint8_t* data, uint16_t size);
int uart_dma_interrupt_init(USART_Module* USARTx);



#endif /* __MPS_UART_H__ */ 