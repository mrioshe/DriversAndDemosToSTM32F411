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
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stm32f4xx.h>
#include <stm32_assert.h>
#include <string.h>

#include "usart_driver_hal.h"
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"
#include "exti_driver_hal.h"
#include "adc_driver_hal.h"
#include "pwm_driver_hal.h"

#include "arm_math.h"



GPIO_Handler_t userLed = { 0 };
GPIO_Handler_t adc_trigger_signal = { 0 };
GPIO_Handler_t PWMpin={0};

EXTI_Config_t adcTriggerSignal={0};

Timer_Handler_t blinkTimer = { 0 };

USART_Handler_t commSerial= { 0 };
GPIO_Handler_t pinTx = {0};
GPIO_Handler_t pinRx = {0};
uint8_t receivedChar=0;
uint8_t sendMsg=0;
char bufferData[64]={0};

PWM_Handler_t pwm={0};

uint16_t duttyValue=20000;
uint16_t PWMperiod=40000;
uint8_t sequencyData=0;

//Elementos para la conversion ADC
ADC_Config_t sensors[3] ={0};

/*arreglos para pruebas de las librerias de CMSIS*/

float32_t srcNumber[4] = {-0.987,32.26,-45.21,-987.321};
float32_t destNumber[4] ={0};
uint32_t dataSize=0;

/*Para utilizar la función seno*/

float32_t sineValue =0.0;
float32_t sineArgValue=0.0;

/*Arreglos para la toma de datos:*/
uint16_t data1[512]={0};
uint16_t data2[512]={0};
uint16_t data3[512]={0};

uint16_t counterData=1;

/*Elementos para generar una señal*/

#define SINE_DATA_SIZE 4096 // Tamaño del arreglo de datos.
float32_t fs=8000.0; //Frecuencia de muestreo
float32_t f0=250.0; //Frecuencia fundamental de la señal
float32_t dt=0.0; //periodo de muestreo (1/fs)
float32_t stopTime=1.0; //periodo de muestreo (1/fs)
float32_t amplitud=5; //amplitud de la señal generada
float32_t sineSignal[SINE_DATA_SIZE];
float32_t transformedSignal[SINE_DATA_SIZE];
float32_t *pSineSignal;

uint32_t ifftFlag=0;
uint32_t doBitReverse=1;
arm_rfft_fast_instance_f32 config_Rfft_fast_f32;
arm_cfft_radix4_instance_f32 configRadix4_f32;
arm_status status=ARM_MATH_ARGUMENT_ERROR;
arm_status statusInitFFT=ARM_MATH_ARGUMENT_ERROR;
uint16_t fftSize=1024;


void initSys(void);
void createSignal(void);

