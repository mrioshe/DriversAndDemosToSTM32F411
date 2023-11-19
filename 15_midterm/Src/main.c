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

#include <string.h>



#include <stm32_assert.h>

#include "usart_driver_hal.h"
#include "gpio_driver_hal.h"
#include "timer_driver_hal.h"
#include "exti_driver_hal.h"
#include "adc_driver_hal.h"
#include "pwm_driver_hal.h"
#include "systick_driver_hal.h"

//Declaracion de variables:

GPIO_Handler_t stateled={0};

Timer_Handler_t blinkTimer={0};
Timer_Handler_t frec_7segment={0};
Timer_Handler_t frec_message={0};

PWM_Handler_t pwm2={0};
PWM_Handler_t pwm1={0};
PWM_Handler_t pwm={0};

EXTI_Config_t interrupt_sw={0};
EXTI_Config_t interrupt_clk={0};

USART_Handler_t commSerial= { 0 };

GPIO_Handler_t sw={0};
GPIO_Handler_t led_selector={0};
GPIO_Handler_t dt={0};
GPIO_Handler_t clk={0};
GPIO_Handler_t leds_7segment[8]={0};
GPIO_Handler_t vcc_units_7seg={0};
GPIO_Handler_t vcc_decs_7seg={0};
GPIO_Handler_t pinTx = {0};
GPIO_Handler_t pinRx = {0};
GPIO_Handler_t PWMpin= { 0 };
GPIO_Handler_t PWMpin1= { 0 };
GPIO_Handler_t PWMpin2= { 0 };

ADC_Config_t adc_signal[3]={0};

uint8_t receivedChar=0;
uint8_t sendMsg=0;
char sentido[128]={0};
char bufferData[128]={0};
uint8_t ccw_cw={0};
uint8_t flag_clk={0};
uint8_t flag_sendMsg={0};
uint8_t flag_selector={0};
uint8_t flag_switcheo={0};

uint16_t duttyValue=0;
uint16_t PWMperiod=20000;

uint8_t counter={0};
uint8_t digit_selector=0;
uint8_t number_desc_aux[2]={0};
uint8_t selector={0};
uint8_t resolution_value={0};


//funciones:

void initSys(void);
void segment_configuration_units(uint8_t number);
void change_num(void);
uint8_t get_resolution_value(ADC_Config_t *adcConfig);

