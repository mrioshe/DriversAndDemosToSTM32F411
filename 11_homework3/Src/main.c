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

#include <stdint.h>
#include <stm32f4xx.h>
#include <stm32_assert.h>
#include "usart_driver_hal.h"
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"
#include "exti_driver_hal.h"
#include "adc_driver_hal.h"
#include <string.h>

//Declaracion de variables:

GPIO_Handler_t sw={0};
GPIO_Handler_t stateled={0};
GPIO_Handler_t led_selector={0};
GPIO_Handler_t dt={0};
GPIO_Handler_t clk={0};
GPIO_Handler_t leds_7segment[8]={0};
GPIO_Handler_t vcc_units_7seg={0};
GPIO_Handler_t vcc_decs_7seg={0};
GPIO_Handler_t pinTx = {0};
GPIO_Handler_t pinRx = {0};

Timer_Handler_t blinkTimer={0};
Timer_Handler_t frec_7segment={0};
Timer_Handler_t frec_message={0};

EXTI_Config_t interrupt_sw={0};
EXTI_Config_t interrupt_clk={0};

USART_Handler_t commSerial= { 0 };

ADC_Config_t adc_signal[3]={0};

uint8_t receivedChar=0;
uint8_t sendMsg=0;
char bufferData[128]={0};

uint8_t flag_clk={0};
uint8_t flag_sendMsg={0};
uint8_t flag_selector={0};
uint8_t flag_switcheo={0};

uint8_t counter={0};
uint8_t digit_selector=0;
uint8_t number_desc_aux[2]={0};
uint8_t selector={0};
uint8_t resolution_value={0};


//funciones:

void initSys(void);
void segment_configuration_units(uint8_t number,uint8_t digit_active);
void segment_configuration_decs(ADC_Config_t *adcConfig,uint8_t digit_active);
uint8_t get_resolution_value(ADC_Config_t *adcConfig);

