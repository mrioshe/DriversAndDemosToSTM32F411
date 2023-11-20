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
#include "pwm_driver_hal.h"


GPIO_Handler_t userLed = { 0 };
GPIO_Handler_t adc_trigger_signal = { 0 };
GPIO_Handler_t PWMpin={0};

Timer_Handler_t blinkTimer = { 0 };

EXTI_Config_t adcTriggerSignal={0};

USART_Handler_t commSerial= { 0 };
GPIO_Handler_t pinTx = {0};
GPIO_Handler_t pinRx = {0};
char bufferData[64]={0};


PWM_Handler_t pwm={0};

uint8_t receivedChar=0;
uint8_t sendMsg=0;
uint8_t sequencyData=0;
uint8_t number_of_sensors=0;

uint16_t duttyValue=1000;
uint16_t PWMperiod=2000;


//Elementos para la conversion ADC
ADC_Config_t sensors[3] ={0};

void initSys(void);

int main() {
	initSys();
while (1) {

	 	__NOP();
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

	adc_trigger_signal.pGPIOx							= GPIOB;
	adc_trigger_signal.pinConfig.GPIO_PinNumber			= PIN_11;
	adc_trigger_signal.pinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	adc_trigger_signal.pinConfig.GPIO_PinOutputType		= GPIO_OTYPE_PUSHPULL;
	adc_trigger_signal.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;
	adc_trigger_signal.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	gpio_Config(&adc_trigger_signal);
	gpio_WritePin(&adc_trigger_signal,SET);

	adcTriggerSignal.edgeType		= EXTERNAL_INTERRUPT_RISING_EDGE;
	adcTriggerSignal.pGPIOHandler	= &adc_trigger_signal;

	/* Configuramos el timer del blink (TIM2) */
	blinkTimer.pTIMx = TIM2;
	blinkTimer.TIMx_Config.TIMx_Prescaler = 16000;
	blinkTimer.TIMx_Config.TIMx_Period = 2000;
	blinkTimer.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	blinkTimer.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;

	timer_Config(&blinkTimer);
	timer_SetState(&blinkTimer, SET);

//	pinTx.pGPIOx = GPIOA;
//	pinTx.pinConfig.GPIO_PinNumber = PIN_2;
//	pinTx.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
//	pinTx.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
//	pinTx.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_FAST;
//	pinTx.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
//	pinTx.pinConfig.GPIO_PinAltFunMode = AF7;
//	gpio_Config(&pinTx);
//
//
//	pinRx.pGPIOx = GPIOA;
//	pinRx.pinConfig.GPIO_PinNumber = PIN_3;
//	pinRx.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
//	pinRx.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
//	pinRx.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_FAST;
//	pinRx.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
//	pinRx.pinConfig.GPIO_PinAltFunMode = AF7;
//	gpio_Config(&pinRx);

//	commSerial.ptrUSARTx = USART2;
//	commSerial.USART_Config.baudrate = USART_BAUDRATE_115200;
//	commSerial.USART_Config.datasize = USART_DATASIZE_8BIT;
//	commSerial.USART_Config.mode = USART_MODE_RXTX;
//	commSerial.USART_Config.parity = USART_PARITY_NONE;
//	commSerial.USART_Config.stopbits = USART_STOPBIT_1;
//	commSerial.USART_Config.enableIntRX = USART_RX_INTERRUP_ENABLE;
//	usart_Config(&commSerial);
//
//	usart_WriteChar(&commSerial,0);


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
	pwm.config.prescaler	= 1600;
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
	adc_startTriggeredAdc(DETECTION_RISING_EDGE,TIM3_CH1_EVENT);

}

void Timer2_Callback(void) {
	gpio_TooglePin(&userLed);
	sendMsg = 1;
}

void usart2_RxCallback(void) {
	receivedChar = usart_getRxData2();
}

void adc_CompleteCallback(void) {

	sensors[sequencyData].adcData = adc_GetValue();
	sequencyData++;
	if(sequencyData>=number_of_sensors){
		sequencyData=0;
	}
}


void assert_failed(uint8_t*file,uint32_t line){
	while(1){
		//problems
	}
}
