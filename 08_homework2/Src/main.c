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



GPIO_Handler_t sw={0};
GPIO_Handler_t stateled={0};
GPIO_Handler_t direction={0};
GPIO_Handler_t dt={0};
GPIO_Handler_t clk={0};
GPIO_Handler_t leds_7segment[7]={0};
GPIO_Handler_t vcc1_7seg={0};
GPIO_Handler_t vcc2_7seg={0};
Timer_Handler_t blinkTimer={0};
EXTI_Config_t interrupt_sw={0};
EXTI_Config_t interrupt_clk={0};
uint8_t flag_clk={0};
uint8_t counter={0};
uint8_t digit_selector=0;
uint8_t number_desc_aux[2]={0};
uint8_t direction_aux={0};
Timer_Handler_t frecTimer={0};




void segment_configuration(uint8_t number);
uint8_t dec_number(uint8_t number_aux);
uint8_t unit_number(uint8_t number_aux);

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

	//Pin de entrada del botón del encoder

	sw.pGPIOx							= GPIOC;
	sw.pinConfig.GPIO_PinNumber			= PIN_0;
	sw.pinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	sw.pinConfig.GPIO_PinOutputType		= GPIO_OTYPE_PUSHPULL;
	sw.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;
	sw.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	//Pin de entrada del data del encoder

	dt.pGPIOx							= GPIOB;
	dt.pinConfig.GPIO_PinNumber			= PIN_5;
	dt.pinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	dt.pinConfig.GPIO_PinOutputType		= GPIO_OTYPE_PUSHPULL;
	dt.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;
	dt.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;


	//Pin de entrada del clock del reloj del encoder

	clk.pGPIOx							= GPIOC;
	clk.pinConfig.GPIO_PinNumber		= PIN_9;
	clk.pinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	clk.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	clk.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;
	clk.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	/*PINES DEL 7 SEGMENTOS:*/

	//segmento A

	leds_7segment[0].pGPIOx							= GPIOD;
	leds_7segment[0].pinConfig.GPIO_PinNumber		= PIN_2;
	leds_7segment[0].pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	leds_7segment[0].pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;

	//segmento B

	leds_7segment[1].pGPIOx							= GPIOA;
	leds_7segment[1].pinConfig.GPIO_PinNumber		= PIN_1;
	leds_7segment[1].pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	leds_7segment[1].pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;

	//segmento C

	leds_7segment[2].pGPIOx							= GPIOB;
	leds_7segment[2].pinConfig.GPIO_PinNumber		= PIN_9;
	leds_7segment[2].pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	leds_7segment[2].pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;

	//segmento D

	leds_7segment[3].pGPIOx							= GPIOB;
	leds_7segment[3].pinConfig.GPIO_PinNumber		= PIN_6;
	leds_7segment[3].pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	leds_7segment[3].pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;

	//segmento E

	leds_7segment[4].pGPIOx							= GPIOB;
	leds_7segment[4].pinConfig.GPIO_PinNumber		= PIN_8;
	leds_7segment[4].pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	leds_7segment[4].pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;


	//segmento F

	leds_7segment[5].pGPIOx							= GPIOA;
	leds_7segment[5].pinConfig.GPIO_PinNumber		= PIN_4;
	leds_7segment[5].pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	leds_7segment[5].pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;

	//segmento G

	leds_7segment[6].pGPIOx							= GPIOA;
	leds_7segment[6].pinConfig.GPIO_PinNumber		= PIN_8;
	leds_7segment[6].pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	leds_7segment[6].pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;



	// pin que alimenta el primer led

	vcc1_7seg.pGPIOx							= GPIOC;
	vcc1_7seg.pinConfig.GPIO_PinNumber			= PIN_6;
	vcc1_7seg.pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	vcc1_7seg.pinConfig.GPIO_PinOutputSpeed		= GPIO_OSPEEDR_MEDIUM;


	// pin que alimenta el segundo led

	vcc2_7seg.pGPIOx							= GPIOC;
	vcc2_7seg.pinConfig.GPIO_PinNumber			= PIN_11;
	vcc2_7seg.pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	vcc2_7seg.pinConfig.GPIO_PinOutputSpeed		= GPIO_OSPEEDR_MEDIUM;


	// Configuramos el timer de 32 bits para el blinky

    blinkTimer.pTIMx								=TIM5;
	blinkTimer.TIMx_Config.TIMx_Prescaler			=16000; //Genera incrementos de 1 ms
	blinkTimer.TIMx_Config.TIMx_Period				=250;   //DE la mano con el prescaler, se toma el periodo en ms
	blinkTimer.TIMx_Config.TIMx_mode				=TIMER_UP_COUNTER;
	blinkTimer.TIMx_Config.TIMx_InterruptEnable		=TIMER_INT_ENABLE;

	// Configuramos el timer de 8 bits para obtener la frecuencia de switcheo de los dos canales del 7segmentos

    frecTimer.pTIMx									=TIM10;
    frecTimer.TIMx_Config.TIMx_Prescaler			=16000; //Genera incrementos de 1 ms
    frecTimer.TIMx_Config.TIMx_Period				=10;   //DE la mano con el prescaler, se toma el periodo en ms
    frecTimer.TIMx_Config.TIMx_mode					=TIMER_UP_COUNTER;
    frecTimer.TIMx_Config.TIMx_InterruptEnable		=TIMER_INT_ENABLE;


	//Configuración de las interrupciones externas

	interrupt_sw.edgeType		= EXTERNAL_INTERRUPT_FALLING_EDGE;
	interrupt_sw.pGPIOHandler	= &sw;

	interrupt_clk.edgeType		= EXTERNAL_INTERRUPT_FALLING_EDGE;
	interrupt_clk.pGPIOHandler	= &clk;


	//Cargamos configuración de los pines

	for(uint8_t i=0;i<=6;i++){
		gpio_Config(&leds_7segment[i]);
	}

	gpio_Config(&sw);
	gpio_Config(&dt);
	gpio_Config(&clk);
	gpio_Config(&direction);
	gpio_Config(&stateled);
	gpio_Config(&vcc1_7seg);
	gpio_Config(&vcc2_7seg);



	/*Cargamos configuracion el Timer*/

	timer_Config(&blinkTimer);
	timer_Config(&frecTimer);


	//Cargamos configuración de interrupciones externas

	exti_Config(&interrupt_sw);
	exti_Config(&interrupt_clk);


	/*Seteo de estados iniciales*/

	//Encendemos el led de estado


	gpio_WritePin(&stateled,SET);
	gpio_WritePin(&direction,SET);
	gpio_WritePin(&vcc1_7seg, SET);
	gpio_WritePin(&vcc2_7seg, SET);


	//Encendemos los Timers

	timer_SetState(&blinkTimer, TIMER_ON);
	timer_SetState(&frecTimer, TIMER_ON);

	segment_configuration(0);
	counter=0;

    /* Loop forever */
	while(1){



		// Encendemos uno de dos canales de voltaje de los canales del 7 segmentos:
		if (digit_selector){
			segment_configuration(unit_number(counter));

		}else{
			segment_configuration(dec_number(counter));
		}

	}


}

