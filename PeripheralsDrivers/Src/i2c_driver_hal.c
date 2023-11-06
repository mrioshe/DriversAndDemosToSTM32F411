/*
 * i2c_driver_hal.c
 *
 *  Created on: Nov 6, 2023
 *      Author: mauricio
 */

#include <stdint.h>
#include "i2c_driver_hal.h"

/*
 * los pines deben ser configurados como opend-drain
 * */

void i2c_config(I2C_Handler_t *ptrHandlerI2C){

	/* 1. Activar la señal de reloj para el módulo I2C seleccionado*/

	if (ptrHandlerI2C->ptrI2Cx==I2C1){
		RCC -> APB1ENR |= RCC_APB1ENR_I2C1EN;
	} else if(ptrHandlerI2C->ptrI2Cx==I2C2){
		RCC -> APB1ENR |= RCC_APB1ENR_I2C2EN;
	} else if(ptrHandlerI2C->ptrI2Cx==I2C3){
		RCC -> APB1ENR |= RCC_APB1ENR_I2C3EN;
	}

	/* 2. Reiniciar el periferico, de forma que incia en un estado conocido*/

	ptrHandlerI2C -> ptrI2Cx->CR1 |= I2C_CR1_SWRST;
	__NOP();
	ptrHandlerI2C -> ptrI2Cx->CR1 &= ~I2C_CR1_SWRST;

	/* 3. Indicamos cual es la velocidad del reloj principal
	 * */

	ptrHandlerI2C->ptrI2Cx->CR2 &= ~(0b111111<<I2C_CR2_FREQ_Pos); //Borrar config

	switch(ptrHandlerI2C ->I2CConfig.frecMainClock){

		case MAIN_CLOCK_4_MHz_FOR_I2C:
			ptrHandlerI2C->ptrI2Cx->CR2 |= (4 << I2C_CR2_FREQ_Pos);
			break;
		case MAIN_CLOCK_16_MHz_FOR_I2C:
			ptrHandlerI2C->ptrI2Cx->CR2 |= (16 << I2C_CR2_FREQ_Pos);
			break;
		case MAIN_CLOCK_20_MHz_FOR_I2C:
			ptrHandlerI2C->ptrI2Cx->CR2 |= (20 << I2C_CR2_FREQ_Pos);
		default:
			__NOP();
			break;
	}

	/*4. COnfigurar el modo I2C*/
	ptrHandlerI2C->ptrI2Cx-> CCR=0;
	ptrHandlerI2C->ptrI2Cx-> TRISE=0;

	switch(ptrHandlerI2C ->I2CConfig.modeI2C){

		case I2C_MODE_SM:
			ptrHandlerI2C->ptrI2Cx-> CCR &= ~I2C_CCR_FS;
			ptrHandlerI2C->ptrI2Cx-> CCR |= (I2C_MODE_SM_SPEED_100KHz << I2C_CCR_CCR_Pos);
			ptrHandlerI2C->ptrI2Cx -> TRISE |= I2C_MAX_RISE_TIME_SM;

			break;
		case I2C_MODE_FM:
			ptrHandlerI2C->ptrI2Cx-> CCR |= I2C_CCR_FS;
			ptrHandlerI2C->ptrI2Cx-> CCR |= (I2C_MODE_SM_SPEED_400KHz << I2C_CCR_CCR_Pos);
			ptrHandlerI2C->ptrI2Cx -> TRISE |= I2C_MAX_RISE_TIME_FM;

		default:
			__NOP();
			break;
	}

	/*5. Activar el modulo I2C*/
	ptrHandlerI2C ->ptrI2Cx->CR1 |= I2C_CR1_PE;

}

/*Comenzar tranmision*/
void i2c_startTransaction(I2C_Handler_t *ptrHandlerI2C){

	/*Verificamos que la linea no esta ocupada*/
	while(ptrHandlerI2C->ptrI2Cx->SR2 & I2C_SR2_BUSY){
		__NOP();
	}

	/*Generar la señal "start"*/

	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_START;

	/* Esperamos que la bandera del evento start se levante
	 * minetras espramos, el valor de SB es 0
	 * */

	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_SB)){
		__NOP();
	}

}

/*reiniciar transmision*/
void i2c_reStartTransaction(I2C_Handler_t *ptrHandlerI2C){
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_START;
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_SB)){
		__NOP();
	}
}
/*Generar condición de stop*/

void i2c_stopTransaction(I2C_Handler_t *ptrHandlerI2C){
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_STOP;
}

/*Activar indicacion para no-ACK(indicacion para el slave de terminar)*/
void i2c_sendNoAck(I2C_Handler_t *ptrHandlerI2C){
	ptrHandlerI2C->ptrI2Cx->CR1 &= ~I2C_CR1_ACK;
}

