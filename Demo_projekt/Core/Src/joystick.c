/*
 * joystick.c
 *
 *  Created on: 23. feb. 2022
 *      Author: marko
 */
#include "joystick.h"

static joystick_t* js;
static ADC_HandleTypeDef* J_hadc;

void joystick_init(joystick_t *joystick, ADC_HandleTypeDef* hadc4)
{
	joystick->raw_min.x = 780;
	joystick->raw_max.x = 3330;
	joystick->raw_min.y = 780;
	joystick->raw_max.y = 3330;
	joystick->out_max.x = 90;
	joystick->out_max.y = 90;

	joystick->offset_correction.x = 0;
	joystick->offset_correction.y = 0;

	joystick->x_k = (float) joystick->out_max.x / (joystick->raw_max.x - joystick->raw_min.x);
	joystick->y_k = (float) joystick->out_max.y / (joystick->raw_max.y - joystick->raw_min.y);
	joystick->n.x = (joystick->raw_max.x+joystick->raw_min.x)/2;
	joystick->n.y = (joystick->raw_max.y+joystick->raw_min.y)/2;

	joystick->sensitivity = 0.35;
	joystick->dead_zone.x = 0.0; // not implemented
	joystick->dead_zone.y = 0.0; // not implemented

	js = joystick;
	J_hadc = hadc4;

	joystick_correct_offset();

}

coord_t joystick_get()
{
	coord_t raw, out;
	HAL_ADC_Start(J_hadc);
	HAL_ADC_PollForConversion(J_hadc, 10); // Waiting for ADC conversion
	raw.x = HAL_ADC_GetValue(J_hadc);

	HAL_ADC_Start(J_hadc);
	HAL_ADC_PollForConversion(J_hadc, 10); // Waiting for ADC conversion
	raw.y = HAL_ADC_GetValue(J_hadc);
	HAL_ADC_Stop(J_hadc);

	out.x = (int) ((js->x_k*(raw.x-js->n.x) + js->offset_correction.x) * js->sensitivity);
	out.y = (int) ((-js->y_k*(raw.y-js->n.y) + js->offset_correction.y) * js->sensitivity);

	return out;
}

void joystick_correct_offset()
{
	coord_t output;
	coord_t raw;

	HAL_ADC_Start(J_hadc);
	HAL_ADC_PollForConversion(J_hadc, 10); // Waiting for ADC conversion
	raw.x = HAL_ADC_GetValue(J_hadc);

	HAL_ADC_Start(J_hadc);
	HAL_ADC_PollForConversion(J_hadc, 10); // Waiting for ADC conversion
	raw.y = HAL_ADC_GetValue(J_hadc);
	HAL_ADC_Stop(J_hadc);

	output.x = js->x_k*(raw.x - js->n.x);
	output.y = -js->y_k*(raw.y - js->n.y);

	js->offset_correction.x = -output.x;
	js->offset_correction.y = -output.y;
}


