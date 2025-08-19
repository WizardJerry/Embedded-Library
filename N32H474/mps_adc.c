#include "mps_adc.h"
#include "n32h47x_48x_rcc.h"
#include "n32h47x_48x_adc.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_dma.h"
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
void adc_init_single_channel_scan(ADC_Module* ADCx, uint8_t ADC_Channel, GPIO_Module* GPIOx, uint16_t GPIO_Pin, adc_resolution_t resolution, bool continuous, uint8_t sampleTime)
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
  ADC_InitStructure.Resolution = resolution;
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
 * @brief Initialize any ADC module for flexible multi-channel scanning mode
 * 
 * This function configures the specified ADC (ADC1/2/3/4) to scan any number of channels (2-16) in sequence.
 * You can choose any combination of GPIO pins and ADC channels.
 * Results are stored in the provided array via DMA in the same order as channel configuration.
 * 
 * @param ADCx: ADC peripheral (ADC1, ADC2, ADC3, or ADC4)
 * @param channels: Array of channel configurations (GPIO port, pin, ADC channel)
 * @param channelCount: Number of channels to scan (2-16)
 * @param resolution: ADC resolution (6/8/10/12 bit)
 * @param adcResults: Array to store ADC conversion results 
 * @param continuous: true for continuous scanning, false for single scan
 * 
 * @note Maximum 16 channels supported by ADC hardware
 * @note Results are stored in adcResults[] in the same order as channels[] array
 * @note Different ADCs use different DMA channels automatically
 * 
 */
void adc_init_multichannel_scan(ADC_Module* ADCx, adc_channel_config_t* channels, uint8_t channelCount, adc_resolution_t resolution, uint16_t* adcResults, bool continuous, uint8_t sampleTime)
{
  /* Configure GPIO pins for ADC inputs dynamically */
  for (uint8_t i = 0; i < channelCount; i++) {
    adc_gpio_config(channels[i].gpio_port, channels[i].gpio_pin);
  }

  /* Enable ADC and DMA clocks based on ADC module */
  uint32_t DMA_CHx;
  
  if (ADCx == ADC1) {
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_ADC1, ENABLE);
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_DMA1, ENABLE);
    DMA_CHx = (uint32_t)DMA1_CH1;
  } 
  else if (ADCx == ADC2) {
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_ADC2, ENABLE);
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_DMA1, ENABLE);
    DMA_CHx = (uint32_t)DMA1_CH2;
  }
  else if (ADCx == ADC3) {
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_ADC3, ENABLE);
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_DMA2, ENABLE);
    DMA_CHx = (uint32_t)DMA2_CH1;
  }
  else if (ADCx == ADC4) {
    RCC_EnableAHB1PeriphClk(RCC_AHB_PERIPHEN_ADC4, ENABLE);
    RCC_EnableAHBPeriphClk(RCC_AHB_PERIPHEN_DMA2, ENABLE);
    DMA_CHx = (uint32_t)DMA2_CH2;
  }

  /* Configure ADC clock (common for all ADCs) */
  ADC_ConfigClk(ADC_CTRL3_CKMOD_AHB, RCC_ADCHCLK_DIV16);
  RCC_ConfigAdc1mClk(RCC_ADC1MCLK_SRC_HSE, RCC_ADC1MCLK_DIV8);
  
  
  /* ADC configuration for multi-channel scanning */
  ADC_InitType ADC_InitStructure;
  ADC_InitStruct(&ADC_InitStructure);
  
  ADC_InitStructure.WorkMode       = ADC_WORKMODE_INDEPENDENT;   // Independent mode
  ADC_InitStructure.MultiChEn      = ENABLE;                     // ✅ Enable multi-channel scanning
  ADC_InitStructure.ContinueConvEn = continuous ? ENABLE : DISABLE; // Continuous or single scan
  ADC_InitStructure.ExtTrigSelect  = ADC_EXT_TRIG_REG_CONV_SOFTWARE; // Software trigger
  ADC_InitStructure.DatAlign       = ADC_DAT_ALIGN_R;           // Right-aligned data
  ADC_InitStructure.ChsNumber      = channelCount;             // ✅ Dynamic channel count
  ADC_InitStructure.Resolution     = resolution;                // ✅ User-specified resolution
  
  ADC_Init(ADCx, &ADC_InitStructure);
  
  /* Configure each channel in scan sequence dynamically */
  for (uint8_t i = 0; i < channelCount; i++) {
    ADC_ConfigRegularChannel(ADCx, channels[i].adc_channel, i + 1, sampleTime);
  }
  
  /* Configure DMA for automatic data transfer */
  DMA_InitType DMA_InitStructure;
  DMA_DeInit((DMA_ChannelType*)DMA_CHx);
  DMA_InitStructure.PeriphAddr     = (uint32_t)&ADCx->DAT;           // ADC data register
  DMA_InitStructure.MemAddr        = (uint32_t)adcResults;          // Target array
  DMA_InitStructure.Direction      = DMA_DIR_PERIPH_SRC;             // Peripheral to memory
  DMA_InitStructure.BufSize        = channelCount;                  // ✅ Dynamic buffer size
  DMA_InitStructure.PeriphInc      = DMA_PERIPH_INC_DISABLE;         // Peripheral address fixed
  DMA_InitStructure.MemoryInc      = DMA_MEM_INC_ENABLE;             // Memory address increment
  DMA_InitStructure.PeriphDataSize = DMA_PERIPH_DATA_WIDTH_HALFWORD; // 16-bit data
  DMA_InitStructure.MemDataSize    = DMA_MEM_DATA_WIDTH_HALFWORD;    // 16-bit data
  DMA_InitStructure.CircularMode   = continuous ? DMA_MODE_CIRCULAR : DMA_MODE_NORMAL; // Circular for continuous
  DMA_InitStructure.Priority       = DMA_PRIORITY_HIGH;              // High priority
  DMA_InitStructure.Mem2Mem        = DMA_M2M_DISABLE;               // Not memory-to-memory
  
  DMA_Init((DMA_ChannelType*)DMA_CHx, &DMA_InitStructure);
  
  /* Configure DMA request remap (CRITICAL!) */
  if (ADCx == ADC1) {
    DMA_RequestRemap(DMA_REMAP_ADC1, DMA1_CH1, ENABLE);
  }
  else if (ADCx == ADC2) {
    DMA_RequestRemap(DMA_REMAP_ADC2, DMA1_CH2, ENABLE);
  }
  else if (ADCx == ADC3) {
    DMA_RequestRemap(DMA_REMAP_ADC3, DMA2_CH1, ENABLE);
  }
  else if (ADCx == ADC4) {
    DMA_RequestRemap(DMA_REMAP_ADC4, DMA2_CH2, ENABLE);
  }
  
  DMA_EnableChannel((DMA_ChannelType*)DMA_CHx, ENABLE);
  
  /* Enable ADC DMA */
  ADC_SetDMATransferMode(ADCx, ADC_MULTI_REG_DMA_EACH_ADC);
  
  /* Enable ADC */
  ADC_Enable(ADCx, ENABLE);
  
  /* Wait for ADC Ready (recommended) */
  while(ADC_GetFlagStatus(ADCx, ADC_FLAG_RDY) == RESET);
  
  /* ADC calibration (following official example) */
  ADC_CalibrationOperation(ADCx, ADC_CALIBRATION_SINGLE_MODE);
  while(ADC_GetCalibrationStatus(ADCx, ADC_CALIBRATION_SINGLE_MODE));
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







