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
#include <stdbool.h>
#include <stm32f4xx.h>
#include <stm32_assert.h>
#include "usart_driver_hal.h"
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"
#include "exti_driver_hal.h"
#include "adc_driver_hal.h"
#include "systick_driver_hal.h"

#define HSI_CLOCK_CONFIGURED 0
#define HSE_CLOCK_CONFIGURED 1
#define PLL_CLOCK_CONFIGURED 2

GPIO_Handler_t userLed = {0};
GPIO_Handler_t userbutton = {0};
EXTI_Config_t userbuttonEXTI= {0};
Timer_Handler_t blinkTimer = {0};
USART_Handler_t commSerial= {0};
GPIO_Handler_t pinTx = {0};
GPIO_Handler_t pinRx = {0};
Systic_Handler_t systick = {0};
uint8_t receivedChar=0;
uint8_t sendMsg=0;
char bufferData[64]={0};

//Elementos para la conversion ADC
ADC_Config_t potenciometro ={0};

void initSys(void);

int main() {
	initSys();
while (1) {

	if (sendMsg > 4){

		usart_writeMsg(&commSerial,"Hola mundo\n\r!!");

		//Pruebas al systick

		gpio_TooglePin(&userLed);
		systick_Delay_ms(300);
		gpio_TooglePin(&userLed);
		systick_Delay_ms(300);
		gpio_TooglePin(&userLed);
		systick_Delay_ms(300);
		gpio_TooglePin(&userLed);
		systick_Delay_ms(300);

		gpio_TooglePin(&userLed);
		systick_Delay_ms(250);
		gpio_TooglePin(&userLed);
		systick_Delay_ms(250);
		gpio_TooglePin(&userLed);
		systick_Delay_ms(250);
		gpio_TooglePin(&userLed);
		systick_Delay_ms(250);

		usart_writeMsg(&commSerial,"Contando cada segundo\n\r");

		gpio_TooglePin(&userLed);
		systick_Delay_ms(1000);
		gpio_TooglePin(&userLed);
		systick_Delay_ms(1000);
		gpio_TooglePin(&userLed);
		systick_Delay_ms(1000);
		gpio_TooglePin(&userLed);
		systick_Delay_ms(10000);
		gpio_TooglePin(&userLed);
		systick_Delay_ms(10000);
		sendMsg=0;

		}
	}
}

void initSys(void) {


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

	/* Configuramos el PinA5 */
	userLed.pGPIOx = GPIOA;
	userLed.pinConfig.GPIO_PinNumber = PIN_5;
	userLed.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	userLed.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	userLed.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_MEDIUM;
	userLed.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	gpio_Config(&userLed);
	gpio_WritePin(&userLed,SET);

	userbutton.pGPIOx							= GPIOC;
	userbutton.pinConfig.GPIO_PinNumber			= PIN_13;
	userbutton.pinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	userbutton.pinConfig.GPIO_PinOutputType		= GPIO_OTYPE_PUSHPULL;
	userbutton.pinConfig.GPIO_PinOutputSpeed	= GPIO_OSPEEDR_MEDIUM;
	userbutton.pinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;


	userbuttonEXTI.edgeType		= EXTERNAL_INTERRUPT_FALLING_EDGE;
	userbuttonEXTI.pGPIOHandler	= &userbutton;
	exti_Config(&userbuttonEXTI);

	systick.Systick							= SysTick;
	systick.Systick_Config_t.systemClock	=HSI_TIMER_16MHz;
	config_systick_ms(&systick);

	/* Configuramos el timer del blink (TIM2) */
	blinkTimer.pTIMx = TIM2;
	blinkTimer.TIMx_Config.TIMx_Prescaler = 16000;
	blinkTimer.TIMx_Config.TIMx_Period = 500;
	blinkTimer.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	blinkTimer.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;

	timer_Config(&blinkTimer);
	timer_SetState(&blinkTimer, SET);

}

void Timer2_Callback(void) {
	//gpio_TooglePin(&userLed);
	sendMsg ++;
}

void usart2_RxCallback(void) {
	receivedChar = usart_getRxData2();
}

void callback_extInt13(void) {
	__NOP();
}


void assert_failed(uint8_t*file,uint32_t line){
	while(1){
		//problems
	}
}
