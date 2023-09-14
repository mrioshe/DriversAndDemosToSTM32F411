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

	/*Se identifican los pines con una variable bit_x*/

	/*pin PA7*/
	GPIO_Handler_t bit_0 ={0};

	bit_0.pGPIOx						= GPIOA;
	bit_0.pinConfig.GPIO_PinNumber		= PIN_7;
	bit_0.pinConfig.GPIO_PinMode		= GPIO_MODE_OUT;
	bit_0.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	bit_0.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;
	bit_0.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	/*pin PC8*/

	GPIO_Handler_t bit_1 ={0};

	bit_1.pGPIOx						= GPIOC;
	bit_1.pinConfig.GPIO_PinNumber		= PIN_8;
	bit_1.pinConfig.GPIO_PinMode		= GPIO_MODE_OUT;
	bit_1.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	bit_1.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;
	bit_1.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	/*pin PC8*/

	GPIO_Handler_t bit_2 ={0};

	bit_2.pGPIOx						= GPIOC;
	bit_2.pinConfig.GPIO_PinNumber		= PIN_7;
	bit_2.pinConfig.GPIO_PinMode		= GPIO_MODE_OUT;
	bit_2.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	bit_2.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;
	bit_2.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	/*pin PA6*/

	GPIO_Handler_t bit_3 ={0};

	bit_3.pGPIOx						= GPIOA;
	bit_3.pinConfig.GPIO_PinNumber		= PIN_6;
	bit_3.pinConfig.GPIO_PinMode		= GPIO_MODE_OUT;
	bit_3.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	bit_3.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;
	bit_3.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	/*pin PB8*/

	GPIO_Handler_t bit_4 ={0};

	bit_4.pGPIOx						= GPIOB;
	bit_4.pinConfig.GPIO_PinNumber		= PIN_8;
	bit_4.pinConfig.GPIO_PinMode		= GPIO_MODE_OUT;
	bit_4.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	bit_4.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;
	bit_4.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	/*pin PC6*/

	GPIO_Handler_t bit_5 ={0};

	bit_5.pGPIOx						= GPIOC;
	bit_5.pinConfig.GPIO_PinNumber		= PIN_6;
	bit_5.pinConfig.GPIO_PinMode		= GPIO_MODE_OUT;
	bit_5.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	bit_5.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;
	bit_5.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	/*pin PC9*/

	GPIO_Handler_t bit_6 ={0};

	bit_6.pGPIOx						= GPIOC;
	bit_6.pinConfig.GPIO_PinNumber		= PIN_9;
	bit_6.pinConfig.GPIO_PinMode		= GPIO_MODE_OUT;
	bit_6.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	bit_6.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;
	bit_6.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	/*Como se debe usar el botón azul de la tarjeta, se debe configurar este botón como entrada,
	 * el cual está conectado internamente al pin 13 (ver diagrama de referencia)*/

	GPIO_Handler_t blue_button={0};

	blue_button.pGPIOx							= GPIOC;
	blue_button.pinConfig.GPIO_PinNumber		= PIN_13;
	blue_button.pinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	blue_button.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	blue_button.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;
	blue_button.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	/*Configuramos todos_ los pines*/

	gpio_Config(&bit_0);
	gpio_Config(&bit_1);
	gpio_Config(&bit_2);
	gpio_Config(&bit_3);
	gpio_Config(&bit_4);
	gpio_Config(&bit_5);
	gpio_Config(&bit_6);
	gpio_Config(&blue_button);


	GPIO_Handler_t location[7];

	location[0] = bit_0;
	location[1] = bit_1;
	location[2] = bit_2;
	location[3] = bit_3;
	location[4] = bit_4;
	location[5] = bit_5;
	location[6] = bit_6;
	location[7] = blue_button;


	/*
	gpio_Config(&bit_0);
	gpio_WritePin(&bit_0, SET);
	delay();
	gpio_TooglePin(&bit_0);
	*/

	uint8_t counter=0;
	uint8_t aux=0;
	uint32_t button=1;


	while(1){

		button=gpio_ReadPin(&blue_button); //Leemos estado del pin y lo guardamos en la variable button

		if (button>=1){ //si el boton==1
			counter++; //incrementamos el contador
			if (counter<=60 && counter >=0){
				for(uint8_t j=0; j<=6;j++){ //variamos j de 0 hasta 6, con incrementos de 1 en 1
					aux=(counter>>j); // la variable aux tomará el valor del contador desplazado j posiciones a la derecha
					aux &=1; // se realiza máscara con and
					gpio_WritePin(&location[j],aux); //se escribe en el pin_j el valor de la variable auxiliar

				}
				delay(); // se hace llamada a función de tiempo

			}
			else if(counter >60){ //el valor del contador llegará a superar a 60, en este caso se le da un valor de 0
				counter=0; //para reiniciar el ciclo
			}

		}
		else if(button==0){ //caso totalmente análogo a lo anterior pero el contador se decrementa en 1

			counter--;

			if (counter<=60 && counter >=0){
							for(uint8_t j=0; j<=6;j++){
								aux=(counter>>j);
								aux &=1;
								gpio_WritePin(&location[j],aux);

							}
							delay();

						}
						else if(counter <0){ //cual valor llega a 0, se le da el valor de 60 para seguir decrementando
							counter=60;
						}



			}
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


