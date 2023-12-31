/*
 * systick_driver_hal.h
 *
 *  Created on: Nov 8, 2023
 *      Author: mauricio
 */

#ifndef SYSTICK_DRIVER_HAL_H_
#define SYSTICK_DRIVER_HAL_H_

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

uint64_t getTicksMs(void);

void config_systick_ms(Systic_Handler_t *pSystick_Handler_t);
void systick_Delay_ms(uint32_t wait_time_ms);


#endif /* SYSTICK_DRIVER_HAL_H_ */