/*INTERRUPCIONES EXTERNAS*/


void callback_extInt9(void){

	/*
				Primer caso: se rota hacia la derecha (CW) y está modo directo
				Segundo caso: se rota hacia la derecha (CW) y está modo inverso
				Tercer caso: se rota hacia la izquierda (CCW) y está en modo directo
				Cuarto caso: se rota hacia la izquierda (CCW) y está en modo inverso

				 */

				if(gpio_ReadPin(&dt) && !direction_aux){

					if(counter==99){
						counter=99;
					} else {
						counter++;
					}

				} else if(!gpio_ReadPin(&dt) && !direction_aux){

					if(counter==0){
						counter=0;
					} else {
						counter--;
					}

				} else if(!gpio_ReadPin(&dt) && direction_aux){

					if(counter==99){
						counter=99;
					} else {
						counter++;
					}

				} else if(gpio_ReadPin(&dt) && direction_aux){

					if(counter==0){
						counter=0;
					} else {
						counter--;
					}

				}



}

void callback_extInt0(void){
	gpio_TooglePin(&direction);
	//Variable auxiliar debido a que no se puede leer un pin de salida:
	direction_aux=!direction_aux;

}


/*INTERRUPCIONES DE LOS TIMERS*/

void Timer5_Callback(void){
	gpio_TooglePin(&stateled);

}

void Timer10_Callback(void){

	gpio_TooglePin(&vcc1_7seg);
	gpio_TooglePin(&vcc2_7seg);
	//Variable auxiliar debido a que no se puede leer un pin de salida:
	digit_selector=!digit_selector;

}



uint8_t unit_number(uint8_t number_aux){

	uint8_t unit = number_aux%10;
	return unit;
}

uint8_t dec_number(uint8_t number_aux){

	uint8_t dec= number_aux/10;
	return dec;
}


