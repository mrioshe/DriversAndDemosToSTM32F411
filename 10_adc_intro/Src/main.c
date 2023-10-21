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
		sendMsg=1;
	}

	void usart2_RxCallback(void) {
		receivedChar = usart_getRxData2();
	}

	void adc_CompleteCallback(void) {
		potenciometro.adcData = adc_GetValue();
	}


void assert_failed(uint8_t*file,uint32_t line){
	while(1){
		//problems
	}
}
