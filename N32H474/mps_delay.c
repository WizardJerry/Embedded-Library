#include "mps_delay.h"
#include "misc.h"
#include "n32h47x_48x_rcc.h"
unsigned char frequency;//command frequency
static __inline void DWT_Delay_us(volatile uint32_t microseconds);
/**
 *\*\name    DBG_SysTick_Config.
 *\*\fun     System tick configuration.
 *\*\param   ticks :system tick
 *\*\return  none
**/
static uint32_t DBG_SysTick_Config(uint32_t ticks)
{ 
  if (ticks > SysTick_LOAD_RELOAD_Msk)  return (1);            /* Reload value impossible */
   
  SysTick->LOAD  = (ticks & SysTick_LOAD_RELOAD_Msk) - 1;      /* set reload register */
  //NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);  /* set Priority for Cortex-M4 System Interrupts */
  SysTick->VAL   = 0;                                          /* Load the SysTick Counter Value */
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | 
   SysTick_CTRL_ENABLE_Msk;                    /* Enable SysTick IRQ and SysTick Timer */
  SysTick->CTRL  &= (~SysTick_CTRL_TICKINT_Msk);
  return (0);                                                  /* Function successful */
}

/**
 *\*\name    systick_delay_us.
 *\*\fun     microsecond delay.
 *\*\param   us :any number
 *\*\return  none
**/
void systick_delay_us(uint32_t us)
{
  uint32_t i;
  RCC_ClocksType RCC_Clocks;

  RCC_GetClocksFreqValue(&RCC_Clocks);
  DBG_SysTick_Config(RCC_Clocks.SysclkFreq / 1000000);

  for(i = 0; i < us; i++)
  {
  /* When the counter value decreases to 0, bit 16 of the CRTL register will be set to 1 */
  /* When set to 1, reading this bit will clear it to 0 */
  while( !((SysTick->CTRL) & (1 << 16)));
  }
  /* Turn off the SysTick timer */
  SysTick->CTRL &= (~SysTick_CTRL_ENABLE_Msk);
}

/**
 *\*\name    systick_delay_ms.
 *\*\fun     millisecond delay.
 *\*\param   ms :any number
 *\*\return  none
**/
void systick_delay_ms(uint32_t ms)
{
  uint32_t i;
  RCC_ClocksType RCC_Clocks;
  
  RCC_GetClocksFreqValue(&RCC_Clocks);
  DBG_SysTick_Config(RCC_Clocks.SysclkFreq / 1000);

  for(i = 0; i < ms; i++)
  {
  /* When the counter value decreases to 0, bit 16 of the CRTL register will be set to 1 */
  /* When set to 1, reading this bit will clear it to 0 */
  while( !((SysTick->CTRL) & (1 << 16)));
  }
  /* Turn off the SysTick timer */
  SysTick->CTRL &= (~SysTick_CTRL_ENABLE_Msk);
}

static __inline void DWT_Delay_us(volatile uint32_t microseconds)
{
  uint32_t clk_cycle_start = DWT->CYCCNT;

  /* Go to number of cycles for system */
  microseconds *= (frequency);

  /* Delay till end */
  while ((DWT->CYCCNT - clk_cycle_start) < microseconds);
}
