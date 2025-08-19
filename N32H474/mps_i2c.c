#include "mps_i2c.h"

/**
 * @brief  Initiates I2C communication by sending START condition and device address
 * @param  I2Cx: I2C peripheral to be used (I2C1, I2C2, I2C3, or I2C4)
 * @param  devAddr: 7-bit device address (should be already left-shifted)
 * @return 1 if successful, 0 if failed (timeout or NACK)
 * @note   This function performs the complete I2C start sequence:
 *     1. Wait for bus to be free
 *     2. Generate START condition
 *     3. Send device address for write operation
 *     4. Wait for ACK from slave device
 */
int8_t i2c_start(I2C_Module* I2Cx, uint8_t devAddr)
{
  /* Check parameters */
  uint32_t I2CTimeout;
  
  /* Wait until I2C bus is not busy */
  I2CTimeout = I2C_TIMEOUT_BUSY_FLAG;
  while (I2C_GetFlag(I2Cx, I2C_FLAG_BUSY))
  {
    if ((I2CTimeout--) == 0)
    {
    return 0;
    }
  }
  
  /* Generate START condition */
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
  
  /* Send slave address (write) */
  I2C_SendAddr7bit(I2Cx, devAddr, I2C_DIRECTION_SEND);
  
  /* Wait for EV6: address sent, ACK received */
  I2CTimeout = I2C_TIMEOUT_FLAG;
  while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_TXMODE_FLAG))
  {
    if ((I2CTimeout--) == 0)
    {
    return 0;
  }
  }
  return 1; // Success
}

/**
 * @brief  Sends a single byte of data via I2C
 * @param  I2Cx: I2C peripheral to be used (I2C1, I2C2, I2C3, or I2C4)
 * @param  data: 8-bit data byte to be transmitted
 * @return 1 if successful, 0 if failed (timeout)
 * @note   This function should be called after i2c_start() or between data bytes.
 *     It waits for the data register to be empty and transmission complete.
 */
int8_t i2c_write(I2C_Module* I2Cx, uint8_t data)
{
  uint32_t I2CTimeout;

  /* Send data byte to I2C data register */
  I2C_SendData(I2Cx, data);
  
  /* Wait for EV8: data byte sent (data register empty and byte transmission complete) */
  I2CTimeout = I2C_TIMEOUT_FLAG;
  while (!I2C_CheckEvent(I2Cx, I2C_EVT_MASTER_DATA_SENDING))
  {
    if ((I2CTimeout--) == 0)
    {
    /* Timeout occurred during data transmission */
    return 0;
  }
  }
  
  return 1; // Success
}
/**
 * Generic I2C initialization function, can initialize specified I2C interface
 * @param I2Cx - I2C interface to initialize (I2C1, I2C2, I2C3, I2C4)
 * @param speedKhz - I2C frequency in kHz (e.g. 100 for 100kHz, 400 for 400kHz)
 * @return 0 if successful, non-zero if failed
 */
