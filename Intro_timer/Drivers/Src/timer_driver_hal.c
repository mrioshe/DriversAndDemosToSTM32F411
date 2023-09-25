/*
 * timer_driver_hal.c
 *
 *  Created on: Sep 25, 2023
 *      Author: mauricio
 */

#include "stm32f4xx.h"
#include "stm32_assert.h"
#include "timer_driver_hal.h"

/*Variable que guarda la referencia del periférico que se está utilizando*/
TIM_TypeDef *ptrTimerUsed;

/* === Headers for private functions === */

static void timer_enable_clock_peripheral(Timer_Handler_t *pTimerHandler);
static void timer_set_prescaler(Timer_Handler_t *pTimerHandler);
static void timer_set_period(Timer_Handler_t *pTimerHandler);
static void timer_set_mode(Timer_Handler_t *pTimerHandler);
static void timer_config_interrupt(Timer_Handler_t *pTimerHandler);

/*Función para cargar la configuración del Timer
 * Recordar que siempre se debe comenzar con activar la señal de reloj
 * del periférico que se está utilizando.
 * Además, en este caso, debemos ser cuidadosos al momento de utilizar las interrupciones.
 * Los Timer están conectados directamente al elemento NVIC del Cortex-Mx
 * Debemos configurar y/o utilizar:
 * TIMx_CR1(control Register 1)
 * TIMx_SMCR(slave mode control register) -> mantener en 0 para modo Timer Básico
 * TIMx_DIER (DMA and Interrupt enable register)
 * TIMx_SR (Status register)
 * TIMx_CNT (counter)
 * TIMxPSC (Pre-scaler)
 * TIMx_ARR (Auto-reload register)
 *
 * Como vamos a trabajar con interrupciones, antes de configurar una nueva, debemos desactivar el sistema global de
 * interrupciones, activar la IRQ específica y luego volver a encender el sistema
 * */

void timer_Config(Timer_Handler_t *pTimerHandler){
	//Guardamos una referencia al periferico que estamos utilizando...
	ptrTimerUsed = pTimerHandler -> pTIMx;

	/*0. Desactivamos las interrupciones globales mientras configuramos el sistema*/
	__disable_irq();

	/* 1. Activar la señal de reloj del periférico requerido */
	timer_enable_clock_peripheral(pTimerHandler);

	/* 2. Configuramos el Pre-scaler*/
	timer_set_prescaler(pTimerHandler);

	/* 3. COnfiguramos si UP_COUNT o COUNT (mode)*/
	timer_set_mode(pTimerHandler);

	/* 4. Configuramos el Auto-reload*/
	timer_set_period(pTimerHandler);

	/* 5. Configuramos la interrupción*/
	timer_config_interrupt(pTimerHandler);

	/* x. VOlvemos a activar las interrupciones del sistema*/
	__enable_irq();

	/* El timer inicia apagado */
	timer_SetState(pTimerHandler, TIMER_OFF);
}

void timer_enable_clock_peripheral(Timer_Handler_t *pTimerHandler){

	//Verificamos que es un timer permitido

	if(pTimerHandler -> pTIMx == TIM2){
		RCC -> APB1ENR |= RCC_APB1ENR_TIM2EN;
	}
	else if(pTimerHandler -> pTIMx == TIM3){
		RCC -> APB1ENR |= RCC_APB1ENR_TIM3EN;
	}
	else{
		__NOP();
	}
}

void timer_set_prescaler(Timer_Handler_t *pTimerHandler){

	//Verificamos que el valor del prescaler es valido
	assert_param(IS_TIMER_PRESC(pTimerHandler -> TIMx_Config.TIMx_Prescaler));

	// Configuramos el valor del prescaler.
	pTimerHandler -> pTIMx -> PSC = pTimerHandler -> TIMx_Config.TIMx_Prescaler -1; //(16000000))
}

/*
 * Esta función configura el límite hasta donde cuenta el Timer para generar un evento "update"
 * (Cuando esta contando de forma ascendente), o configrael valor desde donde se comienza a contar, cuando el sistema
 * funciona de forma descendente
 * */

void timer_set_period(Timer_Handler_t *pTimerHandler){

	// Verificamos que el valor que genera el periodo es valido
	assert_param(IS_TIMER_PERIOD(pTimerHandler -> TIMx_Config.TIMx_Prescaler));

	// Acá hace falta algo...

	// COnfiguramos el valor del autoreload
	pTimerHandler ->pTIMx -> ARR =pTimerHandler -> TIMx_Config.TIMx_Period -1;

}

/*
 * Upcounter of Downcounter
 * */

void timer_set_mode(Timer_Handler_t *pTimerHandler){

	//Verificamos que el modo de funcionamiento es correcto.
	assert_param(IS_TIMER_MODE(pTimerHandler -> TIMx_Config.TIMx_mode));

	//Verificamos cual es el modelo que se desea configurar.

	if(pTimerHandler->TIMx_Config.TIMx_mode ==TIMER_UP_COUNTER){
		// Configuramos en modo Upcounter DIR =0
		pTimerHandler ->pTIMx->CR1 &= ~TIM_CR1_DIR;
	}
	else{
		//Configramos en modo downcounter DIR =1
		pTimerHandler ->pTIMx->CR1 |= TIM_CR1_DIR;
	}

}

void timer_config_interrupt(Timer_Handler_t *pTimerHandler){

	//verificamos el posible valor configurado
	assert_param(IS_TIMER_INTERRUPT(pTimerHandler->TIMx_Config.TIMx_InterruptEnable));

	if(pTimerHandler -> TIMx_Config.TIMx_InterruptEnable == TIMER_INT_ENABLE){

		/*Activamos la interrupción debida al Timerx utilizado*/
		pTimerHandler ->pTIMx -> DIER |= TIM_DIER_UIE;

		/*Activdamos el cana del sistema NVIC para que le la interrupción*/
		if(pTimerHandler ->pTIMx == TIM2){
			NVIC_EnableIRQ(TIM2_IRQn);
		}
		else if(pTimerHandler->pTIMx==TIM3){
			NVIC_EnableIRQ(TIM3_IRQn);

		}
		else{
			__NOP();
		}
	}

}

void timer_SetState(Timer_Handler_t *pTimerHandler, uint8_t newState){

	//verificamos que el estado ingresado es adecuado
	assert_param(IS_TIMER_STATE(newState));

	/*Reiniciamos el registro counter*/
	pTimerHandler ->pTIMx -> CNT=0;

	if(newState == TIMER_ON){
		/* 5a. Activdamos el Timer (el CNT debe comenzar a contar)*/
		pTimerHandler -> pTIMx -> CR1 |= TIM_CR1_CEN;
	}

	else{
		/*5b. Desactivamos el timer (el CNT debe detenerse)*/
		pTimerHandler->pTIMx->CR1 &= ~TIM_CR1_CEN;
	}

}

__attribute__((weak)) void Timer2_Callback(void){
	__NOP();
}

/*
 * Esta es la fucnión a la que apunta el sistema en el vector de interrupciones.
 * Se debe utilizar usando exactamente el mismo nombre definido en el vecto de interrupciones
 * Al hacerlo correctamente, el sistema apunta a esta función y cuando la interrupción
 * el sistema inmediatamente salta a este lugar en la memoria
 *
 * */

void TIM2_IRQHandler(void){

	/*Limpiamos la bandera que indica que la intrrupción se ha generado*/
	TIM2 -> SR &= ~TIM_SR_UIF;
	/*Llamamos a la función que se debe encargar de hacer algo con esta interrupción*/
	Timer2_Callback();
}


