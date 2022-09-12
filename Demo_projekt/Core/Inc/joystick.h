/*
 * joystick.h
 *
 *  Created on: 23. feb. 2022
 *      Author: marko
 */

#ifndef INC_JOYSTICK_H_
#define INC_JOYSTICK_H_

#include <sys/_stdint.h>

#include "main.h"


typedef struct
{
	int x;
	int y;
} coord_t;

typedef struct
{
	coord_t raw_min;
	coord_t raw_max;
	coord_t out_max;
	float x_k,y_k;
	coord_t n;
	coord_t offset_correction;
	float sensitivity;
	coord_t dead_zone;

} joystick_t;

void joystick_init(joystick_t *joystick, ADC_HandleTypeDef* hadc4);
coord_t joystick_get();
void joystick_correct_offset();

#endif /* INC_JOYSTICK_H_ */
