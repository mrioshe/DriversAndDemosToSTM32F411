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


GPIO_Handler_t userLed = { 0 };
Timer_Handler_t blinkTimer = { 0 };

USART_Handler_t commSerial= { 0 };
GPIO_Handler_t pinTx = {0};
GPIO_Handler_t pinRx = {0};
uint8_t receivedChar=0;
uint8_t sendMsg=0;
char bufferData[64]={0};

//Elementos para la conversion ADC
ADC_Config_t potenciometro ={0};

void segment_configuration_units(uint8_t number);
void segment_configuration_decs(ADC_Config_t *adcConfig);

void initSys(void);

int main() {
	initSys();
while (1) {

	 	if (sendMsg== 1){
			sendMsg=0;
			usart_writeMsg(&commSerial,"Hola Mundo !!\n\r");

		}



	if(receivedChar){
		if(receivedChar=='p'){
			usart_writeMsg(&commSerial, "Testing, testing!!\n\r");
		}

		if(receivedChar=='s'){
			usart_writeMsg(&commSerial,"make simple ADC\n\r");
			adc_StartSingleConv();

		}

		if(receivedChar=='C'){
			usart_writeMsg(&commSerial,"make continuous ADC\n\r");
			adc_StartContinuousConv();
		}

		if(receivedChar == 'S'){
			usart_writeMsg(&commSerial,"stop continuous ADC\n\r");
			adc_StopContinuousConv();
		}

		receivedChar=0;

	}

}

}

void initSys(void) {

	/* Configuramos el PinA5 */
	userLed.pGPIOx = GPIOA;
	userLed.pinConfig.GPIO_PinNumber = PIN_5;
	userLed.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	userLed.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	userLed.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_MEDIUM;
	userLed.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

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

	gpio_Config(&userLed);
	gpio_WritePin(&userLed,SET);

	/* Configuramos el timer del blink (TIM2) */
	blinkTimer.pTIMx = TIM2;
	blinkTimer.TIMx_Config.TIMx_Prescaler = 16000;
	blinkTimer.TIMx_Config.TIMx_Period = 500;
	blinkTimer.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	blinkTimer.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;

	timer_Config(&blinkTimer);
	timer_SetState(&blinkTimer, SET);

	pinTx.pGPIOx = GPIOA;
	pinTx.pinConfig.GPIO_PinNumber = PIN_2;
	pinTx.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	pinTx.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	pinTx.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_FAST;
	pinTx.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	pinTx.pinConfig.GPIO_PinAltFunMode = AF7;
	gpio_Config(&pinTx);


	pinRx.pGPIOx = GPIOA;
	pinRx.pinConfig.GPIO_PinNumber = PIN_3;
	pinRx.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	pinRx.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	pinRx.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_FAST;
	pinRx.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	pinRx.pinConfig.GPIO_PinAltFunMode = AF7;
	gpio_Config(&pinRx);

	commSerial.ptrUSARTx = USART2;
	commSerial.USART_Config.baudrate = USART_BAUDRATE_115200;
	commSerial.USART_Config.datasize = USART_DATASIZE_8BIT;
	commSerial.USART_Config.mode = USART_MODE_RXTX;
	commSerial.USART_Config.parity = USART_PARITY_NONE;
	commSerial.USART_Config.stopbits = USART_STOPBIT_1;
	commSerial.USART_Config.enableIntRX = USART_RX_INTERRUP_ENABLE;
	usart_Config(&commSerial);

	usart_WriteChar(&commSerial,0);

	/*Configurando la conversion ADC*/

	potenciometro.channel			= CHANNEL_0;
	potenciometro.resolution		= RESOLUTION_12_BIT;
	potenciometro.dataAlignment 	= ALIGNMENT_RIGHT;
	potenciometro.samplingPeriod	= SAMPLING_PERIOD_84_CYCLES;
	potenciometro.interrupState		= ADC_INT_ENABLE;
	adc_ConfigSingleChannel(&potenciometro);

}



void Timer2_Callback(void) {
	gpio_TooglePin(&userLed);
	sendMsg = 1;
}

void usart2_RxCallback(void) {
	receivedChar = usart_getRxData2();
}

void adc_CompleteCallback(void) {
	potenciometro.adcData = adc_GetValue();
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

void segment_configuration_decs(ADC_Config_t *adcConfig){

	gpio_WritePin(&leds_7segment[7],!SET);

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


void segment_configuration_units(uint8_t number){

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
}


void assert_failed(uint8_t*file,uint32_t line){
	while(1){
		//problems
	}
}