/*FUNCIÓN QUE GENERA LOS NÚMEROS*/
void segment_configuration(uint8_t number){

	switch(number){

		case 0:

			gpio_WritePin(&leds_7segment[0],!SET);
			gpio_WritePin(&leds_7segment[1],!SET);
			gpio_WritePin(&leds_7segment[2],!SET);
			gpio_WritePin(&leds_7segment[3],!SET);
			gpio_WritePin(&leds_7segment[4],!SET);
			gpio_WritePin(&leds_7segment[5],!SET);
			gpio_WritePin(&leds_7segment[6],!RESET);
			break;

		case 1:
			gpio_WritePin(&leds_7segment[0],!RESET);
			gpio_WritePin(&leds_7segment[1],!SET);
			gpio_WritePin(&leds_7segment[2],!SET);
			gpio_WritePin(&leds_7segment[3],!RESET);
			gpio_WritePin(&leds_7segment[4],!RESET);
			gpio_WritePin(&leds_7segment[5],!RESET);
			gpio_WritePin(&leds_7segment[6],!RESET);
			break;

		case 2:
			gpio_WritePin(&leds_7segment[0],!SET);
			gpio_WritePin(&leds_7segment[1],!SET);
			gpio_WritePin(&leds_7segment[2],!RESET);
			gpio_WritePin(&leds_7segment[3],!SET);
			gpio_WritePin(&leds_7segment[4],!SET);
			gpio_WritePin(&leds_7segment[5],!RESET);
			gpio_WritePin(&leds_7segment[6],!SET);
			break;

		case 3:
			gpio_WritePin(&leds_7segment[0],!SET);
			gpio_WritePin(&leds_7segment[1],!SET);
			gpio_WritePin(&leds_7segment[2],!SET);
			gpio_WritePin(&leds_7segment[3],!SET);
			gpio_WritePin(&leds_7segment[4],!RESET);
			gpio_WritePin(&leds_7segment[5],!RESET);
			gpio_WritePin(&leds_7segment[6],!SET);
			break;

		case 4:
			gpio_WritePin(&leds_7segment[0],!RESET);
			gpio_WritePin(&leds_7segment[1],!SET);
			gpio_WritePin(&leds_7segment[2],!SET);
			gpio_WritePin(&leds_7segment[3],!RESET);
			gpio_WritePin(&leds_7segment[4],!RESET);
			gpio_WritePin(&leds_7segment[5],!SET);
			gpio_WritePin(&leds_7segment[6],!SET);
			break;

		case 5:
			gpio_WritePin(&leds_7segment[0],!SET);
			gpio_WritePin(&leds_7segment[1],!RESET);
			gpio_WritePin(&leds_7segment[2],!SET);
			gpio_WritePin(&leds_7segment[3],!SET);
			gpio_WritePin(&leds_7segment[4],!RESET);
			gpio_WritePin(&leds_7segment[5],!SET);
			gpio_WritePin(&leds_7segment[6],!SET);
			break;

		case 6:
			gpio_WritePin(&leds_7segment[0],!SET);
			gpio_WritePin(&leds_7segment[1],!RESET);
			gpio_WritePin(&leds_7segment[2],!SET);
			gpio_WritePin(&leds_7segment[3],!SET);
			gpio_WritePin(&leds_7segment[4],!SET);
			gpio_WritePin(&leds_7segment[5],!SET);
			gpio_WritePin(&leds_7segment[6],!SET);
			break;

		case 7:
			gpio_WritePin(&leds_7segment[0],!SET);
			gpio_WritePin(&leds_7segment[1],!SET);
			gpio_WritePin(&leds_7segment[2],!SET);
			gpio_WritePin(&leds_7segment[3],!RESET);
			gpio_WritePin(&leds_7segment[4],!RESET);
			gpio_WritePin(&leds_7segment[5],!RESET);
			gpio_WritePin(&leds_7segment[6],!RESET);
			break;

		case 8:
			gpio_WritePin(&leds_7segment[0],!SET);
			gpio_WritePin(&leds_7segment[1],!SET);
			gpio_WritePin(&leds_7segment[2],!SET);
			gpio_WritePin(&leds_7segment[3],!SET);
			gpio_WritePin(&leds_7segment[4],!SET);
			gpio_WritePin(&leds_7segment[5],!SET);
			gpio_WritePin(&leds_7segment[6],!SET);
			break;


		case 9:
			gpio_WritePin(&leds_7segment[0],!SET);
			gpio_WritePin(&leds_7segment[1],!SET);
			gpio_WritePin(&leds_7segment[2],!SET);
			gpio_WritePin(&leds_7segment[3],!SET);
			gpio_WritePin(&leds_7segment[4],!RESET);
			gpio_WritePin(&leds_7segment[5],!SET);
			gpio_WritePin(&leds_7segment[6],!SET);
			break;

	}
}


void assert_failed(uint8_t*file,uint32_t line){
	while(1){
		//problems
	}
}
