#include "mpf11770_i2c.h"
#include "mps_i2c.h"
#include "mps_crc.h"   /* For Calculate_CRC32 function */
#include "delay.h"
#include "n32h47x_48x_crc.h"  // Include header file for CRC hardware functions
#include "mps_delay.h"  // Header file for systick_delay_ms function
#include <stddef.h>    /* For NULL definition */

/* Define maximum data length */
#define MAX_DATA_LENGTH 82  // Maximum data length supported by MPF11770

/* Function declaration for GetTick */
static uint32_t GetTick(void)
{
  /* Use systick timer to get current tick value */
  return (uint32_t)SysTick->VAL;
}

/* Error codes */
#define I2C_OK             0

/**
* @brief Write data to an MPF11770 device via I2C with optional software CRC32
* @param I2Cx - I2C interface to use
 * @param devAddr - 7-bit device address
 * @param regAddr - 16-bit register address
* @param pData - pointer to data buffer
 * @param length - length of data to write (1-82 bytes as per spec)
 * @param enableCRC - set true to enable CRC32 calculation and verification
* @return 0 if successful, error code otherwise
*/
int32_t MPF11770_I2C_master_write(I2C_Module* I2Cx, uint8_t devAddr, uint16_t regAddr, 
                         uint8_t* pData, uint8_t length, bool enableCRC)
{
  uint32_t crc32Value = 0;
  uint8_t i;
  uint32_t I2CTimeout;
  uint32_t crcData[2 + (MAX_DATA_LENGTH + 3) / 4]; // For reg addr, length, and data (aligned to 32-bit)

  i2c_start(I2Cx, devAddr);

  /* Send low byte of register address */
  I2C_SendData(I2Cx, (uint8_t)(regAddr & 0xFF));
  
  /* Wait for EV8: data byte sent */
  I2CTimeout = I2C_TIMEOUT_FLAG;
  while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_SENDING))
  {
    if ((I2CTimeout--) == 0)
    {
      return 0;
    }
  }

  /* Send high byte of register address */
  I2C_SendData(I2Cx, (uint8_t)(regAddr >> 8));
  
  /* Wait for EV8: data byte sent */
  I2CTimeout = I2C_TIMEOUT_FLAG;
  while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_SENDING))
  {
    if ((I2CTimeout--) == 0)
    {
      return 0;
    }
  }

  /* Send length byte */
  I2C_SendData(I2Cx, length);
  
  /* Wait for EV8: data byte sent */
  I2CTimeout = I2C_TIMEOUT_FLAG;
  while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_SENDING))
  {
    if ((I2CTimeout--) == 0)
    {
      return 0;
    }
  }

  /* Send data bytes */
  for (i = 0; i < length; i++)
  {
    /* Send data byte */
    I2C_SendData(I2Cx, pData[i]);
    
    /* Wait for EV8: data byte sent */
    I2CTimeout = I2C_TIMEOUT_FLAG;
    while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_SENDING))
    {
    if ((I2CTimeout--) == 0)
    {
      return 0;
    }
    }
  }
  
  /* Wait for EV8-2: last data byte sent */
  I2CTimeout = I2C_TIMEOUT_FLAG;
  while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_SENDED))
  {
    if ((I2CTimeout--) == 0)
    {
      return 0;
    }
  }
  
  /* If CRC is not needed, generate STOP condition */
  if (!enableCRC)
  {
    /* Generate STOP condition */
    I2C_GenerateStop(I2Cx, ENABLE);
  }
  /* If CRC is needed, calculate and send CRC32 */
  else
  {
    /* Calculate CRC32 using hardware */
    //   crc32Value = Calculate_CRC32_HW(regAddr, length, pData);
    
    /* Calculate CRC32 using software */
    crc32Value = Calculate_CRC32(regAddr, length, pData);
    
    /* Send CRC bytes (little-endian) */
    I2C_SendData(I2Cx, (uint8_t)(crc32Value & 0xFF));
    I2CTimeout = I2C_TIMEOUT_FLAG;
    while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_SENDING))
    {
        if ((I2CTimeout--) == 0) 
        {
        
          return 0;
        }
    }
    
    I2C_SendData(I2Cx, (uint8_t)((crc32Value >> 8) & 0xFF));
    I2CTimeout = I2C_TIMEOUT_FLAG;
    while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_SENDING))
    {
        if ((I2CTimeout--) == 0) 
        {
        
          return 0;
        }
    }
    
    I2C_SendData(I2Cx, (uint8_t)((crc32Value >> 16) & 0xFF));
    I2CTimeout = I2C_TIMEOUT_FLAG;
    while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_SENDING))
    {
        if ((I2CTimeout--) == 0) 
        {
        
          return 0;
        }
    }
    
    I2C_SendData(I2Cx, (uint8_t)((crc32Value >> 24) & 0xFF));
    I2CTimeout = I2C_TIMEOUT_FLAG;
    while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_SENDING))
    {
        if ((I2CTimeout--) == 0) 
        {
        
          return 0;
    }
  }

  /* Wait for EV8-2: last data byte sent */
  I2CTimeout = I2C_TIMEOUT_FLAG;
  while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_SENDED))
  {
    if ((I2CTimeout--) == 0)
    {
        
          return 0;
    }
  }

    /* Generate STOP condition after CRC */
  I2C_GenerateStop(I2Cx, ENABLE);
  }
  
  /* Wait until I2C bus is free */
  I2CTimeout = I2C_TIMEOUT_STOP_FLAG;
  while (I2C_GetFlag(I2Cx, I2C_FLAG_BUSY))
  {
    if ((I2CTimeout--) == 0)
    {
      return 0;
    }
  }

  return 1;
}

