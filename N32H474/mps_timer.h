#ifndef __MPS_TIMER_H__
#define __MPS_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "n32h47x_48x_tim.h"
#include <stdbool.h>
#include <stddef.h>
#include "mps_it.h"

void timer_init(TIM_Module* TIMx, uint32_t periodUs, timer_callback_t callBack);
void timer_enable_interrupt(TIM_Module* TIMx, uint8_t priority);
void timer_start(TIM_Module* TIMx);
void timer_stop(TIM_Module* TIMx);
void timer_set_callback(TIM_Module* TIMx, timer_callback_t callBack);

#ifdef __cplusplus
}
#endif

#endif /* __MPS_TIMER_H__ */
