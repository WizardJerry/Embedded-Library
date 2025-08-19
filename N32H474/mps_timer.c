#include "mps_timer.h"
#include "n32h47x_48x_tim.h"
#include "n32h47x_48x_rcc.h"
#include "misc.h"
#include <stddef.h>

/* External system clock variable */
extern uint32_t SystemCoreClock;

/* Static array to store callBack functions for GTIM1-7 */
static timer_callback_t timer_callbacks[7] = {NULL};


/**
 * @brief Get timer IRQ channel
 * @param TIMx: Timer peripheral
 * @return IRQ channel or -1 if invalid
 */
static IRQn_Type get_timer_irq(TIM_Module* TIMx)
{
  if (TIMx == GTIM1) return GTIM1_IRQn;
  else if (TIMx == GTIM2) return GTIM2_IRQn;
  else if (TIMx == GTIM3) return GTIM3_IRQn;
  else if (TIMx == GTIM4) return GTIM4_IRQn;
  else if (TIMx == GTIM5) return GTIM5_IRQn;
  else if (TIMx == GTIM6) return GTIM6_IRQn;
  else if (TIMx == GTIM7) return GTIM7_IRQn;
  else return (IRQn_Type)-1;
}

/**
 * @brief Initialize timer - super simple version
 * @param TIMx: Timer peripheral (GTIM1-GTIM7) 
 * @param periodUs: Timer period in microseconds (10-42949017, any value)
 * @param callBack: Your function to call every periodUs (like LED_ON)
 */
void timer_init(TIM_Module* TIMx, uint32_t periodUs, timer_callback_t callBack)
{
  TIM_TimeBaseInitType timer_config;
  
  /* Configure RCC for the timer */
  RCC_ConfigPclk1(RCC_HCLK_DIV4);
  
  if (TIMx == GTIM1) {
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_GTIM1, ENABLE);
  } else if (TIMx == GTIM2) {
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_GTIM2, ENABLE);
  } else if (TIMx == GTIM3) {
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_GTIM3, ENABLE);
  } else if (TIMx == GTIM4) {
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_GTIM4, ENABLE);
  } else if (TIMx == GTIM5) {
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_GTIM5, ENABLE);
  } else if (TIMx == GTIM6) {
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_GTIM6, ENABLE);
  } else if (TIMx == GTIM7) {
    RCC_EnableAPB1PeriphClk(RCC_APB1_PERIPH_GTIM7, ENABLE);
  }
  
  uint32_t timerClock = SystemCoreClock / 2;  // Timer clock = HCLK/2 (auto ×2 rule)
  uint64_t totalCycles = ((uint64_t)periodUs * timerClock) / 1000000;
  uint32_t prescaler = (uint32_t)((totalCycles + 65535) / 65536);
  uint32_t period = (uint32_t)(totalCycles / prescaler) - 1;
  
  timer_config.Prescaler = prescaler - 1;  // Register value (prescaler - 1)
  timer_config.Period = period;
  timer_config.CounterMode = TIM_CNT_MODE_UP;      // Up counting mode
  timer_config.ClkDiv = TIM_CLK_DIV1;              // No clock division
  timer_config.RepetCnt = 0;                       // Repetition counter (only for ATIM)
  

  IRQn_Type irq = get_timer_irq(TIMx);
  NVIC_InitType NVIC_InitStructure;
  
  /* Enable timer update interrupt */
  TIM_ConfigInt(TIMx, TIM_INT_UPDATE, ENABLE);
  
  /* Configure NVIC */
  NVIC_InitStructure.NVIC_IRQChannel = irq;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Apply timer configuration */
  TIM_InitTimeBase(TIMx, &timer_config);
  
  /* Configure prescaler with immediate reload */
  TIM_ConfigPrescaler(TIMx, timer_config.Prescaler, TIM_PSC_RELOAD_MODE_IMMEDIATE);
  /* Set callBack function */
  timer_set_callback(TIMx, callBack);
}


/**
 * @brief Start timer counting
 * @param TIMx: Timer peripheral
 */
void timer_start(TIM_Module* TIMx)
{
  TIM_Enable(TIMx, ENABLE);
}

