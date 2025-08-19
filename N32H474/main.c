#include "n32h47x_48x.h"
#include "log.h"
#include "delay.h"
#include "mps_i2c.h"
#include "mpf11770_i2c.h"
#include "mps_spi.h"
#include "mps_timer.h"
#include "mps_gpio.h"
#include "mps_adc.h"
#include "mps_hrpwm.h"
#include <stdio.h>

/* UART Echo Buffer and Variables */
#define UART_BUFFER_SIZE 256  // Buffer size can be adjusted as needed
uint8_t rxBuffer[UART_BUFFER_SIZE];  // Receive buffer
volatile uint16_t rxCount = 0;       // Number of bytes received

#define MPF11770_DEVICE_ADDR       (0x08 << 1) 
/**
*\*\name    main.
*\*\fun     main function.
*\*\param   none
*\*\return  none 
**/
int main(void)
{
  CTRL_GPIO_Init();
  LED_Init(LED1_PORT, LED1_PIN, LED1_CLOCK);
  i2c_master_init(I2C1, 100); // 100kHz
  uart_dma_interrupt_init(USART1);
  spi_master_init(SPI1);
  adc_init(ADC1,ADC_CH_0,GPIOA, GPIO_PIN_0,ADC_RESOLUTION_12BIT, false, ADC_SAMP_TIME_CYCLES_239_5);
	
	
	// 0.测试HRPWM，PB14和PB15引脚输出互补PWM信号，频率，占空比，死区时间
	configure_hrpwm_gpio();
  hrpwm_init(SHRTIM1, HRPWM_TIMER_D, 100000, 50.0f, 200);
	hrpwm_start(SHRTIM1, HRPWM_TIMER_D);  
	
	// 1.测试定时器，间隔设置时间后执行LED_Toggle函数
  timer_init(GTIM2, 500000, LED_Toggle);  
  timer_start(GTIM2);                     
	
	// 2.测试I2C，写入后，读回来
  uint8_t write_pData[2];  
  uint8_t read_pData[2];  
  MPF11770_Activate(I2C1);
  write_pData[0] = 1;
  write_pData[1] = 1;
  int b=MPF11770_I2C_master_write(I2C1, MPF11770_DEVICE_ADDR, 0x0002, write_pData, 2, 1);
  systick_delay_ms(2);
  int a=MPF11770_I2C_master_read(I2C1, MPF11770_DEVICE_ADDR, 0x0002, read_pData, 2, 1);

  while (1)
	{
			
				// 3.测试串口，收到串口数据后，回发，中断接收
			if (rxCount > 0) {
					/* Send data using DMA */
					uart_dma_send_data(USART1, USART1_TX_DMA_CH, rxBuffer, rxCount);
					
					/* Wait for DMA transfer completion */
					while (DMA_GetCurrDataCounter(USART1_TX_DMA_CH) != 0);
					
					/* Reset counter after sending */
					rxCount = 0;
			}
			
			// 4.测试ADC，PA0引脚连接你的测试电压 (0-3.3V)
			uint16_t voltage_mv = adc_test_voltage_pa0();
			systick_delay_ms(1000);
			
			
			
	}
}


