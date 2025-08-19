#include "mps_uart.h"
#include "n32h47x_48x_usart.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_dma.h"
#include "misc.h"
#include <stddef.h>    /* For NULL definition */

/* Buffer size definition (should match the one in main.c) */
#define UART_BUFFER_SIZE 256

/* External buffer and variables from main.c */
extern uint8_t rxBuffer[];
extern volatile uint16_t rxCount;

/**
 * Send data using UART with DMA
 * @param USARTx - UART peripheral to use
 * @param txDmaChannel - DMA channel for TX
 * @param data - Data buffer to send
 * @param size - Size of data to send
 * @return 1 if successful, 0 if failed
 */
int uart_dma_send_data(USART_Module* USARTx, DMA_ChannelType* txDmaChannel, uint8_t* data, uint16_t size)
{
  /* Check parameters */
  if (data == NULL || size == 0)
  {
  return 0;
  }
  
  /* Wait until DMA channel is free */
  while (DMA_GetCurrDataCounter(txDmaChannel) != 0)
  {
  /* Wait for the current transfer to complete */
  }
  
  /* Configure DMA Channel buffer size and memory address */
  txDmaChannel->TXNUM = size;
  txDmaChannel->MADDR = (uint32_t)data;
  
  /* Enable DMA Channel */
  DMA_EnableChannel(txDmaChannel, ENABLE);
  
  return 1;
}

/**
 * Configure UART to receive data with DMA
 * @param USARTx - UART peripheral to use
 * @param rxDmaChannel - DMA channel for RX
 * @param data - Data buffer for received data
 * @param size - Size of data buffer
 * @return 1 if successful, 0 if failed
 */
int uart_dma_receive_data(USART_Module* USARTx, DMA_ChannelType* rxDmaChannel, uint8_t* data, uint16_t size)
{
  /* Check parameters */
  if (data == NULL || size == 0)
  {
  return 0;
  }
  
  /* Wait until DMA channel is free */
  while (DMA_GetCurrDataCounter(rxDmaChannel) != 0)
  {
  /* Wait for the current transfer to complete */
  }
  
  /* Configure DMA Channel buffer size and memory address */
  rxDmaChannel->TXNUM = size;
  rxDmaChannel->MADDR = (uint32_t)data;
  
  /* Enable DMA Channel */
  DMA_EnableChannel(rxDmaChannel, ENABLE);
  
  return 1;
}



