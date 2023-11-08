/*
 * pwm_driver_hal.c
 *
 *  Created on: Nov 6, 2023
 *      Author: mauricio
 */

#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32_assert.h"
#include "gpio_driver_hal.h"
#include "pwm_driver_hal.h"

void pwm_Config(PWM_Handler_t *pPWMHandler){

	/* 1. Activar la señal de reloj del periférico requerido */

	switch(pPWMHandler->config.timer){

	case TIMER_TIM1:
		RCC -> APB1ENR |= RCC_APB2ENR_TIM1EN;
		break;
	case TIMER_TIM2:
		RCC -> APB1ENR |= RCC_APB1ENR_TIM2EN;
		break;
	case TIMER_TIM3:
		RCC -> APB1ENR |= RCC_APB1ENR_TIM3EN;
		break;
	case TIMER_TIM4:
		RCC -> APB1ENR |= RCC_APB1ENR_TIM4EN;
		break;
	case TIMER_TIM5:
		RCC -> APB1ENR |= RCC_APB1ENR_TIM5EN;
		break;
	case TIMER_TIM9:
		RCC -> APB2ENR |= RCC_APB2ENR_TIM9EN;
		break;
	case TIMER_TIM10:
		RCC -> APB2ENR |= RCC_APB2ENR_TIM10EN;
		break;
	case TIMER_TIM11:
		RCC -> APB2ENR |= RCC_APB2ENR_TIM11EN;
		break;
	default:
		__NOP();
		break;

	}

	/* 1. Cargamos la frecuencia deseada */
	setFrequency(pPWMHandler);

	/* 2. Cargamos el valor del dutty-Cycle*/
	setDuttyCycle(pPWMHandler);

	/* 2a. Estamos en UP_Mode, el limite se carga en ARR y se comienza en 0 */
	/* agregue acá su código */

	pPWMHandler->pTIMx->CR1 &= ~(TIM_CR1_DIR);
	pPWMHandler->pTIMx->ARR = pPWMHandler->config.period;

	/* 3. Configuramos los bits CCxS del registro TIMy_CCMR1, de forma que sea modo salida
	 * (para cada canal hay un conjunto CCxS)
	 *
	 * 4. Además, en el mismo "case" podemos configurar el modo del PWM, su polaridad...
	 *
	 * 5. Y además activamos el preload bit, para que cada vez que exista un update-event
	 * el valor cargado en el CCRx será recargado en el registro "shadow" del PWM */
	switch(pPWMHandler->config.channel){




	case PWM_CHANNEL_1:{
		pPWMHandler->pTIMx->CCMR1=0;
		// Seleccionamos como salida el canal
		/* agregue acá su código */

		pPWMHandler->pTIMx->CCMR1 &= ~TIM_CCMR1_CC1S_0;
		pPWMHandler->pTIMx->CCMR1 &= ~TIM_CCMR1_CC1S_1;
		pPWMHandler->pTIMx->CCMR1 |= TIM_CCMR1_OC1FE;


		// Configuramos el canal como PWM
		/* agregue acá su código */

		pPWMHandler->pTIMx->CCMR1 &= ~TIM_CCMR1_OC1M_0;
		pPWMHandler->pTIMx->CCMR1 |= TIM_CCMR1_OC1M_1;
		pPWMHandler->pTIMx->CCMR1 |= TIM_CCMR1_OC1M_2;

		// Activamos la funcionalidad de pre-load
		/* agregue acá su código */

		pPWMHandler->pTIMx->CCMR1 |= TIM_CCMR1_OC1PE;

		break;
	}

	case PWM_CHANNEL_2:{
		pPWMHandler->pTIMx->CCMR1=0;
		// Seleccionamos como salida el canal
		/* agregue acá su código */

		pPWMHandler->pTIMx->CCMR1 &= ~TIM_CCMR1_CC2S_0;
		pPWMHandler->pTIMx->CCMR1 &= ~TIM_CCMR1_CC2S_1;
		pPWMHandler->pTIMx->CCMR1 |= TIM_CCMR1_OC2FE;


		// Configuramos el canal como PWM
		/* agregue acá su código */

		pPWMHandler->pTIMx->CCMR1 &= ~TIM_CCMR1_OC2M_0;
		pPWMHandler->pTIMx->CCMR1 |= TIM_CCMR1_OC2M_1;
		pPWMHandler->pTIMx->CCMR1 |= TIM_CCMR1_OC2M_2;

		// Activamos la funcionalidad de pre-load
		/* agregue acá su código */

		pPWMHandler->pTIMx->CCMR1 |= TIM_CCMR1_OC2PE;
		break;
	}

	case PWM_CHANNEL_3:{
		pPWMHandler->pTIMx->CCMR2=0;
		// Seleccionamos como salida el canal
		/* agregue acá su código */

		pPWMHandler->pTIMx->CCMR2 &= ~TIM_CCMR2_CC3S_0;
		pPWMHandler->pTIMx->CCMR2 &= ~TIM_CCMR2_CC3S_1;
		pPWMHandler->pTIMx->CCMR2 |= TIM_CCMR2_OC3FE;


		// Configuramos el canal como PWM
		/* agregue acá su código */

		pPWMHandler->pTIMx->CCMR2 &= ~TIM_CCMR2_OC3M_0;
		pPWMHandler->pTIMx->CCMR2 |= TIM_CCMR2_OC3M_1;
		pPWMHandler->pTIMx->CCMR2 |= TIM_CCMR2_OC3M_2;

		// Activamos la funcionalidad de pre-load
		/* agregue acá su código */

		pPWMHandler->pTIMx->CCMR1 |= TIM_CCMR2_OC3PE;
		break;
	}

	case PWM_CHANNEL_4:{
		pPWMHandler->pTIMx->CCMR2=0;
			// Seleccionamos como salida el canal
			/* agregue acá su código */

			pPWMHandler->pTIMx->CCMR2 &= ~TIM_CCMR2_CC4S_0;
			pPWMHandler->pTIMx->CCMR2 &= ~TIM_CCMR2_CC4S_1;
			pPWMHandler->pTIMx->CCMR2 |= TIM_CCMR2_OC4FE;


			// Configuramos el canal como PWM
			/* agregue acá su código */

			pPWMHandler->pTIMx->CCMR2 &= ~TIM_CCMR2_OC4M_0;
			pPWMHandler->pTIMx->CCMR2 |= TIM_CCMR2_OC4M_1;
			pPWMHandler->pTIMx->CCMR2 |= TIM_CCMR2_OC4M_2;

			// Activamos la funcionalidad de pre-load
			/* agregue acá su código */

			pPWMHandler->pTIMx->CCMR1 |= TIM_CCMR2_OC4PE;
			break;
	}
	default:{
		__NOP();
		break;
	}

	/* 6. Activamos la salida seleccionada */
	enableOutput(pPWMHandler);

	}
}

