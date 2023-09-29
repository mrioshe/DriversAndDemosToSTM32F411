/*
 * exti_driver_hal.c
 *
 *  Created on: Sep 28, 2023
 *      Author: mrioshe
 */

#include "exti_driver_hal.h"
#include "gpio_driver_hal.h"

/* === Headers for private functions === */
static void exti_enable_clock_peripheral(void);
static void exti_assign_channel(EXTI_Config_t *extiConfig);
static void exti_select_edge(EXTI_Config_t *extiConfig);
static void exti_config_interrupt(EXTI_Config_t *extiConfig);

/*
 * Funcion de configuracion del sistema EXTI.
 * Requiere que un pinX ya se encuentre configurado como
 * entrada digital
 * */
void exti_Config(EXTI_Config_t *extiConfig){

	/* 1.0 Se carga la configuración, que debe ser el PINx como entrada "simple" */
	gpio_Config(extiConfig->pGPIOHandler);

	/* 2.0 Activamos el acceso al SYSCFG */
	exti_enable_clock_peripheral();

	/* 3.0 Seleccion de canal */
	exti_assign_channel(extiConfig);

	/* 4.0 Seleccionamos el tipo de flanco */
	exti_select_edge(extiConfig);

	/* 5.0 Desactivo primero las interrupciones globales */
	/*Agregue su código acá*/
	__disable_irq();

	/* 6. 0 Manejo de Interrupciones */
	exti_config_interrupt(extiConfig);

	/* 7.0 Volvemos a activar las interrupciones globales */
	/*Agregue su código acá*/
	__enable_irq();
}

/*
 * No requiere el periferico, ya que solo es necesario activar
 * al SYCFG
 * */
static void exti_enable_clock_peripheral(void){
	/* 2.0 Activamos el acceso al SYSCFG */
	/*Agregue su código acá*/

	RCC -> APB2ENR |= RCC_APB2ENR_SYSCFGEN ;
}

/*
 * Funcion que configura los MUX para asignar el pinX del puerto Y
 * a la entrada EXTI correspondiente.
 * */
