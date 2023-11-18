/*
 * adc_driver_hal.c
 *
 *  Created on: Oct 19, 2023
 *      Author: mauricio
 */
#include <stdint.h>
#include "adc_driver_hal.h"
#include "stm32f4xx.h"
#include "stm32_assert.h"
#include "gpio_driver_hal.h"

/*Headers for private functions ===*/

static void adc_enable_clock_peripheral(void);
static void adc_set_resolution(ADC_Config_t *adcConfig);
static void adc_set_alignment(ADC_Config_t *adcConfig);
static void adc_set_sampling_and_hold(ADC_Config_t *adcConfig);
static void adc_set_one_channel_sequence(ADC_Config_t *adcConfig);
static void adc_config_interrupt(ADC_Config_t *adcConfig);

/*Variables y elementos que necesita internamente el driver para funcionar adecuadamente*/

GPIO_Handler_t 	handlerADCPin={0};
uint16_t		adcRawData=0;

void adc_ConfigSingleChannel(ADC_Config_t *adcConfig){

	/*1. Configuramos el pinX para que cumpla la funci{on del canal an{alogo deseado*/
	adc_ConfigAnalogPin(adcConfig->channel);

	/*2. Activamos la señnal de relog para el ADC*/
	adc_enable_clock_peripheral();

	// Limpiamos los registros antes de comenzar a configurar
	ADC1->CR1=0;
	ADC1->CR2=0;

	/*Comenzamos la configuración de ADC1*/

	/*3. Resolución del ADC*/
	adc_set_resolution(adcConfig);

	/*4. Configramos el modo Scan como desactivado*/
	adc_ScanMode(SCAN_OFF);

	/*5. Configuramos la alineación de los datos (derecha o izquierda)*/
	adc_set_alignment(adcConfig);

	/*6. Desactivamos el "Continuos mode "*/
	adc_StopContinuousConv();

	/*7. Aca se debería configurar el sampling*/
	adc_set_sampling_and_hold(adcConfig);

	/*8. COnfiguramos la secuencia y cuántos elementos hay en la secuencias*/
	adc_set_one_channel_sequence(adcConfig);

	/*9. Configuramos el prescaler del ADC en 2:1 (el más rápido que se puede tener)*/
	ADC->CCR &= ~ADC_CCR_ADCPRE;

	/*10. Desactivamos las interrupciones globales*/
	__disable_irq();

	/*11. Configuramos la interrupción (si se encuentra activa), además de incribir/remover la interrupción en el NVIC*/
	adc_config_interrupt(adcConfig);

	/*12. Activamos el moduloADC*/
	adc_peripheralOnOFF(ADC_ON);

	/*13. Activamos las interrupciones globales*/
	__enable_irq();
}

/*Enable Clock signal for ADC peripheral*/

static void adc_enable_clock_peripheral(void){
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

}

/*Configures the resolution for the conversion*/

static void adc_set_resolution(ADC_Config_t *adcConfig){

	switch(adcConfig->resolution){

	case RESOLUTION_12_BIT:
		ADC1->CR1 &= ~ADC_CR1_RES;
		break;

	case RESOLUTION_10_BIT:
		ADC1->CR1 |= ADC_CR1_RES_0;
		break;

	case RESOLUTION_8_BIT:
		ADC1->CR1 |= ADC_CR1_RES_1;
		break;

	case RESOLUTION_6_BIT:
		ADC1->CR1 |= ADC_CR1_RES;
		break;

	default:
		__NOP();


	}

}

/*Set the number alignment: left or right*/

static void adc_set_alignment(ADC_Config_t *adcConfig){

	switch(adcConfig->dataAlignment){

	case ALIGNMENT_RIGHT:
		ADC1->CR2 &= ~ADC_CR2_ALIGN;
		break;

	case ALIGNMENT_LEFT:
		ADC1->CR2 |= ADC_CR2_ALIGN;
		break;

	default:
		__NOP();


	}


}

/*Relacionado con el valor del tiempo de carga del capacitor HOLD*/

