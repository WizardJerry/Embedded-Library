#include "mps_spi.h"
#include "n32h47x_48x_spi.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_rcc.h"

#include "mps_crc.h"   /* For Calculate_CRC32 and Update_CRC32 functions */
#include <stddef.h>    /* For NULL definition */


/**
*\*\name    N32h47x_SPI_EnableCrc.
*\*\fun     Enable or disable hardware CRC for SPI peripheral
*\*\param   SPIx - SPI module (SPI1, SPI2, SPI3, SPI4, SPI5)
*\*\param   Cmd - Enable or disable (ENABLE or DISABLE)
*\*\return  none
**/
void N32h47x_SPI_EnableCrc(SPI_Module* SPIx, FunctionalState Cmd)
{
  /* Enable or disable CRC calculation */
  SPI_EnableCalculateCrc(SPIx, Cmd);
}

/**
*\*\name    N32h47x_SPI_TransmitData.
*\*\fun     Wrapper function for SPI_I2S_TransmitData
*\*\param   SPIx - SPI module (SPI1, SPI2, SPI3, SPI4, SPI5)
*\*\param   Data - Data to transmit
*\*\return  none
**/
void N32h47x_SPI_TransmitData(SPI_Module* SPIx, uint16_t Data)
{
  /* Call original library function */
  SPI_I2S_TransmitData(SPIx, Data);
}

/**
*\*\name    N32h47x_SPI_ReceiveData.
*\*\fun     Wrapper function for SPI_I2S_ReceiveData
*\*\param   SPIx - SPI module (SPI1, SPI2, SPI3, SPI4, SPI5)
*\*\return  Received data
**/
uint16_t N32h47x_SPI_ReceiveData(SPI_Module* SPIx)
{
  /* Call original library function */
  return SPI_I2S_ReceiveData(SPIx);
}

/**
*\*\name    N32h47x_SPI_GetStatus.
*\*\fun     Wrapper function for SPI_I2S_GetStatus
*\*\param   SPIx - SPI module (SPI1, SPI2, SPI3, SPI4, SPI5)
*\*\param   SPI_I2S_FLAG - Flag to check
*\*\return  Flag status (SET or RESET)
**/
FlagStatus N32h47x_SPI_GetStatus(SPI_Module* SPIx, uint16_t SPI_I2S_FLAG)
{
  /* Call original library function */
  return SPI_I2S_GetStatus(SPIx, SPI_I2S_FLAG);
}

/**
*\*\name    N32h47x_SPI_Init.
*\*\fun     Wrapper function for SPI_Init
*\*\param   SPIx - SPI module (SPI1, SPI2, SPI3, SPI4, SPI5)
*\*\param   SPI_InitStruct - Pointer to SPI initialization structure
*\*\return  none
**/
void N32h47x_SPI_Init(SPI_Module* SPIx, SPI_InitType* SPI_InitStruct)
{
  /* Call original library function */
  SPI_Init(SPIx, SPI_InitStruct);
}

/**
*\*\name    N32h47x_SPI_InitStruct.
*\*\fun     Wrapper function for SPI_InitStruct
*\*\param   pInitStruct - Pointer to SPI initialization structure
*\*\return  none
**/
void N32h47x_SPI_InitStruct(SPI_InitType* pInitStruct)
{
  /* Call original library function */
  SPI_InitStruct(pInitStruct);
}

/**
*\*\name    N32h47x_SPI_Enable.
*\*\fun     Wrapper function for SPI_Enable
*\*\param   SPIx - SPI module (SPI1, SPI2, SPI3, SPI4, SPI5)
*\*\param   Cmd - Enable or disable (ENABLE or DISABLE)
*\*\return  none
**/
void N32h47x_SPI_Enable(SPI_Module* SPIx, FunctionalState Cmd)
{
  /* Call original library function */
  SPI_Enable(SPIx, Cmd);
}