static void exti_assign_channel(EXTI_Config_t *extiConfig){
	/*Asignamos el canal EXTI que corresponde al PIN_y del puerto GPIO_X
		 * Debemos activar la línea PIN_Xy (Y = A, B, C... y x = 0, 1, 2, 3...)
		 * en el módulo EXTI */
		switch (extiConfig->pGPIOHandler->pinConfig.GPIO_PinNumber) {

		/* Configurando para el todos los pines GPIOX_0*/
				/*EXTICR1_EXTI0*/
		case 0: {
			/* SYSCFG_EXTICR1 */
			// Limpiamos primero la posición que deseamos configurar
			SYSCFG->EXTICR[0] &= ~(0xF << SYSCFG_EXTICR1_EXTI0_Pos);

			// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
			// del puerto que vamos a utilizar: GPIOA_0, ó GPIOB_0, ó GPIOC_0, etc
			if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PA);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PB);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PC);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PD);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PE);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PH);

			} else {
				__NOP();
			}

			break;
		}

		/* Configurando para el todos los pines GPIOX_1*/
		/*EXTICR1_EXTI1*/
	case 1: {

			// Limpiamos primero la posición que deseamos configurar
			SYSCFG->EXTICR[0] &= ~(0xF << SYSCFG_EXTICR1_EXTI1_Pos);

			// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
			// del puerto que vamos a utilizar: GPIOA_1, ó GPIOB_1, ó GPIOC_1, etc
			if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI1_PA);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI1_PB);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI1_PC);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI1_PD);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI1_PE);

			} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
				SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI1_PH);

			} else {
				__NOP();
			}

			break;



	}

	/* Configurando para el todos los pines GPIOX_2*/
	/*EXTICR1_EXTI2*/
	case 2: {

		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[0] &= ~(0xF << SYSCFG_EXTICR1_EXTI2_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_2, ó GPIOB_2, ó GPIOC_2, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI2_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI2_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI2_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI2_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI2_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI2_PH);

		} else {
			__NOP();
		}

		break;


	}

	/* Configurando para el todos los pines GPIOX_3*/
	/*EXTICR1_EXTI3*/

	case 3: {

		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[0] &= ~(0xF << SYSCFG_EXTICR1_EXTI3_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_3, ó GPIOB_3, ó GPIOC_3, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI3_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI3_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI3_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI3_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI3_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[0] |= (SYSCFG_EXTICR1_EXTI3_PH);

		} else {
			__NOP();
		}

		break;


	}

	/* SYSCFG_EXTICR2 */
	/* Configurando para el todos los pines GPIOX_4*/
	/*EXTICR2_EXTI4*/

	case 4: {

		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[1] &= ~(0xF << SYSCFG_EXTICR2_EXTI4_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_4, ó GPIOB_4, ó GPIOC_4, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI4_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI4_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI4_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI4_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI4_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI4_PH);

		} else {
			__NOP();
		}

		break;


	}

	/* Configurando para el todos los pines GPIOX_5*/
	/*EXTICR2_EXTI5*/

	case 5: {

		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[1] &= ~(0xF << SYSCFG_EXTICR2_EXTI5_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_5, ó GPIOB_5, ó GPIOC_5, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI5_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI5_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI5_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI5_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI5_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI5_PH);

		} else {
			__NOP();
		}

		break;


	}

	/* Configurando para el todos los pines GPIOX_6*/
	/*EXTICR2_EXTI6*/

	case 6: {

		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[1] &= ~(0xF << SYSCFG_EXTICR2_EXTI6_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_6, ó GPIOB_6, ó GPIOC_6, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI6_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI6_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI6_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI6_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI6_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI6_PH);

		} else {
			__NOP();
		}

		break;


	}


	/* Configurando para el todos los pines GPIOX_7*/
	/*EXTICR2_EXTI7*/

	case 7: {

		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[1] &= ~(0xF << SYSCFG_EXTICR2_EXTI7_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_7, ó GPIOB_7, ó GPIOC_7, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI7_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI7_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI7_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI7_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI7_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[1] |= (SYSCFG_EXTICR2_EXTI7_PH);

		} else {
			__NOP();
		}

		break;


	}

	/* SYSCFG_EXTICR3 */
	/* Configurando para todos los pines GPIOX_8*/
	/*EXTICR3_EXTI8*/

	case 8: {

		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[2] &= ~(0xF << SYSCFG_EXTICR3_EXTI8_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_8, ó GPIOB_8, ó GPIOC_8, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI8_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI8_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI8_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI8_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI8_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI8_PH);

		} else {
			__NOP();
		}

		break;

	/* Configurando para el todos los pines GPIOX_9*/
	/*EXTICR3_EXTI9*/

	}

	case 9: {

		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[2] &= ~(0xF << SYSCFG_EXTICR3_EXTI9_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_9, ó GPIOB_9, ó GPIOC_9, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI9_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI9_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI9_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI9_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI9_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI9_PH);

		} else {
			__NOP();
		}

		break;


	}

	/* Configurando para el todos los pines GPIOX_10*/
	/*EXTICR3_EXTI10*/

	case 10: {

		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[2] &= ~(0xF << SYSCFG_EXTICR3_EXTI10_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_10, ó GPIOB_10, ó GPIOC_10, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI9_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI10_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI10_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI10_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI10_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI10_PH);

		} else {
			__NOP();
		}

		break;


	}

	/* Configurando para el todos los pines GPIOX_11*/
	/*EXTICR3_EXTI11*/

	case 11: {

		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[2] &= ~(0xF << SYSCFG_EXTICR3_EXTI11_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_11, ó GPIOB_11, ó GPIOC_11, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[2] |= (SYSCFG_EXTICR3_EXTI11_PH);

		} else {
			__NOP();
		}

		break;


	}

	/* SYSCFG_EXTICR4 */
	/* Configurando para todos los pines GPIOX_12*/
	/*EXTICR4_EXTI12*/

	case 12: {

		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[3] &= ~(0xF << SYSCFG_EXTICR4_EXTI12_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_12, ó GPIOB_12, ó GPIOC_12, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI12_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI12_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI12_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI12_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI12_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI12_PH);

		} else {
			__NOP();
		}

		break;


	}


	/* Configurando para todos los pines GPIOX_13*/
	/*EXTICR4_EXTI13*/

	case 13: {

		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[3] &= ~(0xF << SYSCFG_EXTICR4_EXTI13_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_13, ó GPIOB_13, ó GPIOC_13, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI13_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI13_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI13_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI13_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI13_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI13_PH);

		} else {
			__NOP();
		}

		break;


	}

	/* Configurando para todos los pines GPIOX_14*/
	/*EXTICR4_EXTI14*/

	case 14: {

		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[3] &= ~(0xF << SYSCFG_EXTICR4_EXTI14_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_14, ó GPIOB_14, ó GPIOC_14, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI14_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI14_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI14_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI14_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI14_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI14_PH);

		} else {
			__NOP();
		}

		break;


	}


	/* Configurando para el todos los pines GPIOX_15 */
	/*EXTICR4_EXTI15*/

	case 15: {

		/* SYSCFG_EXTICR4 */
		// Limpiamos primero la posición que deseamos configurar
		SYSCFG->EXTICR[3] &= ~(0xF << SYSCFG_EXTICR4_EXTI15_Pos);

		// Ahora seleccionamos el valor a cargar en la posición, segun sea la selección
		// del puerto que vamos a utilizar: GPIOA_0, ó GPIOB_0, ó GPIOC_0, etc
		if (extiConfig->pGPIOHandler->pGPIOx == GPIOA) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI15_PA);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOB) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI15_PB);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOC) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI15_PC);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOD) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI15_PD);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOE) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI15_PE);

		} else if (extiConfig->pGPIOHandler->pGPIOx == GPIOH) {
			SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI15_PH);

		} else {
			__NOP();
		}
		break;
	}

	default: {
		__NOP();
		break;
	}

	}// Fin del switch-case
}