static void adc_set_sampling_and_hold(ADC_Config_t *adcConfig){

	switch(adcConfig->channel){

		case CHANNEL_0:

			ADC1->SMPR2 &= ~ADC_SMPR2_SMP0;
			switch(adcConfig->samplingPeriod){

			case SAMPLING_PERIOD_3_CYCLES:

				__NOP();
				break;

			case SAMPLING_PERIOD_15_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP0_0;
				break;

			case SAMPLING_PERIOD_28_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP0_1;

				break;

			case SAMPLING_PERIOD_56_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP0_0;
				ADC1->SMPR2 |= ADC_SMPR2_SMP0_1;

				break;

			case SAMPLING_PERIOD_84_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP0_2;
				break;

			case SAMPLING_PERIOD_112_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP0_0;
				ADC1->SMPR2 |= ADC_SMPR2_SMP0_2;
				break;

			case SAMPLING_PERIOD_144_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP0_1;
				ADC1->SMPR2 |= ADC_SMPR2_SMP0_2;
				break;

			case SAMPLING_PERIOD_480_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP0;
				break;

			default:
				__NOP();
				break;

			}

			break;

		case CHANNEL_1:

			ADC1->SMPR2 &= ~ADC_SMPR2_SMP1;
			switch(adcConfig->samplingPeriod){

			case SAMPLING_PERIOD_3_CYCLES:

				__NOP();
				break;

			case SAMPLING_PERIOD_15_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP1_0;
				break;

			case SAMPLING_PERIOD_28_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP1_1;

				break;

			case SAMPLING_PERIOD_56_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP1_0;
				ADC1->SMPR2 |= ADC_SMPR2_SMP1_1;

				break;

			case SAMPLING_PERIOD_84_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP1_2;
				break;

			case SAMPLING_PERIOD_112_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP1_0;
				ADC1->SMPR2 |= ADC_SMPR2_SMP1_2;
				break;

			case SAMPLING_PERIOD_144_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP1_1;
				ADC1->SMPR2 |= ADC_SMPR2_SMP1_2;
				break;

			case SAMPLING_PERIOD_480_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP1;
				break;

			default:
				__NOP();
				break;

			}

			break;

		case CHANNEL_2:

			ADC1->SMPR2 &= ~ADC_SMPR2_SMP2;
			switch(adcConfig->samplingPeriod){

			case SAMPLING_PERIOD_3_CYCLES:

				__NOP();
				break;

			case SAMPLING_PERIOD_15_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP2_0;
				break;

			case SAMPLING_PERIOD_28_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP2_1;

				break;

			case SAMPLING_PERIOD_56_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP2_0;
				ADC1->SMPR2 |= ADC_SMPR2_SMP2_1;

				break;

			case SAMPLING_PERIOD_84_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP2_2;
				break;

			case SAMPLING_PERIOD_112_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP2_0;
				ADC1->SMPR2 |= ADC_SMPR2_SMP2_2;
				break;

			case SAMPLING_PERIOD_144_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP2_1;
				ADC1->SMPR2 |= ADC_SMPR2_SMP2_2;
				break;

			case SAMPLING_PERIOD_480_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP2;
				break;

			default:
				__NOP();
				break;

			}

			break;

		case CHANNEL_3:

			ADC1->SMPR2 &= ~ADC_SMPR2_SMP3;
			switch(adcConfig->samplingPeriod){

			case SAMPLING_PERIOD_3_CYCLES:

				__NOP();
				break;

			case SAMPLING_PERIOD_15_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP3_0;
				break;

			case SAMPLING_PERIOD_28_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP3_1;

				break;

			case SAMPLING_PERIOD_56_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP3_0;
				ADC1->SMPR2 |= ADC_SMPR2_SMP3_1;

				break;

			case SAMPLING_PERIOD_84_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP3_2;
				break;

			case SAMPLING_PERIOD_112_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP3_0;
				ADC1->SMPR2 |= ADC_SMPR2_SMP3_2;
				break;

			case SAMPLING_PERIOD_144_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP3_1;
				ADC1->SMPR2 |= ADC_SMPR2_SMP3_2;
				break;

			case SAMPLING_PERIOD_480_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP3;
				break;

			default:
				__NOP();
				break;

			}

			break;

		case CHANNEL_4:

			ADC1->SMPR2 &= ~ADC_SMPR2_SMP4;
			switch(adcConfig->samplingPeriod){

			case SAMPLING_PERIOD_3_CYCLES:

				__NOP();
				break;

			case SAMPLING_PERIOD_15_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP4_0;
				break;

			case SAMPLING_PERIOD_28_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP4_1;

				break;

			case SAMPLING_PERIOD_56_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP4_0;
				ADC1->SMPR2 |= ADC_SMPR2_SMP4_1;

				break;

			case SAMPLING_PERIOD_84_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP4_2;
				break;

			case SAMPLING_PERIOD_112_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP4_0;
				ADC1->SMPR2 |= ADC_SMPR2_SMP4_2;
				break;

			case SAMPLING_PERIOD_144_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP4_1;
				ADC1->SMPR2 |= ADC_SMPR2_SMP4_2;
				break;

			case SAMPLING_PERIOD_480_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP4;
				break;

			default:
				__NOP();
				break;

			}

			break;

		case CHANNEL_5:

			ADC1->SMPR2 &= ~ADC_SMPR2_SMP5;
			switch(adcConfig->samplingPeriod){

			case SAMPLING_PERIOD_3_CYCLES:

				__NOP();
				break;

			case SAMPLING_PERIOD_15_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP5_0;
				break;

			case SAMPLING_PERIOD_28_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP5_1;

				break;

			case SAMPLING_PERIOD_56_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP5_0;
				ADC1->SMPR2 |= ADC_SMPR2_SMP5_1;

				break;

			case SAMPLING_PERIOD_84_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP5_2;
				break;

			case SAMPLING_PERIOD_112_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP5_0;
				ADC1->SMPR2 |= ADC_SMPR2_SMP5_2;
				break;

			case SAMPLING_PERIOD_144_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP5_1;
				ADC1->SMPR2 |= ADC_SMPR2_SMP5_2;
				break;

			case SAMPLING_PERIOD_480_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP5;
				break;

			default:
				__NOP();
				break;

			}

			break;

		case CHANNEL_6:

			ADC1->SMPR2 &= ~ADC_SMPR2_SMP6;
			switch(adcConfig->samplingPeriod){

			case SAMPLING_PERIOD_3_CYCLES:

				__NOP();
				break;

			case SAMPLING_PERIOD_15_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP6_0;
				break;

			case SAMPLING_PERIOD_28_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP6_1;

				break;

			case SAMPLING_PERIOD_56_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP6_0;
				ADC1->SMPR2 |= ADC_SMPR2_SMP6_1;

				break;

			case SAMPLING_PERIOD_84_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP6_2;
				break;

			case SAMPLING_PERIOD_112_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP6_0;
				ADC1->SMPR2 |= ADC_SMPR2_SMP6_2;
				break;

			case SAMPLING_PERIOD_144_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP6_1;
				ADC1->SMPR2 |= ADC_SMPR2_SMP6_2;
				break;

			case SAMPLING_PERIOD_480_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP6;
				break;

			default:
				__NOP();
				break;

			}

			break;

		case CHANNEL_7:

			ADC1->SMPR2 &= ~ADC_SMPR2_SMP7;
			switch(adcConfig->samplingPeriod){

			case SAMPLING_PERIOD_3_CYCLES:

				__NOP();
				break;

			case SAMPLING_PERIOD_15_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP7_0;
				break;

			case SAMPLING_PERIOD_28_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP7_1;

				break;

			case SAMPLING_PERIOD_56_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP7_0;
				ADC1->SMPR2 |= ADC_SMPR2_SMP7_1;

				break;

			case SAMPLING_PERIOD_84_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP7_2;
				break;

			case SAMPLING_PERIOD_112_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP7_0;
				ADC1->SMPR2 |= ADC_SMPR2_SMP7_2;
				break;

			case SAMPLING_PERIOD_144_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP7_1;
				ADC1->SMPR2 |= ADC_SMPR2_SMP7_2;
				break;

			case SAMPLING_PERIOD_480_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP7;
				break;

			default:
				__NOP();
				break;

			}

			break;

		case CHANNEL_8:

			ADC1->SMPR2 &= ~ADC_SMPR2_SMP8;
			switch(adcConfig->samplingPeriod){

			case SAMPLING_PERIOD_3_CYCLES:

				__NOP();
				break;

			case SAMPLING_PERIOD_15_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP8_0;
				break;

			case SAMPLING_PERIOD_28_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP8_1;

				break;

			case SAMPLING_PERIOD_56_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP8_0;
				ADC1->SMPR2 |= ADC_SMPR2_SMP8_1;

				break;

			case SAMPLING_PERIOD_84_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP8_2;
				break;

			case SAMPLING_PERIOD_112_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP8_0;
				ADC1->SMPR2 |= ADC_SMPR2_SMP8_2;
				break;

			case SAMPLING_PERIOD_144_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP8_1;
				ADC1->SMPR2 |= ADC_SMPR2_SMP8_2;
				break;

			case SAMPLING_PERIOD_480_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP8;
				break;

			default:
				__NOP();
				break;

			}

			break;

		case CHANNEL_9:

			ADC1->SMPR2 &= ~ADC_SMPR2_SMP9;
			switch(adcConfig->samplingPeriod){

			case SAMPLING_PERIOD_3_CYCLES:

				__NOP();
				break;

			case SAMPLING_PERIOD_15_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP9_0;
				break;

			case SAMPLING_PERIOD_28_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP9_1;

				break;

			case SAMPLING_PERIOD_56_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP9_0;
				ADC1->SMPR2 |= ADC_SMPR2_SMP9_1;

				break;

			case SAMPLING_PERIOD_84_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP9_2;
				break;

			case SAMPLING_PERIOD_112_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP9_0;
				ADC1->SMPR2 |= ADC_SMPR2_SMP9_2;
				break;

			case SAMPLING_PERIOD_144_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP9_1;
				ADC1->SMPR2 |= ADC_SMPR2_SMP9_2;
				break;

			case SAMPLING_PERIOD_480_CYCLES:

				ADC1->SMPR2 |= ADC_SMPR2_SMP9;
				break;

			default:
				__NOP();
				break;

			}

			break;

		case CHANNEL_10:

			ADC1->SMPR1 &= ~ADC_SMPR1_SMP10;
			switch(adcConfig->samplingPeriod){

			case SAMPLING_PERIOD_3_CYCLES:

				__NOP();
				break;

			case SAMPLING_PERIOD_15_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP10_0;
				break;

			case SAMPLING_PERIOD_28_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP10_1;

				break;

			case SAMPLING_PERIOD_56_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP10_0;
				ADC1->SMPR1 |= ADC_SMPR1_SMP10_1;

				break;

			case SAMPLING_PERIOD_84_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP10_2;
				break;

			case SAMPLING_PERIOD_112_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP10_0;
				ADC1->SMPR1 |= ADC_SMPR1_SMP10_2;
				break;

			case SAMPLING_PERIOD_144_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP10_1;
				ADC1->SMPR1 |= ADC_SMPR1_SMP10_2;
				break;

			case SAMPLING_PERIOD_480_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP10;
				break;

			default:
				__NOP();
				break;

			}

			break;

		case CHANNEL_11:

			ADC1->SMPR1 &= ~ADC_SMPR1_SMP11;
			switch(adcConfig->samplingPeriod){

			case SAMPLING_PERIOD_3_CYCLES:

				__NOP();
				break;

			case SAMPLING_PERIOD_15_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP11_0;
				break;

			case SAMPLING_PERIOD_28_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP11_1;

				break;

			case SAMPLING_PERIOD_56_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP11_0;
				ADC1->SMPR1 |= ADC_SMPR1_SMP11_1;

				break;

			case SAMPLING_PERIOD_84_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP11_2;
				break;

			case SAMPLING_PERIOD_112_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP11_0;
				ADC1->SMPR1 |= ADC_SMPR1_SMP11_2;
				break;

			case SAMPLING_PERIOD_144_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP11_1;
				ADC1->SMPR1 |= ADC_SMPR1_SMP11_2;
				break;

			case SAMPLING_PERIOD_480_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP11;
				break;

			default:
				__NOP();
				break;

			}

			break;

		case CHANNEL_12:

			ADC1->SMPR1 &= ~ADC_SMPR1_SMP12;
			switch(adcConfig->samplingPeriod){

			case SAMPLING_PERIOD_3_CYCLES:

				__NOP();
				break;

			case SAMPLING_PERIOD_15_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP12_0;
				break;

			case SAMPLING_PERIOD_28_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP12_1;

				break;

			case SAMPLING_PERIOD_56_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP12_0;
				ADC1->SMPR1 |= ADC_SMPR1_SMP12_1;

				break;

			case SAMPLING_PERIOD_84_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP12_2;
				break;

			case SAMPLING_PERIOD_112_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP12_0;
				ADC1->SMPR1 |= ADC_SMPR1_SMP12_2;
				break;

			case SAMPLING_PERIOD_144_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP12_1;
				ADC1->SMPR1 |= ADC_SMPR1_SMP12_2;
				break;

			case SAMPLING_PERIOD_480_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP12;
				break;

			default:
				__NOP();
				break;

			}

			break;

		case CHANNEL_13:

			ADC1->SMPR1 &= ~ADC_SMPR1_SMP13;
			switch(adcConfig->samplingPeriod){

			case SAMPLING_PERIOD_3_CYCLES:

				__NOP();
				break;

			case SAMPLING_PERIOD_15_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP13_0;
				break;

			case SAMPLING_PERIOD_28_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP13_1;

				break;

			case SAMPLING_PERIOD_56_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP13_0;
				ADC1->SMPR1 |= ADC_SMPR1_SMP13_1;

				break;

			case SAMPLING_PERIOD_84_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP13_2;
				break;

			case SAMPLING_PERIOD_112_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP13_0;
				ADC1->SMPR1 |= ADC_SMPR1_SMP13_2;
				break;

			case SAMPLING_PERIOD_144_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP13_1;
				ADC1->SMPR1 |= ADC_SMPR1_SMP13_2;
				break;

			case SAMPLING_PERIOD_480_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP13;
				break;

			default:
				__NOP();
				break;

			}

			break;

		case CHANNEL_14:

			ADC1->SMPR1 &= ~ADC_SMPR1_SMP14;
			switch(adcConfig->samplingPeriod){

			case SAMPLING_PERIOD_3_CYCLES:

				__NOP();
				break;

			case SAMPLING_PERIOD_15_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP14_0;
				break;

			case SAMPLING_PERIOD_28_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP14_1;

				break;

			case SAMPLING_PERIOD_56_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP14_0;
				ADC1->SMPR1 |= ADC_SMPR1_SMP14_1;

				break;

			case SAMPLING_PERIOD_84_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP14_2;
				break;

			case SAMPLING_PERIOD_112_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP14_0;
				ADC1->SMPR1 |= ADC_SMPR1_SMP14_2;
				break;

			case SAMPLING_PERIOD_144_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP14_1;
				ADC1->SMPR1 |= ADC_SMPR1_SMP14_2;
				break;

			case SAMPLING_PERIOD_480_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP14;
				break;

			default:
				__NOP();
				break;

			}

			break;


		case CHANNEL_15:

			ADC1->SMPR1 &= ~ADC_SMPR1_SMP15;
			switch(adcConfig->samplingPeriod){

			case SAMPLING_PERIOD_3_CYCLES:

				__NOP();
				break;

			case SAMPLING_PERIOD_15_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP15_0;
				break;

			case SAMPLING_PERIOD_28_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP15_1;

				break;

			case SAMPLING_PERIOD_56_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP15_0;
				ADC1->SMPR1 |= ADC_SMPR1_SMP15_1;

				break;

			case SAMPLING_PERIOD_84_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP15_2;
				break;

			case SAMPLING_PERIOD_112_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP15_0;
				ADC1->SMPR1 |= ADC_SMPR1_SMP15_2;
				break;

			case SAMPLING_PERIOD_144_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP15_1;
				ADC1->SMPR1 |= ADC_SMPR1_SMP15_2;
				break;

			case SAMPLING_PERIOD_480_CYCLES:

				ADC1->SMPR1 |= ADC_SMPR1_SMP15;
				break;

			default:
				__NOP();
				break;

			}

			break;

	default:
		__NOP();
		break;


	}
}