/**
*\*\name    SPI_Read.
*\*\fun     Read data from SPI device with optional hardware CRC
*\*\param   SPIx - SPI module (SPI1, SPI2, SPI3, SPI4, SPI5)
*\*\param   Address - Device address
*\*\param   Register - Register address
*\*\param   DataL - Low byte data pointer
*\*\param   DataH - High byte data pointer (can be NULL if only low byte is needed)
*\*\param   usecrc - Whether to use hardware CRC (1: use, 0: don't use)
*\*\return  Result code (0: success, 3: CRC error)
**/
uint8_t SPI_Read(SPI_Module* SPIx, uint8_t Address, uint8_t Register, uint8_t* DataL, uint8_t* DataH, uint8_t usecrc)
{
  uint8_t result = 0;
  
  /* Check parameters */
  if (DataL == NULL)
  return 1; // Error: Invalid parameter
  
  /* Use chip select for corresponding SPI device */
  spi_cs_low(SPIx);
  
  /* Enable/disable CRC calculation if needed */
  if (usecrc) {
  SPI_EnableCalculateCrc(SPIx, ENABLE);
  } else {
  SPI_EnableCalculateCrc(SPIx, DISABLE);
  }
  
  /* Send address and register */
  N32h47x_SPI_TransmitData(SPIx, Address);
  while (SPI_I2S_GetStatus(SPIx, SPI_I2S_TE_FLAG) == RESET); // Wait for transmission complete
  
  N32h47x_SPI_TransmitData(SPIx, Register);
  while (SPI_I2S_GetStatus(SPIx, SPI_I2S_TE_FLAG) == RESET); // Wait for transmission complete
  
  /* Receive low byte data */
  N32h47x_SPI_TransmitData(SPIx, 0xFF); // Send dummy data to receive data
  while (SPI_I2S_GetStatus(SPIx, SPI_I2S_RNE_FLAG) == RESET); // Wait for reception complete
  *DataL = N32h47x_SPI_ReceiveData(SPIx);
  
  /* If high byte is needed */
  if (DataH != NULL) {
  N32h47x_SPI_TransmitData(SPIx, 0xFF); // Send dummy data to receive data
  while (SPI_I2S_GetStatus(SPIx, SPI_I2S_RNE_FLAG) == RESET); // Wait for reception complete
  *DataH = N32h47x_SPI_ReceiveData(SPIx);
  }
  
  /* If using CRC, check CRC error flag */
  if (usecrc) {
  /* Wait for the next CRC transmission/reception */
  N32h47x_SPI_TransmitData(SPIx, 0xFF); // Send dummy data to trigger CRC
  while (SPI_I2S_GetStatus(SPIx, SPI_I2S_RNE_FLAG) == RESET); // Wait for CRC reception
  N32h47x_SPI_ReceiveData(SPIx); // Read CRC value (discard)
  
  /* Check CRC error flag */
  if (SPI_I2S_GetStatus(SPIx, SPI_CRCERR_FLAG) == SET) {
  result = 3; // CRC error
  }
  
  /* Disable CRC calculation */
  SPI_EnableCalculateCrc(SPIx, DISABLE);
  }
  
  /* Deselect chip */
  spi_cs_high(SPIx);
  
  return result;
}

