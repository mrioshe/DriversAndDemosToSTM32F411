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

ADC_Config_t signal[3]={0};

uint8_t receivedChar=0;
uint8_t sendMsg=0;
char bufferData[64]={0};

uint8_t flag_clk={0};
uint8_t counter={0};
uint8_t digit_selector=0;
uint8_t number_desc_aux[2]={0};
uint8_t direction_aux={0};
uint8_t flag_selector={0};
uint8_t selector_aux={0};


enum{
	SIGNAL_CONFIG=0,
	RESOLUTION_CONFIG
};

//funciones:

void initSys(void);
void segment_configuration_units(uint8_t number);
void segment_configuration_decs(ADC_Config_t *adcConfig);
void config(void);


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

	if(flag_selector){

		flag_selector=0;

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
	gpio_Config(&vcc_units_7seg);

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

    blinkTimer.pTIMx								=TIM5;
	blinkTimer.TIMx_Config.TIMx_Prescaler			=16000; //Genera incrementos de 1 ms
	blinkTimer.TIMx_Config.TIMx_Period				=250;   //DE la mano con el prescaler, se toma el periodo en ms
	blinkTimer.TIMx_Config.TIMx_mode				=TIMER_UP_COUNTER;
	blinkTimer.TIMx_Config.TIMx_InterruptEnable		=TIMER_INT_ENABLE;

	// Configuramos el timer  para obtener la frecuencia de switcheo de los dos canales del 7segmentos

    frec_7segment.pTIMx								=TIM10;
    frec_7segment.TIMx_Config.TIMx_Prescaler		=16000; //Genera incrementos de 1 ms
    frec_7segment.TIMx_Config.TIMx_Period			=10;   //DE la mano con el prescaler, se toma el periodo en ms
    frec_7segment.TIMx_Config.TIMx_mode				=TIMER_UP_COUNTER;
    frec_7segment.TIMx_Config.TIMx_InterruptEnable	=TIMER_INT_ENABLE;

    // Timer que controla la frecuencia de refresco del mensaje

	frec_message.pTIMx 								= TIM2;
	frec_message.TIMx_Config.TIMx_Prescaler 		= 16000;
	frec_message.TIMx_Config.TIMx_Period 			= 500;
	frec_message.TIMx_Config.TIMx_mode 				= TIMER_UP_COUNTER;
	frec_message.TIMx_Config.TIMx_InterruptEnable 	= TIMER_INT_ENABLE;

	timer_Config(&blinkTimer);
	timer_Config(&blinkTimer);
	timer_Config(&blinkTimer);

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
	signal[0].channel				= CHANNEL_6;
	signal[0].resolution			= RESOLUTION_12_BIT;
	signal[0].dataAlignment 		=ALIGNMENT_RIGHT;
	signal[0].samplingPeriod		= SAMPLING_PERIOD_84_CYCLES;
	signal[0].interrupState			= ADC_INT_ENABLE;

	signal[1].channel				= CHANNEL_7;
	signal[1].resolution			= RESOLUTION_12_BIT;
	signal[1].dataAlignment 		=ALIGNMENT_RIGHT;
	signal[1].samplingPeriod		= SAMPLING_PERIOD_84_CYCLES;
	signal[1].interrupState			= ADC_INT_ENABLE;

	signal[2].channel				= CHANNEL_15;
	signal[2].resolution			= RESOLUTION_12_BIT;
	signal[2].dataAlignment 		=ALIGNMENT_RIGHT;
	signal[2].samplingPeriod		= SAMPLING_PERIOD_84_CYCLES;
	signal[2].interrupState			= ADC_INT_ENABLE;

	for(uint8_t i=0;i<=2;i++){
			adc_ConfigSingleChannel(&signal[i]);
		}

	/*SETEO DE VALORES INICIALES*/

	gpio_WritePin(&stateled,SET);
	gpio_WritePin(&direction,SET);
	gpio_WritePin(&vcc1_7seg, SET);
	gpio_WritePin(&vcc2_7seg, SET);

	timer_SetState(&blinkTimer, TIMER_ON);
	timer_SetState(&frec_7segment, TIMER_ON);
	timer_SetState(&frec_message, TIMER_ON);

	usart_WriteChar(&commSerial,0);
	segment_configuration(0);
	counter=0;
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

void config(void);

void callback_extInt0(void){
	flag_selector=1;
	gpio_TooglePin(&direction);
	selector_aux=!selector_aux;
}


void Timer5_Callback(void){
	gpio_TooglePin(&stateled);
}

void Timer2_Callback(void) {
	gpio_TooglePin(&userLed);
	sendMsg = 1;
}

void usart1_RxCallback(void) {
	receivedChar = usart_getRxData1();
}

void adc_CompleteCallback(void) {
	potenciometro.adcData = adc_GetValue();
}


void assert_failed(uint8_t*file,uint32_t line){
	while(1){
		//problems
	}
}
