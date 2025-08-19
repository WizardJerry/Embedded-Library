#include "mps_it.h"
#include "n32h47x_48x_tim.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_usart.h"
#include "misc.h"
#include <stddef.h>



/* Static array to store GPIO interrupt callbacks for EXTI0-15 */
static gpio_callback_t gpio_callbacks[16] = {NULL};

/* ADC callback storage */
static void (*adc1_callback)(uint16_t value) = NULL;
static void (*adc2_callback)(uint16_t value) = NULL;

/* External UART buffer variables (defined in main.c) */
extern uint8_t rxBuffer[];
extern volatile uint16_t rxCount;

/* UART buffer size definition */
#define UART_BUFFER_SIZE 256

/**
 * @brief Set timer callBack function - simplified
 */
/**
 * @brief Initialize GPIO external interrupt - super simple version (all-in-one)
 */
void gpio_interrupt_init(GPIO_Module* GPIOx, uint16_t pin, gpio_trigger_t trigger, uint8_t exti_line, gpio_callback_t callBack)
{
  GPIO_InitType GPIO_InitStructure;
  EXTI_InitType EXTI_InitStructure;
  NVIC_InitType NVIC_InitStructure;
  uint8_t pin_number = 0;
  uint32_t exti_line_mask = 0;
  IRQn_Type irq_channel = 0;
  
  // Enable GPIO clock
  if (GPIOx == GPIOA) RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOA, ENABLE);
  else if (GPIOx == GPIOB) RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOB, ENABLE);
  else if (GPIOx == GPIOC) RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOC, ENABLE);
  else if (GPIOx == GPIOD) RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOD, ENABLE);
  else if (GPIOx == GPIOE) RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOE, ENABLE);
  else if (GPIOx == GPIOF) RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOF, ENABLE);
  else if (GPIOx == GPIOG) RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOG, ENABLE);
  else if (GPIOx == GPIOH) RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOH, ENABLE);
  
  // Enable AFIO clock
  RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_AFIO, ENABLE);
  
  // Get pin number (0-15) from pin mask
  for(int i = 0; i < 16; i++) {
    if(pin & (1 << i)) {
      pin_number = i;
      break;
    }
  }
  
  // Calculate EXTI line mask from user specified line number
  exti_line_mask = 1 << exti_line;
  
  // Configure GPIO as input with pull-up
  GPIO_InitStructure.Pin = pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_MODE_INPUT;
  GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;
  GPIO_InitPeripheral(GPIOx, &GPIO_InitStructure);
  
  // Connect EXTI line to GPIO pin (map GPIO pin to specified EXTI line)
  GPIO_ConfigEXTILine(exti_line, GPIO_GET_INDEX((uint32_t)GPIOx), pin_number);
  
  // Configure EXTI line
  EXTI_InitStructure.EXTI_Line = exti_line_mask;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  
  // Set trigger type
  switch(trigger) {
    case GPIO_TRIGGER_RISING:
      EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
      break;
    case GPIO_TRIGGER_FALLING:
      EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
      break;
    case GPIO_TRIGGER_BOTH:
      EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
      break;
  }
  
  EXTI_InitPeripheral(&EXTI_InitStructure);
  
  // Get IRQ channel based on EXTI line number (not pin number)
  if (exti_line == 0) irq_channel = EXTI0_IRQn;
  else if (exti_line == 1) irq_channel = EXTI1_IRQn;
  else if (exti_line == 2) irq_channel = EXTI2_IRQn;
  else if (exti_line == 3) irq_channel = EXTI3_IRQn;
  else if (exti_line == 4) irq_channel = EXTI4_IRQn;
  else if (exti_line >= 5 && exti_line <= 9) irq_channel = EXTI9_5_IRQn;
  else if (exti_line >= 10 && exti_line <= 15) irq_channel = EXTI15_10_IRQn;
  
  // Configure NVIC
  NVIC_InitStructure.NVIC_IRQChannel = irq_channel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // Default priority
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  // Set callBack function directly using EXTI line number
  if (exti_line < 16) {
    gpio_callbacks[exti_line] = callBack;
  }
}





/* GPIO EXTI interrupt handlers */

/**
 * @brief EXTI0 interrupt handler
 */
void EXTI0_IRQHandler(void)
{
  if (EXTI_GetITStatus(EXTI_LINE0) != RESET) {
    EXTI_ClrITPendBit(EXTI_LINE0);  // Clear interrupt flag
    if (gpio_callbacks[0] != NULL) {
      gpio_callbacks[0]();  // Call user callBack function
        }
    }
}

/**
 * @brief EXTI1 interrupt handler
 */
void EXTI1_IRQHandler(void)
{
  if (EXTI_GetITStatus(EXTI_LINE1) != RESET) {
    EXTI_ClrITPendBit(EXTI_LINE1);
    if (gpio_callbacks[1] != NULL) {
      gpio_callbacks[1]();
        }
    }
}

/**
 * @brief EXTI2 interrupt handler
 */
void EXTI2_IRQHandler(void)
{
  if (EXTI_GetITStatus(EXTI_LINE2) != RESET) {
    EXTI_ClrITPendBit(EXTI_LINE2);
    if (gpio_callbacks[2] != NULL) {
      gpio_callbacks[2]();
        }
    }
}

