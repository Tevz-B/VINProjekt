/*
 * LED.c
 *
 *  Created on: Jan 18, 2022
 *      Author: Gasper
 */

/* **************** MODULE DESCRIPTION *************************

This module implements the system LEDs based on the LEDs connected
to the GPIOs. The LEDs can be controlled individually or as an
ordered group.

************************************************************* */



// ----------- Include other modules (for private) -------------

#include "LED.h"



// ---------------------- Private definitions ------------------


// Here we must first define a handle structure that will help us control the individual LED.
// According to the LL reference manual, in order control the GPIO, you need to specify
// the pin port and number.
// Example: void LL_GPIO_SetOutputPin(GPIO_TypeDef *GPIOx, uint32_t PinMask)
typedef struct
{
	GPIO_TypeDef 	*port;
	uint32_t 		pin;

} LED_handle_t;


// Now we have a handle structure for a single LED. But our system contains more than one LED and
// they are organized in some kind of LED array / strip / row - into an ordered LED series. Therefore
// it would make sense to consider this ordered LED series as a system device. And this means
// that we must also prepare a handle structure for this device.

// In other words: we have a device called "LED series". And this device consists of several individual LEDs
// being organized into an ordered group.

// Let us define the handle structure for the LED series.
// The "LED series" handle should contain handles to all individual LEDs in the series.
// And since we have more than one LED in a string, it makes sense to organize all the LED handles into an array.

typedef struct
{
	LED_handle_t 	LEDs[NUM_OF_LEDS];		// Also note how we can use a reserved enumerated value to get the required array length.

	// Each array element above is a handle for one LED.

} LED_series_handle_t;



// And now we will create an actual "LED series" handle variable that we will use to operate the "LED series".
LED_series_handle_t		LED_series;


// Note that we defined the "LED series" handle inside the .c source file in order to make it local to the LED module.
// And we will then initialize this handle inside the _init() function.

// And one more thing: note the bottom-up approach for defining the system device: 
// we start with the individual LED handles and then use them to define the LED series.



// -------------- Public function implementations --------------


void LED_init(void)
{
	// 1. Initialize the individual LED handles

		// "Connect" the system LEDs to the GPIO pins and ports
		// by correctly initializing the individual LED handles.
		// Use GPIOn and LL_GPIO_PIN_n LL macros.

		// LEDs on port F
		LED_series.LEDs[LED0].pin = LL_GPIO_PIN_2;
		LED_series.LEDs[LED0].port = GPIOF;

		LED_series.LEDs[LED1].pin = LL_GPIO_PIN_3;
		LED_series.LEDs[LED1].port = GPIOF;

		LED_series.LEDs[LED2].pin = LL_GPIO_PIN_4;
		LED_series.LEDs[LED2].port = GPIOF;

		LED_series.LEDs[LED3].pin = LL_GPIO_PIN_5;
		LED_series.LEDs[LED3].port = GPIOF;

		// LEDs on port C
		LED_series.LEDs[LED4].pin = LL_GPIO_PIN_0;
		LED_series.LEDs[LED4].port = GPIOC;

		LED_series.LEDs[LED5].pin = LL_GPIO_PIN_1;
		LED_series.LEDs[LED5].port = GPIOC;

		LED_series.LEDs[LED6].pin = LL_GPIO_PIN_2;
		LED_series.LEDs[LED6].port = GPIOC;

		LED_series.LEDs[LED7].pin = LL_GPIO_PIN_3;
		LED_series.LEDs[LED7].port = GPIOC;



	// 2. Set LED default states

		// Put all LEDs in their default "off" state. Use the LEDs_write() function.
		LEDs_write(0x00);

}







// ------- Controlling a single LED ------------

// Note how the enumerated type LEDs_enum_t can be used to specify
// which LED we are controlling. This value is used as an index
// for the array of LED handles.


void LED_on(LEDs_enum_t LEDn)
{
	LL_GPIO_SetOutputPin( LED_series.LEDs[LEDn].port, LED_series.LEDs[LEDn].pin );
}


void LED_off(LEDs_enum_t LEDn)
{
	LL_GPIO_ResetOutputPin( LED_series.LEDs[LEDn].port, LED_series.LEDs[LEDn].pin );
}