int main() {
	initSys();
while (1) {

		if (receivedChar=='a'){
			createSignal();
			sprintf(bufferData,"La información de la señal 1 es... \n");
			usart_writeMsg(&commSerial,bufferData);
			receivedChar='\0';

		}

		if (receivedChar=='b'){
			createSignal();
			sprintf(bufferData,"La información de la señal 1 es... \n");
			usart_writeMsg(&commSerial,bufferData);
			receivedChar='\0';

		}

		if (receivedChar=='c'){
			createSignal();
			sprintf(bufferData,"CLa información de la señal 1 es... \n");
			usart_writeMsg(&commSerial,bufferData);
			receivedChar='\0';

		}

	 	if (receivedChar=='C'){
	 		createSignal();
	 		sprintf(bufferData,"Creando la señal... \n");
			usart_writeMsg(&commSerial,bufferData);
			receivedChar='\0';

		}

	 	if (receivedChar=='P'){

	 		stopTime=0.0;
	 		int i=0;

	 		sprintf(bufferData,"Signal values:time - Sine \n");
	 		usart_writeMsg(&commSerial,bufferData);

	 		while(stopTime<0.01){
	 			stopTime=dt*i;
	 			i++;
	 			sprintf(bufferData,"%#.5f ; %#.6f\n",stopTime,sineSignal[i]);
	 			usart_writeMsg(&commSerial,bufferData);

	 		}
	 		receivedChar='\0';
		}



	 	if (receivedChar=='A'){

	 		stopTime=0.0;
	 		int i=0;

	 		sprintf(bufferData,"Valor absoluto \n");
	 		usart_writeMsg(&commSerial,bufferData);

	 		arm_abs_f32(sineSignal,transformedSignal,SINE_DATA_SIZE);

	 		while(stopTime<0.01){
	 			stopTime=dt*i;
	 			i++;
	 			sprintf(bufferData,"%#.5f ; %#.6f\n",stopTime,transformedSignal[i]);
	 			usart_writeMsg(&commSerial,bufferData);

	 		}
	 		receivedChar='\0';
		}

	 	if (receivedChar=='I'){

	 		statusInitFFT=arm_rfft_fast_init_f32(&config_Rfft_fast_f32,fftSize);

	 		if(statusInitFFT==ARM_MATH_SUCCESS){
	 			sprintf(bufferData, "Initialization... SUCCESS! \n");
	 			usart_writeMsg(&commSerial,bufferData);
	 		}

	 		receivedChar='\0';

		}

	 	if (receivedChar=='F'){

	 		stopTime=0.0;
	 		int i=0;
	 		int j=0;

	 		sprintf(bufferData,"FFT \n");
	 		usart_writeMsg(&commSerial,bufferData);

	 		if(statusInitFFT==ARM_MATH_SUCCESS){

		 		arm_rfft_fast_f32(&config_Rfft_fast_f32,sineSignal,transformedSignal,ifftFlag);

		 		arm_abs_f32(transformedSignal, sineSignal,fftSize);

		 		for(i=1;i<fftSize;i++){
		 			if(i%2){
		 				sprintf(bufferData,"%u ; %#.6f\n",j,2*sineSignal[i]);
		 				usart_writeMsg(&commSerial,bufferData);
		 				j++;
		 			}
		 		}
	 		}

	 		else{
	 			usart_writeMsg(&commSerial,"FFT no Initialized");
	 		}

	 		receivedChar='\0';
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

	gpio_Config(&userLed);
	gpio_WritePin(&userLed,SET);


	/* Configuramos el timer del blink (TIM2) */
	blinkTimer.pTIMx = TIM2;
	blinkTimer.TIMx_Config.TIMx_Prescaler = 16000;
	blinkTimer.TIMx_Config.TIMx_Period = 2000;
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

	PWMpin.pGPIOx = GPIOC;
	PWMpin.pinConfig.GPIO_PinNumber			= PIN_6;
	PWMpin.pinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	PWMpin.pinConfig.GPIO_PinOutputType 	= GPIO_OTYPE_PUSHPULL;
	PWMpin.pinConfig.GPIO_PinOutputSpeed 	= GPIO_OSPEEDR_FAST;
	PWMpin.pinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	PWMpin.pinConfig.GPIO_PinAltFunMode 	= AF2;

	gpio_Config(&PWMpin);

	pwm.pTIMx			 	= TIM3;
	pwm.config.timer		= TIMER_TIM3;
	pwm.config.dutty		= duttyValue;
	pwm.config.channel		= PWM_CHANNEL_1;
	pwm.config.prescaler	= 16;
	pwm.config.period		= PWMperiod;
	pwm_Config(&pwm);

	startPWMsignal(&pwm);

	/*Configurando la conversion ADC*/

	sensors[0].channel			= CHANNEL_9;
	sensors[0].resolution		= RESOLUTION_12_BIT;
	sensors[0].dataAlignment 	= ALIGNMENT_RIGHT;
	sensors[0].samplingPeriod	= SAMPLING_PERIOD_84_CYCLES;
	sensors[0].interrupState	= ADC_INT_ENABLE;

	sensors[1].channel			= CHANNEL_6;
	sensors[1].resolution		= RESOLUTION_12_BIT;
	sensors[1].dataAlignment 	= ALIGNMENT_RIGHT;
	sensors[1].samplingPeriod	= SAMPLING_PERIOD_84_CYCLES;
	sensors[1].interrupState	= ADC_INT_ENABLE;

	sensors[2].channel			= CHANNEL_15;
	sensors[2].resolution		= RESOLUTION_12_BIT;
	sensors[2].dataAlignment 	= ALIGNMENT_RIGHT;
	sensors[2].samplingPeriod	= SAMPLING_PERIOD_84_CYCLES;
	sensors[2].interrupState	= ADC_INT_ENABLE;


	number_of_sensors=3;
	adc_ConfigMultiChannel(sensors,number_of_sensors);


}

void createSignal(void){
	dt=1/fs;

	for(int i=0;i<SINE_DATA_SIZE;i++){
		sineSignal[i]=amplitud*arm_sin_f32(2*M_PI*f0*(dt*i));
	}
}

void Timer2_Callback(void) {
	gpio_TooglePin(&userLed);
}

void usart2_RxCallback(void) {
	receivedChar = usart_getRxData2();
	adc_startTriggeredAdc(DETECTION_RISING_EDGE,TIM3_CH1_EVENT);

}

void adc_CompleteCallback(void) {

	if(counterData !=(512*3)){
		sensors[sequencyData].adcData = adc_GetValue();

		switch(sequencyData){

		case 0:
			data1[couterData]=sensors[sequencyData].adcData;
			break;
		case 1:
			data2[couterData]=sensors[sequencyData].adcData;
			break;
		case 2:
			data3[couterData]=sensors[sequencyData].adcData;
			break;
		default:
			_NOP();
			break;
		}

		sequencyData++;
		if(sequencyData>=number_of_sensors){
			sequencyData=0;
		}
		counterData++;
	} else if(counterData ==(512*3)){
		counterData=0;
		adc_StopTriggeredAdc();
	}


}


void assert_failed(uint8_t*file,uint32_t line){
	while(1){
		//problems
	}
}
void assert_failed(uint8_t*file,uint32_t line){
	while(1){
		//problems
	}
}