int main() {
	// Iniciamos el sistema:
	initSys();
while (1) {

	// Cada vez que el timer de switcheo lanza una interrupcion:

	if(flag_switcheo){
		flag_switcheo=0; //bajar la bandera
		digit_selector=!digit_selector; //Cambiar de estado el selector,
		/*para encender el digito que no estaba activo y apagar el
		que estaba activo. Se tiene cuatro opciones dependiendo del estado
		del selector y de que se está modificando si la senal o la resolucion
		*/
		if(selector && digit_selector){
			gpio_WritePin(&vcc_units_7seg,!SET);
			gpio_WritePin(&vcc_decs_7seg,RESET);
			//se configura el 7segmentos:
			segment_configuration_units(counter,1);

		}else if(selector && !digit_selector){
			gpio_WritePin(&vcc_units_7seg,!RESET);
			gpio_WritePin(&vcc_decs_7seg,SET);
			segment_configuration_decs(&adc_signal[counter-1],0);

		} else if(!selector && digit_selector){
			gpio_WritePin(&vcc_units_7seg,!SET);
			gpio_WritePin(&vcc_decs_7seg,RESET);
			segment_configuration_units(counter,0);

		} else if (!selector & !digit_selector){
			gpio_WritePin(&vcc_units_7seg,!RESET);
			gpio_WritePin(&vcc_decs_7seg,SET);
			segment_configuration_decs(&adc_signal[counter-1],1);
		}
	}

	// Cada vez que se tiene interrupcion externa debido al boton del encoder:
	if(flag_selector){
		flag_selector=0;
		selector=!selector;

		if(selector){
			usart_writeMsg(&commSerial, "Modificando la señal (sensor 1,2 o 3) \n\r");
		}
		else if(!selector){
			usart_writeMsg(&commSerial, "Modificando la resolución \n\r");
		}
		//variable que selecciona si se esta modificando
		//el canal o la resulcion (0: resolucion, 1:señal)
		//apagamos/prendemos el led de estado:
		if (gpio_ReadPin(&led_selector)){
			gpio_WritePin(&led_selector,RESET);
		} else if (!gpio_ReadPin(&led_selector)){
			gpio_WritePin(&led_selector,SET);
		}
	}

	// Cada vez que el enconder se mueve:

	if(flag_clk){
		flag_clk=0;
		/*se tiene cuatro casos:
		 * 1. activo para modificar la senal (1,2 o 3) y el se debe aumentar (1->2 o 2->3)
		 * 2. activo para modificar la senal (1,2 o 3) y el se debe disminuir (2->1 o 3->2)
		 * 3. activo para modificar la resolucion y el se debe aumentar la resolucion (ex: 10bits->12bits)
		 * 4. activo para modificar la resolucion y el se debe dismunuir la resolucion (ex: 10bits->8bits)
		 * */
		if (selector && gpio_ReadPin(&dt)){
			if(counter==3){
				counter=3;
			} else {
				counter++;
				usart_writeMsg(&commSerial, "Aumentando el canal de lectura de sensor \n\r");
			}


		} else if(selector && !gpio_ReadPin(&dt)){
			if(counter==1){
				counter=1;
			} else {
				counter--;
				usart_writeMsg(&commSerial, "Disminuyendo el canal de lectura de sensor \n\r");

			}

		} else if ( !selector && gpio_ReadPin(&dt)){
			if(adc_signal[counter-1].resolution==RESOLUTION_12_BIT){
				adc_signal[counter-1].resolution=RESOLUTION_12_BIT;
			} else {
				//hay que dismunir el valor, pues el valor 0 representa es 12 bits y 3 es 6 bits
				adc_signal[counter-1].resolution--;
				usart_writeMsg(&commSerial, "Aumentando la resolución \n\r");

			}

		}else if (!selector && !gpio_ReadPin(&dt)){

			if(adc_signal[counter-1].resolution==RESOLUTION_6_BIT){
				adc_signal[counter-1].resolution=RESOLUTION_6_BIT;
			} else {
				adc_signal[counter-1].resolution++;
				usart_writeMsg(&commSerial, "Disminuyendo el canal de lectura de sensor\n\r");
			}
		}
	}

	if(flag_sendMsg){
		flag_sendMsg=0;
		resolution_value=get_resolution_value(&adc_signal[counter-1]);
		sprintf(bufferData,"El valor de la conversión ADC del sensor %d es %d, con una "
				"resolución de %d bits. \n\r",counter,adc_signal[counter-1].adcData,resolution_value );
		usart_writeMsg(&commSerial,bufferData);
	}



	if(receivedChar){
		if(receivedChar=='p'){
			usart_writeMsg(&commSerial, "Testing, testing!!\n\r");
		}

		if(receivedChar=='m'){
			flag_selector=0;
			selector=!selector;
			if(selector){
				usart_writeMsg(&commSerial, "Modificando la señal (sensor 1,2 o 3) \n\r");
			}
			else if(!selector){
				usart_writeMsg(&commSerial, "Modificando la resolución \n\r");
			}

			//variable que selecciona si se esta modificando
			//el canal o la resulcion (0: resolucion, 1:señal)
			//apagamos/prendemos el led de estado:
			if (gpio_ReadPin(&led_selector)){
				gpio_WritePin(&led_selector,RESET);
			} else if (!gpio_ReadPin(&led_selector)){
				gpio_WritePin(&led_selector,SET);
			}

		}

		if(receivedChar=='a'){
			if(selector){
				if(counter==3){
					counter=3;
				} else {
					counter++;
					usart_writeMsg(&commSerial, "Aumentando el canal de lectura de sensor \n\r");
				}

			} else if(!selector){
				if(adc_signal[counter-1].resolution==RESOLUTION_12_BIT){
					adc_signal[counter-1].resolution=RESOLUTION_12_BIT;
				} else {
					adc_signal[counter-1].resolution--;
					usart_writeMsg(&commSerial, "Aumentando la resolución \n\r");
				}

			}
		}

		if(receivedChar == 'd'){
			if(selector){
				if(counter==1){
						counter=1;
					} else {
						counter--;
						usart_writeMsg(&commSerial, "disminuyendo el canal de lectura de sensor\n\r");
					}

			} else if(!selector){
				if(adc_signal[counter-1].resolution==RESOLUTION_6_BIT){
					adc_signal[counter-1].resolution=RESOLUTION_6_BIT;
				} else {
					adc_signal[counter-1].resolution++;
					usart_writeMsg(&commSerial, "Disminuyendo la resolución \n\r");
				}
			}
		}

		receivedChar=0;

	}


}

}