int main() {
	// Iniciamos la unidad de punto flotante:
	SCB->CPACR |=(0xF<<20);
	// Iniciamos el sistema:
	initSys();
while (1) {

	// Cada vez que se tiene interrupcion externa debido al boton del encoder:
	if(flag_selector){
		flag_selector=0;
		change_num();

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
		if (gpio_ReadPin(&dt)){
			ccw_cw=1;
			if(counter==9){
				counter=0;
			} else {
				counter++;
			}

			segment_configuration_units(counter);

			}
	}





	if(flag_sendMsg){
		flag_sendMsg=0;
		if(ccw_cw){
			sprintf(bufferData,"ADC_RED:%d, ADC_GREEN: %d, ADC_BLUE:%d y el sentido de rotación es horario\n\r",adc_signal[0].adcData,adc_signal[1].adcData,adc_signal[2].adcData);
			usart_writeMsg(&commSerial,bufferData);
		} else {
			sprintf(bufferData,"ADC_RED:%d, ADC_GREEN: %d, ADC_BLUE:%d y el sentido de rotación es antiohorario\n\r",adc_signal[0].adcData,adc_signal[1].adcData,adc_signal[2].adcData);
			usart_writeMsg(&commSerial,bufferData);
		}
	}



 	if (duttyValue== 20000){
 		duttyValue=0;
 		//PWMperiod=2000;
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

	pinTx.pGPIOx = GPIOA;
	pinTx.pinConfig.GPIO_PinNumber = PIN_2;
	pinTx.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	pinTx.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	pinTx.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_FAST;
	pinTx.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	pinTx.pinConfig.GPIO_PinAltFunMode = AF7;

	//Pin de recepcion

	pinRx.pGPIOx = GPIOA;
	pinRx.pinConfig.GPIO_PinNumber = PIN_3;
	pinRx.pinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	pinRx.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
	pinRx.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_FAST;
	pinRx.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	pinRx.pinConfig.GPIO_PinAltFunMode = AF7;

	gpio_Config(&pinTx);
	gpio_Config(&pinRx);

	/*Pines para el PWM*/

	PWMpin.pGPIOx = GPIOC;
	PWMpin.pinConfig.GPIO_PinNumber			= PIN_7;
	PWMpin.pinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	PWMpin.pinConfig.GPIO_PinOutputType 	= GPIO_OTYPE_PUSHPULL;
	PWMpin.pinConfig.GPIO_PinOutputSpeed 	= GPIO_OSPEEDR_FAST;
	PWMpin.pinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	PWMpin.pinConfig.GPIO_PinAltFunMode 	= AF2;

	PWMpin1.pGPIOx = GPIOB;
	PWMpin1.pinConfig.GPIO_PinNumber		= PIN_0;
	PWMpin1.pinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	PWMpin1.pinConfig.GPIO_PinOutputType 	= GPIO_OTYPE_PUSHPULL;
	PWMpin1.pinConfig.GPIO_PinOutputSpeed 	= GPIO_OSPEEDR_FAST;
	PWMpin1.pinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	PWMpin1.pinConfig.GPIO_PinAltFunMode 	= AF2;

	PWMpin2.pGPIOx = GPIOB;
	PWMpin2.pinConfig.GPIO_PinNumber		= PIN_7;
	PWMpin2.pinConfig.GPIO_PinMode 			= GPIO_MODE_ALTFN;
	PWMpin2.pinConfig.GPIO_PinOutputType 	= GPIO_OTYPE_PUSHPULL;
	PWMpin2.pinConfig.GPIO_PinOutputSpeed 	= GPIO_OSPEEDR_FAST;
	PWMpin2.pinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	PWMpin2.pinConfig.GPIO_PinAltFunMode 	= AF2;

	gpio_Config(&PWMpin);
	gpio_Config(&PWMpin1);
	gpio_Config(&PWMpin2);

	/*CONFIGURACION TIMERS*/

	// Configuramos el timer para el blinky

    blinkTimer.pTIMx								=TIM9;
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

	frec_message.pTIMx 								= TIM10;
	frec_message.TIMx_Config.TIMx_Prescaler 		= 16000;
	frec_message.TIMx_Config.TIMx_Period 			= 2000;
	frec_message.TIMx_Config.TIMx_mode 				= TIMER_UP_COUNTER;
	frec_message.TIMx_Config.TIMx_InterruptEnable 	= TIMER_INT_ENABLE;

	timer_Config(&blinkTimer);
	timer_Config(&frec_7segment);
	timer_Config(&frec_message);

	/*CONFIGURACION EXITS*/

	//Interrucion del boton del enconder

	interrupt_sw.edgeType		= EXTERNAL_INTERRUPT_RISING_EDGE;
	interrupt_sw.pGPIOHandler	= &sw;

	//Interrucion del clock del enconder

	interrupt_clk.edgeType		= EXTERNAL_INTERRUPT_FALLING_EDGE;
	interrupt_clk.pGPIOHandler	= &clk;

	exti_Config(&interrupt_sw);
	exti_Config(&interrupt_clk);

	/*CONFIGURACION USART*/

	commSerial.ptrUSARTx 			 		= USART2;
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
	adc_signal[1].resolution			= RESOLUTION_12_BIT;
	adc_signal[1].dataAlignment 		= ALIGNMENT_RIGHT;
	adc_signal[1].samplingPeriod		= SAMPLING_PERIOD_84_CYCLES;
	adc_signal[1].interrupState			= ADC_INT_ENABLE;

	adc_signal[2].channel				= CHANNEL_15;
	adc_signal[2].resolution			= RESOLUTION_12_BIT;
	adc_signal[2].dataAlignment 		= ALIGNMENT_RIGHT;
	adc_signal[2].samplingPeriod		= SAMPLING_PERIOD_84_CYCLES;
	adc_signal[2].interrupState			= ADC_INT_ENABLE;

	adc_ConfigSingleChannel(&adc_signal[0]);

	/*Configuración del PWM*/

	pwm.pTIMx			 	= TIM3;
	pwm.config.timer		= TIMER_TIM3;
	pwm.config.dutty		= duttyValue;
	pwm.config.channel		= PWM_CHANNEL_2;
	pwm.config.prescaler	= 16;
	pwm.config.period		= PWMperiod;

	pwm1.pTIMx			 	= TIM3;
	pwm1.config.timer		= TIMER_TIM3;
	pwm1.config.dutty		= 20000-duttyValue;
	pwm1.config.channel		= PWM_CHANNEL_3;
	pwm1.config.prescaler	= 16;
	pwm1.config.period		= PWMperiod;


	pwm2.pTIMx			 	= TIM4;
	pwm2.config.timer		= TIMER_TIM4;
	pwm2.config.dutty		= duttyValue;
	pwm2.config.channel		= PWM_CHANNEL_2;
	pwm2.config.prescaler	= 16;
	pwm2.config.period		= PWMperiod;

	pwm_Config(&pwm2);
	pwm_Config(&pwm);
	pwm_Config(&pwm1);

	startPWMsignal(&pwm);
	startPWMsignal(&pwm1);
	startPWMsignal(&pwm2);

	/*SETEO DE VALORES INICIALES*/

	gpio_WritePin(&stateled,SET);
	gpio_WritePin(&led_selector,SET);
	gpio_WritePin(&vcc_units_7seg, RESET);
//	gpio_WritePin(&vcc_decs_7seg, RESET);

	timer_SetState(&blinkTimer, TIMER_ON);
	timer_SetState(&frec_7segment, TIMER_ON);
	timer_SetState(&frec_message, TIMER_ON);

	usart_WriteChar(&commSerial,0);

	counter=1;
	selector=1;
	segment_configuration_units(counter);

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



//Funcion para modificar el digito de las unidades del 7segmentos

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

void change_num(){

	while(gpio_ReadPin(&sw)){
		if(counter==9){
			counter=0;
			systick_Delay_ms(1000);
			segment_configuration_units(counter);
		} else {
			counter++;
			systick_Delay_ms(1000);
			segment_configuration_units(counter);
		}


	}

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
    duttyValue=duttyValue+100;
    //PWMperiod=PWMperiod+300;
    updateDuttyCycle(&pwm, duttyValue);
    updateDuttyCycle(&pwm1, 20000-duttyValue);
    updateDuttyCycle(&pwm2, 20000-duttyValue);
    //updateFrequency(&pwm, PWMperiod);
}

//interrupcion del timer que configura la frecuencia del mensaje
void Timer10_Callback(void) {


	//cuando se cumple el tiempo se inicia la conversion adc
	adc_ConfigSingleChannel(&adc_signal[0]);
	adc_StartSingleConv();

	adc_ConfigSingleChannel(&adc_signal[1]);
	adc_StartSingleConv();

	adc_ConfigSingleChannel(&adc_signal[2]);
	adc_StartSingleConv();


}

//interrupcion del timer para activar/desactivar el led blinky
void Timer9_Callback(void) {
	gpio_TooglePin(&stateled);

}



//interrupcion por recepcion en el USART1
void usart2_RxCallback(void) {
	receivedChar = usart_getRxData2();
}

//interrupcion por finalizacion de conversion ADC1
void adc_CompleteCallback(void) {
	flag_sendMsg=1;
	adc_signal[counter-1].adcData = adc_GetValue();
	adc_peripheralOnOFF(ADC_OFF);
	//luego de tener el valor de la conversion adc mandamos el mensaje:

}


void assert_failed(uint8_t*file,uint32_t line){
	while(1){
		//problems
	}
}
