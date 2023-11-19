/*
 * i2c_driver_hal.h
 *
 *  Created on: Nov 6, 2023
 *      Author: mauricio
 */

#ifndef I2C_DRIVER_HAL_H_
#define I2C_DRIVER_HAL_H_
#include <stm32f4xx.h>



enum{
	I2C_WRITE_DATA=0,
	I2C_READ_DATA
};

enum{
	MAIN_CLOCK_4_MHz_FOR_I2C=0,
	MAIN_CLOCK_16_MHz_FOR_I2C,
	MAIN_CLOCK_20_MHz_FOR_I2C
};

enum{
	I2C_MODE_SM=0,
	I2C_MODE_FM
};


enum{
	I2C_MAX_RISE_TIME_SM=0,
	I2C_MAX_RISE_TIME_FM
};

#define I2C_MODE_SM_SPEED_100KHz 80
#define I2C_MODE_SM_SPEED_400KHz 13

#define I2C_MAX_RISE_TIME_SM 17
#define I2C_MAX_RISE_TIME_FM 5

typedef struct
{
	uint8_t				frecMainClock;
	uint8_t				modeI2C;
	uint8_t				slaveAddress;
	uint8_t				dataI2C;

} I2C_Config_t;


typedef struct
{
	I2C_TypeDef			*ptrI2Cx;
	I2C_Config_t		I2CConfig;

}I2C_Handler_t;

/*Prototipos de las funciones públicas*/

void i2c_config(I2C_Handler_t *ptrHandlerI2C);
void i2c_startTransaction(I2C_Handler_t *ptrHandlerI2C);
void i2c_reStartTransaction(I2C_Handler_t *ptrHandlerI2C);
void i2c_sendSlaveAddressRW(I2C_Handler_t *ptrHandlerI2C, uint8_t slaveAddress, uint8_t readOrWrite);
void i2c_sendMemoryAddress(I2C_Handler_t *ptrHandlerI2C, uint8_t memAddr);
void i2c_sendDataByte(I2C_Handler_t *ptrHandlerI2C, uint8_t dataToWrite);
uint8_t i2c_readDataByte(I2C_Handler_t *ptrHandlerI2C);
void i2c_stopTransaction(I2C_Handler_t *ptrHandlerI2C);
void i2c_sendAck(I2C_Handler_t *ptrHandlerI2C);
void i2c_sendNoAck(I2C_Handler_t *ptrHandlerI2C);
uint8_t i2c_readSingleRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t regToRead);
void i2c_writeSingleRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t regToRead, uint8_t newValue);



#endif /* I2C_DRIVER_HAL_H_ */
