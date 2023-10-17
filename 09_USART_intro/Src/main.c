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
#include "usart_driver_hal.h"
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"
#include "exti_driver_hal.h"

USART_Handler_t usart2 = { 0 };
GPIO_Handler_t usart2t = { 0 };

Timer_Handler_t blinkTimer = { 0 };
GPIO_Handler_t userLed = { 0 };

EXTI_Config_t imprimir = {0};
GPIO_Handler_t user13 = {0};

//USART_Handler_t usart2rx = {0};
GPIO_Handler_t usart2trx = {0};

char bufferMsg[7] = {0};

uint8_t bandera = 0;
uint8_t RXdata = 0;

void initSys(void);

int main() {
initSys();
while (1) {
if ( bandera == 1){
usart_writeMsg(&usart2, bufferMsg);
bandera = 0;
}

if (RXdata == 'P'){
usart_writeMsg(&usart2, "JEJE");
RXdata = 0;
}
}
return 0;
}

void initSys(void) {
/* Configuramos el timer del blink (TIM2) */
blinkTimer.pTIMx = TIM2;
blinkTimer.TIMx_Config.TIMx_Prescaler = 16000;
blinkTimer.TIMx_Config.TIMx_Period = 250;
blinkTimer.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
blinkTimer.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;

timer_Config(&blinkTimer);
timer_SetState(&blinkTimer, SET);

//Configuramos los pines que se van a utilizar

/* Configuramos el PinA5 */
userLed.pGPIOx = GPIOA;
userLed.pinConfig.GPIO_PinNumber = PIN_5;
userLed.pinConfig.GPIO_PinMode = GPIO_MODE_OUT;
userLed.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
userLed.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_MEDIUM;
userLed.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

gpio_Config(&userLed);

usart2.ptrUSARTx = USART2;
usart2.USART_Config.baudrate = USART_BAUDRATE_115200;
usart2.USART_Config.datasize = USART_DATASIZE_8BIT;
usart2.USART_Config.mode = USART_MODE_RXTX;
usart2.USART_Config.parity = USART_PARITY_NONE;
usart2.USART_Config.stopbits = USART_STOPBIT_1;
usart2.USART_Config.enableIntTX = USART_TX_INTERRUP_DISABLE;
usart2.USART_Config.enableIntRX = USART_RX_INTERRUP_ENABLE;

usart_Config(&usart2);

/* Configuramos el PinA5 */
usart2t.pGPIOx = GPIOA;
usart2t.pinConfig.GPIO_PinNumber = PIN_2;
usart2t.pinConfig.GPIO_PinMode = GPIO_MODE_ALFTN;
usart2t.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
usart2t.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_MEDIUM;
usart2t.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
usart2t.pinConfig.GPIO_PinAltFunMode = AF7;

gpio_Config(&usart2t);

bufferMsg[0] = 'H';
bufferMsg[1] = 'o';
bufferMsg[2] = 'l';
bufferMsg[3] = 'a';
bufferMsg[4] = '\n';
bufferMsg[5] = 0;

usart_writeMsg(&usart2, bufferMsg);

user13.pGPIOx = GPIOC;
user13.pinConfig.GPIO_PinNumber = PIN_13;
user13.pinConfig.GPIO_PinMode = GPIO_MODE_IN;

gpio_Config(&user13);

imprimir.pGPIOHandler = &user13;
imprimir.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;

exti_Config(&imprimir);

// usart2rx.ptrUSARTx = USART2;
// usart2rx.USART_Config.baudrate = USART_BAUDRATE_115200;
// usart2rx.USART_Config.datasize = USART_DATASIZE_8BIT;
// usart2rx.USART_Config.mode = USART_MODE_RX;
// usart2rx.USART_Config.parity = USART_PARITY_NONE;
// usart2rx.USART_Config.stopbits = USART_STOPBIT_1;
// usart2rx.USART_Config.enableIntTX = USART_TX_INTERRUP_DISABLE;
// usart2rx.USART_Config.enableIntRX = USART_RX_INTERRUP_DISABLE;
//
// usart_Config(&usart2rx);

/* Configuramos el PinA5 */
usart2trx.pGPIOx = GPIOA;
usart2trx.pinConfig.GPIO_PinNumber = PIN_3;
usart2trx.pinConfig.GPIO_PinMode = GPIO_MODE_ALFTN;
usart2trx.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
usart2trx.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEED_MEDIUM;
usart2trx.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
usart2trx.pinConfig.GPIO_PinAltFunMode = AF7;

gpio_Config(&usart2trx);

}

void Timer2_Callback(void) {
gpio_TooglePin(&userLed);
}

void callback_ExtInt13(void){
bandera = 1;
}
void usart2_RxCallback(void){
RXdata = usart_getRxData2();
}






void assert_failed(uint8_t*file,uint32_t line){
	while(1){
		//problems
	}
}