/*
 * Configura el numero de elementos en la secuencia (solo un elemento)
 * Configra cual es el canal adquiere la señal ADC
 * */

static void adc_set_one_channel_sequence (ADC_Config_t *adcConfig){

	ADC1->SQR1 &= ~ADC_SQR1_L;
	ADC1->SQR3 &= ~ADC_SQR3_SQ1;
	switch(adcConfig->channel){

		case CHANNEL_0:
			__NOP();
			break;

		case CHANNEL_1:
			ADC1->SQR3 |= ADC_SQR3_SQ1_0;
			break;

		case CHANNEL_2:
			ADC1->SQR3 |= ADC_SQR3_SQ1_1;
			break;

		case CHANNEL_3:
			ADC1->SQR3 |= ADC_SQR3_SQ1_0;
			ADC1->SQR3 |= ADC_SQR3_SQ1_1;
			break;

		case CHANNEL_4:
			ADC1->SQR3 |= ADC_SQR3_SQ1_2;
			break;

		case CHANNEL_5:
			ADC1->SQR3 |= ADC_SQR3_SQ1_0;
			ADC1->SQR3 |= ADC_SQR3_SQ1_2;
			break;

		case CHANNEL_6:
			ADC1->SQR3 |= ADC_SQR3_SQ1_1;
			ADC1->SQR3 |= ADC_SQR3_SQ1_2;
			break;

		case CHANNEL_7:
			ADC1->SQR3 |= ADC_SQR3_SQ1_0;
			ADC1->SQR3 |= ADC_SQR3_SQ1_1;
			ADC1->SQR3 |= ADC_SQR3_SQ1_2;
			break;

		case CHANNEL_8:
			ADC1->SQR3 |= ADC_SQR3_SQ1_3;
			break;

		case CHANNEL_9:
			ADC1->SQR3 |= ADC_SQR3_SQ1_0;
			ADC1->SQR3 |= ADC_SQR3_SQ1_3;
			break;

		case CHANNEL_10:
			ADC1->SQR3 |= ADC_SQR3_SQ1_3;
			ADC1->SQR3 |= ADC_SQR3_SQ1_1;
			break;

		case CHANNEL_11:
			ADC1->SQR3 |= ADC_SQR3_SQ1_3;
			ADC1->SQR3 |= ADC_SQR3_SQ1_1;
			ADC1->SQR3 |= ADC_SQR3_SQ1_0;
			break;

		case CHANNEL_12:
			ADC1->SQR3 |= ADC_SQR3_SQ1_2;
			ADC1->SQR3 |= ADC_SQR3_SQ1_3;
		break;

		case CHANNEL_13:
			ADC1->SQR3 |= ADC_SQR3_SQ1_0;
			ADC1->SQR3 |= ADC_SQR3_SQ1_2;
			ADC1->SQR3 |= ADC_SQR3_SQ1_3;
		break;

		case CHANNEL_14:
			ADC1->SQR3 |= ADC_SQR3_SQ1_2;
			ADC1->SQR3 |= ADC_SQR3_SQ1_1;
			ADC1->SQR3 |= ADC_SQR3_SQ1_3;
		break;

		case CHANNEL_15:
			ADC1->SQR3 |= ADC_SQR3_SQ1_0;
			ADC1->SQR3 |= ADC_SQR3_SQ1_1;
			ADC1->SQR3 |= ADC_SQR3_SQ1_2;
			ADC1->SQR3 |= ADC_SQR3_SQ1_3;
		break;


	}

}

