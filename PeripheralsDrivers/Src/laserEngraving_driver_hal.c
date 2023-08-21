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
	motor_enable(pLaser_engraving_t);
	set_velocity(pLaser_engraving_t);
	set_power(pLaser_engraving_t);
}
void motor_enable(laser_engraving_t *plaser_engraving_t){

	gpio_WritePin(plaser_engraving_t->pGIPO_enable);
}


void setvelocity(laser_engraving_t *plaser_engraving_t){
	switch(laser_engraving_t->config.velocity){
	case(LASER_VELOCITY_200Hz):
			break;
	case(LASER_VELOCITY_400Hz):
			break;
	case(LASER_VELOCITY_600Hz):
			break;
	case(LASER_VELOCITY_800Hz):
			break;
	case(LASER_VELOCITY_1000Hz):
			break;
	case(LASER_VELOCITY_1200Hz):
			break;
	case(LASER_VELOCITY_1400Hz):
			break;
	case(LASER_VELOCITY_1600Hz):
			break;
	case(LASER_VELOCITY_1800Hz):
			break;
	case(LASER_VELOCITY_2000Hz):
			break;
	default:

		break;

	}

}
void x_movement(laser_engraving_t *plaser_engraving_t);
void y_movement(laser_engraving_t *plaser_engraving_t);
void set_velocity(laser_engraving_t *plaser_engraving_t);
void set_power(laser_engraving_t *plaser_engraving_t);
void engraving(laser_engraving_t *plaser_engraving_t);
void make_circle(laser_engraving_t *plaser_engraving_t);
void make_square(laser_engraving_t *plaser_engraving_t);
void start_continuous_engraving(laser_engraving_t *plaser_engraving_t);
void stop_continuous_engraving(laser_engraving_t *plaser_engraving_t);
void change_motor_direction(laser_engraving_t *plaser_engraving_t);