/*
 * Funcion para seleccionar adecuadamente el flanco que lanza la interrupcion
 * en el canal EXTI especifico.
 * */
static void exti_select_edge(EXTI_Config_t *extiConfig){

	if(extiConfig->edgeType == EXTERNAL_INTERRUPT_FALLING_EDGE){

		/* Falling Trigger selection register*/

		switch (extiConfig->pGPIOHandler->pinConfig.GPIO_PinNumber) {
			case 0:{

				EXTI->FTSR |= EXTI_FTSR_TR0;
				break;

			}

			case 1:{

				EXTI->FTSR |= EXTI_FTSR_TR1;
				break;

					}

			case 2:{

				EXTI->FTSR |= EXTI_FTSR_TR2;
				break;

					}

			case 3:{

				EXTI->FTSR |= EXTI_FTSR_TR3;
				break;

					}

			case 4:{

				EXTI->FTSR |= EXTI_FTSR_TR4;
				break;

					}

			case 5:{

				EXTI->FTSR |= EXTI_FTSR_TR5;
				break;

					}

			case 6:{

				EXTI->FTSR |= EXTI_FTSR_TR6;
				break;

					}

			case 7:{

				EXTI->FTSR |= EXTI_FTSR_TR7;
				break;

					}

			case 8:{

				EXTI->FTSR |= EXTI_FTSR_TR8;
				break;

					}

			case 9:{

				EXTI->FTSR |= EXTI_FTSR_TR9;
				break;

					}

			case 10:{

				EXTI->FTSR |= EXTI_FTSR_TR10;
				break;

					}

			case 11:{

				EXTI->FTSR |= EXTI_FTSR_TR11;
				break;

					}

			case 12:{

				EXTI->FTSR |= EXTI_FTSR_TR12;
				break;

					}

			case 13:{

				EXTI->FTSR |= EXTI_FTSR_TR13;
				break;

					}

			case 14:{

				EXTI->FTSR |= EXTI_FTSR_TR14;
				break;

					}

			case 15:{

				EXTI->FTSR |= EXTI_FTSR_TR15;
				break;

					}

		}





	}
	/* Rising Trigger selection register*/
	else{

		switch (extiConfig->pGPIOHandler->pinConfig.GPIO_PinNumber) {
					case 0:{

						EXTI->RTSR |= EXTI_RTSR_TR0;
						break;

					}

					case 1:{

						EXTI->RTSR |= EXTI_RTSR_TR1;
						break;

							}

					case 2:{

						EXTI->RTSR |= EXTI_RTSR_TR2;
						break;

							}

					case 3:{

						EXTI->RTSR |= EXTI_RTSR_TR3;
						break;

							}

					case 4:{

						EXTI->RTSR |= EXTI_RTSR_TR4;
						break;

							}

					case 5:{

						EXTI->RTSR |= EXTI_RTSR_TR5;
						break;

							}

					case 6:{

						EXTI->RTSR |= EXTI_RTSR_TR6;
						break;

							}

					case 7:{

						EXTI->RTSR |= EXTI_RTSR_TR7;
						break;

							}

					case 8:{

						EXTI->RTSR |= EXTI_RTSR_TR8;
						break;

							}

					case 9:{

						EXTI->RTSR |= EXTI_RTSR_TR9;
						break;

							}

					case 10:{

						EXTI->RTSR |= EXTI_RTSR_TR10;
						break;

							}

					case 11:{

						EXTI->RTSR |= EXTI_RTSR_TR11;
						break;

							}

					case 12:{

						EXTI->RTSR |= EXTI_RTSR_TR12;
						break;

							}

					case 13:{

						EXTI->RTSR |= EXTI_RTSR_TR13;
						break;

							}

					case 14:{

						EXTI->RTSR |= EXTI_RTSR_TR14;
						break;

							}

					case 15:{

						EXTI->RTSR |= EXTI_RTSR_TR15;
						break;

							}
				}
	}
}