void initSys(void) {

	/* CONFIGURACION PINES PARA LEDS*/

	//Pin para conectar led de estado

	stateled.pGPIOx							= GPIOA;
	stateled.pinConfig.GPIO_PinNumber		= PIN_5;
	stateled.pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	stateled.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	stateled.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;
	stateled.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;


	//Pin para conectar led que indica si se está modificando la resolución o si se está cambiando
	//de canal ADC

	led_selector.pGPIOx							= GPIOC;
	led_selector.pinConfig.GPIO_PinNumber		= PIN_1;
	led_selector.pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	led_selector.pinConfig.GPIO_PinOutputType	= GPIO_OTYPE_PUSHPULL;
	led_selector.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;
	led_selector.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	gpio_Config(&led_selector);
	gpio_Config(&stateled);

	/* CONFIGURACION PINES ENCODER*/

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


	gpio_Config(&sw);
	gpio_Config(&dt);
	gpio_Config(&clk);


	/*CONFIGURACIÓN PINES DEL 7 SEGMENTOS:*/

	/*
	 * leds_7segment[0]		A segment
	 * leds_7segment[1]		B segment
	 * leds_7segment[2]		C segment
	 * leds_7segment[3]		D segment
	 * leds_7segment[4]		E segment
	 * leds_7segment[5]		F segment
	 * leds_7segment[6]		G segment
	 * leds_7segment[7]		DP
	 * */

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

	//DP

	leds_7segment[7].pGPIOx							= GPIOC;
	leds_7segment[7].pinConfig.GPIO_PinNumber		= PIN_8;
	leds_7segment[7].pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	leds_7segment[7].pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;

	for(uint8_t i=0;i<=7;i++){
			gpio_Config(&leds_7segment[i]);
		}

	// pin que alimenta vcc de las unidades

	vcc_units_7seg.pGPIOx							= GPIOC;
	vcc_units_7seg.pinConfig.GPIO_PinNumber			= PIN_6;
	vcc_units_7seg.pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	vcc_units_7seg.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;


	// pin que alimenta las decenas

	vcc_decs_7seg.pGPIOx							= GPIOC;
	vcc_decs_7seg.pinConfig.GPIO_PinNumber			= PIN_11;
	vcc_decs_7seg.pinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	vcc_decs_7seg.pinConfig.GPIO_PinOutputSpeed		= GPIO_OSPEEDR_MEDIUM;

	gpio_Config(&vcc_units_7seg);
	gpio_Config(&vcc_decs_7seg);

	/*CONFIGURACION PINES PARA USART*/

	//Pin de transmision

	pinTx.pGPIOx 							= GPIOA;
	pinTx.pinConfig.GPIO_PinNumber 			= PIN_9;
	pinTx.pinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	pinTx.pinConfig.GPIO_PinOutputType 		= GPIO_OTYPE_PUSHPULL;
	pinTx.pinConfig.GPIO_PinOutputSpeed 	= GPIO_OSPEEDR_FAST;
	pinTx.pinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	pinTx.pinConfig.GPIO_PinAltFunMode 		= AF7;

	//pin de recepcion

	pinRx.pGPIOx 							= GPIOA;
	pinRx.pinConfig.GPIO_PinNumber 			= PIN_10;
	pinRx.pinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	pinRx.pinConfig.GPIO_PinOutputType 		= GPIO_OTYPE_PUSHPULL;
	pinRx.pinConfig.GPIO_PinOutputSpeed 	= GPIO_OSPEEDR_FAST;
	pinRx.pinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	pinRx.pinConfig.GPIO_PinAltFunMode 		= AF7;

	gpio_Config(&pinTx);
	gpio_Config(&pinRx);

	/*CONFIGURACION TIMERS*/

	// Configuramos el timer para el blinky

    blinkTimer.pTIMx								=TIM3;
	blinkTimer.TIMx_Config.TIMx_Prescaler			=16000; //Genera incrementos de 1 ms
	blinkTimer.TIMx_Config.TIMx_Period				=250;   //DE la mano con el prescaler, se toma el periodo en ms
	blinkTimer.TIMx_Config.TIMx_mode				=TIMER_UP_COUNTER;
	blinkTimer.TIMx_Config.TIMx_InterruptEnable		=TIMER_INT_ENABLE;

	// Configuramos el timer  para obtener la frecuencia de switcheo de los dos canales del 7segmentos

    frec_7segment.pTIMx								=TIM5;
    frec_7segment.TIMx_Config.TIMx_Prescaler		=16000; //Genera incrementos de 1 ms
    frec_7segment.TIMx_Config.TIMx_Period			=5;   //DE la mano con el prescaler, se toma el periodo en ms
    frec_7segment.TIMx_Config.TIMx_mode				=TIMER_UP_COUNTER;
    frec_7segment.TIMx_Config.TIMx_InterruptEnable	=TIMER_INT_ENABLE;

    // Timer que controla la frecuencia de refresco del mensaje

	frec_message.pTIMx 								= TIM2;
	frec_message.TIMx_Config.TIMx_Prescaler 		= 16000;
	frec_message.TIMx_Config.TIMx_Period 			= 2000;
	frec_message.TIMx_Config.TIMx_mode 				= TIMER_UP_COUNTER;
	frec_message.TIMx_Config.TIMx_InterruptEnable 	= TIMER_INT_ENABLE;

	timer_Config(&blinkTimer);
	timer_Config(&frec_7segment);
	timer_Config(&frec_message);

	/*CONFIGURACION EXITS*/

	//Interrucion del boton del enconder

	interrupt_sw.edgeType		= EXTERNAL_INTERRUPT_FALLING_EDGE;
	interrupt_sw.pGPIOHandler	= &sw;

	//Interrucion del clock del enconder

	interrupt_clk.edgeType		= EXTERNAL_INTERRUPT_FALLING_EDGE;
	interrupt_clk.pGPIOHandler	= &clk;

	exti_Config(&interrupt_sw);
	exti_Config(&interrupt_clk);

	/*CONFIGURACION USART*/

	commSerial.ptrUSARTx 			 		= USART1;
	commSerial.USART_Config.baudrate 		= USART_BAUDRATE_115200;
	commSerial.USART_Config.datasize		= USART_DATASIZE_8BIT;
	commSerial.USART_Config.mode 	 		= USART_MODE_RXTX;
	commSerial.USART_Config.parity  		= USART_PARITY_NONE;
	commSerial.USART_Config.stopbits 		= USART_STOPBIT_1;
	commSerial.USART_Config.enableIntRX 	= USART_RX_INTERRUP_ENABLE;
	usart_Config(&commSerial);

	/*CONFIGURACION ADC*/

	//Señal 1
	adc_signal[0].channel				= CHANNEL_9;
	adc_signal[0].resolution			= RESOLUTION_12_BIT;
	adc_signal[0].dataAlignment 		= ALIGNMENT_RIGHT;
	adc_signal[0].samplingPeriod		= SAMPLING_PERIOD_84_CYCLES;
	adc_signal[0].interrupState			= ADC_INT_ENABLE;

	adc_signal[1].channel				= CHANNEL_6;
	adc_signal[1].resolution			= RESOLUTION_8_BIT;
	adc_signal[1].dataAlignment 		= ALIGNMENT_RIGHT;
	adc_signal[1].samplingPeriod		= SAMPLING_PERIOD_84_CYCLES;
	adc_signal[1].interrupState			= ADC_INT_ENABLE;

	adc_signal[2].channel				= CHANNEL_15;
	adc_signal[2].resolution			= RESOLUTION_6_BIT;
	adc_signal[2].dataAlignment 		= ALIGNMENT_RIGHT;
	adc_signal[2].samplingPeriod		= SAMPLING_PERIOD_84_CYCLES;
	adc_signal[2].interrupState			= ADC_INT_ENABLE;

	adc_ConfigSingleChannel(&adc_signal[0]);

	/*SETEO DE VALORES INICIALES*/

	gpio_WritePin(&stateled,SET);
	gpio_WritePin(&led_selector,SET);
	gpio_WritePin(&vcc_units_7seg, RESET);
	gpio_WritePin(&vcc_decs_7seg, RESET);

	timer_SetState(&blinkTimer, TIMER_ON);
	timer_SetState(&frec_7segment, TIMER_ON);
	timer_SetState(&frec_message, TIMER_ON);

	usart_WriteChar(&commSerial,0);


	counter=1;
	selector=1;
	segment_configuration_units(counter,1);



}

