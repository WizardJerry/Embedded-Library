#include "mps_hrpwm.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_gpio.h"

/* SHRTIM input clock frequency (250MHz) */
#define SHRTIM_INPUT_CLOCK  250000000

/* Internal structure to store PWM configurations */
typedef struct {
  uint32_t period;
  uint32_t compareValue;
  uint32_t prescalerMultiplier;  /* Stores the prescaler multiplier used */
  bool is_initialized;
} hrpwm_config_t;

/* Static storage for each timer's configuration */
static hrpwm_config_t hrpwmConfigs[6] = {{0}};  // Timer A-F

/* Static function declarations */

static uint32_t get_shrtim_timer(hrpwm_timer_t timer);
static uint32_t get_shrtim_output(hrpwm_timer_t timer, hrpwm_channel_t channel);

/**
 * @brief Calculate frequency parameters (prescaler and period)
 * @param frequencyHz: Target frequency in Hz
 * @param prescalerMultiplier: Output prescaler multiplier
 * @param shrtimPrescaler: Output SHRTIM prescaler constant
 * @return Calculated period value
 */
static uint32_t calculate_frequency_parameters(uint32_t frequencyHz, uint32_t* prescalerMultiplier, uint32_t* shrtimPrescaler)
{
  uint32_t period;
  
  /* Choose appropriate prescaler based on frequency range */
  if (frequencyHz >= 100000) {        /* >= 100kHz: use DIV1 (×1) */
    *prescalerMultiplier = 1;
    *shrtimPrescaler = SHRTIM_PRESCALERRATIO_DIV1;
  } else if (frequencyHz >= 50000) {  /* >= 50kHz: use MUL2 */
    *prescalerMultiplier = 2;
    *shrtimPrescaler = SHRTIM_PRESCALERRATIO_MUL2;
  } else if (frequencyHz >= 25000) {  /* >= 25kHz: use MUL4 */
    *prescalerMultiplier = 4;
    *shrtimPrescaler = SHRTIM_PRESCALERRATIO_MUL4;
  } else if (frequencyHz >= 12500) {  /* >= 12.5kHz: use MUL8 */
    *prescalerMultiplier = 8;
    *shrtimPrescaler = SHRTIM_PRESCALERRATIO_MUL8;
  } else if (frequencyHz >= 6250) {   /* >= 6.25kHz: use MUL16 */
    *prescalerMultiplier = 16;
    *shrtimPrescaler = SHRTIM_PRESCALERRATIO_MUL16;
  } else {                           /* < 6.25kHz: use MUL32 */
    *prescalerMultiplier = 32;
    *shrtimPrescaler = SHRTIM_PRESCALERRATIO_MUL32;
  }
  
  /* Calculate period using the selected prescaler */
  period = (uint32_t)(((uint64_t)SHRTIM_INPUT_CLOCK * (*prescalerMultiplier)) / frequencyHz);
  
  return period;
}

/**
 * @brief Calculate duty cycle compare value with high precision
 * @param dutyPercent: Duty cycle in percent (0.0-100.0, supports decimal places)
 * @param period: Timer period value
 * @return Calculated compare value
 */
static uint32_t calculate_duty_compare_value(float dutyPercent, uint32_t period)
{
  if (dutyPercent > 100.0f) dutyPercent = 100.0f;
  if (dutyPercent < 0.0f) dutyPercent = 0.0f;
  
  /* Use high precision calculation like official example */
  /* compareValue = (duty_percent * (period - 1)) / 100 */
  return (uint32_t)((dutyPercent * (float)(period - 1)) / 100.0f);
}

/**
 * @brief Configure GPIO pins for SHRTIM high-resolution PWM complementary outputs
 * 
 * This function sets up GPIO pins for SHRTIM Timer D complementary PWM operation.
 * It configures two pins on GPIOB for dual-channel complementary PWM output:
 * - PB14: TD1 (Timer D Channel 1) - High-side output for upper switch control
 * - PB15: TD2 (Timer D Channel 2) - Low-side output for lower switch control (complementary)
 */
 void configure_hrpwm_gpio(GPIOxA, pinA , GPIOxB, pinB)
 {
   GPIO_InitType GPIO_InitStructure;
   
   /* Enable GPIOB clock */
   RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOB, ENABLE);
   
   /* Initialize GPIO structure */
   GPIO_InitStruct(&GPIO_InitStructure);
   
   /* Configure GPIO for SHRTIM outputs */
   GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AF_PP;        // Alternate function push-pull
   GPIO_InitStructure.GPIO_Slew_Rate = GPIO_SLEW_RATE_FAST;  // Fast slew rate for PWM
   GPIO_InitStructure.GPIO_Pull = GPIO_PULL_UP;           // Pull-up for safety
   GPIO_InitStructure.GPIO_Alternate = GPIO_AF10;        // SHRTIM alternate function
   GPIO_InitStructure.Pin = GPIO_PIN_14 | GPIO_PIN_15;   // PB14(TD1), PB15(TD2)
   
   /* Apply configuration */
   GPIO_InitPeripheral(GPIOB, &GPIO_InitStructure);
 }


