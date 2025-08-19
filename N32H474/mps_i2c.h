#ifndef __MPS_I2C_H__
#define __MPS_I2C_H__
#include "n32h47x_48x.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_i2c.h"
#include "n32h47x_48x_rcc.h"
#include "misc.h" /* For NVIC functions */
#include "n32h47x_48x_dma.h"  /* For DMA definitions */
#include "n32h47x_48x_usart.h" /* For USART/UART definitions */

#include "mps_spi.h"    /* For spi_master_init */
#include "mps_uart.h"   /* For uart_init and uart_dma_init */
#include "mps_delay.h"  /* For systick_delay_ms */




#ifdef __cplusplus
extern "C" {
#endif
/* Error codes for I2C communication */
/* I2C Configuration Definitions - All available I2C interfaces */
/* I2C1 Configuration */
#define I2C1_RCC            RCC_APB1_PERIPH_I2C1
#define I2C1_SCL_PIN        GPIO_PIN_11
#define I2C1_SDA_PIN        GPIO_PIN_1
#define I2C1_SCL_GPIO       GPIOD
#define I2C1_SDA_GPIO       GPIOD
#define I2C1_SCL_AF         GPIO_AF5
#define I2C1_SDA_AF         GPIO_AF7
#define I2C1_CLK_EN         RCC_AHB_PERIPHEN_GPIOD

/* I2C2 Configuration */
#define I2C2_RCC            RCC_APB1_PERIPH_I2C2
#define I2C2_SCL_PIN        GPIO_PIN_10
#define I2C2_SDA_PIN        GPIO_PIN_11
#define I2C2_SCL_GPIO       GPIOB
#define I2C2_SDA_GPIO       GPIOB
#define I2C2_SCL_AF         GPIO_AF7
#define I2C2_SDA_AF         GPIO_AF7
#define I2C2_CLK_EN         RCC_AHB_PERIPHEN_GPIOB

/* I2C3 Configuration */
#define I2C3_RCC            RCC_APB1_PERIPH_I2C3
#define I2C3_SCL_PIN        GPIO_PIN_0
#define I2C3_SDA_PIN        GPIO_PIN_5
#define I2C3_SCL_GPIO       GPIOC
#define I2C3_SDA_GPIO       GPIOB
#define I2C3_SCL_AF         GPIO_AF8
#define I2C3_SDA_AF         GPIO_AF11
#define I2C3_CLK_EN         (RCC_AHB_PERIPHEN_GPIOB|RCC_AHB_PERIPHEN_GPIOC)

/* I2C4 Configuration */
#define I2C4_RCC            RCC_APB1_PERIPH_I2C4
#define I2C4_SCL_PIN        GPIO_PIN_14
#define I2C4_SDA_PIN        GPIO_PIN_15
#define I2C4_SCL_GPIO       GPIOD
#define I2C4_SDA_GPIO       GPIOD
#define I2C4_SCL_AF         GPIO_AF9
#define I2C4_SDA_AF         GPIO_AF9
#define I2C4_CLK_EN         RCC_AHB_PERIPHEN_GPIOD

/* I2C Address Definitions */
#define I2C_MASTER_ADDR   0x30
#define I2C_SLAVE_ADDR    0x10

/* Timeout values for I2C operations */
#define I2C_TIMEOUT_FLAG      1000
#define I2C_TIMEOUT_BUSY_FLAG   1000
#define I2C_TIMEOUT_STOP_FLAG   1000

int i2c_master_init(I2C_Module* I2Cx, uint32_t speedKhz);  // Generic I2C initialization function, supports all I2C interfaces
int i2c_slave_init(I2C_Module* I2Cx, uint32_t speedKhz);   // Generic I2C slave initialization function, supports all I2C interfaces
int8_t i2c_start(I2C_Module* I2Cx, uint8_t devAddr);
#ifdef __cplusplus
}
#endif

#endif /* __MPS_I2C_H__ */