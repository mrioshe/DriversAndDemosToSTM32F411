/*
 * systick_driver_hal.c
 *
 *  Created on: Nov 8, 2023
 *      Author: mauricio
 */

#include <stm32f4xx.h>
#include "systick_driver_hal.h"

uint64_t ticks = 0;
uint64_t ticks_start =0;
uint64_t ticks_counting=0;

void config_systick_ms(Systic_Handler_t *Systick_Handler_t){
	//reiniciamos el valor de la variable que cuenta tiempo
	ticks=0;

	switch(Systick_Handler_t->Systick_Config_t.systemClock){

	case HSI_TIMER_16MHz:
		Systick_Handler_t->Systick->LOAD=SYSTIC_LOAD_VALUE_16MHz_1ms;
		break;
	case HSI_TIMER_100MHz:
		Systick_Handler_t->Systick->LOAD=SYSTIC_LOAD_VALUE_100MHz_1ms;

	default:
		Systick_Handler_t->Systick->LOAD=SYSTIC_LOAD_VALUE_16MHz_1ms;
		break;
	}

	// Limpiamos el valor actual del systic
	Systick_Handler_t->Systick->VAL=0;

	//Configuramos el reloj interno como el reloj para el timer

	Systick_Handler_t->Systick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;

	//Desactivamos las interrupcion del NVIC

	__disable_irq();

	//Matriculamos la interrupcion en el NVIC

	NVIC_EnableIRQ(SysTick_IRQn);

	// Activamos la interrupcion debido al conteo a cero del systick

	Systick_Handler_t->Systick->CTRL |= SysTick_CTRL_TICKINT_Msk;

	//Activamos el timer

	Systick_Handler_t->Systick->CTRL |= SysTick_CTRL_ENABLE_Msk;

	// Activamos las interrupciones externas

	__enable_irq();

}

uint64_t getTicks_ms(void){
	return ticks;
}

void delay_ms(uint32_t wait_time_ms){
	//captura el primer valor de tiempo para comparar
	ticks_start=getTicks_ms();

	//captura el segundo valor de tiempo para comparar
	ticks_counting=getTicks_ms();

	/*Compara: si el valor "counting" es menor que el "start+wait"
	 * actualiza el valor "counting"
	 * Repite esta operacion hasta que counting sea mayor (se cumple el tiempo de espera)*/

	while(ticks_counting<(ticks_start+(uint64_t)wait_time_ms)){

		//Actulizar  el valor
		ticks_counting=getTicks_ms();
	}

}

void systick_handler(Systic_Handler_t *Systick_Handler_t){
	//verificamos que la interrupcion se lanzo
	if(Systick_Handler_t->Systick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){

		//Limpiamos la bandera
		Systick_Handler_t->Systick->CTRL &= ~SysTick_CTRL_COUNTFLAG_Msk;

		//Incrementos en 1 el contador
		ticks++;

	}
}