/**
 * @brief Initialize High-Resolution PWM for complementary output with dead time
 * @param SHRTIMx: Pointer to SHRTIM peripheral instance (typically SHRTIM1)
 * @param timer: SHRTIM Timer unit selection (HRPWM_TIMER_A to HRPWM_TIMER_F)
 *   - HRPWM_TIMER_D recommended (uses PB14/PB15 pins)
 * @param frequencyHz: PWM frequency in Hertz (3,815 Hz to 25 MHz supported)
 *   - Hardware minimum: ~3.8kHz (with automatic prescaler selection)
 *   - Theoretical maximum: 250MHz, practical maximum: ~25MHz (for useful PWM resolution)
 *   - Higher frequencies: smaller components, better regulation, more losses
 *   - Typical motor control: 20-50kHz, Power supplies: 50-200kHz
 * @param dutyPercent: PWM duty cycle percentage (0.0-100.0, supports decimal like 67.5)
 * @param deadtimeNs: Dead time in nanoseconds (0-31875ns, resolution ~125ps)
 */
 void hrpwm_init(SHRTIM_Module* SHRTIMx, hrpwm_timer_t timer, uint32_t frequencyHz, float dutyPercent, uint16_t deadtimeNs)
 {
   uint32_t shrtimTimer = get_shrtim_timer(timer);
   uint32_t shrtimOutputCh1 = get_shrtim_output(timer, HRPWM_CHANNEL_1);
   uint32_t shrtimOutputCh2 = get_shrtim_output(timer, HRPWM_CHANNEL_2);
   uint32_t period;
   uint32_t prescalerMultiplier;
   uint32_t shrtimPrescaler;
   
     /* Calculate period and prescaler for desired frequency */
  period = calculate_frequency_parameters(frequencyHz, &prescalerMultiplier, &shrtimPrescaler);
   
   /* Store configuration */
  hrpwmConfigs[timer].period = period;
  hrpwmConfigs[timer].prescalerMultiplier = prescalerMultiplier;
  hrpwmConfigs[timer].is_initialized = true;
     
  /* Configure SHRTPLL */
  /* Enable HSE */
  RCC_ConfigHse(RCC_HSE_ENABLE);
  
  /* Wait till HSE is ready */
  RCC_WaitHseStable();
  
  /* Configure SHRTPLL: HSE=8MHz -> 250MHz */
  RCC_ConfigSHRTPll(RCC_SHRTPLL_SRC_HSE, 8000000, SHRTIM_INPUT_CLOCK, ENABLE);
  
  /* Wait till SHRTPLL is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_SHRTPLLRDF) != SET) {
    // Wait for PLL lock
  }
     
   /* Enable SHRTIM clock */
   RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_SHRTIM, ENABLE);
   
   
     /* Configure SHRTIM timer - basic settings */
  SHRTIM_TIM_SetPrescaler(SHRTIMx, shrtimTimer, shrtimPrescaler);
   SHRTIM_TIM_SetCounterMode(SHRTIMx, shrtimTimer, SHRTIM_MODE_CONTINUOUS);
   SHRTIM_TIM_SetPeriod(SHRTIMx, shrtimTimer, period);
   
   /* Configure Channel 1 (High-side) output timing */
   SHRTIM_OUT_SetOutputSetSrc(SHRTIMx, shrtimOutputCh1, SHRTIM_OUTPUTSET_TIMPER);
   SHRTIM_OUT_SetOutputResetSrc(SHRTIMx, shrtimOutputCh1, SHRTIM_OUTPUTRESET_TIMCMP1);
   
   /* Configure Channel 2 (Low-side) output timing for complementary operation */
   SHRTIM_OUT_SetOutputSetSrc(SHRTIMx, shrtimOutputCh2, SHRTIM_OUTPUTSET_TIMCMP1);  // Set when CH1 resets
   SHRTIM_OUT_SetOutputResetSrc(SHRTIMx, shrtimOutputCh2, SHRTIM_OUTPUTRESET_TIMPER);  // Reset when CH1 sets
   

  
  /* Set frequency using the dedicated function to verify it works correctly */
  hrpwm_set_frequency(SHRTIMx, timer, frequencyHz);
  
  /* Set duty cycle using the dedicated function to verify it works correctly */
  hrpwm_set_duty(SHRTIMx, timer, dutyPercent);
  

         /* Configure dead time for both channels to prevent shoot-through */
  hrpwm_set_deadtime(SHRTIMx, timer, deadtimeNs);
  
  /* Force update to apply all settings */
  SHRTIM_ForceUpdate(SHRTIMx, shrtimTimer);

 }


