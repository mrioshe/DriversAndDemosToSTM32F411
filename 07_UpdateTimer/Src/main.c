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

GPIO_Handler_t userLed={0};
GPIO_Handler_t userbutton={0};
Timer_Handler_t blinkTimer={0};
EXTI_Config_t interrupt_button={0};


int main(void)
{

	userLed.pGPIOx							= GPIOA;
	userLed.pinConfig.GPIO_PinNumber		= PIN_6;
	userLed.pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	userLed.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	userLed.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;
	userLed.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	gpio_Config(&userLed);
	gpio_WritePin(&userLed,SET);

/*blinkTimer.pTIMx								=TIM11;
	blinkTimer.TIMx_Config.TIMx_Prescaler			=16000; //Genera incrementos de 1 ms
	blinkTimer.TIMx_Config.TIMx_Period				=2000;   //DE la mano con el prescaler, se genera los incrementos
	blinkTimer.TIMx_Config.TIMx_mode				=TIMER_UP_COUNTER;
	blinkTimer.TIMx_Config.TIMx_InterruptEnable		=TIMER_INT_ENABLE;	*/

	/*Configuramos el Timer*/
	//timer_Config(&blinkTimer);

	//Encendemos el Timer
	//timer_SetState(&blinkTimer, TIMER_ON);

	userbutton.pGPIOx							= GPIOC;
	userbutton.pinConfig.GPIO_PinNumber			= PIN_9;
	userbutton.pinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	userbutton.pinConfig.GPIO_PinOutputType		= GPIO_OTYPE_PUSHPULL;
	userbutton.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;
	userbutton.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	gpio_Config(&userbutton);

	interrupt_button.edgeType		= EXTERNAL_INTERRUPT_FALLING_EDGE;
	interrupt_button.pGPIOHandler	= &userbutton;

	exti_Config(&interrupt_button);



	/*control version*/

    /* Loop forever */
	while(1){



		}

	}

void callback_extInt9(void){
	gpio_TooglePin(&userLed);
}


void assert_failed(uint8_t*file,uint32_t line){
	while(1){
		//problems
	}
}