/*Activar indicacion para ACK*/
void i2c_sendAck(I2C_Handler_t *ptrHandlerI2C){
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_ACK;
}

/*enviar la dirección del esclavo (junto a readorwrite=0 para indicar que vamos a escribir)*/
void i2c_sendSlaveAddressRW(I2C_Handler_t *ptrHandlerI2C, uint8_t slaveAddress, uint8_t readOrWrite){
	uint8_t auxByte=0;
	(void) auxByte;

	/*Enviamos la direccion del Slave Y el bit que indica que desamos escribir*/

	ptrHandlerI2C->ptrI2Cx->DR =(slaveAddress<<1) | readOrWrite;

	/* Esperamos hasta que la bandera del evento "addr" se levante, esto indica que la address se envió correctamente*/

	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_ADDR)){
		__NOP();
	}

	/* Se debe limpiar la bandera de la recepción ACK de la "addr" para lo cual se debe leer
	 * en secuencia primero el I2C_SR1 y luego I2C_SR2*/
	auxByte=ptrHandlerI2C->ptrI2Cx->SR1;
	auxByte=ptrHandlerI2C->ptrI2Cx->SR2;

}

/*enviar direccion de memora*/
void i2c_sendMemoryAddress(I2C_Handler_t *ptrHandlerI2C, uint8_t memAddr){
	/*Enviamos la dirección de memoria que desamos leer*/
	ptrHandlerI2C->ptrI2Cx->DR =memAddr;

	/*Esperamos hasta que el byte sea transmitido */
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_TXE)){
		__NOP();
	}
}

void i2c_sendDataByte(I2C_Handler_t *ptrHandlerI2C, uint8_t dataToWrite){
	/*Cargamos el valor que deseamos escribir*/
	ptrHandlerI2C->ptrI2Cx->DR = dataToWrite;
	/* Esperamos hasta que el byte sea transmitido*/
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_BTF)){
		__NOP();
	}
}

uint8_t i2c_readDataByte(I2C_Handler_t *ptrHandlerI2C){
	/*esperamos hasta que el byte entrate sea recibido*/
	while(!(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_RXNE)){
		__NOP();
	}

	ptrHandlerI2C->I2CConfig.dataI2C=ptrHandlerI2C->ptrI2Cx->DR;
	return ptrHandlerI2C ->I2CConfig.dataI2C;
}

uint8_t i2c_readSingleRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t regToRead){

	/*0. Creamos variable auxiliar para recibir el dato que leemos*/
	uint8_t auxRead=0;
	/*1. Generamos la condición de start*/
	i2c_startTransaction(ptrHandlerI2C);
	/*2. Enviamos la direccion del esclavo y la indicación de escribir*/
	i2c_sendSlaveAddressRW(ptrHandlerI2C,ptrHandlerI2C->I2CConfig.slaveAddress,I2C_WRITE_DATA);
	/*3 Enviamos la dirección de memoria que deseamos leer*/
	i2c_sendMemoryAddress(ptrHandlerI2C, regToRead);
	/*4  Creamos una condición de restart*/
	i2c_reStartTransaction(ptrHandlerI2C);
	/*5. Enviamos la direccióndel esclavo y la indicación de LEER*/
	i2c_sendSlaveAddressRW(ptrHandlerI2C,ptrHandlerI2C->I2CConfig.slaveAddress,I2C_READ_DATA);
	/*6. leemos el dato que envia el esclavo*/
	auxRead=i2c_readDataByte(ptrHandlerI2C);
	/*7. Se genera la condición de NoAck, para que el Master no responda y el slave solo envie 1 byte*/
	i2c_sendNoAck(ptrHandlerI2C);
	/*8. Generamos la donción de estop, para que el slave se detenga despues de 1 byte*/
	i2c_stopTransaction(ptrHandlerI2C);

	return auxRead;
}

void i2c_writeSingleRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t regToRead, uint8_t newValue){

	/*1. generamos la condición start*/
	i2c_startTransaction(ptrHandlerI2C);
	/*2. Enviamos la dirección del esclavo y la indicación de escribir*/
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->I2CConfig.slaveAddress,I2C_WRITE_DATA);
	/*3. Enviamos la dirección de memoria que deseamos escribir*/
	i2c_sendMemoryAddress(ptrHandlerI2C, regToRead);
	/*4. Enviamos el valor que desamos escribir en el registro seleccionado*/
	i2c_sendDataByte(ptrHandlerI2C,newValue);
	/*5. Generamos la condición stop, para que el slave se deternga despues de 1 byte*/
	i2c_stopTransaction(ptrHandlerI2C);
}

