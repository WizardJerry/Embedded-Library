#include "mps_adc.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_adc.h"
#include "n32h47x_48x_gpio.h"
/**
 * @brief Configure GPIO pin for ADC input
 * 
 * This function enables the appropriate GPIO clock and configures the specified
 * pin(s) as analog input for ADC operation.
 * 
 * @param GPIOx: GPIO module (GPIOA, GPIOB, GPIOC, GPIOD, etc.)
 * @param GpioPin: GPIO pin(s) to configure (can be ORed together)
 */
void adc_gpio_config(GPIO_Module* GPIOx, uint16_t GpioPin)
 {
   GPIO_InitType GPIO_InitStructure;
   
   /* Enable GPIO clock based on the port */
   if (GPIOx == GPIOA) {
     RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOA, ENABLE);
   } else if (GPIOx == GPIOB) {
     RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOB, ENABLE);
   } else if (GPIOx == GPIOC) {
     RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOC, ENABLE);
   } else if (GPIOx == GPIOD) {
     RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOD, ENABLE);
   } else if (GPIOx == GPIOE) {
     RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOE, ENABLE);
   } else if (GPIOx == GPIOF) {
     RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOF, ENABLE);
   } else if (GPIOx == GPIOG) {
     RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_GPIOG, ENABLE);
   }
   
   /* Initialize GPIO structure with default values */
   GPIO_InitStruct(&GPIO_InitStructure);
   
   /* Configure pin(s) as analog input */
   GPIO_InitStructure.Pin = GpioPin;
   GPIO_InitStructure.GPIO_Mode = GPIO_MODE_ANALOG;
   
   /* Apply configuration */
   GPIO_InitPeripheral(GPIOx, &GPIO_InitStructure);
 }


/**
 * @brief Initialize ADC module with full configuration
 * 
 * Performs complete ADC initialization including:
 * - Clock enable and configuration  
 * - ADC module initialization and calibration
 * - Resolution and conversion mode setup
 * 
 * @param ADCx: ADC peripheral (ADC1, ADC2, ADC3, or ADC4)
 * @param ADC_Channel: ADC channel to configure (e.g., ADC_CH_0, ADC_CH_1, etc.)
 * @param GPIOx: GPIO port for ADC input (GPIOA, GPIOB, etc.)
 * @param GPIO_Pin: GPIO pin for ADC input (GPIO_PIN_0, GPIO_PIN_1, etc.)
 * @param resolution: ADC resolution (6/8/10/12 bit)
 * @param continuous: true for continuous conversion, false for single
 * @param sampleTime: ADC sampling time (ADC_SAMP_TIME_CYCLES_1_5 to ADC_SAMP_TIME_CYCLES_601_5)
 */