/*
 * Funcion que configura las mascaras de interrupciones (registro de mascaras) y
 * ademas matricula cada una de las posibles interrupciones en el NVIC
 * */
static void exti_config_interrupt(EXTI_Config_t *extiConfig){


	/* 6.0 Activamos la interrupción del canal que estamos configurando */
		// Interrupt Mask register


		switch(extiConfig->pGPIOHandler->pinConfig.GPIO_PinNumber){

		case 0: {
			EXTI -> IMR |= EXTI_IMR_MR0;
			break;

		}

		case 1: {
			EXTI -> IMR |= EXTI_IMR_MR1;
			break;

		}

		case 2: {
			EXTI -> IMR |= EXTI_IMR_MR2;
			break;

		}
		case 3: {
			EXTI -> IMR |= EXTI_IMR_MR3;
			break;

		}
		case 4: {
			EXTI -> IMR |= EXTI_IMR_MR4;
			break;

		}
		case 5: {
			EXTI -> IMR |= EXTI_IMR_MR5;
			break;

		}
		case 6: {
			EXTI -> IMR |= EXTI_IMR_MR6;
			break;

		}
		case 7: {
			EXTI -> IMR |= EXTI_IMR_MR7;
			break;

		}
		case 8: {
			EXTI -> IMR |= EXTI_IMR_MR8;
			break;

		}
		case 9: {
			EXTI -> IMR |= EXTI_IMR_MR9;
			break;

		}
		case 10: {
			EXTI -> IMR |= EXTI_IMR_MR10;
			break;

		}
		case 11: {
			EXTI -> IMR |= EXTI_IMR_MR11;
			break;

		}
		case 12: {
			EXTI -> IMR |= EXTI_IMR_MR12;
			break;

		}
		case 13: {
			EXTI -> IMR |= EXTI_IMR_MR13;
			break;

		}
		case 14: {
			EXTI -> IMR |= EXTI_IMR_MR14;
			break;

		}
		case 15: {
			EXTI -> IMR |= EXTI_IMR_MR15;
			break;

		}

	}

		/* 6.1 Matriculamos la interrupción en el NVIC para el canal correspondiente,
		 * donde el canal 0 corresponde al EXTI_0, canal 1 al EXTI_1, etc.
		 *
		 * NOTA: Observar que algunos canales EXTI comparten un mismo vector de interrupción
		 * */
		switch (extiConfig->pGPIOHandler->pinConfig.GPIO_PinNumber) {
		case 0: {
			__NVIC_EnableIRQ(EXTI0_IRQn);
			break;
		}

		case 1: {
			__NVIC_EnableIRQ(EXTI1_IRQn);

		break;
		}

		case 2: {
			__NVIC_EnableIRQ(EXTI2_IRQn);

		break;
		}
		case 3: {
			__NVIC_EnableIRQ(EXTI3_IRQn);

		break;
		}

		case 4: {
			__NVIC_EnableIRQ(EXTI4_IRQn);

		break;
		}

		case 5: {
			__NVIC_EnableIRQ(EXTI9_5_IRQn );

		break;
		}

		case 6: {
			__NVIC_EnableIRQ(EXTI9_5_IRQn );

		break;
		}

		case 7: {
			__NVIC_EnableIRQ(EXTI9_5_IRQn );

		break;
		}

		case 8: {
			__NVIC_EnableIRQ(EXTI9_5_IRQn );

		break;
		}

		case 9: {
			__NVIC_EnableIRQ(EXTI9_5_IRQn );

		break;
		}

		case 10: {
			__NVIC_EnableIRQ(EXTI15_10_IRQn);

		break;
		}

		case 11: {
			__NVIC_EnableIRQ(EXTI15_10_IRQn);

		break;
		}

		case 12: {
			__NVIC_EnableIRQ(EXTI15_10_IRQn);

		break;
		}

		case 13: {
			__NVIC_EnableIRQ(EXTI15_10_IRQn);

		break;
		}

		case 14: {
			__NVIC_EnableIRQ(EXTI15_10_IRQn);

		break;
		}

		case 15: {
			__NVIC_EnableIRQ(EXTI15_10_IRQn);

		break;
		}



	default: {
		break;
	}

	}
}