/* Función para activar el Timer y activar todo el módulo PWM */
void startPwmSignal(PWM_Handler_t *pPWMHandler) {
	/* agregue acá su código */

	pPWMHandler->pTIMx->CR1 |= TIM_CR1_CEN;

}

/* Función para desactivar el Timer y detener todo el módulo PWM*/
void stopPwmSignal(PWM_Handler_t *pPWMHandler) {
	pPWMHandler->pTIMx->CR1 &= ~TIM_CR1_CEN;
}

/* Función encargada de activar cada uno de los canales con los que cuenta el TimerX */
void enableOutput(PWM_Handler_t *pPWMHandler) {
	switch (pPWMHandler->config.channel) {
	case PWM_CHANNEL_1: {
		// Activamos la salida del canal 1
		/* agregue acá su código */
		pPWMHandler->pTIMx->CCER |= TIM_CCER_CC1E;
		break;
	}

	/* agregue acá su código para los otros tres casos */

	case PWM_CHANNEL_2: {
		// Activamos la salida del canal 1
		/* agregue acá su código */
		pPWMHandler->pTIMx->CCER |= TIM_CCER_CC2E;
		break;
	}

	case PWM_CHANNEL_3: {
		// Activamos la salida del canal 1
		/* agregue acá su código */
		pPWMHandler->pTIMx->CCER |= TIM_CCER_CC3E;
		break;
	}
	case PWM_CHANNEL_4: {
		// Activamos la salida del canal 1
		/* agregue acá su código */
		pPWMHandler->pTIMx->CCER |= TIM_CCER_CC4E;
		break;
	}

	default: {
		__NOP();
		break;
	}
	}
}

/*
 * La frecuencia es definida por el conjunto formado por el preescaler (PSC)
 * y el valor límite al que llega el Timer (ARR), con estos dos se establece
 * la frecuencia.
 * */
void setFrequency(PWM_Handler_t *pPWMHandler){

	// Cargamos el valor del prescaler, nos define la velocidad (en ns) a la cual
	// se incrementa el Timer
	/* agregue acá su código */

	pPWMHandler->pTIMx->PSC=pPWMHandler->config.prescaler;

	// Cargamos el valor del ARR, el cual es el límite de incrementos del Timer
	// antes de hacer un update y reload.
	/* agregue acá su código */
	pPWMHandler->pTIMx->ARR=pPWMHandler->config.period;
}


/* Función para actualizar la frecuencia, funciona de la mano con setFrequency */
void updateFrequency(PWM_Handler_t *pPWMHandler, uint16_t newFreq){
	// Actualizamos el registro que manipula el periodo
    /* agregue acá su código */

	pPWMHandler->config.period=newFreq;
	// Llamamos a la fucnión que cambia la frecuencia
	/* agregue acá su código */
	setFrequency(pPWMHandler);
}

/* El valor del dutty debe estar dado en valores de %, entre 0% y 100%*/
void setDuttyCycle(PWM_Handler_t *pPWMHandler){

	// Seleccionamos el canal para configurar su dutty
	switch(pPWMHandler->config.channel){
	case PWM_CHANNEL_1:{
		pPWMHandler->pTIMx->CCR1 = pPWMHandler->config.dutty;

		break;
	}

	case PWM_CHANNEL_2:{
		pPWMHandler->pTIMx->CCR2 = pPWMHandler->config.dutty;

		break;
	}

	case PWM_CHANNEL_3:{
		pPWMHandler->pTIMx->CCR3 = pPWMHandler->config.dutty;

		break;
	}

	case PWM_CHANNEL_4:{
		pPWMHandler->pTIMx->CCR4 = pPWMHandler->config.dutty;

		break;
	}

	default:{
		__NOP();
		break;
	}

	}// fin del switch-case

}


/* Función para actualizar el Dutty, funciona de la mano con setDuttyCycle */
void updateDuttyCycle(PWM_Handler_t *pPWMHandler, uint16_t newDutty){
	// Actualizamos el registro que manipula el dutty
    /* agregue acá su código */
	pPWMHandler->config.dutty=newDutty;

	// Llamamos a la fucnión que cambia el dutty y cargamos el nuevo valor
	/* agregue acá su código */
	setDuttyCycle(pPWMHandler);

}
