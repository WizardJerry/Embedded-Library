/**
*     Copyright (c) 2023, Nations Technologies Inc.
* 
*     All rights reserved.
*
*     This software is the exclusive property of Nations Technologies Inc. (Hereinafter 
* referred to as NATIONS). This software, and the product of NATIONS described herein 
* (Hereinafter referred to as the Product) are owned by NATIONS under the laws and treaties
* of the People's Republic of China and other applicable jurisdictions worldwide.
**/

/**
*\*\file mps_gpio.h
*\*\author Nations 
*\*\version v1.0.0 
*\*\copyright Copyright (c) 2023, Nations Technologies Inc. All rights reserved.
**/

#ifndef __MPS_GPIO_H__
#define __MPS_GPIO_H__

#include "n32h47x_48x.h"
#include "n32h47x_48x_gpio.h"
#include "n32h47x_48x_rcc.h"

/* LED Pin Definition */
#define LED1_PORT   GPIOA
#define LED1_PIN    GPIO_PIN_3
#define LED1_CLOCK  RCC_AHB_PERIPHEN_GPIOA

/* CTRL Pin Definition */
#define CTRL_PIN                 GPIO_PIN_1
#define CTRL_GPIO                GPIOA
#define CTRL_GPIO_RCC            RCC_AHB_PERIPHEN_GPIOA

/* CTRL Pin Control Macros */
#define CTRL_H                   GPIO_SetBits(CTRL_GPIO, CTRL_PIN)
#define CTRL_L                   GPIO_ResetBits(CTRL_GPIO, CTRL_PIN)

/* Function Declarations */
void CTRL_GPIO_Init(void);

/* LED Control Functions */
void LED_Init(GPIO_Module* GPIOx,uint16_t Pin,uint32_t clock);
void LED_On(GPIO_Module* GPIOx,uint16_t Pin);
void LED_Off(GPIO_Module* GPIOx,uint16_t Pin);
void LED_Toggle(void);

#endif /* __MPS_GPIO_H__ */ 