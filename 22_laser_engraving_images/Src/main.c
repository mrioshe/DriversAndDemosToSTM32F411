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
GPIO_Handler_t SWpin={0};

EXTI_Config_t SWinterrupt={0};

Timer_Handler_t blinkTimer = { 0 };
Timer_Handler_t GetDataTimer = { 0 };

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


/*Elemento para usar el systick*/

Systic_Handler_t systick = {0};

/*Elementos de grabado laser*/

laser_engraving_t motorx={0};
laser_engraving_t motory={0};
uint8_t LaserStatus={0};

uint16_t imageHeight=451;
uint16_t imageWidth=640;
uint8_t rowData[640]={0};
uint16_t counterRow={0};
uint16_t counterRowAux={0};
uint16_t counterColum={0};

uint8_t flagStart={0};
uint8_t flagRowComplete={0};

void initSys(void);
void EngravingImage(void);
int8_t char2int(char number2convert);

int main() {

	initSys();
	while (1) {

		if (flagStart){
			flagStart=0;
			EngravingImage();
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

	/*Configuramos botón del Joystick*/

	SWpin.pGPIOx = GPIOB;
	SWpin.pinConfig.GPIO_PinNumber 		= PIN_10;
	SWpin.pinConfig.GPIO_PinMode 		= GPIO_MODE_IN;
	SWpin.pinConfig.GPIO_PinOutputType  = GPIO_OTYPE_PUSHPULL;
	SWpin.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_MEDIUM;
	SWpin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;


	/* Configuramos el timer del blink (TIM2) */
	blinkTimer.pTIMx = TIM2;
	blinkTimer.TIMx_Config.TIMx_Prescaler = 16000;
	blinkTimer.TIMx_Config.TIMx_Period = 1000;
	blinkTimer.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	blinkTimer.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;

	timer_Config(&blinkTimer);
	timer_SetState(&blinkTimer, SET);

	/*Configuramos el timer para leer el adc*/

	GetDataTimer.pTIMx = TIM10;
	GetDataTimer.TIMx_Config.TIMx_Prescaler = 16000;
	GetDataTimer.TIMx_Config.TIMx_Period = 16;
	GetDataTimer.TIMx_Config.TIMx_mode = TIMER_UP_COUNTER;
	GetDataTimer.TIMx_Config.TIMx_InterruptEnable = TIMER_INT_ENABLE;

	timer_Config(&GetDataTimer);
	timer_SetState(&GetDataTimer, SET);


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

	PWMpinMotorx.pGPIOx = GPIOB;
	PWMpinMotorx.pinConfig.GPIO_PinNumber			= PIN_7;
	PWMpinMotorx.pinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	PWMpinMotorx.pinConfig.GPIO_PinOutputType 		= GPIO_OTYPE_PUSHPULL;
	PWMpinMotorx.pinConfig.GPIO_PinOutputSpeed 		= GPIO_OSPEEDR_FAST;
	PWMpinMotorx.pinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	PWMpinMotorx.pinConfig.GPIO_PinAltFunMode 		= AF2;

	PWMpinMotory.pGPIOx = GPIOA;
	PWMpinMotory.pinConfig.GPIO_PinNumber			= PIN_1;
	PWMpinMotory.pinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	PWMpinMotory.pinConfig.GPIO_PinOutputType 		= GPIO_OTYPE_PUSHPULL;
	PWMpinMotory.pinConfig.GPIO_PinOutputSpeed 		= GPIO_OSPEEDR_FAST;
	PWMpinMotory.pinConfig.GPIO_PinPuPdControl	 	= GPIO_PUPDR_NOTHING;
	PWMpinMotory.pinConfig.GPIO_PinAltFunMode 		= AF2;

	gpio_Config(&PWMpinLaser);
	gpio_Config(&PWMpinMotorx);
	gpio_Config(&PWMpinMotory);

	/*Configuración interrupción externa*/

	SWinterrupt.edgeType		= EXTERNAL_INTERRUPT_RISING_EDGE;
	SWinterrupt.pGPIOHandler	= &SWpin;

	exti_Config(&SWinterrupt);

	/*Configuración de los PWM*/

	PWMlaser.pTIMx			 	= TIM3;
	PWMlaser.config.timer		= TIMER_TIM3;
	PWMlaser.config.dutty		= duttyValue;
	PWMlaser.config.channel		= PWM_CHANNEL_1;
	PWMlaser.config.prescaler	= 16;
	PWMlaser.config.period		= PWMperiod;

	PWMmotorx.pTIMx			 	= TIM4;
	PWMmotorx.config.timer		= TIMER_TIM4;
	PWMmotorx.config.dutty		= duttyValue;
	PWMmotorx.config.channel	= PWM_CHANNEL_2;
	PWMmotorx.config.prescaler	= 16;
	PWMmotorx.config.period		= PWMperiod;

	PWMmotory.pTIMx			 	= TIM5;
	PWMmotory.config.timer		= TIMER_TIM5;
	PWMmotory.config.dutty		= duttyValue;
	PWMmotory.config.channel	= PWM_CHANNEL_2;
	PWMmotory.config.prescaler	= 16;
	PWMmotory.config.period		= PWMperiod;


	pwm_Config(&PWMlaser);
	pwm_Config(&PWMmotorx);
	pwm_Config(&PWMmotory);

////	startPWMsignal(&PWMlaser);
	startPWMsignal(&PWMmotorx);
	startPWMsignal(&PWMmotory);
//	gpio_WritePin(&enablePinMotorx,1);
//	gpio_WritePin(&enablePinMotory,1);


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
	commSerial.USART_Config.baudrate = USART_BAUDRATE_921600;
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
	motorx.config.laser_power		= LASER_POWER_4000Hz;
	motorx.config.time_step			= 5;					//valor en ms
	motorx.config.velocity			= LASER_VELOCITY_200Hz;

	motory.pGIPO_enable_laser 		= &enablePinLaser;
	motory.pGIPO_enable_motor 		= &enablePinMotory;
	motory.pGIPO_motor_direction	= &directionPinMotory;
	motory.pPWM_laser 				= &PWMlaser;
	motory.pPWM_motor 				= &PWMmotory;
	motory.config.direction			= DIRECTION1;
	motory.config.laser_power		= LASER_POWER_1000Hz;
	motory.config.time_step			= 5;					//valor en ms
	motory.config.velocity			= LASER_VELOCITY_200Hz;


	laser_init_config(&motorx);
	laser_init_config(&motory);


}

int8_t char2int(char number2convert){
	uint8_t numberconverted;
	if(number2convert=='0'){
		numberconverted=0;
	} else if(number2convert=='1'){
		numberconverted=1;
	} else if(number2convert=='2'){
		numberconverted=2;
	} else if(number2convert=='3'){
		numberconverted=3;
	} else if(number2convert=='4'){
		numberconverted=4;
	} else if(number2convert=='5'){
		numberconverted=5;
	} else if(number2convert=='6'){
		numberconverted=6;
	} else if(number2convert=='7'){
		numberconverted=7;
	}

	return numberconverted;
}

void EngravingImage(void){

	usart_writeMsg(&commSerial, "s");
	while(counterRowAux<imageHeight){
		while(!flagRowComplete){
				__NOP();
			}
			flagRowComplete=0;
			//In this point, the data transmition by python to row j is complete
			counterRow=0;
			if(counterRow%2==0){
				StartContinuosMovement(&motorx,DIRECTION1);
				for(int j=0;j<imageWidth;j++){
					start_continuous_engraving(&motorx);
					set_power(&motory,rowData[j]);
					systick_Delay_ms(motorx.config.time_step);
					stop_continuous_engraving(&motorx);
				}
				StopSimulataneousMovement(&motorx,&motory);
				set_motor_direction(&motory,DIRECTION1);
				movement(&motory);
				counterRow++;
			}else if(counterRow%2==1){
				StartContinuosMovement(&motorx,DIRECTION2);
				for(int j=0;j<imageWidth;j++){
					set_power(&motory,rowData[j]);
					start_continuous_engraving(&motorx);
					systick_Delay_ms(motorx.config.time_step);
					stop_continuous_engraving(&motorx);
				}
				StopSimulataneousMovement(&motorx,&motory);
				set_motor_direction(&motory,DIRECTION1);
				movement(&motory);
				counterRow++;
			}
			counterRowAux++;
	}

}


void Timer2_Callback(void) {
	gpio_TooglePin(&userLed);

}

void usart2_RxCallback(void) {
	receivedChar = usart_getRxData2();
	rowData[counterColum]=char2int(receivedChar);
//	receivedChar=0;
	usart_writeMsg(&commSerial, "c");
	counterColum++;
	if (counterColum==(imageWidth-1)){
		counterColum=0;
		flagRowComplete=1;
	}
}

void callback_extInt10(void){
	flagStart=1;
}



void assert_failed(uint8_t*file,uint32_t line){
	while(1){
		//problems
	}
}