/**/
__attribute__ ((weak)) void callback_extInt0(void){
	__NOP();
}
__attribute__ ((weak)) void callback_extInt1(void){
	__NOP();
}
__attribute__ ((weak)) void callback_extInt2(void){
	__NOP();
}
__attribute__ ((weak)) void callback_extInt3(void){
	__NOP();
}
__attribute__ ((weak)) void callback_extInt4(void){
	__NOP();
}
__attribute__ ((weak)) void callback_extInt5(void){
	__NOP();
}
__attribute__ ((weak)) void callback_extInt6(void){
	__NOP();
}
__attribute__ ((weak)) void callback_extInt7(void){
	__NOP();
}
__attribute__ ((weak)) void callback_extInt8(void){
	__NOP();
}
__attribute__ ((weak)) void callback_extInt9(void){
	__NOP();
}
__attribute__ ((weak)) void callback_extInt10(void){
	__NOP();
}
__attribute__ ((weak)) void callback_extInt11(void){
	__NOP();
}
__attribute__ ((weak)) void callback_extInt12(void){
	__NOP();
}
__attribute__ ((weak)) void callback_extInt13(void){
	__NOP();
}
__attribute__ ((weak)) void callback_extInt14(void){
	__NOP();
}
__attribute__ ((weak)) void callback_extInt15(void){
	__NOP();
}



