/*
 * kbd.h
 *
 *  Created on: Jan 19, 2022
 *      Author: Gasper
 */

#ifndef INCLUDE_KBD_H_
#define INCLUDE_KBD_H_


// ----------- Include other modules (for public) -------------

// Include the hardware-level LL libraries in order to provide the low-level functionality to operate the keyboard.
#include "stm32g4xx_ll_gpio.h"		// support for GPIOs
#include "stm32g4xx_ll_tim.h"		// support for timers


// We will need the following LL functions:
// for GPIOs:
//		- uint32_t LL_GPIO_IsInputPinSet(GPIO_TypeDef *GPIOx, uint32_t PinMask)
//
// for the timer:
//		- void LL_TIM_EnableIT_UPDATE(TIM_TypeDef *TIMx)
//		- void LL_TIM_EnableCounter(TIM_TypeDef *TIMx)




// -------------------- Public definitions --------------------


// What we define in the module header file (i.e. the kbd.h), can be used in other modules. Therefore it makes
// sense to define which keyboard buttons are available in our system, so that we can use this in other modules
// or in the main program.

// We do this by defining a enumerated list of button names.
typedef enum {BTN_OK, BTN_ESC, BTN_UP, BTN_DOWN, BTN_RIGHT, BTN_LEFT, BTN_JOY, NUM_OF_BTNS,    BTN_NONE} buttons_enum_t;

// We have added the "BTN_" prefix to this button names in order to indicate that these names
// are related to buttons.

// Mind that we also have to take into account the situation when no button is pressed.
// Therefore we introduced a special "fictional" button called BTN_NONE.

// We added this fictional button at the end of the list. Note that we can still use the NUM_OF_BTNS
// item to get the number of real available buttons: we just have to put it at the end of the
// list of real buttons. And we put fictional "reserved" items after the NUM_OF_ item.



// Note: the order in which the button names are defined will also define the priority when
// several buttons are pressed simultaneously. Why? Because this will be the order in which
// we check the states of the keyboard buttons.


// We can also add an additional definition which will give information about button signal
// values when button state is on or off. This is going to be useful if we are going to work
// with button states.
typedef enum {BTN_VALUE_ON=0, BTN_VALUE_OFF=1} button_sig_value_t;


// Note the inverted logic: we are using a pull-up resistor and a button to pull the signal
// low when pressed.



// The remaining definitions belong in the source .c file and are internal to the KBD module.



// ---------------- Public function prototypes ----------------


void KBD_init(void);
void KBD_scan(void);

extern void KBD_scan_Callback(void);


buttons_enum_t KBD_get_pressed_key(void);
button_sig_value_t KBD_get_button_state(buttons_enum_t button);


void KBD_demo(void);
void KBD_demo_Timer_IRQ_scan(void);




#endif /* INCLUDE_KBD_H_ */