void LED_toggle( LEDs_enum_t LEDn )
{
	LL_GPIO_TogglePin( LED_series.LEDs[LEDn].port, LED_series.LEDs[LEDn].pin );
}



// Set the LED state according to the given parameter.
void LED_set(LEDs_enum_t LEDn, uint8_t state)
{
	if (state)
		LL_GPIO_SetOutputPin( LED_series.LEDs[LEDn].port, LED_series.LEDs[LEDn].pin);
	else
		LL_GPIO_ResetOutputPin( LED_series.LEDs[LEDn].port, LED_series.LEDs[LEDn].pin);
}





// ------ Controlling LEDs as an ordered group - the series  ------------
//
// The LED group can be controlled using a bitmask or by directly
// applying the given value to define the LED states.


// ---- Using bitmasks ------


void LEDs_on(uint8_t LED_bitmask)
{
	uint8_t bitmask = 0x01;

	for(uint8_t i=0; i < NUM_OF_LEDS; i++)
	{
		if ( LED_bitmask & bitmask )
			LL_GPIO_SetOutputPin( LED_series.LEDs[i].port, LED_series.LEDs[i].pin );

		bitmask <<= 1;
	}
}


void LEDs_off(uint8_t LED_bitmask)
{
	uint8_t bitmask = 0x01;

	for(uint8_t i=0; i < NUM_OF_LEDS; i++)
	{
		if ( LED_bitmask & bitmask )
			LL_GPIO_ResetOutputPin( LED_series.LEDs[i].port, LED_series.LEDs[i].pin );

		bitmask <<= 1;
	}
}





// --- Using values -----

void LEDs_write(uint8_t value)
{
	uint8_t bitmask = 0x01;

	for(uint8_t i=0; i < NUM_OF_LEDS; i++)
	{
		if (value & bitmask)
			LL_GPIO_SetOutputPin( LED_series.LEDs[i].port, LED_series.LEDs[i].pin );
		else
			LL_GPIO_ResetOutputPin( LED_series.LEDs[i].port, LED_series.LEDs[i].pin );

		bitmask <<= 1;
	}
}





// ---------- Getting information about LED states --------

uint8_t LED_is_on(LEDs_enum_t LEDn)
{
	return LL_GPIO_IsOutputPinSet( LED_series.LEDs[LEDn].port, LED_series.LEDs[LEDn].pin );
}


uint8_t LEDs_read(void)
{
	uint8_t value = 0;
	uint8_t bitmask = 0x01;

	for(uint8_t i=0; i < NUM_OF_LEDS; i++)
	{
		if ( LL_GPIO_IsOutputPinSet( LED_series.LEDs[i].port, LED_series.LEDs[i].pin ) )
			value |= bitmask;

		bitmask <<= 1;
	}

	return value;
}






// ------- Test functions ---------

// An LED demonstration/test function.

void LED_demo(void)
{
	// use LEDs to display an 8-bit counter using "value control"
	for(uint16_t i=0; i<256; i++)
	{
			LEDs_write(i);
			HAL_Delay(15);
	}


	// all LEDs on then off using "value control"
	LEDs_write(0xFF);
	HAL_Delay(1000);
	LEDs_write(0x00);
	HAL_Delay(1000);


	// toggle one LED
	for(uint8_t i=0; i<3; i++)
	{
		LED_toggle(LED0);
		HAL_Delay(1000);
	}


	// controling the LED series with "bitmask control"
	for(uint8_t i=0; i<3; i++)
	{

		LEDs_on(0b01010101);
		HAL_Delay(1000);
		LEDs_off(0b01010101);
		HAL_Delay(1000);
	}


	// reading a single LED state
	for(uint8_t i=0; i<3; i++)
	{

		LED_toggle(LED0);

		if ( LED_is_on(LED0) )
			LED_on(LED7);
		else
			LED_off(LED7);

		HAL_Delay(1000);
	}


	// reading states of all LEDs
	LEDs_write(0x0F);

	HAL_Delay(1000);

	if ( LEDs_read() == 0x0F )
		LEDs_on(0xF0);
	else
		LEDs_off(0xF0);

}






// -------------- Private function implementations -------------