/**
*\*\name    SPI_Write.
*\*\fun     Write data to SPI device with optional hardware CRC
*\*\param   SPIx - SPI module (SPI1, SPI2, SPI3, SPI4, SPI5)
*\*\param   Address - Device address
*\*\param   Register - Register address
*\*\param   DataL - Low byte data
*\*\param   DataH - High byte data (optional, pass 0 to send only DataL)
*\*\param   usecrc - Whether to use hardware CRC (1: use, 0: don't use)
*\*\return  Result code (0: success)
**/
uint8_t SPI_Write(SPI_Module* SPIx, uint8_t Address, uint8_t Register, uint8_t DataL, uint8_t DataH, uint8_t usecrc)
{
  uint8_t result = 0;
  uint8_t has_high_byte = (DataH != 0);  // Whether high byte data exists
  
  /* Use chip select for corresponding SPI device */
  spi_cs_low(SPIx);
  
  /* Enable/disable CRC calculation if needed */
  if (usecrc) {
  SPI_EnableCalculateCrc(SPIx, ENABLE);
  } else {
  SPI_EnableCalculateCrc(SPIx, DISABLE);
  }
  
  /* Send address and register */
  N32h47x_SPI_TransmitData(SPIx, Address);
  while (SPI_I2S_GetStatus(SPIx, SPI_I2S_TE_FLAG) == RESET); // Wait for transmission complete
  while (SPI_I2S_GetStatus(SPIx, SPI_I2S_RNE_FLAG) == RESET); // Wait for reception complete (discard received data)
  N32h47x_SPI_ReceiveData(SPIx); // Read and discard received data
  
  N32h47x_SPI_TransmitData(SPIx, Register);
  while (SPI_I2S_GetStatus(SPIx, SPI_I2S_TE_FLAG) == RESET); // Wait for transmission complete
  while (SPI_I2S_GetStatus(SPIx, SPI_I2S_RNE_FLAG) == RESET); // Wait for reception complete (discard received data)
  N32h47x_SPI_ReceiveData(SPIx); // Read and discard received data
  
  /* Send low byte data */
  N32h47x_SPI_TransmitData(SPIx, DataL);
  while (SPI_I2S_GetStatus(SPIx, SPI_I2S_TE_FLAG) == RESET); // Wait for transmission complete
  while (SPI_I2S_GetStatus(SPIx, SPI_I2S_RNE_FLAG) == RESET); // Wait for reception complete (discard received data)
  N32h47x_SPI_ReceiveData(SPIx); // Read and discard received data
  
  /* If high byte needs to be sent */
  if (has_high_byte) {
  N32h47x_SPI_TransmitData(SPIx, DataH);
  while (SPI_I2S_GetStatus(SPIx, SPI_I2S_TE_FLAG) == RESET); // Wait for transmission complete
  while (SPI_I2S_GetStatus(SPIx, SPI_I2S_RNE_FLAG) == RESET); // Wait for reception complete (discard received data)
  N32h47x_SPI_ReceiveData(SPIx); // Read and discard received data
  }
  
  /* If using CRC, send and verify CRC */
  if (usecrc) {
  /* CRC value will be automatically transmitted after the last data */
  /* Wait for CRC transmission */
  while (SPI_I2S_GetStatus(SPIx, SPI_I2S_TE_FLAG) == RESET); // Wait for CRC transmission complete
  while (SPI_I2S_GetStatus(SPIx, SPI_I2S_RNE_FLAG) == RESET); // Wait for reception complete (discard received data)
  N32h47x_SPI_ReceiveData(SPIx); // Read and discard received CRC
  
  /* Disable CRC calculation */
  SPI_EnableCalculateCrc(SPIx, DISABLE);
  }
  
  /* Deselect chip */
  spi_cs_high(SPIx);
  
  return result;
}