/*
 * leds_7segment[0]		A segment
 * leds_7segment[1]		B segment
 * leds_7segment[2]		C segment
 * leds_7segment[3]		D segment
 * leds_7segment[4]		E segment
 * leds_7segment[5]		F segment
 * leds_7segment[6]		G segment
 * leds_7segment[7]		DP
 * */

//Funcion para modificar el digito de las decenas del 7segmentos de 2 numeros

void segment_configuration_decs(ADC_Config_t *adcConfig,uint8_t digit_active){

	//Se tiene dos posibilidades dependiendo si el digito que se cargar esta activo para modificar
	// si esta activo para modificor el led de DP debe estar activo:
	if(digit_active){

		gpio_WritePin(&leds_7segment[7],!SET); // se activa el DP

		for(uint8_t i=0;i<=6;i++){
			gpio_WritePin(&leds_7segment[i],!RESET);
		}

		switch(adcConfig->resolution){

		case RESOLUTION_12_BIT:
			gpio_WritePin(&leds_7segment[0],!SET);
			gpio_WritePin(&leds_7segment[3],!SET);
			gpio_WritePin(&leds_7segment[6],!SET);
			break;

		case RESOLUTION_10_BIT:

			gpio_WritePin(&leds_7segment[3],!SET);
			gpio_WritePin(&leds_7segment[6],!SET);
			break;

		case RESOLUTION_8_BIT:
			gpio_WritePin(&leds_7segment[3],!SET);
			break;

		case RESOLUTION_6_BIT:
			gpio_WritePin(&leds_7segment[4],!SET);
			break;

		default:
			__NOP();
			break;
		}

	} else {

		gpio_WritePin(&leds_7segment[7],!RESET); // se desactiva el DP

		for(uint8_t i=0;i<=6;i++){
			gpio_WritePin(&leds_7segment[i],!RESET);
		}

		switch(adcConfig->resolution){

		case RESOLUTION_12_BIT:

			gpio_WritePin(&leds_7segment[0],!SET);
			gpio_WritePin(&leds_7segment[3],!SET);
			gpio_WritePin(&leds_7segment[6],!SET);
			break;

		case RESOLUTION_10_BIT:

			gpio_WritePin(&leds_7segment[3],!SET);
			gpio_WritePin(&leds_7segment[6],!SET);
			break;

		case RESOLUTION_8_BIT:
			gpio_WritePin(&leds_7segment[3],!SET);
			break;

		case RESOLUTION_6_BIT:
			gpio_WritePin(&leds_7segment[4],!SET);
			break;

		default:
			__NOP();
			break;
		}

	}


}