/* ISR de la interrupción canal 0*/
void EXTI0_IRQHandler(void){
	// Evaluamos si la interrupción que se lanzo corresponde al PIN_0 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR0){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR0;

		// llamamos al callback
		callback_extInt0();
	}
}

void EXTI1_IRQHandler(void){
	// Evaluamos si la interrupción que se lanzo corresponde al PIN_1 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR1){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR1;

		// llamamos al callback
		callback_extInt1();
	}
}

void EXTI2_IRQHandler(void){
	// Evaluamos si la interrupción que se lanzo corresponde al PIN_2 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR2){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR2;

		// llamamos al callback
		callback_extInt2();
	}
}

void EXTI3_IRQHandler(void){
	// Evaluamos si la interrupción que se lanzo corresponde al PIN_3 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR3){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR3;

		// llamamos al callback
		callback_extInt3();
	}
}

void EXTI4_IRQHandler(void){
	// Evaluamos si la interrupción que se lanzo corresponde al PIN_4 del GPIO_X
	if(EXTI->PR & EXTI_PR_PR4){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR4;

		// llamamos al callback
		callback_extInt4();
	}
}

/* Agregue las demas IRQs de las interrupciones EXTI independientes ...
 * Por favor recuerde que debe agregar el bloque if para verificar que
 * en efecto esa es la interrupcion que se está atendiendo.
 */


/* ISR de la interrupción canales 9_5
 * Observe que debe agregar totos los posibles casos, los cuales
 * son identificados por un bloque if() y el analisis de la bandera
 * (pending register -> EXTI_PR)
 */
void EXTI9_5_IRQHandler(void){
    /* Agregar todos los casos*/

	if(EXTI->PR & EXTI_PR_PR5){
			// Bajamos la bandera correspondiente
			EXTI->PR |= EXTI_PR_PR5;

			// llamamos al callback
			callback_extInt5();

		} else if(EXTI->PR & EXTI_PR_PR6){
			EXTI->PR |= EXTI_PR_PR6;
			callback_extInt6();
		} else if(EXTI->PR & EXTI_PR_PR7){
			EXTI->PR |= EXTI_PR_PR7;
			callback_extInt7();
		} else if(EXTI->PR & EXTI_PR_PR8){
			EXTI->PR |= EXTI_PR_PR8;
			callback_extInt8();
		} else if(EXTI->PR & EXTI_PR_PR9){
			EXTI->PR |= EXTI_PR_PR9;
			callback_extInt9();
		}

}



/* ISR de la interrupción canales 15_10
 * Observe que debe agregar totos los posibles casos, los cuales
 * son identificados por un bloque if() y el analisis de la bandera
 * (pending register -> EXTI_PR)
 */
void EXTI15_10_IRQHandler(void){
	// Evaluamos si la interrupción que se lanzo corresponde al PIN_Y_15
	if(EXTI->PR & EXTI_PR_PR10){
		// Bajamos la bandera correspondiente
		EXTI->PR |= EXTI_PR_PR10;

		// llamamos al callback
		callback_extInt10();

	} else if(EXTI->PR & EXTI_PR_PR11){
		EXTI->PR |= EXTI_PR_PR11;
		callback_extInt11();
	} else if(EXTI->PR & EXTI_PR_PR12){
		EXTI->PR |= EXTI_PR_PR12;
		callback_extInt12();
	} else if(EXTI->PR & EXTI_PR_PR13){
		EXTI->PR |= EXTI_PR_PR13;
		callback_extInt13();
	} else if(EXTI->PR & EXTI_PR_PR14){
		EXTI->PR |= EXTI_PR_PR14;
		callback_extInt14();
	} else if(EXTI->PR & EXTI_PR_PR15){
		EXTI->PR |= EXTI_PR_PR15;
		callback_extInt15();
	}


}

