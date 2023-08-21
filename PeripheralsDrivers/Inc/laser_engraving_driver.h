/*
 * pwm_driver_hal.h
 *
 *  Created on: Nov 6, 2023
 *      Author: mauricio
 */

#ifndef LASER_ENGRAVING_DRIVER_H_
#define LASER_ENGRAVING_DRIVER_H_

#include "stm32f4xx.h"
#include "pwm_driver_hal.h"

enum{
	LASER_VELOCITY_200Hz=0,
	LASER_VELOCITY_400Hz,
	LASER_VELOCITY_600Hz,
	LASER_VELOCITY_800Hz,
	LASER_VELOCITY_1000Hz,
	LASER_VELOCITY_1200Hz,
	LASER_VELOCITY_1400Hz,
	LASER_VELOCITY_1600Hz,
	LASER_VELOCITY_1800Hz,
	LASER_VELOCITY_2000Hz
};

enum{
	LASER_POWER_1000Hz=0,
	LASER_POWER_2000Hz,
	LASER_POWER_3000Hz,
	LASER_POWER_4000Hz,
	LASER_POWER_5000Hz,
	LASER_POWER_6000Hz,
	LASER_POWER_7000Hz,
	LASER_POWER_8000Hz
};

enum{
	MOTOR_Y=0,
	MOTOR_X,
};

enum{
	LASER_ENABLE=0,
	LASER_DISABLE,
};

enum{
	DIRECTION_1=0,
	DIRECTION_2,
};



typedef struct
{
	uint16_t	laser_power;
	uint16_t	velocity;
	uint16_t	motor;
	uint8_t		laser_state;
	uint8_t		direction;

	} laser_engraving_Config_t;

typedef struct
{
	PWM_Handler_t	 	*pPWM_motor;
	PWM_Handler_t	 	*pPWM_laser;
	GPIO_Handler_t		*pGIPO_enable;
	laser_engraving_Config_t 	config;
} 	laser_engraving_t;

void laser_init_config(laser_engraving_t *laser_engraving_t);
void motor_enable(laser_engraving_t *laser_engraving_t);
void x_movement(laser_engraving_t *laser_engraving_t);
void y_movement(laser_engraving_t *laser_engraving_t);
void set_velocity(laser_engraving_t *laser_engraving_t);
void set_power(laser_engraving_t *laser_engraving_t);
void engraving(laser_engraving_t *laser_engraving_t);
void make_circle(laser_engraving_t *laser_engraving_t);
void make_square(laser_engraving_t *laser_engraving_t);
void start_continuous_engraving(laser_engraving_t *laser_engraving_t);
void stop_continuous_engraving(laser_engraving_t *laser_engraving_t);
void change_motor_direction(laser_engraving_t *laser_engraving_t);




#endif /*LASER_ENGRAVING_DRIVER_H_ */