void adc_init(ADC_Module* ADCx, uint8_t ADC_Channel, GPIO_Module* GPIOx, uint16_t GPIO_Pin, adc_resolution_t resolution, bool continuous, uint8_t sampleTime)
{

  adc_gpio_config(GPIOx, GPIO_Pin);

  ADC_InitType ADC_InitStructure;
  
  /* Configure RCC - Enable ADC clock */
  if (ADCx == ADC1) {
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_ADC1, ENABLE);
  } else if (ADCx == ADC2) {
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_ADC2, ENABLE);
  } else if (ADCx == ADC3) {
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_ADC3, ENABLE);
  } else if (ADCx == ADC4) {
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_ADC4, ENABLE);
  }
  
  /* Configure ADC clock based on official examples */
  ADC_ConfigClk(ADC_CTRL3_CKMOD_AHB, RCC_ADCHCLK_DIV16);
  
  /* Configure ADC1M clock - HSI/8 = 8MHz/8 = 1MHz */
  RCC_ConfigAdc1mClk(RCC_ADC1MCLK_SRC_HSI, RCC_ADC1MCLK_DIV8);
  
  /* Initialize ADC structure */
  ADC_InitStruct(&ADC_InitStructure);
  ADC_InitStructure.WorkMode = ADC_WORKMODE_INDEPENDENT;
  ADC_InitStructure.MultiChEn = DISABLE;
  ADC_InitStructure.ContinueConvEn = continuous ? ENABLE : DISABLE;
  ADC_InitStructure.ExtTrigSelect = ADC_EXT_TRIG_REG_CONV_SOFTWARE;
  ADC_InitStructure.DatAlign = ADC_DAT_ALIGN_R;
  ADC_InitStructure.ChsNumber = 1;
  /* Set resolution based on user parameter */
  switch (resolution) {
    case ADC_RESOLUTION_6BIT:  ADC_InitStructure.Resolution = ADC_DATA_RES_6BIT; break;
    case ADC_RESOLUTION_8BIT:  ADC_InitStructure.Resolution = ADC_DATA_RES_8BIT; break;
    case ADC_RESOLUTION_10BIT: ADC_InitStructure.Resolution = ADC_DATA_RES_10BIT; break;
    case ADC_RESOLUTION_12BIT: ADC_InitStructure.Resolution = ADC_DATA_RES_12BIT; break;
    default: ADC_InitStructure.Resolution = ADC_DATA_RES_12BIT; break;
  }
  ADC_Init(ADCx, &ADC_InitStructure);
  
  /* Enable ADC */
  ADC_Enable(ADCx, ENABLE);
  
  /* Wait for ADC ready */
  while (ADC_GetFlagStatus(ADCx, ADC_FLAG_RDY) == RESET) {
    // Wait for ADC ready
  }
  
  /* Start calibration */
  ADC_CalibrationOperation(ADCx, ADC_CALIBRATION_SINGLE_MODE);
  
  /* Wait for calibration complete */
  while (ADC_GetCalibrationStatus(ADCx, ADC_CALIBRATION_SINGLE_MODE)) {
    // Wait for calibration complete
  }
  
  /* Configure ADC channel with user-defined sampling time */
  ADC_ConfigRegularChannel(ADCx, ADC_Channel, 1, sampleTime);
  
  /* If continuous mode is enabled, start continuous conversion */
  if (continuous) {
    ADC_EnableSoftwareStartConv(ADCx, ENABLE);
  }
}


/**
 * @brief Read ADC value from pre-configured channel
 * 
 * This function intelligently reads ADC data based on conversion mode:
 * - For continuous mode: waits for next conversion completion
 * - For single mode: triggers conversion and waits for completion
 * 
 * @param ADCx: ADC module to use (ADC1, ADC2, ADC3, ADC4)
 * @return: Digital conversion result (0-4095 for 12-bit resolution)
 * 
 */
uint16_t adc_read(ADC_Module* ADCx)
{
  /* Check ADC conversion mode by reading CTU bit (0=single, 1=continuous) */
  if ((ADCx->CTRL2 & ADC_CONT_EN_MASK) == 0) {
    /* CTU=0: Single conversion mode */
    /* Trigger new conversion and wait for completion */
    ADC_EnableSoftwareStartConv(ADCx, ENABLE);
    while (ADC_GetFlagStatus(ADCx, ADC_FLAG_ENDC) == RESET);
    /* Clear completion flag after reading */
    ADC_ClearFlag(ADCx, ADC_FLAG_ENDC);
    ADC_ClearFlag(ADCx, ADC_FLAG_STR);
  }  
  /* Return conversion result */
  return ADC_GetDat(ADCx);
}


/**
 * @brief Start ADC conversion (direct wrapper)
 */
void adc_start(ADC_Module* ADCx)
{
  ADC_EnableSoftwareStartConv(ADCx, ENABLE);
}


/**
 * @brief Stop ADC conversion (direct wrapper)
 */
void adc_stop(ADC_Module* ADCx)
{
  ADC_EnableSoftwareStartConv(ADCx, DISABLE);
}


/**
 * @brief ADC Test Function - Read voltage from PA0 pin
 * 
 * This function provides a simple test interface for ADC functionality.
 * It initializes ADC1, reads the voltage on PA0 pin, and returns the voltage value.
 * 
 * @return: Voltage value in mV (0-3300mV for 0-3.3V input)
 * 
 * @note: Test Pin: PA0 (ADC1_IN3)
 *        - Connect your test voltage to PA0 pin
 *        - Voltage range: 0V to 3.3V
 *        - Resolution: ~0.8mV (12-bit ADC)
 */
uint16_t adc_test_voltage_pa0(void)
{
  /* Read ADC value from PA0 (ADC1_IN3) */
  uint16_t adc_value = adc_read(ADC1);
  
  /* Convert ADC value to voltage (mV) */
  /* Formula: voltage_mV = (adc_value * 3300) / 4095 */
  uint32_t voltage_mv = ((uint32_t)adc_value * 3300) / 4095;
  
  return (uint16_t)voltage_mv;
}