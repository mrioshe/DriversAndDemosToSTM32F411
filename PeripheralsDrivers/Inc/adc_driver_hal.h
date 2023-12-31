/*
 * adc_driver_hal.h
 *
 *  Created on: Oct 19, 2023
 *      Author: mauricio
 */

#ifndef ADC_DRIVER_HAL_H_
#define ADC_DRIVER_HAL_H_


#include <stdint.h>
#include "stm32f4xx.h"

enum{
	CHANNEL_0=0,
	CHANNEL_1,
	CHANNEL_2,
	CHANNEL_3,
	CHANNEL_4,
	CHANNEL_5,
	CHANNEL_6,
	CHANNEL_7,
	CHANNEL_8,
	CHANNEL_9,
	CHANNEL_10,
	CHANNEL_11,
	CHANNEL_12,
	CHANNEL_13,
	CHANNEL_14,
	CHANNEL_15
};

enum{
	SQ_1=0,
	SQ_2,
	SQ_3,
	SQ_4,
	SQ_5,
	SQ_6,
	SQ_7,
	SQ_8,
	SQ_9,
	SQ_10,
	SQ_11,
	SQ_12,
	SQ_13,
	SQ_14,
	SQ_15,
	SQ_16
};

enum{
	TRIGGER_DETECTION_DISABLE=0,
	DETECTION_RISING_EDGE,
	DETECTION_FALLING_EDGE,
	DETECTION_RISING_FALLING_EDGES
};

enum{
	TIM1_CH1_EVENT=0,
	TIM1_CH2_EVENT,
	TIM1_CH3_EVENT,
	TIM2_CH2_EVENT,
	TIM2_CH3_EVENT,
	TIM2_CH4_EVENT,
	TIM2_TRGO_EVENT,
	TIM3_CH1_EVENT,
	TIM3_TRGO_EVENT,
	TIM4_CH4_EVENT,
	TIM5_CH1_EVENT,
	TIM5_CH2_EVENT,
	TIM5_CH3_EVENT,
	EXTI_LINE11_EVENT
};

enum{
	RESOLUTION_12_BIT=0,
	RESOLUTION_10_BIT,
	RESOLUTION_8_BIT,
	RESOLUTION_6_BIT
};

enum{
	ALIGNMENT_RIGHT=0,
	ALIGNMENT_LEFT
};

enum{
	SCAN_OFF=0,
	SCAN_ON
};

enum{
	ADC_OFF=0,
	ADC_ON
};

enum{
	ADC_INT_DISABLE=0,
	ADC_INT_ENABLE
};

enum{
	TRIGGER_AUTO=0,
	TRIGGER_MANUAL,
	TRIGGER_EXT
};

enum{
	SAMPLING_PERIOD_3_CYCLES=0b000,
	SAMPLING_PERIOD_15_CYCLES=0b001,
	SAMPLING_PERIOD_28_CYCLES=0b010,
	SAMPLING_PERIOD_56_CYCLES=0b011,
	SAMPLING_PERIOD_84_CYCLES=0b100,
	SAMPLING_PERIOD_112_CYCLES=0b101,
	SAMPLING_PERIOD_144_CYCLES=0b110,
	SAMPLING_PERIOD_480_CYCLES=0b111
};

/*
 * ADC Handler definiton
 * This handler is used to configure a single ADC channel
 * -Channels 		->  configures inside the driver the correct GPIO pin as ADC channel
 * -Resolution		-> 6bit, 8bit, 10bit, 12bit
 * -Sampling Period	-> Related to the SARs procedure
 * -Data alignment	->left or right, depends on the app. Default is right
 * -adc data		-> holds the data
 * */

typedef struct
{
uint8_t		channel;			// Canal ADC que será utilizado para la conversión ADC
uint8_t		resolution;			// Precisión con la que el ADC hace la dquisición del dato
uint16_t	samplingPeriod;		// Tiempo deseado para hacer la adquisicióon del dato
uint8_t		dataAlignment;		// Alineación a la izquierda
uint16_t	adcData;			// Dato de la conversión
uint8_t		interrupState;		// Para configura si se desea o no trabajar con la interrupción
} ADC_Config_t;

/*Header definition for the "public functions" of adc_driver_hal*/

void adc_ConfigSingleChannel (ADC_Config_t *adcConfig);
void adc_ConfigAnalogPin(uint8_t adcChannel);
void adc_CompleteCallback(void);
void adc_StartSingleConv(void);
void adc_ScanMode(uint8_t state);
void adc_StartContinuousConv(void);
void adc_StopContinuousConv(void);
void adc_peripheralOnOFF(uint8_t state);
void adc_ConfigMultiChannel(ADC_Config_t adcConfig[16], uint8_t numberOfChannels);
void adc_defineSQ(ADC_Config_t *adcConfig,uint8_t position);
void adc_startTriggeredAdc(uint8_t triggerPolarity,uint8_t event);
void adc_StopTriggeredAdc(void);
void SetTriggerPolarity(uint8_t triggerPolarity);
void SetTriggerEvent(uint8_t event);
uint16_t adc_GetValue(void);

/*Configuraciones avanzadas del ADC*/

// void adc_ConfigMultichannel (ADC_Config_t *adcConfig, uint8_t numeroDeCanales);
// void adc_COnfigTrigger (uint8_t sourceType, PWM_Handler_t *triggerSignal);

#endif /*ADC_DRIVER_HAL_H_*/