/**
*\*\name    spi_master_init.
*\*\fun     Initialize SPI peripheral in master mode.
*\*\param   SPIx - SPI module (SPI1, SPI2, SPI3, SPI4, SPI5)
*\*\return  0 if successful, error code otherwise 
**/
int spi_master_init(SPI_Module* SPIx)
{
  GPIO_InitType GPIO_InitStructure;
  SPI_InitType SPI_InitStructure;

  /* Select configuration based on SPI interface */
  if (SPIx == SPI1)
  {
  /* Enable SPI clock */
  RCC_EnableAPB2PeriphClk(SPI1_RCC, ENABLE);

  /* Enable GPIO clock */
  RCC_EnableAHB1PeriphClk(SPI1_CLK_EN, ENABLE);
  
  /* Enable AFIO clock */
  RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);

  /* Initialize GPIO_InitStructure */
  GPIO_InitStruct(&GPIO_InitStructure);

  /* Configure SPI pins: SCK */
  GPIO_InitStructure.Pin        = SPI1_SCK_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
  GPIO_InitStructure.GPIO_Pull  = GPIO_NO_PULL;
  GPIO_InitStructure.GPIO_Alternate = SPI1_SCK_AF;
  GPIO_InitPeripheral(SPI1_SCK_GPIO, &GPIO_InitStructure);

  /* Configure SPI pins: MOSI */
  GPIO_InitStructure.Pin        = SPI1_MOSI_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
  GPIO_InitStructure.GPIO_Pull  = GPIO_NO_PULL;
  GPIO_InitStructure.GPIO_Alternate = SPI1_MOSI_AF;
  GPIO_InitPeripheral(SPI1_MOSI_GPIO, &GPIO_InitStructure);

  /* Configure SPI pins: MISO */
  GPIO_InitStructure.Pin        = SPI1_MISO_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_INPUT;
  GPIO_InitStructure.GPIO_Pull  = GPIO_NO_PULL;
  GPIO_InitStructure.GPIO_Alternate = SPI1_MISO_AF;
  GPIO_InitPeripheral(SPI1_MISO_GPIO, &GPIO_InitStructure);

  /* Configure SPI pins: CS */
  GPIO_InitStructure.Pin        = SPI1_CS_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.GPIO_Pull  = GPIO_PULL_UP;
  GPIO_InitPeripheral(SPI1_CS_GPIO, &GPIO_InitStructure);

  /* Set CS high (inactive) */
  GPIO_SetBits(SPI1_CS_GPIO, SPI1_CS_PIN);
  }
  else if (SPIx == SPI2)
  {
  /* Enable SPI clock */
  RCC_EnableAPB1PeriphClk(SPI2_RCC, ENABLE);

  /* Enable GPIO clock */
  RCC_EnableAHB1PeriphClk(SPI2_CLK_EN, ENABLE);
  
  /* Enable AFIO clock */
  RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);

  /* Initialize GPIO_InitStructure */
  GPIO_InitStruct(&GPIO_InitStructure);

  /* Configure SPI pins: SCK */
  GPIO_InitStructure.Pin        = SPI2_SCK_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
  GPIO_InitStructure.GPIO_Pull  = GPIO_NO_PULL;
  GPIO_InitStructure.GPIO_Alternate = SPI2_SCK_AF;
  GPIO_InitPeripheral(SPI2_SCK_GPIO, &GPIO_InitStructure);

  /* Configure SPI pins: MOSI */
  GPIO_InitStructure.Pin        = SPI2_MOSI_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
  GPIO_InitStructure.GPIO_Pull  = GPIO_NO_PULL;
  GPIO_InitStructure.GPIO_Alternate = SPI2_MOSI_AF;
  GPIO_InitPeripheral(SPI2_MOSI_GPIO, &GPIO_InitStructure);

  /* Configure SPI pins: MISO */
  GPIO_InitStructure.Pin        = SPI2_MISO_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_INPUT;
  GPIO_InitStructure.GPIO_Pull  = GPIO_NO_PULL;
  GPIO_InitStructure.GPIO_Alternate = SPI2_MISO_AF;
  GPIO_InitPeripheral(SPI2_MISO_GPIO, &GPIO_InitStructure);

  /* Configure SPI pins: CS */
  GPIO_InitStructure.Pin        = SPI2_CS_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.GPIO_Pull  = GPIO_PULL_UP;
  GPIO_InitPeripheral(SPI2_CS_GPIO, &GPIO_InitStructure);

  /* Set CS high (inactive) */
  GPIO_SetBits(SPI2_CS_GPIO, SPI2_CS_PIN);
  }
  else if (SPIx == SPI3)
  {
  /* Enable SPI clock */
  RCC_EnableAPB1PeriphClk(SPI3_RCC, ENABLE);

  /* Enable GPIO clock */
  RCC_EnableAHB1PeriphClk(SPI3_CLK_EN, ENABLE);
  
  /* Enable AFIO clock */
  RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);

  /* Initialize GPIO_InitStructure */
  GPIO_InitStruct(&GPIO_InitStructure);

  /* Configure SPI pins: SCK */
  GPIO_InitStructure.Pin        = SPI3_SCK_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
  GPIO_InitStructure.GPIO_Pull  = GPIO_NO_PULL;
  GPIO_InitStructure.GPIO_Alternate = SPI3_SCK_AF;
  GPIO_InitPeripheral(SPI3_SCK_GPIO, &GPIO_InitStructure);

  /* Configure SPI pins: MOSI */
  GPIO_InitStructure.Pin        = SPI3_MOSI_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
  GPIO_InitStructure.GPIO_Pull  = GPIO_NO_PULL;
  GPIO_InitStructure.GPIO_Alternate = SPI3_MOSI_AF;
  GPIO_InitPeripheral(SPI3_MOSI_GPIO, &GPIO_InitStructure);

  /* Configure SPI pins: MISO */
  GPIO_InitStructure.Pin        = SPI3_MISO_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_INPUT;
  GPIO_InitStructure.GPIO_Pull  = GPIO_NO_PULL;
  GPIO_InitStructure.GPIO_Alternate = SPI3_MISO_AF;
  GPIO_InitPeripheral(SPI3_MISO_GPIO, &GPIO_InitStructure);

  /* Configure SPI pins: CS */
  GPIO_InitStructure.Pin        = SPI3_CS_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.GPIO_Pull  = GPIO_PULL_UP;
  GPIO_InitPeripheral(SPI3_CS_GPIO, &GPIO_InitStructure);

  /* Set CS high (inactive) */
  GPIO_SetBits(SPI3_CS_GPIO, SPI3_CS_PIN);
  }
  else if (SPIx == SPI4)
  {
  /* Enable SPI clock */
  RCC_EnableAPB2PeriphClk(SPI4_RCC, ENABLE);

  /* Enable GPIO clock */
  RCC_EnableAHB1PeriphClk(SPI4_CLK_EN, ENABLE);
  
  /* Enable AFIO clock */
  RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);

  /* Initialize GPIO_InitStructure */
  GPIO_InitStruct(&GPIO_InitStructure);

  /* Configure SPI pins: SCK */
  GPIO_InitStructure.Pin        = SPI4_SCK_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
  GPIO_InitStructure.GPIO_Pull  = GPIO_NO_PULL;
  GPIO_InitStructure.GPIO_Alternate = SPI4_SCK_AF;
  GPIO_InitPeripheral(SPI4_SCK_GPIO, &GPIO_InitStructure);

  /* Configure SPI pins: MOSI */
  GPIO_InitStructure.Pin        = SPI4_MOSI_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
  GPIO_InitStructure.GPIO_Pull  = GPIO_NO_PULL;
  GPIO_InitStructure.GPIO_Alternate = SPI4_MOSI_AF;
  GPIO_InitPeripheral(SPI4_MOSI_GPIO, &GPIO_InitStructure);

  /* Configure SPI pins: MISO */
  GPIO_InitStructure.Pin        = SPI4_MISO_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_INPUT;
  GPIO_InitStructure.GPIO_Pull  = GPIO_NO_PULL;
  GPIO_InitStructure.GPIO_Alternate = SPI4_MISO_AF;
  GPIO_InitPeripheral(SPI4_MISO_GPIO, &GPIO_InitStructure);

  /* Configure SPI pins: CS */
  GPIO_InitStructure.Pin        = SPI4_CS_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.GPIO_Pull  = GPIO_PULL_UP;
  GPIO_InitPeripheral(SPI4_CS_GPIO, &GPIO_InitStructure);

  /* Set CS high (inactive) */
  GPIO_SetBits(SPI4_CS_GPIO, SPI4_CS_PIN);
  }
  else if (SPIx == SPI5)
  {
  /* Enable SPI clock */
  RCC_EnableAPB2PeriphClk(SPI5_RCC, ENABLE);

  /* Enable GPIO clock */
  RCC_EnableAHB1PeriphClk(SPI5_CLK_EN, ENABLE);
  
  /* Enable AFIO clock */
  RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);

  /* Initialize GPIO_InitStructure */
  GPIO_InitStruct(&GPIO_InitStructure);

  /* Configure SPI pins: SCK */
  GPIO_InitStructure.Pin        = SPI5_SCK_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
  GPIO_InitStructure.GPIO_Pull  = GPIO_NO_PULL;
  GPIO_InitStructure.GPIO_Alternate = SPI5_SCK_AF;
  GPIO_InitPeripheral(SPI5_SCK_GPIO, &GPIO_InitStructure);

  /* Configure SPI pins: MOSI */
  GPIO_InitStructure.Pin        = SPI5_MOSI_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_AF_PP;
  GPIO_InitStructure.GPIO_Pull  = GPIO_NO_PULL;
  GPIO_InitStructure.GPIO_Alternate = SPI5_MOSI_AF;
  GPIO_InitPeripheral(SPI5_MOSI_GPIO, &GPIO_InitStructure);

  /* Configure SPI pins: MISO */
  GPIO_InitStructure.Pin        = SPI5_MISO_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_INPUT;
  GPIO_InitStructure.GPIO_Pull  = GPIO_NO_PULL;
  GPIO_InitStructure.GPIO_Alternate = SPI5_MISO_AF;
  GPIO_InitPeripheral(SPI5_MISO_GPIO, &GPIO_InitStructure);

  /* Configure SPI pins: CS */
  GPIO_InitStructure.Pin        = SPI5_CS_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.GPIO_Pull  = GPIO_PULL_UP;
  GPIO_InitPeripheral(SPI5_CS_GPIO, &GPIO_InitStructure);

  /* Set CS high (inactive) */
  GPIO_SetBits(SPI5_CS_GPIO, SPI5_CS_PIN);
  }
  else
  {
  return 0; /* Invalid SPI peripheral */
  }

  /* SPI configuration */
  SPI_InitStruct(&SPI_InitStructure);
  SPI_InitStructure.DataDirection = SPI_DIR_DOUBLELINE_FULLDUPLEX;
  SPI_InitStructure.SpiMode       = SPI_MODE_MASTER;
  SPI_InitStructure.DataLen       = SPI_DATA_SIZE_8BITS;
  SPI_InitStructure.CLKPOL        = SPI_CLKPOL_HIGH;
  SPI_InitStructure.CLKPHA        = SPI_CLKPHA_SECOND_EDGE;
  SPI_InitStructure.NSS           = SPI_NSS_SOFT;
  SPI_InitStructure.BaudRatePres  = SPI_BR_PRESCALER_32;
  SPI_InitStructure.FirstBit      = SPI_FB_MSB;
  SPI_InitStructure.CRCPoly       = 7;
  SPI_Init(SPIx, &SPI_InitStructure);

  /* Enable SPI */
  SPI_Enable(SPIx, ENABLE);
  
  /* Enable SPI CRC calculation */
  N32h47x_SPI_EnableCrc(SPIx, ENABLE);

  return 1;
}