/**
 * @brief Get SHRTIM timer constant from enum
 */
static uint32_t get_shrtim_timer(hrpwm_timer_t timer)
{
  switch (timer) {
    case HRPWM_TIMER_A: return SHRTIM_TIMER_A;
    case HRPWM_TIMER_B: return SHRTIM_TIMER_B;
    case HRPWM_TIMER_C: return SHRTIM_TIMER_C;
    case HRPWM_TIMER_D: return SHRTIM_TIMER_D;
    case HRPWM_TIMER_E: return SHRTIM_TIMER_E;
    case HRPWM_TIMER_F: return SHRTIM_TIMER_F;
    default: return SHRTIM_TIMER_A;
  }
}


/**
 * @brief Get SHRTIM output constant from timer and channel
 */
static uint32_t get_shrtim_output(hrpwm_timer_t timer, hrpwm_channel_t channel)
{
  if (channel == HRPWM_CHANNEL_1) {
    switch (timer) {
      case HRPWM_TIMER_A: return SHRTIM_OUTPUT_TA1;
      case HRPWM_TIMER_B: return SHRTIM_OUTPUT_TB1;
      case HRPWM_TIMER_C: return SHRTIM_OUTPUT_TC1;
      case HRPWM_TIMER_D: return SHRTIM_OUTPUT_TD1;
      case HRPWM_TIMER_E: return SHRTIM_OUTPUT_TE1;
      case HRPWM_TIMER_F: return SHRTIM_OUTPUT_TF1;
      default: return SHRTIM_OUTPUT_TA1;
    }
  } else {
    switch (timer) {
      case HRPWM_TIMER_A: return SHRTIM_OUTPUT_TA2;
      case HRPWM_TIMER_B: return SHRTIM_OUTPUT_TB2;
      case HRPWM_TIMER_C: return SHRTIM_OUTPUT_TC2;
      case HRPWM_TIMER_D: return SHRTIM_OUTPUT_TD2;
      case HRPWM_TIMER_E: return SHRTIM_OUTPUT_TE2;
      case HRPWM_TIMER_F: return SHRTIM_OUTPUT_TF2;
      default: return SHRTIM_OUTPUT_TA2;
    }
  }
}


/**
 * @brief Set complementary PWM duty cycle
 * @param SHRTIMx: SHRTIM module (e.g., SHRTIM1)
 * @param timer: HRPWM timer selection
 * @param dutyPercent: New duty cycle in percent (0-100)
 */
void hrpwm_set_duty(SHRTIM_Module* SHRTIMx, hrpwm_timer_t timer, float dutyPercent)
{
  uint32_t shrtimTimer = get_shrtim_timer(timer);
  uint32_t compareValue;
  
  /* Calculate new compare value using the helper function */
  compareValue = calculate_duty_compare_value(dutyPercent, hrpwmConfigs[timer].period);
  hrpwmConfigs[timer].compareValue = compareValue;
  
  /* Update both channels for true complementary operation */
  SHRTIM_TIM_SetCompare1(SHRTIMx, shrtimTimer, compareValue);  // Channel 1 (high-side)
  SHRTIM_TIM_SetCompare2(SHRTIMx, shrtimTimer, compareValue);  // Channel 2 (starts when CH1 ends)
}


/**
 * @brief Start complementary PWM output
 * @param SHRTIMx: SHRTIM module (e.g., SHRTIM1)
 * @param timer: HRPWM timer selection
 */