/**
 * @brief Stop timer counting
 * @param TIMx: Timer peripheral
 */
void timer_stop(TIM_Module* TIMx)
{
  TIM_Enable(TIMx, DISABLE);
}

/**
 * @brief Set timer callback function
 * @param TIMx: Timer peripheral
 * @param callBack: Callback function to be called on timer interrupt
 */
void timer_set_callback(TIM_Module* TIMx, timer_callback_t callBack)
{
  // Direct assignment - can be NULL
  if (TIMx == GTIM1) timer_callbacks[0] = callBack;
  else if (TIMx == GTIM2) timer_callbacks[1] = callBack;
  else if (TIMx == GTIM3) timer_callbacks[2] = callBack;
  else if (TIMx == GTIM4) timer_callbacks[3] = callBack;
  else if (TIMx == GTIM5) timer_callbacks[4] = callBack;
  else if (TIMx == GTIM6) timer_callbacks[5] = callBack;
  else if (TIMx == GTIM7) timer_callbacks[6] = callBack;
}

/* Timer interrupt handlers */

/**
 * @brief GTIM1 interrupt handler
 */
void GTIM1_IRQHandler(void)
{
  if (TIM_GetIntStatus(GTIM1, TIM_INT_UPDATE) != RESET) {
    TIM_ClrIntPendingBit(GTIM1, TIM_INT_UPDATE);  // Clear interrupt flag
    if (timer_callbacks[0] != NULL) {
      timer_callbacks[0]();  // Call user callBack function
    }
  }
}

/**
 * @brief GTIM2 interrupt handler
 */
void GTIM2_IRQHandler(void)
{
  if (TIM_GetIntStatus(GTIM2, TIM_INT_UPDATE) != RESET) {
    TIM_ClrIntPendingBit(GTIM2, TIM_INT_UPDATE);
    if (timer_callbacks[1] != NULL) {
      timer_callbacks[1]();  // Call user callBack function
        }
    }
}

/**
 * @brief GTIM3 interrupt handler
 */
void GTIM3_IRQHandler(void)
{
  if (TIM_GetIntStatus(GTIM3, TIM_INT_UPDATE) != RESET) {
    TIM_ClrIntPendingBit(GTIM3, TIM_INT_UPDATE);
    if (timer_callbacks[2] != NULL) {
      timer_callbacks[2]();  // Call user callBack function
        }
    }
}

/**
 * @brief GTIM4 interrupt handler
 */
void GTIM4_IRQHandler(void)
{
  if (TIM_GetIntStatus(GTIM4, TIM_INT_UPDATE) != RESET) {
    TIM_ClrIntPendingBit(GTIM4, TIM_INT_UPDATE);
    if (timer_callbacks[3] != NULL) {
      timer_callbacks[3]();  // Call user callBack function
        }
    }
}

/**
 * @brief GTIM5 interrupt handler
 */
void GTIM5_IRQHandler(void)
{
  if (TIM_GetIntStatus(GTIM5, TIM_INT_UPDATE) != RESET) {
    TIM_ClrIntPendingBit(GTIM5, TIM_INT_UPDATE);
    if (timer_callbacks[4] != NULL) {
      timer_callbacks[4]();  // Call user callBack function
        }
    }
}

/**
 * @brief GTIM6 interrupt handler
 */
void GTIM6_IRQHandler(void)
{
  if (TIM_GetIntStatus(GTIM6, TIM_INT_UPDATE) != RESET) {
    TIM_ClrIntPendingBit(GTIM6, TIM_INT_UPDATE);
    if (timer_callbacks[5] != NULL) {
      timer_callbacks[5]();  // Call user callBack function
        }
    }
}

/**
 * @brief GTIM7 interrupt handler
 */
void GTIM7_IRQHandler(void)
{
  if (TIM_GetIntStatus(GTIM7, TIM_INT_UPDATE) != RESET) {
    TIM_ClrIntPendingBit(GTIM7, TIM_INT_UPDATE);
    if (timer_callbacks[6] != NULL) {
      timer_callbacks[6]();  // Call user callBack function
        }
    }
}