/**
*\*\name    spi_cs_low.
*\*\fun     Set SPI chip select line low (active).
*\*\param   SPIx - SPI module (SPI1, SPI2, SPI3, SPI4, SPI5)
*\*\return  none
**/
void spi_cs_low(SPI_Module* SPIx)
{
  if (SPIx == SPI1)
  {
  GPIO_ResetBits(SPI1_CS_GPIO, SPI1_CS_PIN);
  }
  else if (SPIx == SPI2)
  {
  GPIO_ResetBits(SPI2_CS_GPIO, SPI2_CS_PIN);
  }
  else if (SPIx == SPI3)
  {
  GPIO_ResetBits(SPI3_CS_GPIO, SPI3_CS_PIN);
  }
  else if (SPIx == SPI4)
  {
  GPIO_ResetBits(SPI4_CS_GPIO, SPI4_CS_PIN);
  }
  else if (SPIx == SPI5)
  {
  GPIO_ResetBits(SPI5_CS_GPIO, SPI5_CS_PIN);
  }
}

/**
*\*\name    spi_cs_high.
*\*\fun     Set SPI chip select line high (inactive).
*\*\param   SPIx - SPI module (SPI1, SPI2, SPI3, SPI4, SPI5)
*\*\return  none
**/
void spi_cs_high(SPI_Module* SPIx)
{
  if (SPIx == SPI1)
  {
  GPIO_SetBits(SPI1_CS_GPIO, SPI1_CS_PIN);
  }
  else if (SPIx == SPI2)
  {
  GPIO_SetBits(SPI2_CS_GPIO, SPI2_CS_PIN);
  }
  else if (SPIx == SPI3)
  {
  GPIO_SetBits(SPI3_CS_GPIO, SPI3_CS_PIN);
  }
  else if (SPIx == SPI4)
  {
  GPIO_SetBits(SPI4_CS_GPIO, SPI4_CS_PIN);
  }
  else if (SPIx == SPI5)
  {
  GPIO_SetBits(SPI5_CS_GPIO, SPI5_CS_PIN);
  }
}