#ifndef __MPS_HRPWM_H__
#define __MPS_HRPWM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32h47x_48x_shrtim.h"
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief SHRTIM timer selection
 */
typedef enum {
  HRPWM_TIMER_A = 0,    // SHRTIM Timer A
  HRPWM_TIMER_B,        // SHRTIM Timer B  
  HRPWM_TIMER_C,        // SHRTIM Timer C
  HRPWM_TIMER_D,        // SHRTIM Timer D
  HRPWM_TIMER_E,        // SHRTIM Timer E
  HRPWM_TIMER_F         // SHRTIM Timer F
} hrpwm_timer_t;

/**
 * @brief PWM output channel
 */
typedef enum {
  HRPWM_CHANNEL_1 = 0,  // Channel 1 output
  HRPWM_CHANNEL_2       // Channel 2 output (complementary)
} hrpwm_channel_t;

void configure_hrpwm_gpio(void);
void hrpwm_init(SHRTIM_Module* SHRTIMx, hrpwm_timer_t timer, uint32_t frequencyHz, float dutyPercent, uint16_t deadtimeNs);
void hrpwm_set_duty(SHRTIM_Module* SHRTIMx, hrpwm_timer_t timer, float dutyPercent);
void hrpwm_start(SHRTIM_Module* SHRTIMx, hrpwm_timer_t timer);
void hrpwm_stop(SHRTIM_Module* SHRTIMx, hrpwm_timer_t timer);
void hrpwm_set_frequency(SHRTIM_Module* SHRTIMx, hrpwm_timer_t timer, uint32_t frequencyHz);
void hrpwm_set_phase(SHRTIM_Module* SHRTIMx, hrpwm_timer_t timer, float phasePercent);
void hrpwm_set_deadtime(SHRTIM_Module* SHRTIMx, hrpwm_timer_t timer, uint16_t deadtimeNs);

#ifdef __cplusplus
}
#endif

#endif /* __MPS_HRPWM_H__ */
