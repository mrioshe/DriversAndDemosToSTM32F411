/**
 ******************************************************************************
 * @file           : main.c
 * @author         : mnrioshe
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

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "stm32_assert.h"
#include "gpio_driver_hal.h"

//headers definition

int add(int x, int y);
void delay(){
	uint32_t  counter =0;
		for(uint32_t i=1;i<=1300000;i++){
			counter++;
		}
}


//Definimos un Pin de prueba
GPIO_Handler_t userLed={0}; // pinA5

/*
 * the main function, where everything happens
 */

int main(void)
{
    /* Configuramos el pin*/
	userLed.pGPIOx							= GPIOA;
	userLed.pinConfig.GPIO_PinNumber		= PIN_5;
	userLed.pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	userLed.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	userLed.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;
	userLed.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	/* Cargamos la configuración en los registros que gobiernan el puerto*/

	/*
	 * gpio_Config(&userLed);
	uint8_t a=gpio_ReadPin(&userLed); código de prueba del ReadPin
	para probar este se dene temer eñ user led en modo salida (MODE_IN)
	*/

	/*
	 * 	gpio_Config(&userLed);
	gpio_WritePin(&userLed, SET); Código para probar el Tooglepin (MODE OUT)
	gpio_Tooglepin(&userLed);
	 * */
	/* Configuración de los pines:*/

	gpio_Config(&userLed);
	gpio_WritePin(&userLed, SET);
	delay();
	gpio_TooglePin(&userLed);

while(1){


}



}

/*
 * Esta función sirve para detectar problemas de parámetros
 * incorrectos al momento de ejecutar un programa
 * */

void assert_failed(uint8_t*file,uint32_t line){
	while(1){
		//problems
	}
}


