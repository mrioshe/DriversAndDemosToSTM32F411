/*
 * exti_driver_hal.h
 *
 *  Created on: Sep 28, 2023
 *      Author: mrioshe
 */

#ifndef EXTI_DRIVER_HAL_H_
#define EXTI_DRIVER_HAL_H_

#include "stm32f4xx.h"
#include "gpio_driver_hal.h"

enum
{
	EXTERNAL_INTERRUPT_FALLING_EDGE	= 0,
	EXTERNAL_INTERRUPT_RISING_EDGE
};

typedef struct
{
	GPIO_Handler_t *pGPIOHandler;	// Handler del pin GPIO que lanzara la interrupción
	uint8_t			edgeType;		// Se selecciona si se desea un tipo de flanco subiendo o bajando
}EXTI_Config_t;


void exti_Config(EXTI_Config_t *extiConfig);
void callback_extInt0(void);
void callback_extInt1(void);
void callback_extInt2(void);
void callback_extInt3(void);
void callback_extInt4(void);
void callback_extInt5(void);
void callback_extInt6(void);
void callback_extInt7(void);
void callback_extInt8(void);
void callback_extInt9(void);
void callback_extInt10(void);
void callback_extInt11(void);
void callback_extInt12(void);
void callback_extInt13(void);
void callback_extInt14(void);
void callback_extInt15(void);

#endif /* EXTI_DRIVER_HAL_H_ */