/* Configura el enable de la interrupcion y la activacion del NVIC*/

static void adc_config_interrupt(ADC_Config_t *adcConfig){

	if (adcConfig->interrupState ==ADC_INT_DISABLE){
		ADC1->CR1 &= ~ADC_CR1_EOCIE;

	} else if(adcConfig->interrupState ==ADC_INT_ENABLE){
		ADC1->CR1 |= ADC_CR1_EOCIE;

		// Matriculamos la interrupción:

		__NVIC_EnableIRQ(ADC_IRQn);
		__NVIC_SetPriority(ADC_IRQn, 2);

	}

}

/*Controla la activacion y desactivacion del modulo ADC desde el registro CR2 del adc*/

void adc_peripheralOnOFF(uint8_t state){

	if(state==ADC_OFF){
		ADC1->CR2 &= ~ADC_CR2_ADON;
	}
	else if(state==ADC_ON){
		ADC1->CR2 |= ADC_CR2_ADON;
	}

}

/* Enables and disables the Scan mode...
 * Funciona de la mano con la secuencia de varios canales
 * NO es necesario para elcaso de un solo canal simple
 * */

void adc_ScanMode(uint8_t state){

	if(state==SCAN_OFF){
		ADC1->CR1 &= ~ADC_CR1_SCAN;
	}
	else if(state==SCAN_ON){
		ADC1->CR1 |= ADC_CR1_SCAN;
	}

}