int i2c_master_init(I2C_Module* I2Cx, uint32_t speedKhz)
{
  I2C_InitType i2cx_master;
  GPIO_InitType i2cx_gpio;
  
  /* Select configuration based on I2C interface */
  if (I2Cx == I2C1)
  {
  /* Enable I2C1 and corresponding GPIO clocks */
  RCC_EnableAPB1PeriphClk(I2C1_RCC, ENABLE);
  RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
  RCC_EnableAHB1PeriphClk(I2C1_CLK_EN, ENABLE);
  
  /* Set SCL pin pull-up */
  I2C1_SCL_GPIO->POD |= (I2C1_SCL_PIN);
  
  /* Set SDA pin pull-up */
  I2C1_SDA_GPIO->POD |= (I2C1_SDA_PIN);
  
  /* Initialize GPIO structure */
  GPIO_InitStruct(&i2cx_gpio);
  
  /* Configure SCL pin */
  i2cx_gpio.Pin    = I2C1_SCL_PIN;
  i2cx_gpio.GPIO_Pull  = GPIO_PULL_UP;
  i2cx_gpio.GPIO_Alternate = I2C1_SCL_AF;
  i2cx_gpio.GPIO_Mode  = GPIO_MODE_AF_OD;
  i2cx_gpio.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
  GPIO_InitPeripheral(I2C1_SCL_GPIO, &i2cx_gpio);
  
  /* Configure SDA pin */
  i2cx_gpio.Pin    = I2C1_SDA_PIN;
  i2cx_gpio.GPIO_Pull  = GPIO_PULL_UP;
  i2cx_gpio.GPIO_Alternate = I2C1_SDA_AF;
  i2cx_gpio.GPIO_Mode  = GPIO_MODE_AF_OD;
  i2cx_gpio.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
  GPIO_InitPeripheral(I2C1_SDA_GPIO, &i2cx_gpio);
  }
  else if (I2Cx == I2C2)
  {
  /* Enable I2C2 and corresponding GPIO clocks */
  RCC_EnableAPB1PeriphClk(I2C2_RCC, ENABLE);
  RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
  RCC_EnableAHB1PeriphClk(I2C2_CLK_EN, ENABLE);
  
  /* Set SCL pin pull-up */
  I2C2_SCL_GPIO->POD |= (I2C2_SCL_PIN);
  
  /* Set SDA pin pull-up */
  I2C2_SDA_GPIO->POD |= (I2C2_SDA_PIN);
  
  /* Initialize GPIO structure */
  GPIO_InitStruct(&i2cx_gpio);
  
  /* Configure SCL pin */
  i2cx_gpio.Pin    = I2C2_SCL_PIN;
  i2cx_gpio.GPIO_Pull  = GPIO_PULL_UP;
  i2cx_gpio.GPIO_Alternate = I2C2_SCL_AF;
  i2cx_gpio.GPIO_Mode  = GPIO_MODE_AF_OD;
  i2cx_gpio.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
  GPIO_InitPeripheral(I2C2_SCL_GPIO, &i2cx_gpio);
  
  /* Configure SDA pin */
  i2cx_gpio.Pin    = I2C2_SDA_PIN;
  i2cx_gpio.GPIO_Pull  = GPIO_PULL_UP;
  i2cx_gpio.GPIO_Alternate = I2C2_SDA_AF;
  i2cx_gpio.GPIO_Mode  = GPIO_MODE_AF_OD;
  i2cx_gpio.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
  GPIO_InitPeripheral(I2C2_SDA_GPIO, &i2cx_gpio);
  }
  else if (I2Cx == I2C3)
  {
  /* Enable I2C3 and corresponding GPIO clocks */
  RCC_EnableAPB1PeriphClk(I2C3_RCC, ENABLE);
  RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
  RCC_EnableAHB1PeriphClk(I2C3_CLK_EN, ENABLE);
  
  /* Set SCL pin pull-up */
  I2C3_SCL_GPIO->POD |= (I2C3_SCL_PIN);
  
  /* Set SDA pin pull-up */
  I2C3_SDA_GPIO->POD |= (I2C3_SDA_PIN);
  
  /* Initialize GPIO structure */
  GPIO_InitStruct(&i2cx_gpio);
  
  /* Configure SCL pin */
  i2cx_gpio.Pin    = I2C3_SCL_PIN;
  i2cx_gpio.GPIO_Pull  = GPIO_PULL_UP;
  i2cx_gpio.GPIO_Alternate = I2C3_SCL_AF;
  i2cx_gpio.GPIO_Mode  = GPIO_MODE_AF_OD;
  i2cx_gpio.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
  GPIO_InitPeripheral(I2C3_SCL_GPIO, &i2cx_gpio);
  
  /* Configure SDA pin */
  i2cx_gpio.Pin    = I2C3_SDA_PIN;
  i2cx_gpio.GPIO_Pull  = GPIO_PULL_UP;
  i2cx_gpio.GPIO_Alternate = I2C3_SDA_AF;
  i2cx_gpio.GPIO_Mode  = GPIO_MODE_AF_OD;
  i2cx_gpio.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
  GPIO_InitPeripheral(I2C3_SDA_GPIO, &i2cx_gpio);
  }
  else if (I2Cx == I2C4)
  {
  /* Enable I2C4 and corresponding GPIO clocks */
  RCC_EnableAPB1PeriphClk(I2C4_RCC, ENABLE);
  RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
  RCC_EnableAHB1PeriphClk(I2C4_CLK_EN, ENABLE);
  
  /* Set SCL pin pull-up */
  I2C4_SCL_GPIO->POD |= (I2C4_SCL_PIN);
  
  /* Set SDA pin pull-up */
  I2C4_SDA_GPIO->POD |= (I2C4_SDA_PIN);
  
  /* Initialize GPIO structure */
  GPIO_InitStruct(&i2cx_gpio);
  
  /* Configure SCL pin */
  i2cx_gpio.Pin    = I2C4_SCL_PIN;
  i2cx_gpio.GPIO_Pull  = GPIO_PULL_UP;
  i2cx_gpio.GPIO_Alternate = I2C4_SCL_AF;
  i2cx_gpio.GPIO_Mode  = GPIO_MODE_AF_OD;
  i2cx_gpio.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
  GPIO_InitPeripheral(I2C4_SCL_GPIO, &i2cx_gpio);
  
  /* Configure SDA pin */
  i2cx_gpio.Pin    = I2C4_SDA_PIN;
  i2cx_gpio.GPIO_Pull  = GPIO_PULL_UP;
  i2cx_gpio.GPIO_Alternate = I2C4_SDA_AF;
  i2cx_gpio.GPIO_Mode  = GPIO_MODE_AF_OD;
  i2cx_gpio.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
  GPIO_InitPeripheral(I2C4_SDA_GPIO, &i2cx_gpio);
  }
  else
  {
  /* Unsupported I2C interface */
  return -1;
  }

  /* Initialize I2C interface */
  I2C_DeInit(I2Cx);
  I2C_InitStruct(&i2cx_master);
  i2cx_master.BusMode   = I2C_BUSMODE_I2C;
  i2cx_master.FmDutyCycle = I2C_FMDUTYCYCLE_2;
  i2cx_master.OwnAddr1  = I2C_MASTER_ADDR;
  i2cx_master.AckEnable   = I2C_ACKEN;
  i2cx_master.AddrMode  = I2C_ADDR_MODE_7BIT;
  i2cx_master.ClkSpeed  = speedKhz * 1000; // Convert kHz to Hz

  I2C_Init(I2Cx, &i2cx_master);
  I2C_Enable(I2Cx, ENABLE);
  
  return 0;
}
/**
 * Generic I2C slave initialization function, can initialize specified I2C interface in slave mode
 * @param I2Cx - I2C interface to initialize (I2C1, I2C2, I2C3, I2C4)
 * @param speedKhz - I2C frequency in kHz (e.g. 100 for 100kHz, 400 for 400kHz)
 * @return 0 if successful, non-zero if failed
 */