/**
* @brief Read data from an MPF11770 device via I2C with optional CRC32
* @param I2Cx - I2C interface to use
 * @param devAddr - 7-bit device address
 * @param regAddr - 16-bit register address
* @param pData - pointer to data buffer for read data
* @param length - length of data to read (1-82 bytes as per spec)
 * @param enableCRC - set true to enable CRC32 calculation and verification
* @return 0 if successful, error code otherwise
*/
int32_t MPF11770_I2C_master_read(I2C_Module* I2Cx, uint8_t devAddr, uint16_t regAddr, 
                       uint8_t* pData, uint8_t length, bool enableCRC)
{
  uint32_t receivedCRC = 0;
  uint32_t calculatedCRC = 0;
  uint8_t i;
  uint32_t I2CTimeout;
  
  i2c_start(I2Cx, devAddr);
  
  /* Send low byte of register address */
  I2C_SendData(I2Cx, (uint8_t)(regAddr & 0xFF));
  
  /* Wait for EV8: data byte sent */
  I2CTimeout = I2C_TIMEOUT_FLAG;
  while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_SENDING))
  {
    if ((I2CTimeout--) == 0) 
    {
      return 0;
    }
  }
  
  /* Send high byte of register address */
  I2C_SendData(I2Cx, (uint8_t)(regAddr >> 8));
  
  /* Wait for EV8: data byte sent */
  I2CTimeout = I2C_TIMEOUT_FLAG;
  while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_SENDING))
  {
    if ((I2CTimeout--) == 0) 
    {
      return 0;
    }
  }
  
  /* Send length byte */
  I2C_SendData(I2Cx, length);
  
  /* Wait for EV8: data byte sent */
  I2CTimeout = I2C_TIMEOUT_FLAG;
  while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_SENDING))
  {
    if ((I2CTimeout--) == 0) 
    {
      return 0;
    }
  }
  
  /* Wait for EV8-2: master send data end */
  I2CTimeout = I2C_TIMEOUT_FLAG;
  while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_SENDED))
  {
    if ((I2CTimeout--) == 0) 
    {
      return 0;
    }
  }
  
  /* ---- Phase 2: Restart and read data ---- */
  
  /* Generate repeated START condition */
  I2C_GenerateStart(I2Cx, ENABLE);
  
  /* Wait for EV5: START transmitted */
  I2CTimeout = I2C_TIMEOUT_FLAG;
  while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_MODE_FLAG))
  {
    if ((I2CTimeout--) == 0) 
    {
      return 0;
    }
  }
  
  /* Send slave address (read) */
  I2C_SendAddr7bit(I2Cx, devAddr, I2C_DIRECTION_RECV);
  
  /* Wait for EV6: address sent, ACK received */
  I2CTimeout = I2C_TIMEOUT_FLAG;
  while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_RXMODE_FLAG))
  {
    if ((I2CTimeout--) == 0) 
    {
      return 0;
    }
  }
  
  /* Use different strategies based on receive length */
  if (length == 1)
  {
    /* For single byte reception, disable ACK if CRC is not needed */
    if (!enableCRC)
    {
    I2C_ConfigAck(I2Cx, DISABLE);
    I2C_GenerateStop(I2Cx, ENABLE);
    }
    
    /* Clear ADDR flag */
    (void)(I2Cx->STS1);
    (void)(I2Cx->STS2);
    
    /* Wait for data reception */
    I2CTimeout = I2C_TIMEOUT_FLAG;
    while (!I2C_GetFlag(I2Cx, I2C_FLAG_RXDATNE))
    {
    if ((I2CTimeout--) == 0) 
    {
      return 0;
    }
    }
    
    /* Read data byte */
    pData[0] = I2C_RecvData(I2Cx);
  }
  else if (length == 2)
  {
    /* Implement two-byte reception exactly as in the reference code */
    /* Set ACKPOS bit */
    I2Cx->CTRL1 |= 0x0400;
    
    /* Clear ADDR flag */
    (void)(I2Cx->STS1);
    (void)(I2Cx->STS2);
    
    /* Disable ACK if CRC is not needed */
    if (!enableCRC)
    {
    I2C_ConfigAck(I2Cx, DISABLE);
    I2C_GenerateStop(I2Cx, ENABLE);
    }
    
    /* Wait for BTF flag - indicates both bytes have been received */
    I2CTimeout = I2C_TIMEOUT_FLAG;
    while (!I2C_GetFlag(I2Cx, I2C_FLAG_BSF))
    {
    if ((I2CTimeout--) == 0)
    {
      return 0;
    }
    }
    
    /* Read first byte */
    pData[0] = I2C_RecvData(I2Cx);
    
    /* Read second byte */
    pData[1] = I2C_RecvData(I2Cx);
  }
  else
  {
    /* For multi-byte reception, enable ACK first */
    I2C_ConfigAck(I2Cx, ENABLE);
    
    /* Clear ADDR flag */
    (void)(I2Cx->STS1);
    (void)(I2Cx->STS2);
    
    /* Receive data */
    i = 0;
    while (length)
    {
    /* Special handling for the last 3 bytes */
    if (length == 3)
    {
      /* Wait for BTF flag - first byte received and moved to shift register */
      I2CTimeout = I2C_TIMEOUT_FLAG;
      while (!I2C_GetFlag(I2Cx, I2C_FLAG_BSF))
      {
        if ((I2CTimeout--) == 0)
        {
        return 0;
        }
      }
      
      /* Disable ACK if CRC is not needed */
      if (!enableCRC)
      {
        I2C_ConfigAck(I2Cx, DISABLE);
        I2C_GenerateStop(I2Cx, ENABLE);
      }
      
      /* Read first byte */
      pData[i++] = I2C_RecvData(I2Cx);
      length--;
      
      /* Wait for BTF flag - second byte received */
      I2CTimeout = I2C_TIMEOUT_FLAG;
      while (!I2C_GetFlag(I2Cx, I2C_FLAG_BSF))
      {
        if ((I2CTimeout--) == 0)
        {
        return 0;
        }
      }
      
      /* Read second byte */
      pData[i++] = I2C_RecvData(I2Cx);
      length--;
      
      /* Read third byte */
      pData[i++] = I2C_RecvData(I2Cx);
      length--;
      
      break;
    }
    else /* Normal byte reception */
    {
      /* Wait for data reception */
      I2CTimeout = I2C_TIMEOUT_FLAG;
      while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_RECVD_FLAG))
      {
        if ((I2CTimeout--) == 0)
        {
        return 0;
        }
      }
      
      /* Read data byte */
      pData[i++] = I2C_RecvData(I2Cx);
      length--;
    }
    }
  }
  
  /* If CRC is enabled, read and verify CRC32 */
  if (enableCRC)
  {
    /* Ensure ACK is enabled to receive CRC bytes */
    I2C_ConfigAck(I2Cx, ENABLE);
    
    /* Receive 4 CRC bytes */
    receivedCRC = 0;
    
    /* First CRC byte (LSB) */
    I2CTimeout = I2C_TIMEOUT_FLAG;
    while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_RECVD_FLAG))
    {
    if ((I2CTimeout--) == 0) 
    {
      return 0;
    }
    }
    receivedCRC = I2C_RecvData(I2Cx);
    
    /* Second CRC byte */
    I2CTimeout = I2C_TIMEOUT_FLAG;
    while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_RECVD_FLAG))
    {
    if ((I2CTimeout--) == 0)
    {
      return 0;
    }
    }
    receivedCRC |= (uint32_t)I2C_RecvData(I2Cx) << 8;
    
    /* Third CRC byte */
    I2CTimeout = I2C_TIMEOUT_FLAG;
    while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_RECVD_FLAG))
    {
    if ((I2CTimeout--) == 0)
    {
      return 0;
    }
    }
    receivedCRC |= (uint32_t)I2C_RecvData(I2Cx) << 16;
    
    /* Fourth CRC byte (MSB) - disable ACK before reading the last byte */
    I2C_ConfigAck(I2Cx, DISABLE);
    
    /* Generate STOP signal before reading the last CRC byte */
    I2C_GenerateStop(I2Cx, ENABLE);
    
    I2CTimeout = I2C_TIMEOUT_FLAG;
    while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_RECVD_FLAG))
    {
    if ((I2CTimeout--) == 0)
    {
      return 0;
    }
    }
    receivedCRC |= (uint32_t)I2C_RecvData(I2Cx) << 24;
    
    /* Calculate CRC32 using software method */
    uint32_t calculatedCRC = Calculate_CRC32(regAddr, length, pData);
    
    /* Verify CRC */
    if (receivedCRC != calculatedCRC)
    {
    return 0;
    }
  }
  
  /* Wait until I2C bus is free */
  I2CTimeout = I2C_TIMEOUT_STOP_FLAG;
  while (I2C_GetFlag(I2Cx, I2C_FLAG_BUSY))
  {
    if ((I2CTimeout--) == 0)
    {
      return 0;
    }
  }
  
  return 1;
}
/**
* @brief  Activates the MPF11770 chip by pulling SDA low for 10ms
* @param  I2Cx: I2C peripheral to be used
* @return int32_t: 0 if successful, error code otherwise
*/
int32_t MPF11770_Activate(I2C_Module* I2Cx)
{
  GPIO_InitType GPIO_InitStructure;
  GPIO_Module* SDA_GPIO = NULL;
  GPIO_Module* SCL_GPIO = NULL;
  uint16_t SDA_PIN = 0;
  uint16_t SCL_PIN = 0;
  
  /* Configure I2C pins according to definitions in mps_init.h */
  if (I2Cx == I2C1)
  {
    /* I2C1 pins: PD11(SCL), PD1(SDA) */
    SCL_GPIO = GPIOD;
    SDA_GPIO = GPIOD;
    SCL_PIN = GPIO_PIN_11;
    SDA_PIN = GPIO_PIN_1;
  }
  else if (I2Cx == I2C2)
  {
    /* I2C2 pins: PB10(SCL), PB11(SDA) */
    SCL_GPIO = GPIOB;
    SDA_GPIO = GPIOB;
    SCL_PIN = GPIO_PIN_10;
    SDA_PIN = GPIO_PIN_11;
  }
  else if (I2Cx == I2C3)
  {
    /* I2C3 pins: PC0(SCL), PB5(SDA) */
    SCL_GPIO = GPIOC;
    SDA_GPIO = GPIOB;
    SCL_PIN = GPIO_PIN_0;
    SDA_PIN = GPIO_PIN_5;
  }
  else if (I2Cx == I2C4)
  {
    /* I2C4 pins: PD14(SCL), PD15(SDA) */
    SCL_GPIO = GPIOD;
    SDA_GPIO = GPIOD;
    SCL_PIN = GPIO_PIN_14;
    SDA_PIN = GPIO_PIN_15;
  }
  else
  {
    /* Invalid I2C peripheral */
    return -1;
  }
  
  /* Disable the I2C peripheral */
  I2C_Enable(I2Cx, DISABLE);
  
  /* Configure SDA and SCL as GPIO output open-drain */
  GPIO_InitStructure.Pin      = SDA_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_OUTPUT_OD;  /* Open-Drain output */
  GPIO_InitStructure.GPIO_Current = GPIO_DC_4mA;
  GPIO_InitStructure.GPIO_Alternate = 0;
  GPIO_InitStructure.GPIO_Pull = GPIO_NO_PULL;
  GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
  GPIO_InitPeripheral(SDA_GPIO, &GPIO_InitStructure);
  /* Pull SDA low for 10ms */
  GPIO_ResetBits(SDA_GPIO, SDA_PIN);
  systick_delay_ms(6);  /* Delay 10ms */
  /* Release SDA (pull high) */
  GPIO_SetBits(SDA_GPIO, SDA_PIN);
  systick_delay_ms(1);  /* Delay 1ms */
  i2c_master_init(I2Cx, 100); // 100kHz
  
  return 0;  /* Success */
}