/*Funcion que comienza la conversion ADC simple*/

void adc_StartSingleConv(void){

	ADC1->CR2 &= ~ADC_CR2_CONT;
	ADC1->CR2 |= ADC_CR2_SWSTART;

}

/*Funcion que comienza la conversion ADC continua*/
void adc_StartContinuousConv(void){

	ADC1->CR2 |= ADC_CR2_SWSTART;
	ADC1->CR2 |= ADC_CR2_CONT;

	// ADC1->CR2 &= ~ADC_CR2_SWSTART;

}

/*Funcion que para la conversion ADC continua*/
void adc_StopContinuousConv(void){

	 ADC1->CR2 &= ~ADC_CR2_CONT;
	 ADC1->CR2 &= ~ADC_CR2_SWSTART;
}

/*Funcion que me retorna el ultimo dato adquirido por la ADC*/

uint16_t adc_GetValue(void){

	//while (!(ADC1->SR & ADC_SR_EOC)){
		//__NOP();
	//}
	adcRawData= ADC1->DR;
	return adcRawData;
}

void adc_ConfigAnalogPin(uint8_t adcChannel){

	switch (adcChannel){

	case CHANNEL_0:

		handlerADCPin.pGPIOx = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_0;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
		handlerADCPin.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_FAST;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_1:

		handlerADCPin.pGPIOx = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_1;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
		handlerADCPin.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_FAST;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_2:

		handlerADCPin.pGPIOx = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_2;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
		handlerADCPin.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_FAST;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_3:

		handlerADCPin.pGPIOx = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_3;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
		handlerADCPin.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_FAST;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_4:

		handlerADCPin.pGPIOx = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_4;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
		handlerADCPin.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_FAST;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_5:

		handlerADCPin.pGPIOx = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_5;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
		handlerADCPin.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_FAST;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_6:

		handlerADCPin.pGPIOx = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_6;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
		handlerADCPin.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_FAST;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_7:

		handlerADCPin.pGPIOx = GPIOA;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_7;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
		handlerADCPin.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_FAST;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_8:

		handlerADCPin.pGPIOx = GPIOB;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_0;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
		handlerADCPin.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_FAST;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_9:

		handlerADCPin.pGPIOx = GPIOB;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_1;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
		handlerADCPin.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_FAST;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_10:

		handlerADCPin.pGPIOx = GPIOC;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_0;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
		handlerADCPin.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_FAST;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_11:

		handlerADCPin.pGPIOx = GPIOC;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_1;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
		handlerADCPin.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_FAST;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_12:

		handlerADCPin.pGPIOx = GPIOC;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_2;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
		handlerADCPin.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_FAST;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_13:

		handlerADCPin.pGPIOx = GPIOC;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_3;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
		handlerADCPin.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_FAST;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_14:

		handlerADCPin.pGPIOx = GPIOC;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_4;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
		handlerADCPin.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_FAST;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	case CHANNEL_15:

		handlerADCPin.pGPIOx = GPIOC;
		handlerADCPin.pinConfig.GPIO_PinNumber = PIN_5;
		handlerADCPin.pinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
		handlerADCPin.pinConfig.GPIO_PinOutputType = GPIO_OTYPE_PUSHPULL;
		handlerADCPin.pinConfig.GPIO_PinOutputSpeed = GPIO_OSPEEDR_FAST;
		handlerADCPin.pinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
		gpio_Config(&handlerADCPin);
		break;

	default:
		__NOP();

	}

}

