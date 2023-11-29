/*
 * laser_engraving_driver.c
 *
 *  Created on: Nov 28, 2023
 *      Author: if401-09
 */


#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32_assert.h"
#include "gpio_driver_hal.h"
#include "pwm_driver_hal.h"
#include "laserEngraving_driver_hal.h"

void laser_init_config(laser_engraving_t *pLaser_engraving_t){

	set_velocity(pLaser_engraving_t,LASER_VELOCITY_200Hz);
	set_power(pLaser_engraving_t,LASER_POWER_1000Hz);
	laser_disable(pLaser_engraving_t);
	set_motor_direction(pLaser_engraving_t);
	motor_disable(pLaser_engraving_t);
}


void set_velocity(laser_engraving_t *pLaser_engraving_t,uint8_t newVelocity){

	// IMPORTANTE, SE TRABAJA CON FRECUENCIA BASE DE 1 MHz, se debe configurar el prescaler del timer en 16

	switch(newVelocity){
	case(LASER_VELOCITY_200Hz):
		updateFrequency(pLaser_engraving_t->pPWM_motor, 5000);
			break;
	case(LASER_VELOCITY_400Hz):
		updateFrequency(pLaser_engraving_t->pPWM_motor, 2500);
			break;
	case(LASER_VELOCITY_600Hz):
		updateFrequency(pLaser_engraving_t->pPWM_motor, 1666);
			break;
	case(LASER_VELOCITY_800Hz):
		updateFrequency(pLaser_engraving_t->pPWM_motor, 1250);
			break;
	case(LASER_VELOCITY_1000Hz):
		updateFrequency(pLaser_engraving_t->pPWM_motor, 1000);
			break;
	case(LASER_VELOCITY_1200Hz):
		updateFrequency(pLaser_engraving_t->pPWM_motor, 833);
			break;
	case(LASER_VELOCITY_1400Hz):
		updateFrequency(pLaser_engraving_t->pPWM_motor, 714);
			break;
	case(LASER_VELOCITY_1600Hz):
		updateFrequency(pLaser_engraving_t->pPWM_motor, 625);
			break;
	case(LASER_VELOCITY_1800Hz):
		updateFrequency(pLaser_engraving_t->pPWM_motor, 555);
			break;
	case(LASER_VELOCITY_2000Hz):
		updateFrequency(pLaser_engraving_t->pPWM_motor, 500);
			break;
	default:
		__NOP();
		break;

	}

}

void set_power(laser_engraving_t *pLaser_engraving_t,uint8_t newPower){
	// IMPORTANTE, SE TRABAJA CON FRECUENCIA BASE DE 1 MHz, se debe configurar el prescaler del timer en 16

	switch(newPower){
	case(LASER_POWER_1000Hz):
		updateFrequency(pLaser_engraving_t->pPWM_motor, 1000);
		updateDuttyCycle(pLaser_engraving_t->pPWM_motor,300);
			break;
	case(LASER_POWER_2000Hz):
		updateFrequency(pLaser_engraving_t->pPWM_motor, 500);
		updateDuttyCycle(pLaser_engraving_t->pPWM_motor,200);
			break;
	case(LASER_POWER_3000Hz):
		updateFrequency(pLaser_engraving_t->pPWM_motor, 333.33);
		updateDuttyCycle(pLaser_engraving_t->pPWM_motor,166);
			break;
	case(LASER_POWER_4000Hz):
		updateFrequency(pLaser_engraving_t->pPWM_motor, 250);
		updateDuttyCycle(pLaser_engraving_t->pPWM_motor,120);
			break;
	case(LASER_POWER_5000Hz):
		updateFrequency(pLaser_engraving_t->pPWM_motor, 200);
		updateDuttyCycle(pLaser_engraving_t->pPWM_motor,140);
			break;
	case(LASER_POWER_6000Hz):
		updateFrequency(pLaser_engraving_t->pPWM_motor, 166);
		updateDuttyCycle(pLaser_engraving_t->pPWM_motor,133);
			break;
	case(LASER_POWER_7000Hz):
		updateFrequency(pLaser_engraving_t->pPWM_motor, 142);
		updateDuttyCycle(pLaser_engraving_t->pPWM_motor,128);
			break;
	case(LASER_POWER_8000Hz):
		updateFrequency(pLaser_engraving_t->pPWM_motor, 125);
		updateDuttyCycle(pLaser_engraving_t->pPWM_motor,120);

			break;
	default:
		__NOP();
		break;

	}

}

void motor_enable(laser_engraving_t *pLaser_engraving_t){
	gpio_WritePin(pLaser_engraving_t->pGIPO_enable_motor,1);
}

void laser_enable(laser_engraving_t *pLaser_engraving_t){
	gpio_WritePin(pLaser_engraving_t->pGIPO_enable_laser,1);
}

void motor_disable(laser_engraving_t *pLaser_engraving_t){
	gpio_WritePin(pLaser_engraving_t->pGIPO_enable_motor,0);
}

void laser_disable(laser_engraving_t *pLaser_engraving_t){
	gpio_WritePin(pLaser_engraving_t->pGIPO_enable_laser,0);
}

void set_motor_direction(laser_engraving_t *pLaser_engraving_t){
	//Para que el cambio de dirección no sea brusco se activa y desactiva el motor, además se agrega tiempo de espera
	motor_disable(pLaser_engraving_t);
	systick_Delay_ms(pLaser_engraving_t->config.time_step);
	if(pLaser_engraving_t->config.direction){
		gpio_WritePin(pLaser_engraving_t->pGIPO_motor_direction,1);

	} else{
		gpio_WritePin(pLaser_engraving_t->pGIPO_motor_direction,0);
	}
	motor_enable(pLaser_engraving_t);
}

void movement(laser_engraving_t *pLaser_engraving_t){

	motor_enable(pLaser_engraving_t);
	startPWMsignal(pLaser_engraving_t->pPWM_laser);
	systick_Delay_ms(pLaser_engraving_t->config.time_step);
	stopPWMSignal(pLaser_engraving_t->pPWM_laser);
	motor_disable(pLaser_engraving_t);
}

void engraving(laser_engraving_t *pLaser_engraving_t){
	laser_enable(pLaser_engraving_t);
	startPWMsignal(pLaser_engraving_t->pPWM_laser);
	systick_Delay_ms(pLaser_engraving_t->config.time_step);
	stopPWMSignal(pLaser_engraving_t->pPWM_laser);
	laser_disable(pLaser_engraving_t);
}

void start_continuous_engraving(laser_engraving_t *pLaser_engraving_t){
	laser_enable(pLaser_engraving_t);
	startPWMsignal(pLaser_engraving_t->pPWM_laser);
}


void stop_continuous_engraving(laser_engraving_t *pLaser_engraving_t){
	stopPWMSignal(pLaser_engraving_t->pPWM_laser);
	laser_disable(pLaser_engraving_t);
}

