/*
 * pwm_driver_hal.h
 *
 *  Created on: Nov 6, 2023
 *      Author: mauricio
 */

#ifndef PWM_DRIVER_HAL_H_
#define PWM_DRIVER_HAL_H_

#include "stm32f4xx.h"

enum{
	TIMER_TIM1=0,
	TIMER_TIM2,
	TIMER_TIM3,
	TIMER_TIM4,
	TIMER_TIM5,
	TIMER_TIM9,
	TIMER_TIM10,
	TIMER_TIM11,
	TIMER_TIM2,
};

enum{
	PWM_CHANNEL_1= 0,
	PWM_CHANNEL_2,
	PWM_CHANNEL_3,
	PWM_CHANNEL_4
};

#define PWM_DUTTY_0_PERCENT 0
#define PWM_DUTTY_100_PERCENT 100

typedef struct
{
	uint8_t		channel;
	uint32_t	prescaler;
	uint16_t	period;  // este está dado por Time_fosc*PSC*ARR
	} PWM_Config_t;

typedef struct
{
	TIM_TypeDef	 	*pTIMx;
	PWM_Config_t 	config;
} PWM_Handler_t;

void pwm_Config(PWM_Handler_t *pPWMHandler);
void setFrequency(PWM_Handler_t *pPWMHandler);
void updateFrequency(PWM_Handler_t *pPWMHandler, uint16_t newFreq);
void setDuttyCycle(PWM_Handler_t *pPWMHandler);
void updateDuttyCycle(PWM_Handler_t *pPWMHandler, uint16_t newDutty);
void enableOutput(PWM_Handler_t *pPWMHandler);
void startPWMsignal(PWM_Handler_t *pPWMHandler);
void stopPWMsignal(PWM_Handler_t *pPWMHandler);






#endif /* PWM_DRIVER_HAL_H_ */

