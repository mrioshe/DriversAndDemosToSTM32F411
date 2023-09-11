/*
 * stm32f4xx_hal.h
 *
 *  Created on: Sep 10, 2023
 *      Author: mauricio


* Created on: sep 09, 2023
*
* Author: mrioshe
*
*Este archivo contiene la información más báscia del micro:
* - Valores del reloj principal
* - Distribución básica de la memoria (descrito en la figura 14 de la hoja de datos del micro)
* - Posiciones de memoria de los periféricos disponibles en el micro descrito en la tabla 1 (Memory Map)
* - Incluir los demás drivers de los periféricos
* - Definiciones de las constantes más básicas.
*
* NOTA: La definición del NVIC será realizada al momento de describir el uso de las interrupciones
 *************************************************************************************************/

#ifndef STM32F4XX_HAL_H_
#define STM32F4XX_HAL_H_

#include <stdint.h>
#include <stddef.h>

#define HSI_CLOCK_SPEED 8000000 // Value for the main clock signal (HSI -> High Speed internal)
#define HSE_CLOCK_SPEED 16000000 // Value for the main clock signal (HSE -> High Speed internal)

// #define NOP()	(__asm__("NOP"))

#define NOP ()		asm("NOP")
#define __weak		__attribute__((weak))

/*
 * Base addresses of Flash and SRAM memories
 * Datasheet, Memory map, FIgure 14
 * (Remember, 1Byte=1024 bytes)
 */

#define FLASH_BASE_ADDR			0x08000000U		// Esta es la memoria del programa, 512KB
#define SRAM_BASE_ADDR			0x20000000U		// Esta es la memoria RAM, 128KB.

/* NOTA: Observar que existen unos registros específicos del Cortex m4 en la región 0xE0000000u
 * Los controladores de las interrupciones se encuentra allí, por ejemplo
 */

/* NOTA:
 * Ahora agregamos la dirección de memoria base para cada uno de los periféricos que posee el
 * en el "Datasheet" del micro, figura 14 (Memory map) encontramos el mapa de los buses:
 * APDB1 (Advance peripheral Bus)
 * APB2
 * AHB1 (Advance High-performance Bus)
 * AHB2
 */

/**
 * AHBx and APBx Bus peripherals base addresses
 */

#define APB1_BASE_ADDR			0x400000000U
#define APB2_BASE_ADDR			0x400100000U
#define AHB1_BASE_ADDR			0x400200000U
#define AHB2_BASE_ADDR			0x500000000U

/**
 * Y ahora debemos hacer lo mismo pero cada una de las posiciones de memoria de cada uno de los
 * periféricos descritos en la Tabla 1 del manual de referencia del micro.
 * Observe que en dicha tabla está a su vez dividida en cuatro segmentos, cada uno correspondiente
 * a APB1, APB2, AHB1, AHB2.
 *
 * Comenzar de arriba hacia abajo como se muestra en la tabla. Inicia USB_OTG_FS (AHB2)
 */

/* Posición de memoria para periféricos del AHB2 */

#define USB_OTG_FS_BASE_ADDR	(AHB2_BASE_ADDR + 0x0000U)

/*Posiciones de memoria para periféricos del AHB1*/

#define DMA2_BASE_ADDR			(AHB1_BASE_ADDR + 0x6400U)
#define DMA1_BASE_ADDR			(AHB1_BASE_ADDR + 0x6000U)
#define FIR_BASE_ADDR			(AHB1_BASE_ADDR + 0x3C00U)  // Flash Interface Register
#define	RCC_BASE_ADDR			(AHB1_BASE_ADDR + 0x3800U)
#define CRC_BASE_ADDR			(AHB1_BASE_ADDR + 0x3000U)
#define GPIOH_BASE_ADDR			(AHB1_BASE_ADDR + 0x1C00U)
#define GPIOE_BASE_ADDR			(AHB1_BASE_ADDR + 0x1000U)
#define GPIOD_BASE_ADDR			(AHB1_BASE_ADDR + 0x0C00U)
#define GPIOC_BASE_ADDR			(AHB1_BASE_ADDR + 0x0800U)
#define GPIOB_BASE_ADDR			(AHB1_BASE_ADDR + 0x0400U)
#define GPIOA_BASE_ADDR			(AHB1_BASE_ADDR + 0x0000U)

/*
 * Macros genéricos
 */

#define DISABLE			(0)
#define ENABLE			(1)
#define SET 			ENABLE
#define CLEAR			DISABLE
#define RESET			DISABLE
#define FLAG_SET		SET
#define FLAG_RESET		RESET
#define I2C_WRITE		(0)
#define I2C_READ		(1)

/* +++======= INICIO de la descripción de los elementos que componen el periféricos =======+++ */