//Funcion para modificar el digito de las unidades del 7segmentos de 2 numeros

void segment_configuration_units(uint8_t number, uint8_t digit_active){


	//Se tiene dos posibilidades dependiendo si el digito que se cargar esta activo para modificar
	// si esta activo para modificor el led de DP debe estar activo:
	// esta funcion se hizo en general para los 9 numeros pero con el caso para 1,2 y 3 también funcionaria

	if(digit_active){

		gpio_WritePin(&leds_7segment[7],!SET);
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

			default:
				__NOP();
				break;
		}

	} else{

		gpio_WritePin(&leds_7segment[7],!RESET);
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

			default:
				__NOP();
				break;
		}

	}


}

uint8_t get_resolution_value(ADC_Config_t *adcConfig){
	int re_value=0;
	switch(adcConfig->resolution){
		case RESOLUTION_12_BIT:
			re_value = 12;
			break;
		case RESOLUTION_10_BIT:
			re_value = 10;
			break;
		case RESOLUTION_8_BIT:
			re_value = 8;
			break;
		case RESOLUTION_6_BIT:
			re_value = 6;
			break;
		default:
			__NOP();
			break;
	}
	return re_value;
}

// interrupcion externa del boton del encoder:
void callback_extInt0(void){
	flag_selector=1;
}

// interrupcion externa del clock del encoder:
void callback_extInt9(void){
	flag_clk=1;
}

//interrupcion del timer de frencuencia del 7 segmentos
void Timer5_Callback(void){
	flag_switcheo=1;

}

//interrupcion del timer que configura la frecuencia del mensaje
void Timer2_Callback(void) {

	//cuando se cumple el tiempo se inicia la conversion adc
	adc_ConfigSingleChannel(&adc_signal[counter-1]);
	adc_StartSingleConv();


}

//interrupcion del timer para activar/desactivar el led blinky
void Timer3_Callback(void) {
	gpio_TooglePin(&stateled);

}

//interrupcion por recepcion en el USART1
void usart1_RxCallback(void) {
	receivedChar = usart_getRxData1();
}

//interrupcion por finalizacion de conversion ADC1
void adc_CompleteCallback(void) {

	adc_signal[counter-1].adcData = adc_GetValue();
	flag_sendMsg=1;
	adc_peripheralOnOFF(ADC_OFF);
	//luego de tener el valor de la conversion adc mandamos el mensaje:




}


void assert_failed(uint8_t*file,uint32_t line){
	while(1){
		//problems
	}
}