/**
 * UART DMA interrupt initialization function
 * @param USARTx - UART peripheral to use (USART1, USART2, USART3)
 * @return 1 if successful, 0 if failed
 */
 int uart_dma_interrupt_init(USART_Module* USARTx)
 {
   USART_InitType USART_InitStructure;
   GPIO_InitType GPIO_InitStructure;
   DMA_InitType DMA_InitStructure;
   NVIC_InitType NVIC_InitStructure;
   
   GPIO_Module* UART_GPIO;
   uint32_t UART_CLK;
   uint32_t UART_GPIO_CLK;
   uint16_t UART_TX_PIN;
   uint16_t UART_RX_PIN;
   uint8_t UART_TX_AF;
   uint8_t UART_RX_AF;
   DMA_ChannelType* UART_TX_DMA_Channel;
   DMA_ChannelType* UART_RX_DMA_Channel;
   uint32_t UART_TX_DMA_FLAG;
   uint32_t UART_RX_DMA_FLAG;
   uint32_t UART_TX_DMA_REMAP;
   uint32_t UART_RX_DMA_REMAP;
   uint8_t UART_IRQn;
   uint32_t UART_DAT_Base;
   
   /* Select UART peripheral configuration based on the USARTx parameter */
   if (USARTx == USART1)
   {
   UART_GPIO = USART1_GPIO;
   UART_CLK = USART1_CLK;
   UART_GPIO_CLK = USART1_GPIO_CLK;
   UART_TX_PIN = USART1_TX_PIN;
   UART_RX_PIN = USART1_RX_PIN;
   UART_TX_AF = USART1_TX_AF;
   UART_RX_AF = USART1_RX_AF;
   UART_TX_DMA_Channel = USART1_TX_DMA_CH;
   UART_RX_DMA_Channel = USART1_RX_DMA_CH;
   UART_TX_DMA_FLAG = USART1_TX_DMA_FLAG;
   UART_RX_DMA_FLAG = USART1_RX_DMA_FLAG;
   UART_TX_DMA_REMAP = USART1_TX_DMA_REMAP;
   UART_RX_DMA_REMAP = USART1_RX_DMA_REMAP;
   UART_IRQn = USART1_IRQn;
   UART_DAT_Base = USART1_DAT_BASE;
   }
   else if (USARTx == USART2)
   {
   UART_GPIO = USART2_GPIO;
   UART_CLK = USART2_CLK;
   UART_GPIO_CLK = USART2_GPIO_CLK;
   UART_TX_PIN = USART2_TX_PIN;
   UART_RX_PIN = USART2_RX_PIN;
   UART_TX_AF = USART2_TX_AF;
   UART_RX_AF = USART2_RX_AF;
   UART_TX_DMA_Channel = USART2_TX_DMA_CH;
   UART_RX_DMA_Channel = USART2_RX_DMA_CH;
   UART_TX_DMA_FLAG = USART2_TX_DMA_FLAG;
   UART_RX_DMA_FLAG = USART2_RX_DMA_FLAG;
   UART_TX_DMA_REMAP = USART2_TX_DMA_REMAP;
   UART_RX_DMA_REMAP = USART2_RX_DMA_REMAP;
   UART_IRQn = USART2_IRQn;
   UART_DAT_Base = USART2_DAT_BASE;
   }
   else if (USARTx == USART3)
   {
   UART_GPIO = USART3_GPIO;
   UART_CLK = USART3_CLK;
   UART_GPIO_CLK = USART3_GPIO_CLK;
   UART_TX_PIN = USART3_TX_PIN;
   UART_RX_PIN = USART3_RX_PIN;
   UART_TX_AF = USART3_TX_AF;
   UART_RX_AF = USART3_RX_AF;
   UART_TX_DMA_Channel = USART3_TX_DMA_CH;
   UART_RX_DMA_Channel = USART3_RX_DMA_CH;
   UART_TX_DMA_FLAG = USART3_TX_DMA_FLAG;
   UART_RX_DMA_FLAG = USART3_RX_DMA_FLAG;
   UART_TX_DMA_REMAP = USART3_TX_DMA_REMAP;
   UART_RX_DMA_REMAP = USART3_RX_DMA_REMAP;
   UART_IRQn = USART3_IRQn;
   UART_DAT_Base = USART3_DAT_BASE;
   }
   else
   {
   return 0; /* Invalid UART peripheral */
   }
   
   /* System Clocks Configuration */
   /* DMA clock enable */
   RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_DMA1, ENABLE);
   
   /* Enable GPIO clock */
   RCC_EnableAHB1PeriphClk(UART_GPIO_CLK, ENABLE);
   RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
   
   /* Enable UART Clock */
  
   RCC_EnableAPB2PeriphClk(UART_CLK, ENABLE);
   RCC_EnableAPB1PeriphClk(UART_CLK, ENABLE);
   
   
   /* Configure GPIO ports */
   /* Initialize GPIO_InitStructure */
   GPIO_InitStruct(&GPIO_InitStructure);
   
   /* Configure UART Tx as alternate function push-pull */
   GPIO_InitStructure.Pin            = UART_TX_PIN;    
   GPIO_InitStructure.GPIO_Mode      = GPIO_MODE_AF_PP;
   GPIO_InitStructure.GPIO_Alternate = UART_TX_AF;
   GPIO_InitStructure.GPIO_Pull      = GPIO_PULL_UP;
   GPIO_InitPeripheral(UART_GPIO, &GPIO_InitStructure);
   
   /* Configure UART Rx as alternate function push-pull */
   GPIO_InitStructure.Pin            = UART_RX_PIN;
   GPIO_InitStructure.GPIO_Alternate = UART_RX_AF;
   GPIO_InitPeripheral(UART_GPIO, &GPIO_InitStructure);
   
   /* NVIC Configuration */
   /* Configure the Priority Grouping */
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
   
   /* Enable the UART Interrupt */
   NVIC_InitStructure.NVIC_IRQChannel                   = UART_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
   
   /* DMA Configuration */
   /* DMA TX Channel Configuration */
   DMA_DeInit(UART_TX_DMA_Channel);
   DMA_StructInit(&DMA_InitStructure);
   DMA_InitStructure.PeriphAddr     = UART_DAT_Base;
   DMA_InitStructure.MemAddr        = 0; /* Will be set when sending data */
   DMA_InitStructure.Direction      = DMA_DIR_PERIPH_DST;
   DMA_InitStructure.BufSize        = 0; /* Will be set when sending data */
   DMA_InitStructure.PeriphInc      = DMA_PERIPH_INC_DISABLE;
   DMA_InitStructure.MemoryInc      = DMA_MEM_INC_ENABLE;
   DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_WIDTH_BYTE;
   DMA_InitStructure.MemDataSize    = DMA_MEM_DATA_WIDTH_BYTE;
   DMA_InitStructure.CircularMode   = DMA_MODE_NORMAL;
   DMA_InitStructure.Priority       = DMA_PRIORITY_HIGH;
   DMA_InitStructure.Mem2Mem        = DMA_M2M_DISABLE;
   DMA_Init(UART_TX_DMA_Channel, &DMA_InitStructure);
   DMA_RequestRemap(UART_TX_DMA_REMAP, UART_TX_DMA_Channel, ENABLE);
   
   /* DMA RX Channel Configuration */
   DMA_DeInit(UART_RX_DMA_Channel);
   DMA_InitStructure.PeriphAddr = UART_DAT_Base;
   DMA_InitStructure.MemAddr    = 0; /* Will be set when receiving data */
   DMA_InitStructure.Direction  = DMA_DIR_PERIPH_SRC;
   DMA_InitStructure.BufSize    = 0; /* Will be set when receiving data */
   DMA_Init(UART_RX_DMA_Channel, &DMA_InitStructure);
   DMA_RequestRemap(UART_RX_DMA_REMAP, UART_RX_DMA_Channel, ENABLE);
   
   /* UART configuration */
   USART_StructInit(&USART_InitStructure);
   USART_InitStructure.BaudRate            = 115200;
   USART_InitStructure.WordLength          = USART_WL_8B;
   USART_InitStructure.StopBits            = USART_STPB_1;
   USART_InitStructure.Parity              = USART_PE_NO;
   USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;
   USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;
   USART_Init(USARTx, &USART_InitStructure);
   
   /* Enable UART DMA Tx and Rx request */
   USART_EnableDMA(USARTx, USART_DMAREQ_TX | USART_DMAREQ_RX, ENABLE);
   
   /* Enable UART Receive Interrupt */
   USART_ConfigInt(USARTx, USART_INT_RXDNE, ENABLE);
   
   /* Enable UART */
   USART_Enable(USARTx, ENABLE);
   
   return 1; /* Initialization successful */
 }
 