/* Definición de la estructura de datos que representa a cada uno de los registros que componen el
 * periférico RCC.
 *
 * Debido a los temas que se van a manejar en el curso, solo deben definir los bits de los registros:
 * 6.3.1 (RCC_CR) hasta el 6.3.12 (RCC_APB2ENR), 6.3.17 (RCC_BDCR) y 6.3.18 (RCC_CSR)
 *
 * NOTA: La posición de memoria (offset) debe encajar perfectamente con la posción de memoria indicada
 * en la hoja de datos del equipo. Observe que los elementos "reservadx" también están presentes allí
 */

typedef struct
{
	volatile uint32_t CR;			// Clock Control Register						ADDR_OFFSET:	0x00
	volatile uint32_t PLLCFGR;		// PLL Configuration register					ADDR_OFFSET:	0x04
	volatile uint32_t CFGR;			// Clock Configuration register 				ADDR_OFFSET:  	0x08
	volatile uint32_t CIR; 			// Clock Interrupt register						ADDR_OFFSET: 	0x0C
	volatile uint32_t AHB1RSTR;		// AHB1 Peripheral reset register				ADDR_OFFSET:	0x10
	volatile uint32_t AHB2RSTR;		// AHB2 Peripheral reset register				ADDR_OFFSET:	0x14
	volatile uint32_t reserved0;	// reserved										ADDR_OFFSET:	0x18
	volatile uint32_t reserved1;	// reserved										ADDR_OFFSET:	0x1C
	volatile uint32_t APB1RSTR;		// APB1 Peripheral Reset register				ADDR_OFFSET:	0x20
	volatile uint32_t APB2RSTR;		// APB2 Peripheral Reset register				ADDR_OFFSET: 	0x24
	volatile uint32_t reserved2;	// reserved										ADDR_OFFSET:	0x28
	volatile uint32_t reserved3;	// reserved										ADDR_OFFSET:	0x2C
	volatile uint32_t AHB1ENR;		// AHB1 Peripheral clock enable register		ADDR_OFFSET: 	0x30
	volatile uint32_t AHB2ENR;		// AHB2 Peripheral clock enable register		ADDR_OFFSET: 	0x34
	volatile uint32_t reserved4;	// reserved										ADDR_OFFSET:	0x38
	volatile uint32_t reserved5;	// reserved										ADDR_OFFSET:	0x3C
	volatile uint32_t APB1ENR;		// APB1 Peripheral clock enable register		ADDR_OFFSET: 	0x40
	volatile uint32_t APB2ENR;		// APB2 Peripheral clock enable register		ADDR_OFFSET: 	0x44
	volatile uint32_t reserved6;	// reserved										ADDR_OFFSET:	0x48
	volatile uint32_t reserved7;	// reserved										ADDR_OFFSET:	0x4C
	volatile uint32_t AHB1LPENR;	// AHB1 clock enable low power register			ADDR_OFFSET: 	0x50
	volatile uint32_t AHB2LPENR;	// AHB2 clock enable low power register			ADDR_OFFSET: 	0x54
	volatile uint32_t reserved8;	// reserved										ADDR_OFFSET:	0x58
	volatile uint32_t reserved9;	// reserved										ADDR_OFFSET:	0x5C
	volatile uint32_t APB1LPENR;	// APB1 clock enable low power register			ADDR_OFFSET: 	0x60
	volatile uint32_t APB2LPENR;	// APB2 clock enable low power register			ADDR_OFFSET: 	0x64
	volatile uint32_t reserved10;	// reserved										ADDR_OFFSET:	0x68
	volatile uint32_t reserved11;	// reserved										ADDR_OFFSET:	0x6C
	volatile uint32_t BDCR;			// Backup Domain control register				ADDR_OFFSET: 	0x70
	volatile uint32_t CSR;			// Clock control Status register				ADDR_OFFSET: 	0x74
	volatile uint32_t reserved12;	// reserved										ADDR_OFFSET:	0x78
	volatile uint32_t reserved13;	// reserved										ADDR_OFFSET:	0x7C
	volatile uint32_t SSCGR;		// Spread Spectrum Clock Generation register	ADDR_OFFSET: 	0x80
	volatile uint32_t PLLI2SCFGR;	// PLLI2S Configuration register				ADDR_OFFSET: 	0x84
	volatile uint32_t reserved14;	// reserved										ADDR_OFFSET:	0x88
	volatile uint32_t DCKCFGR;		// DEDICATED Clocks Configuration register		ADDR_OFFSET:	0x8C

} RCC_RegDef_t;

/*
 * Hacemos como un "merge", en el cual ubicamos RCC_RegDef_t a apuntar a la posición de memoria correspondiente,
 * de forma que cada variable dentro de la estructura coincide con cada uno de los SFR de la memoria del micro-controlador
 */