int i2c_slave_init(I2C_Module* I2Cx, uint32_t speedKhz)
{
  I2C_InitType i2cx_slave;
  GPIO_InitType i2cx_gpio;
  
  /* Select configuration based on I2C interface */
  if (I2Cx == I2C1)
  {
  /* Enable I2C1 and corresponding GPIO clocks */
  RCC_EnableAPB1PeriphClk(I2C1_RCC, ENABLE);
  RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
  RCC_EnableAHB1PeriphClk(I2C1_CLK_EN, ENABLE);
  
  /* Set SCL pin pull-up */
  I2C1_SCL_GPIO->POD |= (I2C1_SCL_PIN);
  
  /* Set SDA pin pull-up */
  I2C1_SDA_GPIO->POD |= (I2C1_SDA_PIN);
  
  /* Initialize GPIO structure */
  GPIO_InitStruct(&i2cx_gpio);
  
  /* Configure SCL pin */
  i2cx_gpio.Pin    = I2C1_SCL_PIN;
  i2cx_gpio.GPIO_Pull  = GPIO_PULL_UP;
  i2cx_gpio.GPIO_Alternate = I2C1_SCL_AF;
  i2cx_gpio.GPIO_Mode  = GPIO_MODE_AF_OD;
  i2cx_gpio.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
  GPIO_InitPeripheral(I2C1_SCL_GPIO, &i2cx_gpio);
  
  /* Configure SDA pin */
  i2cx_gpio.Pin    = I2C1_SDA_PIN;
  i2cx_gpio.GPIO_Pull  = GPIO_PULL_UP;
  i2cx_gpio.GPIO_Alternate = I2C1_SDA_AF;
  i2cx_gpio.GPIO_Mode  = GPIO_MODE_AF_OD;
  i2cx_gpio.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
  GPIO_InitPeripheral(I2C1_SDA_GPIO, &i2cx_gpio);
  }
  else if (I2Cx == I2C2)
  {
  /* Enable I2C2 and corresponding GPIO clocks */
  RCC_EnableAPB1PeriphClk(I2C2_RCC, ENABLE);
  RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
  RCC_EnableAHB1PeriphClk(I2C2_CLK_EN, ENABLE);
  
  /* Set SCL pin pull-up */
  I2C2_SCL_GPIO->POD |= (I2C2_SCL_PIN);
  
  /* Set SDA pin pull-up */
  I2C2_SDA_GPIO->POD |= (I2C2_SDA_PIN);
  
  /* Initialize GPIO structure */
  GPIO_InitStruct(&i2cx_gpio);
  
  /* Configure SCL pin */
  i2cx_gpio.Pin    = I2C2_SCL_PIN;
  i2cx_gpio.GPIO_Pull  = GPIO_PULL_UP;
  i2cx_gpio.GPIO_Alternate = I2C2_SCL_AF;
  i2cx_gpio.GPIO_Mode  = GPIO_MODE_AF_OD;
  i2cx_gpio.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
  GPIO_InitPeripheral(I2C2_SCL_GPIO, &i2cx_gpio);
  
  /* Configure SDA pin */
  i2cx_gpio.Pin    = I2C2_SDA_PIN;
  i2cx_gpio.GPIO_Pull  = GPIO_PULL_UP;
  i2cx_gpio.GPIO_Alternate = I2C2_SDA_AF;
  i2cx_gpio.GPIO_Mode  = GPIO_MODE_AF_OD;
  i2cx_gpio.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
  GPIO_InitPeripheral(I2C2_SDA_GPIO, &i2cx_gpio);
  }
  else if (I2Cx == I2C3)
  {
  /* Enable I2C3 and corresponding GPIO clocks */
  RCC_EnableAPB1PeriphClk(I2C3_RCC, ENABLE);
  RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
  RCC_EnableAHB1PeriphClk(I2C3_CLK_EN, ENABLE);
  
  /* Set SCL pin pull-up */
  I2C3_SCL_GPIO->POD |= (I2C3_SCL_PIN);
  
  /* Set SDA pin pull-up */
  I2C3_SDA_GPIO->POD |= (I2C3_SDA_PIN);
  
  /* Initialize GPIO structure */
  GPIO_InitStruct(&i2cx_gpio);
  
  /* Configure SCL pin */
  i2cx_gpio.Pin    = I2C3_SCL_PIN;
  i2cx_gpio.GPIO_Pull  = GPIO_PULL_UP;
  i2cx_gpio.GPIO_Alternate = I2C3_SCL_AF;
  i2cx_gpio.GPIO_Mode  = GPIO_MODE_AF_OD;
  i2cx_gpio.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
  GPIO_InitPeripheral(I2C3_SCL_GPIO, &i2cx_gpio);
  
  /* Configure SDA pin */
  i2cx_gpio.Pin    = I2C3_SDA_PIN;
  i2cx_gpio.GPIO_Pull  = GPIO_PULL_UP;
  i2cx_gpio.GPIO_Alternate = I2C3_SDA_AF;
  i2cx_gpio.GPIO_Mode  = GPIO_MODE_AF_OD;
  i2cx_gpio.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
  GPIO_InitPeripheral(I2C3_SDA_GPIO, &i2cx_gpio);
  }
  else if (I2Cx == I2C4)
  {
  /* Enable I2C4 and corresponding GPIO clocks */
  RCC_EnableAPB1PeriphClk(I2C4_RCC, ENABLE);
  RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
  RCC_EnableAHB1PeriphClk(I2C4_CLK_EN, ENABLE);
  
  /* Set SCL pin pull-up */
  I2C4_SCL_GPIO->POD |= (I2C4_SCL_PIN);
  
  /* Set SDA pin pull-up */
  I2C4_SDA_GPIO->POD |= (I2C4_SDA_PIN);
  
  /* Initialize GPIO structure */
  GPIO_InitStruct(&i2cx_gpio);
  
  /* Configure SCL pin */
  i2cx_gpio.Pin    = I2C4_SCL_PIN;
  i2cx_gpio.GPIO_Pull  = GPIO_PULL_UP;
  i2cx_gpio.GPIO_Alternate = I2C4_SCL_AF;
  i2cx_gpio.GPIO_Mode  = GPIO_MODE_AF_OD;
  i2cx_gpio.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
  GPIO_InitPeripheral(I2C4_SCL_GPIO, &i2cx_gpio);
  
  /* Configure SDA pin */
  i2cx_gpio.Pin    = I2C4_SDA_PIN;
  i2cx_gpio.GPIO_Pull  = GPIO_PULL_UP;
  i2cx_gpio.GPIO_Alternate = I2C4_SDA_AF;
  i2cx_gpio.GPIO_Mode  = GPIO_MODE_AF_OD;
  i2cx_gpio.GPIO_Slew_Rate = GPIO_SLEW_RATE_SLOW;
  GPIO_InitPeripheral(I2C4_SDA_GPIO, &i2cx_gpio);
  }
  else
  {
  /* Unsupported I2C interface */
  return -1;
  }

  /* Initialize I2C interface in slave mode */
  I2C_DeInit(I2Cx);
  I2C_InitStruct(&i2cx_slave);
  i2cx_slave.BusMode   = I2C_BUSMODE_I2C;
  i2cx_slave.FmDutyCycle = I2C_FMDUTYCYCLE_2;
  i2cx_slave.OwnAddr1  = I2C_SLAVE_ADDR;  // Use slave address
  i2cx_slave.AckEnable   = I2C_ACKEN;
  i2cx_slave.AddrMode  = I2C_ADDR_MODE_7BIT;
  i2cx_slave.ClkSpeed  = speedKhz * 1000; // Convert kHz to Hz

  I2C_Init(I2Cx, &i2cx_slave);
  I2C_Enable(I2Cx, ENABLE);
  
  return 0;
}


