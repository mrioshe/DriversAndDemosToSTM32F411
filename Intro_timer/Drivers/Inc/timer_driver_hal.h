/*
 * timer_driver_hal.h
 *
 *  Created on: Sep 25, 2023
 *      Author: mauricio
 */

#ifndef INC_TIMER_DRIVER_HAL_H_
#define INC_TIMER_DRIVER_HAL_H_

#include "stm32f4xx.h"

enum
{
	TIMER_INT_DISABLE=0,
	TIMER_INT_ENABLE
};

enum
{
	TIMER_UP_COUNTER=0,
	TIMER_DOWN_COUNTER
};

enum
{
	TIMER_OFF=0,
	TIMER_ON
};

/*Estructura que contiene la configuración mínima necesaria para el mano del timer*/

typedef struct
{
	uint8_t	 	TIMx_mode;				//Up or down
	uint16_t 	TIMx_Prescaler;			//Prescaler....
	uint32_t 	TIMx_Period;			//Valor en ms del periodo del timer
	uint8_t		TIMx_InterruptEnable;	//Activa o desactiva el modo interrupciones
} Timer_BasicConfig_t;

/*Handler para el Timer*/

typedef struct

{
	TIM_TypeDef				*pTIMx;
	Timer_BasicConfig_t		TIMx_Config;
} Timer_Handler_t;

/*For testing assert parameters - checking basic configurations.*/

#define IS_TIMER_INTERRUPT(VALUE)		(((VALUE) == TIMER_INT_DISABLE) || ((VALUE) == TIMER_INT_ENABLE))
#define IS_TIMER_MODE(VALUE)		    (((VALUE) == TIMER_UP_COUNTER) || ((VALUE) == TIMER_DOWN_COUNTER))
#define IS_TIMER_STATE(VALUE)			(((VALUE) == TIMER_OFF) || ((VALUE) == TIMER_ON))
#define IS_TIMER_PRESC(VALUE)			(((uint32_t)VALUE) > 1 && ((uint32_t)VALUE)<0xFFFE)
#define IS_TIMER_PERIOD(PERIOD)			(((uint32_t)PERIOD) > 1)

void timer_Config(Timer_Handler_t *pTimerHandler);
void timer_SetState(Timer_Handler_t *pTimerHandler,uint8_t newState);

/*Esta función debe ser sobre-escrita en la main para que el sistema funcione*/
void timer2_Callback(void);


#endif /* INC_TIMER_DRIVER_HAL_H_ */
