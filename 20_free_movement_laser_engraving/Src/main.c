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
#include <math.h>
#include <stm32f4xx.h>
#include <stm32_assert.h>
#include <string.h>

#include "usart_driver_hal.h"
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"
#include "exti_driver_hal.h"
#include "adc_driver_hal.h"
#include "laserEngraving_driver_hal.h"

#include "arm_math.h"

GPIO_Handler_t userLed ={0};
GPIO_Handler_t PWMpinLaser={0};
GPIO_Handler_t enablePinLaser={0};
GPIO_Handler_t PWMpinMotorx={0};
GPIO_Handler_t enablePinMotorx={0};
GPIO_Handler_t directionPinMotorx={0};
GPIO_Handler_t PWMpinMotory={0};
GPIO_Handler_t enablePinMotory={0};
GPIO_Handler_t directionPinMotory={0};

EXTI_Config_t EncoderInterrupt={0};

Timer_Handler_t blinkTimer = { 0 };

USART_Handler_t commSerial= { 0 };
GPIO_Handler_t pinTx = {0};
GPIO_Handler_t pinRx = {0};
uint8_t receivedChar=0;
uint8_t sendMsg=0;
char bufferData[64]={0};
char bufferData2[128]={0};

PWM_Handler_t PWMlaser={0};
PWM_Handler_t PWMmotorx={0};
PWM_Handler_t PWMmotory={0};

uint16_t PWMperiod=5000;
uint16_t duttyValue=2500;

//Elementos para la conversion ADC
ADC_Config_t tempSensor ={0};

/*Elementos para utilizar las funciones de ARM math*/

/*Elemento para usar el systick*/

Systic_Handler_t systick = {0};

/*Elementos de grabado laser*/

laser_engraving_t motorx={0};
laser_engraving_t motory={0};


void initSys(void);

int main() {

initSys();
while (1) {


	if(receivedChar=='w'){
		set_motor_direction(&motory,1);
		movement(&motory);
		receivedChar=0;
	}

	if(receivedChar=='a'){
		set_motor_direction(&motorx,1);
		movement(&motorx);
		receivedChar=0;

	}

	if(receivedChar=='s'){
		set_motor_direction(&motory,0);
		movement(&motory);
		receivedChar=0;
	}

	if(receivedChar == 'd'){
		set_motor_direction(&motorx,0);
		movement(&motorx);
		receivedChar=0;
	}

	if(receivedChar == 'l'){
		engraving(&motorx);
		receivedChar=0;
	}


	}
}

