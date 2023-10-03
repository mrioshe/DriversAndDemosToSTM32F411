/**
 ******************************************************************************
 * @file           : main.c
 * @author         : mrioshe
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stm32f4xx.h>
#include <stdint.h>
#include "stm32_assert.h"
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"
#include "exti_driver_hal.h"


#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif


GPIO_Handler_t sw={0};
Timer_Handler_t blinkTimer={0};
EXTI_Config_t interrupt_sw={0};
GPIO_Handler_t stateled={0};
GPIO_Handler_t direction={0};


int main(void){

	/*Configuración:*/

	//Pin para conectar led de estado

	stateled.pGPIOx							= GPIOA;
	stateled.pinConfig.GPIO_PinNumber		= PIN_5;
	stateled.pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	stateled.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	stateled.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;
	stateled.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	//Pin para conectar led que indica el sentido de rotación

	direction.pGPIOx							= GPIOC;
	direction.pinConfig.GPIO_PinNumber			= PIN_1;
	direction.pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	direction.pinConfig.GPIO_PinOutputType		= GPIO_OTYPE_PUSHPULL;
	direction.pinConfig.GPIO_PinOutputSpeed		= GPIO_OSPEEDR_MEDIUM;
	direction.pinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;

	// Configuramos el timer de 32 bits para el blinky



    blinkTimer.pTIMx								=TIM5;
	blinkTimer.TIMx_Config.TIMx_Prescaler			=16000; //Genera incrementos de 1 ms
	blinkTimer.TIMx_Config.TIMx_Period				=250;   //DE la mano con el prescaler, se toma el periodo en ms
	blinkTimer.TIMx_Config.TIMx_mode				=TIMER_UP_COUNTER;
	blinkTimer.TIMx_Config.TIMx_InterruptEnable		=TIMER_INT_ENABLE;


	sw.pGPIOx							= GPIOC;
	sw.pinConfig.GPIO_PinNumber			= PIN_0;
	sw.pinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	sw.pinConfig.GPIO_PinOutputType		= GPIO_OTYPE_PUSHPULL;
	sw.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;
	sw.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	//Configuración de las interrupciones externas

	interrupt_sw.edgeType		= EXTERNAL_INTERRUPT_FALLING_EDGE;
	interrupt_sw.pGPIOHandler	= &sw;

	//Cargamos configuración de los pines


	gpio_Config(&sw);
	gpio_Config(&direction);
	gpio_Config(&stateled);

	/*Cargamos configuracion el Timer*/

	timer_Config(&blinkTimer);


	//Cargamos configuración de interrupciones externas

	exti_Config(&interrupt_sw);

	/*Seteo de estados iniciales*/

	//Encendemos el led de estado


	gpio_WritePin(&stateled,SET);
	gpio_WritePin(&direction,SET);

	//Encendemos el Timer

	timer_SetState(&blinkTimer, TIMER_ON);



    /* Loop forever */
	while(1){



		}

	}

void callback_extInt0(void){
	gpio_TooglePin(&direction);
}

void Timer5_Callback(void){
	gpio_TooglePin(&stateled);
}


void assert_failed(uint8_t*file,uint32_t line){
	while(1){
		//problems
	}
}
