/*
 * LED.h
 *
 *  Created on: Jan 18, 2022
 *      Author: Gasper
 */

#ifndef INCLUDE_LED_H_
#define INCLUDE_LED_H_


// ----------- Include other modules (for public) -------------

// Include the hardware-level LL libraries in order to provide the low-level functionality to operate LEDs.
#include "stm32g4xx_ll_gpio.h"

// We will need the following LL functions:
//		- void LL_GPIO_SetOutputPin(GPIO_TypeDef *GPIOx, uint32_t PinMask)
//		- void LL_GPIO_ResetOutputPin(GPIO_TypeDef *GPIOx, uint32_t PinMask)
// 		- void LL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint32_t PinMask)
//		- uint32_t LL_GPIO_IsOutputPinSet(GPIO_TypeDef *GPIOx, uint32_t PinMask)



// -------------------- Public definitions --------------------


// What we define in the module header file (i.e. the LED.h), can be used in other modules. Therefore it makes
// sense to define which LEDs are available in our system, so that we can use this in other modules or in the
// main program.

// We will not only define the available LEDs, but will also define the ordered LED group - the LED series at the same time.

// We define the ordered LED series by specifying the names of the LEDs in a desired order (LSB LED to MSB LED).
// We can use the enumerated list to do this. Also, this list can be used elegantly to access the LED handles
// for an individual LEDs (handles are defined in the source .c file).
typedef enum {LED0, LED1, LED2, LED3, LED4, LED5, LED6, LED7, NUM_OF_LEDS} LEDs_enum_t;

// Note how we can add one additional enumerated value at the end of the list which then holds the number of all the
// elements in the list.

// We will use the definition above in order to specify which LED we want to control or to control
// all LEDs at the same time as an ordered group.

// The remaining definitions belong in the source .c file and are internal to the LED module.




// ---------------- Public function prototypes ----------------

// We are going to implement functions that control a single LED or all LEDs as an ordered group - the series.
// We will also implement function that provide information about LED current states.
// At the end, we will test the LED functions in a demonstration.


void LED_init(void);

void LED_on(LEDs_enum_t LEDn);		// Note how we use the predefined LEDs_enum_t type to specify which LED we want to control.
void LED_off(LEDs_enum_t LEDn);
void LED_toggle(LEDs_enum_t LEDn);
void LED_set(LEDs_enum_t LEDn, uint8_t state);

void LEDs_on(uint8_t LED_bitmask);
void LEDs_off(uint8_t LED_bitmask);

uint8_t LED_is_on(LEDs_enum_t LEDn);

void LEDs_write(uint8_t value);
uint8_t LEDs_read(void);


void LED_demo(void);




#endif /* INCLUDE_LED_H_ */
