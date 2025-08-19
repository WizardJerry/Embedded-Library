#ifndef __MPS_IT_H__
#define __MPS_IT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32h47x_48x_tim.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_exti.h"
#include "n32h47x_48x_usart.h"

/**
 * @brief Timer callBack function type
 */
typedef void (*timer_callback_t)(void);

/**
 * @brief GPIO interrupt callBack function type
 */
typedef void (*gpio_callback_t)(void);

/**
 * @brief GPIO interrupt trigger type
 */
typedef enum {
    GPIO_TRIGGER_RISING = 0,        // Rising edge trigger
    GPIO_TRIGGER_FALLING,           // Falling edge trigger  
    GPIO_TRIGGER_BOTH               // Both edge trigger
} gpio_trigger_t;

void gpio_interrupt_init(GPIO_Module* GPIOx, uint16_t pin, gpio_trigger_t trigger, uint8_t exti_line, gpio_callback_t callBack);
void adc_set_callback(uint8_t adc_number, void (*callBack)(uint16_t value));
void timer_set_callback(TIM_Module* TIMx, timer_callback_t callBack);

/* Timer interrupt handlers */
void GTIM1_IRQHandler(void);
void GTIM2_IRQHandler(void);
void GTIM3_IRQHandler(void);
void GTIM4_IRQHandler(void);
void GTIM5_IRQHandler(void);
void GTIM6_IRQHandler(void);
void GTIM7_IRQHandler(void);

/* GPIO EXTI interrupt handlers */
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);
void EXTI4_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQHandler(void);

/* UART interrupt handlers */
void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void USART3_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MPS_IT_H__ */
