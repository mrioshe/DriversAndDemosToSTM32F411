/*
 * adc_driver_hal.h
 *
 *  Created on: Oct 19, 2023
 *      Author: mauricio
 */

#ifndef SYSTIC_DRIVER_HAL_H
#define SYSTIC_DRIVER_HAL_H

#include <stm32f4xx.h>

#define SYSTIC_LOAD_VALUE_16MHz_1ms		16000
#define SYSTIC_LOAD_VALUE_100MHz_1ms	100000

enum{
	HSI_TIMER_16MHz=0,
	HSI_TIMER_100MHz
};

typedef struct
{

uint8_t systemClock;

} Systick_Config_t;

typedef struct

{
	SysTick_Type		   *Systick;
	Systick_Config_t		Systick_Config_t;
} Systic_Handler_t;


void config_systick_ms(uint8_t systemClock);
uint64_t getTicksMs(void);
void delay_ms(uint32_t wait_time_ms);


#endif