void hrpwm_start(SHRTIM_Module* SHRTIMx, hrpwm_timer_t timer)
{
  uint32_t shrtimTimer = get_shrtim_timer(timer);
  uint32_t shrtimOutputCh1 = get_shrtim_output(timer, HRPWM_CHANNEL_1);
  uint32_t shrtimOutputCh2 = get_shrtim_output(timer, HRPWM_CHANNEL_2);
  
  /* Enable both complementary outputs simultaneously before starting counter */
  SHRTIM_EnableOutput(SHRTIMx, shrtimOutputCh1);
  SHRTIM_EnableOutput(SHRTIMx, shrtimOutputCh2);
  
  /* Start counter - both channels will start together */
  SHRTIM_TIM_CounterEnable(SHRTIMx, shrtimTimer);
}


/**
 * @brief Stop complementary PWM output
 * @param SHRTIMx: SHRTIM module (e.g., SHRTIM1)
 * @param timer: HRPWM timer selection
 */
void hrpwm_stop(SHRTIM_Module* SHRTIMx, hrpwm_timer_t timer)
{
  uint32_t shrtimTimer = get_shrtim_timer(timer);
  uint32_t shrtimOutputCh1 = get_shrtim_output(timer, HRPWM_CHANNEL_1);
  uint32_t shrtimOutputCh2 = get_shrtim_output(timer, HRPWM_CHANNEL_2);
  
  /* Disable both complementary outputs */
  SHRTIM_DisableOutput(SHRTIMx, shrtimOutputCh1);
  SHRTIM_DisableOutput(SHRTIMx, shrtimOutputCh2);
  
  /* Stop counter */
  SHRTIM_TIM_CounterDisable(SHRTIMx, shrtimTimer);
}


/**
 * @brief Set PWM frequency
 */
void hrpwm_set_frequency(SHRTIM_Module* SHRTIMx, hrpwm_timer_t timer, uint32_t frequencyHz)
{
  uint32_t shrtimTimer = get_shrtim_timer(timer);
  uint32_t period;
  uint32_t compareValue;
  uint8_t currentDuty;
  uint32_t prescalerMultiplier;
  uint32_t shrtimPrescaler;
  
  /* Calculate current duty cycle percentage */
  currentDuty = (uint8_t)(((uint64_t)hrpwmConfigs[timer].compareValue * 100) / hrpwmConfigs[timer].period);
  
  /* Calculate new period and prescaler using the helper function */
  period = calculate_frequency_parameters(frequencyHz, &prescalerMultiplier, &shrtimPrescaler);
  
  /* Calculate new compare value to maintain duty cycle */
  compareValue = (uint32_t)(((uint64_t)currentDuty * period) / 100);
  
  /* Update configuration */
  hrpwmConfigs[timer].period = period;
  hrpwmConfigs[timer].compareValue = compareValue;
  hrpwmConfigs[timer].prescalerMultiplier = prescalerMultiplier;
  
  /* Update SHRTIM registers */
  SHRTIM_TIM_SetPrescaler(SHRTIMx, shrtimTimer, shrtimPrescaler);
  SHRTIM_TIM_SetPeriod(SHRTIMx, shrtimTimer, period);
  SHRTIM_TIM_SetCompare1(SHRTIMx, shrtimTimer, compareValue);
  
  /* Force update - verified from official examples */
  SHRTIM_ForceUpdate(SHRTIMx, shrtimTimer);
}


/**
 * @brief Set complementary PWM phase shift 
 * @param SHRTIMx: SHRTIM module (e.g., SHRTIM1)
 * @param timer: HRPWM timer selection
 * @param phasePercent: Phase shift in percent (0.0-100.0, supports decimal like 10.5)
 *   - High precision: 0.1% = 0.36° @ 100kHz, 0.036° @ 50kHz
 *   - Shifts both channels together maintaining complementary relationship
 */
void hrpwm_set_phase(SHRTIM_Module* SHRTIMx, hrpwm_timer_t timer, float phasePercent)
{
  uint32_t shrtimTimer = get_shrtim_timer(timer);
  uint32_t period;
  uint32_t phaseOffset;
  uint32_t compareValueCh1, compareValueCh2;
  
  period = hrpwmConfigs[timer].period;
  
  /* Validate and clamp phase percentage */
  if (phasePercent > 100.0f) phasePercent = 100.0f;
  if (phasePercent < 0.0f) phasePercent = 0.0f;
  
  /* Calculate phase offset with high precision: Phase = (period * phasePercent) / 100 */
  phaseOffset = (uint32_t)((phasePercent * (float)period) / 100.0f);
  
  /* Get current duty cycle compare value and add phase offset for both channels */
  compareValueCh1 = hrpwmConfigs[timer].compareValue + phaseOffset;
  compareValueCh2 = hrpwmConfigs[timer].compareValue + phaseOffset;  // True complementary: both channels use same compare value
  
  /* Wrap around if exceeds period */
  if (compareValueCh1 >= period) {
    compareValueCh1 -= period;
  }
  if (compareValueCh2 >= period) {
    compareValueCh2 -= period;
  }
  
  /* Set phase-shifted compare values for both complementary channels */
  SHRTIM_TIM_SetCompare1(SHRTIMx, shrtimTimer, compareValueCh1);
  SHRTIM_TIM_SetCompare2(SHRTIMx, shrtimTimer, compareValueCh2);
}