/*Esta es la ISR de la interrupciON POR CONVERSION ADC*/

void ADC_IRQHandler(void){

	adcRawData= ADC1->DR;
	adc_CompleteCallback();


}

void adc_defineSQ(ADC_Config_t *adcConfig,uint8_t position){

	switch(position-1){

	case SQ_1:
			ADC1->SQR3 |= (adcConfig->channel<<ADC_SQR3_SQ1_Pos);
		break;

	case SQ_2:
			ADC1->SQR3 |= (adcConfig->channel<<ADC_SQR3_SQ2_Pos);
		break;

	case SQ_3:
			ADC1->SQR3 |= (adcConfig->channel<<ADC_SQR3_SQ3_Pos);
		break;

	case SQ_4:
			ADC1->SQR3 |= (adcConfig->channel<<ADC_SQR3_SQ4_Pos);
		break;

	case SQ_5:
			ADC1->SQR3 |= (adcConfig->channel<<ADC_SQR3_SQ5_Pos);
		break;

	case SQ_6:
			ADC1->SQR3 |= (adcConfig->channel<<ADC_SQR3_SQ6_Pos);
		break;

	case SQ_7:
			ADC1->SQR2 |= (adcConfig->channel<<ADC_SQR2_SQ7_Pos);
		break;

	case SQ_8:
			ADC1->SQR2 |= (adcConfig->channel<<ADC_SQR2_SQ8_Pos);
		break;

	case SQ_9:
			ADC1->SQR2 |= (adcConfig->channel<<ADC_SQR2_SQ9_Pos);
		break;

	case SQ_10:
			ADC1->SQR2 |= (adcConfig->channel<<ADC_SQR2_SQ10_Pos);
		break;

	case SQ_11:
			ADC1->SQR2 |= (adcConfig->channel<<ADC_SQR2_SQ11_Pos);
		break;

	case SQ_12:
			ADC1->SQR2 |= (adcConfig->channel<<ADC_SQR2_SQ12_Pos);
		break;

	case SQ_13:
			ADC1->SQR1 |= (adcConfig->channel<<ADC_SQR1_SQ13_Pos);
		break;

	case SQ_14:
			ADC1->SQR1 |= (adcConfig->channel<<ADC_SQR1_SQ14_Pos);
		break;

	case SQ_15:
			ADC1->SQR1 |= (adcConfig->channel<<ADC_SQR1_SQ15_Pos);
		break;

	case SQ_16:
			ADC1->SQR1 |= (adcConfig->channel<<ADC_SQR1_SQ16_Pos);
		break;

	default:
		__NOP();
		break;
	}
}

void adc_ConfigMultiChannel(ADC_Config_t *adcConfig[], uint8_t numberOfChannels){


	//Configuramos la longtitud de la secuencia
	ADC1->SQR1 &= ~ADC_SQR1_L;
	ADC1->SQR1 |= ((numberOfChannels-1)<<ADC_SQR1_L_Pos);

	//Configuración de cada uno de los canales:

    for (uint8_t i = 0; i < numberOfChannels; i++) {

    	adc_ConfigSingleChannel(adcConfig[i]);
    	// Configuracición de la secuencia:
    	adc_defineSQ(adcConfig[i],i+1);

  }



    // Activamos el modo SCAN

    adc_ScanMode(SCAN_ON);

    //Configuramos para que la interrupción se de al final de la secuencia
    ADC1->CR1 |= ADC_CR1_EOCIE;
    ADC1->CR2 |= ADC_CR2_EOCS;

    }






__attribute__((weak)) void adc_CompleteCallback(void){
	__NOP();
}

/*Configuracion para hacer conversiones en multiles canales y en un orden especifico*/
/*Configuracion para trigger externo*/