/**
 * @brief EXTI3 interrupt handler
 */
void EXTI3_IRQHandler(void)
{
  if (EXTI_GetITStatus(EXTI_LINE3) != RESET) {
    EXTI_ClrITPendBit(EXTI_LINE3);
    if (gpio_callbacks[3] != NULL) {
      gpio_callbacks[3]();
        }
    }
}

/**
 * @brief EXTI4 interrupt handler
 */
void EXTI4_IRQHandler(void)
{
  if (EXTI_GetITStatus(EXTI_LINE4) != RESET) {
    EXTI_ClrITPendBit(EXTI_LINE4);
    if (gpio_callbacks[4] != NULL) {
      gpio_callbacks[4]();
        }
    }
}

/**
 * @brief EXTI9_5 interrupt handler (handles EXTI5-9)
 */
void EXTI9_5_IRQHandler(void)
{
  // Check each line from 5 to 9
  for (int i = 5; i <= 9; i++) {
    uint32_t exti_line = 1 << i;
      if (EXTI_GetITStatus(exti_line) != RESET) {
        EXTI_ClrITPendBit(exti_line);
        if (gpio_callbacks[i] != NULL) {
          gpio_callbacks[i]();
            }
        }
    }
}

/**
 * @brief EXTI15_10 interrupt handler (handles EXTI10-15)
 */
void EXTI15_10_IRQHandler(void)
{
  // Check each line from 10 to 15
  for (int i = 10; i <= 15; i++) {
    uint32_t exti_line = 1 << i;
      if (EXTI_GetITStatus(exti_line) != RESET) {
        EXTI_ClrITPendBit(exti_line);
        if (gpio_callbacks[i] != NULL) {
          gpio_callbacks[i]();
            }
        }
    }
}

/* UART interrupt handlers */

/**
 * @brief USART1 IRQ Handler with simple buffer for echo
 */
void USART1_IRQHandler(void)
{
    /* Check if receive data register not empty interrupt occurred */
    if ((USART_GetFlagStatus(USART1, USART_FLAG_RXDNE) != RESET) &&
        (USART_GetIntStatus(USART1, USART_INT_RXDNE) != RESET))
    {
        /* Read data from the receive data register */
        if (rxCount < UART_BUFFER_SIZE) {
            rxBuffer[rxCount++] = USART_ReceiveData(USART1);
        }
    }
    
    /* Check for other UART interrupts and handle them */
    /* For example: overrun error, noise error, framing error, etc. */
    if ((USART_GetFlagStatus(USART1, USART_FLAG_OREF) != RESET) || 
        (USART_GetFlagStatus(USART1, USART_FLAG_NEF) != RESET) ||
        (USART_GetFlagStatus(USART1, USART_FLAG_PEF) != RESET) ||
        (USART_GetFlagStatus(USART1, USART_FLAG_FEF) != RESET))
    {
        /* Clear error flags by reading STS and DAT registers */
        (void)USART1->STS;
        (void)USART1->DAT;
    }
}

/**
 * @brief Sample UART IRQ Handler for USART2
 */
void USART2_IRQHandler(void)
{
    uint8_t receivedData;
    
    /* Check if receive data register not empty interrupt occurred */
    if ((USART_GetFlagStatus(USART2, USART_FLAG_RXDNE) != RESET) &&
        (USART_GetIntStatus(USART2, USART_INT_RXDNE) != RESET))
    {
        /* Read data from the receive data register */
        receivedData = USART_ReceiveData(USART2);
        
        /* Process received data here... */
    }
    
    /* Handle error flags */
    if ((USART_GetFlagStatus(USART2, USART_FLAG_OREF) != RESET) || 
        (USART_GetFlagStatus(USART2, USART_FLAG_NEF) != RESET) ||
        (USART_GetFlagStatus(USART2, USART_FLAG_PEF) != RESET) ||
        (USART_GetFlagStatus(USART2, USART_FLAG_FEF) != RESET))
    {
        (void)USART2->STS;
        (void)USART2->DAT;
    }
}

/**
 * @brief Sample UART IRQ Handler for USART3
 */
void USART3_IRQHandler(void)
{
    uint8_t receivedData;
    
    /* Check if receive data register not empty interrupt occurred */
    if ((USART_GetFlagStatus(USART3, USART_FLAG_RXDNE) != RESET) &&
        (USART_GetIntStatus(USART3, USART_INT_RXDNE) != RESET))
    {
        /* Read data from the receive data register */
        receivedData = USART_ReceiveData(USART3);
        
        /* Process received data here... */
    }
    
    /* Handle error flags */
    if ((USART_GetFlagStatus(USART3, USART_FLAG_OREF) != RESET) || 
        (USART_GetFlagStatus(USART3, USART_FLAG_NEF) != RESET) ||
        (USART_GetFlagStatus(USART3, USART_FLAG_PEF) != RESET) ||
        (USART_GetFlagStatus(USART3, USART_FLAG_FEF) != RESET))
    {
        (void)USART3->STS;
        (void)USART3->DAT;
    }
}