/**
 * @brief Set dead time for complementary outputs using mixed deadtime signs with compensation
 * @param SHRTIMx: SHRTIM module (e.g., SHRTIM1)
 * @param timer: HRPWM timer
 * @param deadtimeNs: Dead time in nanoseconds (0-31875ns, resolution 125ps based on 8GHz high-res clock)
 * @note Uses MIXED deadtime (SDTR=1 negative rising, SDTF=0 positive falling) to balance duty cycle
 * @note Automatically adds 25% compensation to achieve target deadtime in mixed mode
 * @note This function automatically locks dead time sign to prevent accidental modification
 */
void hrpwm_set_deadtime(SHRTIM_Module* SHRTIMx, hrpwm_timer_t timer, uint16_t deadtimeNs)
{
  uint32_t shrtimTimer = get_shrtim_timer(timer);
  uint32_t deadtimeTicks;
  uint32_t dtPrescaler;
  
  /* Use official calculation formula - consistent with PWM prescaler */
  uint32_t prescalerMultiplier = hrpwmConfigs[timer].prescalerMultiplier;
  
  /* Compensate for mixed deadtime mode: increase by 25% to account for timing reduction */
  uint16_t compensatedDeadtimeNs = deadtimeNs + (deadtimeNs / 4);  // Add 25% compensation
  deadtimeTicks = (uint32_t)(((uint64_t)SHRTIM_INPUT_CLOCK * prescalerMultiplier * compensatedDeadtimeNs) / 1000000000ULL);
  
  /* Map PWM prescaler to dead time prescaler */
  switch (prescalerMultiplier) {
    case 1:  dtPrescaler = SHRTIM_DT_PRESCALER_DIV1; break;     // DIV1 → DIV1
    case 2:  dtPrescaler = SHRTIM_DT_PRESCALER_MUL2; break;     // MUL2 → MUL2
    case 4:  dtPrescaler = SHRTIM_DT_PRESCALER_MUL4; break;     // MUL4 → MUL4
    case 8:  dtPrescaler = SHRTIM_DT_PRESCALER_MUL8; break;     // MUL8 → MUL8
    case 16: dtPrescaler = SHRTIM_DT_PRESCALER_MUL8; deadtimeTicks /= 2; break; // MUL16 → MUL8 (compensate)
    case 32: dtPrescaler = SHRTIM_DT_PRESCALER_MUL8; deadtimeTicks /= 4; break; // MUL32 → MUL8 (compensate)
    default: dtPrescaler = SHRTIM_DT_PRESCALER_DIV1; break;
  }
  
  /* Step 1: Enable dead time first (following official example sequence) */
  SHRTIM_TIM_EnableDeadTime(SHRTIMx, shrtimTimer);
  
  /* Step 2: Configure dead time with MIXED sign (rising negative, falling positive) to balance duty cycle */
  uint32_t dt_config = dtPrescaler | SHRTIM_DT_RISING_NEGATIVE | SHRTIM_DT_FALLING_POSITIVE;
  SHRTIM_DT_Config(SHRTIMx, shrtimTimer, dt_config);
  
  /* Step 3: Set dead time values for rising and falling edges */
  SHRTIM_DT_SetRisingValue(SHRTIMx, shrtimTimer, deadtimeTicks);
  SHRTIM_DT_SetFallingValue(SHRTIMx, shrtimTimer, deadtimeTicks);
  
  /* Step 4: Lock dead time sign for both rising and falling edges to prevent accidental modification */
  SHRTIM_DT_LockRisingSign(SHRTIMx, shrtimTimer);
  SHRTIM_DT_LockFallingSign(SHRTIMx, shrtimTimer);
  
  /* Force update to ensure settings take effect immediately */
  SHRTIM_ForceUpdate(SHRTIMx, shrtimTimer);
}
