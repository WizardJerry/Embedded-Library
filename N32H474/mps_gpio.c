#include "mps_gpio.h"
#include "mps_delay.h"

/**
*\*\name    CTRL_GPIO_Init.
*\*\fun     Initialize CTRL pin (PA1) as output.
*\*\param   none
*\*\return  none
**/
void CTRL_GPIO_Init(void)
{
  GPIO_InitType GPIO_InitStructure;
  
  /* Enable GPIOA clock */
  RCC_EnableAHB1PeriphClk(CTRL_GPIO_RCC, ENABLE);
  
  /* Initialize GPIO_InitStructure */
  GPIO_InitStruct(&GPIO_InitStructure);
  
  /* Configure PA1 as output */
  GPIO_InitStructure.Pin        = CTRL_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitPeripheral(CTRL_GPIO, &GPIO_InitStructure);
  
  /* Default state: low */
  CTRL_L;
}


/**
*\*\name    LED_Init.
*\*\fun     Initialize LED GPIO pin.
*\*\param   GPIOx - GPIO module
*\*\param   Pin - GPIO pin
*\*\param   clock - RCC clock for the GPIO
*\*\return  none
**/
void LED_Init(GPIO_Module* GPIOx,uint16_t Pin,uint32_t clock)
{
  GPIO_InitType InitStruct;
  
  /* Enable GPIO clock */
  RCC_EnableAHB1PeriphClk(clock,ENABLE);
  
  /* Init GPIO as output push-pull mode */
  InitStruct.Pin            = Pin;
  InitStruct.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;
  InitStruct.GPIO_Mode      = GPIO_MODE_OUTPUT_PP;
  InitStruct.GPIO_Alternate = GPIO_AF0;
  InitStruct.GPIO_Pull      = GPIO_NO_PULL;
  InitStruct.GPIO_Current   = GPIO_DS_8mA;
  
  GPIO_InitPeripheral(GPIOx, &InitStruct);
}

/**
*\*\name    LED_On.
*\*\fun     Turn on LED.
*\*\param   GPIOx - GPIO module
*\*\param   Pin - GPIO pin
*\*\return  none
**/
void LED_On(GPIO_Module* GPIOx,uint16_t Pin)
{
  GPIO_SetBits(GPIOx, Pin);
}

/**
*\*\name    LED_Off.
*\*\fun     Turn off LED.
*\*\param   GPIOx - GPIO module
*\*\param   Pin - GPIO pin
*\*\return  none
**/
void LED_Off(GPIO_Module* GPIOx,uint16_t Pin)
{
  GPIO_ResetBits(GPIOx, Pin);
}

/**
*\*\name    LED_Toggle.
*\*\fun     Toggle LED state.
*\*\param   none
*\*\return  none
**/
void LED_Toggle(void)
{
  /* Toggle LED */
  GPIO_TogglePin(LED1_PORT, LED1_PIN);
} 

