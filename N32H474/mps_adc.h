#ifndef __MPS_ADC_H__
#define __MPS_ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "n32h47x_48x_adc.h"
#include "n32h47x_48x_gpio.h"
#include <stdbool.h>
#include <stddef.h>


typedef enum {
  ADC_RESOLUTION_6BIT = 0,      // 6-bit resolution (0-63, fastest)
  ADC_RESOLUTION_8BIT,          // 8-bit resolution (0-255, fast)
  ADC_RESOLUTION_10BIT,         // 10-bit resolution (0-1023, medium)
  ADC_RESOLUTION_12BIT          // 12-bit resolution (0-4095, highest precision)
} adc_resolution_t;


void adc_init(ADC_Module* ADCx,uint8_t ADC_Channel,GPIO_Module* GPIOx, uint16_t GPIO_Pin, adc_resolution_t resolution, bool continuous, uint8_t sampleTime);
uint16_t adc_read(ADC_Module* ADCx);
void adc_start(ADC_Module* ADCx);
void adc_stop(ADC_Module* ADCx);
void adc_gpio_config(GPIO_Module* GPIOx, uint16_t GpioPin);
uint16_t adc_test_voltage_pa0(void);

#ifdef __cplusplus
}
#endif

#endif /* __MPS_ADC_H__ */