void initSys(void) {


	/* Configuramos el PinA5 para el blinky */
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
	blinkTimer.TIMx_Config.TIMx_Period = 1000;
	blinkTimer.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	blinkTimer.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;

	timer_Config(&blinkTimer);
	timer_SetState(&blinkTimer, SET);

	tempSensor.channel				= CHANNEL_15;
	tempSensor.resolution			= RESOLUTION_12_BIT;
	tempSensor.dataAlignment 		= ALIGNMENT_RIGHT;
	tempSensor.samplingPeriod		= SAMPLING_PERIOD_84_CYCLES;
	tempSensor.interrupState		= ADC_INT_ENABLE;
	adc_ConfigSingleChannel(&tempSensor);

	/*Configuración pines para control de motores y laser*/

	enablePinLaser.pGPIOx = GPIOC;
	enablePinLaser.pinConfig.GPIO_PinNumber 	= 4;
	enablePinLaser.pinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
	enablePinLaser.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	enablePinLaser.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_MEDIUM;
	enablePinLaser.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	enablePinMotorx.pGPIOx = GPIOA;
	enablePinMotorx.pinConfig.GPIO_PinNumber 	= PIN_12;
	enablePinMotorx.pinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
	enablePinMotorx.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	enablePinMotorx.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_MEDIUM;
	enablePinMotorx.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	enablePinMotory.pGPIOx = GPIOC;
	enablePinMotory.pinConfig.GPIO_PinNumber 	= PIN_7;
	enablePinMotory.pinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
	enablePinMotory.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	enablePinMotory.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_MEDIUM;
	enablePinMotory.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	directionPinMotorx.pGPIOx = GPIOA;
	directionPinMotorx.pinConfig.GPIO_PinNumber 	= PIN_7;
	directionPinMotorx.pinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
	directionPinMotorx.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	directionPinMotorx.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_MEDIUM;
	directionPinMotorx.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	directionPinMotory.pGPIOx = GPIOB;
	directionPinMotory.pinConfig.GPIO_PinNumber 	= PIN_8;
	directionPinMotory.pinConfig.GPIO_PinMode 		= GPIO_MODE_OUT;
	directionPinMotory.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	directionPinMotory.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_MEDIUM;
	directionPinMotory.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	gpio_Config(&enablePinLaser);
	gpio_Config(&enablePinMotorx);
	gpio_Config(&enablePinMotory);
	gpio_Config(&directionPinMotorx);
	gpio_Config(&directionPinMotory);

	/*Configurando pines para PWM*/

	PWMpinLaser.pGPIOx = GPIOC;
	PWMpinLaser.pinConfig.GPIO_PinNumber			= PIN_6;
	PWMpinLaser.pinConfig.GPIO_PinMode 				= GPIO_MODE_ALTFN;
	PWMpinLaser.pinConfig.GPIO_PinOutputType 		= GPIO_OTYPE_PUSHPULL;
	PWMpinLaser.pinConfig.GPIO_PinOutputSpeed 		= GPIO_OSPEEDR_FAST;
	PWMpinLaser.pinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	PWMpinLaser.pinConfig.GPIO_PinAltFunMode 		= AF2;

	PWMpinMotorx.pGPIOx = GPIOA;
	PWMpinMotorx.pinConfig.GPIO_PinNumber			= PIN_1;
	PWMpinMotorx.pinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	PWMpinMotorx.pinConfig.GPIO_PinOutputType 		= GPIO_OTYPE_PUSHPULL;
	PWMpinMotorx.pinConfig.GPIO_PinOutputSpeed 		= GPIO_OSPEEDR_FAST;
	PWMpinMotorx.pinConfig.GPIO_PinPuPdControl	 	= GPIO_PUPDR_NOTHING;
	PWMpinMotorx.pinConfig.GPIO_PinAltFunMode 		= AF2;

	PWMpinMotory.pGPIOx = GPIOB;
	PWMpinMotory.pinConfig.GPIO_PinNumber			= PIN_7;
	PWMpinMotory.pinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	PWMpinMotory.pinConfig.GPIO_PinOutputType 		= GPIO_OTYPE_PUSHPULL;
	PWMpinMotory.pinConfig.GPIO_PinOutputSpeed 		= GPIO_OSPEEDR_FAST;
	PWMpinMotory.pinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	PWMpinMotory.pinConfig.GPIO_PinAltFunMode 		= AF2;

	gpio_Config(&PWMpinLaser);
	gpio_Config(&PWMpinMotorx);
	gpio_Config(&PWMpinMotory);

	/*Configuración de los PWM*/

	PWMlaser.pTIMx			 	= TIM5;
	PWMlaser.config.timer		= TIMER_TIM5;
	PWMlaser.config.dutty		= duttyValue;
	PWMlaser.config.channel		= PWM_CHANNEL_2;
	PWMlaser.config.prescaler	= 16;
	PWMlaser.config.period		= PWMperiod;

	PWMmotorx.pTIMx			 	= TIM4;
	PWMmotorx.config.timer		= TIMER_TIM4;
	PWMmotorx.config.dutty		= duttyValue;
	PWMmotorx.config.channel	= PWM_CHANNEL_2;
	PWMmotorx.config.prescaler	= 16;
	PWMmotorx.config.period		= PWMperiod;

	PWMmotory.pTIMx			 	= TIM3;
	PWMmotory.config.timer		= TIMER_TIM3;
	PWMmotory.config.dutty		= duttyValue;
	PWMmotory.config.channel	= PWM_CHANNEL_1;
	PWMmotory.config.prescaler	= 16;
	PWMmotory.config.period		= PWMperiod;


	pwm_Config(&PWMlaser);
	pwm_Config(&PWMmotorx);
	pwm_Config(&PWMmotory);


	/*Activamos FPU*/
	SCB->CPACR |= (0xF<<20);

	/*Configuración USART2*/

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

	/*Configuración del sistick*/

	systick.Systick							= SysTick;
	systick.Systick_Config_t.systemClock	=HSI_TIMER_16MHz;
	config_systick_ms(&systick);

/*Configuración de las estructuras para grabar*/

	motorx.pGIPO_enable_laser 		= &enablePinLaser;
	motorx.pGIPO_enable_motor 		= &enablePinMotorx;
	motorx.pGIPO_motor_direction	= &directionPinMotorx;
	motorx.pPWM_laser 				= &PWMlaser;
	motorx.pPWM_motor 				= &PWMmotorx;
	motorx.config.direction			= DIRECTION1;
	motorx.config.laser_power		= LASER_POWER_1000Hz;
	motorx.config.time_step			= 20;					//valor en ms
	motorx.config.velocity			= LASER_VELOCITY_200Hz;

	motory.pGIPO_enable_laser 		= &enablePinLaser;
	motory.pGIPO_enable_motor 		= &enablePinMotory;
	motory.pGIPO_motor_direction	= &directionPinMotory;
	motory.pPWM_laser 				= &PWMlaser;
	motory.pPWM_motor 				= &PWMmotory;
	motory.config.direction			= DIRECTION1;
	motory.config.laser_power		= LASER_POWER_1000Hz;
	motory.config.time_step			= 20;					//valor en ms
	motory.config.velocity			= LASER_VELOCITY_200Hz;

	laser_init_config(&motorx);
	laser_init_config(&motory);

}


void Timer2_Callback(void) {
	gpio_TooglePin(&userLed);

}

void usart2_RxCallback(void) {
	receivedChar = usart_getRxData2();


}

//void adc_CompleteCallback(void) {
//
//
//}


void assert_failed(uint8_t*file,uint32_t line){
	while(1){
		//problems
	}
}