/**
* @brief  callBack function for I2C communication timeout and errors
* @param  errcode: Error code indicating the type of error
* @return none
*/
//void CommTimeOut_CallBack(I2C_ErrorCode_t errcode)
//{
//  /* Print error information for debugging */
//  switch (errcode)
//  {
//    case I2C_ERROR_PARAM:
//        // printf("I2C Error: Invalid parameters\r\n");
//        break;
//    case I2C_ERROR_BUS_BUSY:
//        // printf("I2C Error: Bus is busy\r\n");
//        break;
//    case I2C_ERROR_TIMEOUT:
//        // printf("I2C Error: Communication timeout\r\n");
//        break;
//    case I2C_ERROR_NACK:
//        // printf("I2C Error: No ACK received\r\n");
//        break;
//    case I2C_ERROR_CRC:
//        // printf("I2C Error: CRC verification failed\r\n");
//        break;
//    case I2C_ERROR_BUSY:
//        // printf("I2C Error: Device busy\r\n");
//        break;
//    case I2C_ERROR_MODE:
//        // printf("I2C Error: Mode error\r\n");
//        break;
//    case I2C_ERROR_TXMODE:
//        // printf("I2C Error: Transmit mode error\r\n");
//        break;
//    case I2C_ERROR_RXMODE:
//        // printf("I2C Error: Receive mode error\r\n");
//        break;
//    case I2C_ERROR_SENDING:
//        // printf("I2C Error: Data sending error\r\n");
//        break;
//    case I2C_ERROR_SENDED:
//        // printf("I2C Error: Data sent error\r\n");
//        break;
//    case I2C_ERROR_RECVD:
//        // printf("I2C Error: Data received error\r\n");
//        break;
//    case I2C_ERROR_BYTEF:
//        // printf("I2C Error: Byte frame error\r\n");
//        break;
//    default:
//        // printf("I2C Error: Unknown error code %d\r\n", errcode);
//        break;
//  }
//  
//  /* Here you can add additional error recovery mechanisms */
//  /* For example: */
//  /* - Reset I2C peripheral */
//  /* - Reinitialize I2C */
//  /* - System reset if necessary */
//}