#define RCC			((RCC_RefDef_t *) RCC_BASE_ADDR)

/*Descripción bit a bit de cada uno de los registros del que componen al perfiférico RCC*/
/* 6.3.1 RCC_CR */
/* 6.3.2 RCC_PLLCFGR */
/* 6.3.3 RCC_CFGR */
/* 6.3.4 RCC_CIR */
/* 6.3.5 RCC_AHB1RSTR */
/* 6.3.6 RCC_AHB2RSTR */
/* 6.3.7 RCC_APB1RSTR */
/* 6.3.8 RCC_APB2RSTR */

/*	6.3.9 RCC_AHB1ENR*/

#define RCC_AHB1ENR_GPIOAEN		0
#define RCC_AHB1ENR_GPIOBEN		1
#define RCC_AHB1ENR_GPIOCEN		2
#define RCC_AHB1ENR_GPIODEN		3
#define RCC_AHB1ENR_GPIOEEN		4
#define RCC_AHB1ENR_GPIOHEN		7
#define RCC_AHB1ENR_CRCEN		12
#define RCC_AHB1ENR_DMA1EN		21
#define RCC_AHB1ENR_DMA2EN		22

/*	6.3.10 RCC_AHB2ENR */
/*	6.3.11 RCC_APB1ENR */
/*	6.3.12 RCC_APB2ENR */
/*	6.3.17 RCC_BDCR */
/*	6.3.18 RCC_CSR */

/* +++======= FIN de la descripción de los elementos que componen el periférico =======+++ */

/* ++++++++ Posibles valores de las configuraciones */
/* System Clock (main clock) possible options */
/* Microncontroller Clock Output 1 (MCO1) options */
/* Microncontroller preescaler Clock Output1 (MCO1PRE) options */
/* Microncontroller Clock Output 2 (MCO2) options */

/* +++======= FIN de la descripción de los elementos que componen el periférico =======+++ */
/* definición de la estructura de datos que representa a cada uno de los registros que componen el
 * periférico GPIO
 *
 * Debido a que el periférico GPIOx es muy simple, no es muy necesario crear la descripción bit a
 * bit de cada uno de los resgistros que componen el dicho periférico, pero si es necesario
 * comprender qué hace cada registro, para poder cargar correctamente la configuración
 *
 */

typedef struct

{
	volatile uint32_t MODER; 		// port mode register					ADDR_OFFSET: 0x00
	volatile uint32_t OTYPER;		// port output type register			ADDR_OFFSET: 0x04
	volatile uint32_t OSPEEDR;		// port output speed register			ADDR_OFFSET: 0x08
	volatile uint32_t PUPDR; 		// Port pull_up/pull-down register		ADDR_OFFSET: 0x0C
	volatile uint32_t IDR; 			// port input data register				ADDR_OFFSET: 0x10
	volatile uint32_t ODR;			// port output data register			ADDR_OFFSET: 0x14
	volatile uint32_t BSRR;			// port bit set/reset register			ADDR_OFFSET: 0x18
	volatile uint32_t LCKR; 		// port configuration lock register		ADDR_OFFSET: 0x1C
	volatile uint32_t AFR[2];		// port alternate function registers	ADDR_OFFSET: 0x20-0x24
} GPIO_TypeDef;

/* Creamos un objeto de la estructura definida y hacemos que quede ubicada exactamente sobre la posición
 * de memoria donde se encuentra en registro
 * Debido a que son varios periféricos GPIOx, es necesario hacer la definición para cada uno.
 *
 * Tener cuidado que cada elemento coincida con su respectiva dirección base.
 * */

#define GPIOA			((GPIO_TypeDef *) GPIOA_BASE_ADDR)
#define GPIOB			((GPIO_TypeDef *) GPIOB_BASE_ADDR)
#define GPIOC			((GPIO_TypeDef *) GPIOC_BASE_ADDR)
#define GPIOD			((GPIO_TypeDef *) GPIOD_BASE_ADDR)
#define GPIOE			((GPIO_TypeDef *) GPIOE_BASE_ADDR)
#define GPIOH			((GPIO_TypeDef *) GPIOH_BASE_ADDR)

/*For testing assert parameters. */

#define IS_GPIO_ALL_INSTANCE(GPIOx)((GPIOx == GPIOA) || \
									(GPIOx == GPIOB) || \
									(GPIOx == GPIOC) || \
									(GPIOx == GPIOD) || \
									(GPIOx == GPIOE) || \
									(GPIOx == GPIOH))



/*Incluimos las librerías de cada periférico*/


#endif /* STM32F4XX_HAL_H_ */

