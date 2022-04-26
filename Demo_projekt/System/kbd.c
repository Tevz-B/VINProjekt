/*
 * kbd.c
 *
 *  Created on: Jan 19, 2022
 *      Author: Gasper
 */

/* **************** MODULE DESCRIPTION *************************

This module implements a system keyboard based on the available buttons.
The module implements keyboard scanning, detecting pressed buttons and
saving the information about pressed buttons in a buffer.

************************************************************* */


// ----------- Include other modules (for private) -------------


#include "LED.h"		// include only for keyboard demo


// ---------------------- Private definitions ------------------



// -------- Keyboard definitions ----------

#include "kbd.h"


// Here we must first define a handle structure that will help us read the individual buttons.
// According to the LL reference manual, in order read the GPIO, you need to specify
// the pin port and number.
//
// Example: uint32_t LL_GPIO_IsInputPinSet (GPIO_TypeDef * GPIOx, uint32_t PinMask)
//
// We will also need to have information about the button state. And if we want to
// detect the actual pressing of the button, we will also need to know the previous
// state of the button.
typedef struct
{
	GPIO_TypeDef 	*port;
	uint32_t 		pin;
	uint8_t			state_new;
	uint8_t			state_old;

} button_handle_t;



// All the individual buttons form a system device called the keyboard. Therefore it makes sense
// to define the keyboard handle structure. The keyboard handle structure will contain
// handles to individual buttons and now we will add another handle to the timer that will help us
// implement automatic keyboard scanning.


// When we decide to include a timer into the keyboard functionality,
// we also need to store additional information about the microcontroller periphery:
// which timer is going to be used for triggering the keyboard scan.

// We simply need to extend the keyboard_handle_t structure definition
// and add a timer handle.
typedef struct
{
	// button handles
	button_handle_t 	buttons[NUM_OF_BTNS];

	// timer handle
	TIM_TypeDef 		*timer;

} keyboard_handle_t;



// And now we will create an actual keyboard handle variable that we will use to operate the keyboard.
keyboard_handle_t 		keyboard;


// Note that we defined the keyboard handle inside the .c source file in order to make it local to the keyboard kbd.c module.
// And we will then initialize this handle inside the _init() function.

// And one more thing: note the bottom-up approach for defining the system device:
// we start with the individual button handles and then use them to define the keyboard.
// And later we simply upgrade our existing keyboard device by adding the timer functionality.







// -------- Keyboard buffer definitions ----------

// We must also prepare the data structures that will implement the keyboard buffer.

// First, include a circular buffer module to help us implement the keyboard buffer,
// where the information about the pressed buttons will be kept until processed.
#include "buf.h"

// Define the length of the keyboard buffer.
#define KBD_BUF_LEN 	32

// And define the data structures required to implement the circular buffer:
uint8_t kbd_buffer[KBD_BUF_LEN];	// the buffer data array
BUF_handle_t kbd_buf_handle;		// the buffer handle structure






// -------------- Public function implementations --------------


void KBD_init(void)
{
	// 1. Initialize the button handles on the hardware-level

		// "Connect" the system buttons to the GPIO pins and ports
		// by correctly initializing the individual button handles.
		// Use GPIOn and LL_GPIO_PIN_n LL macros.

		// buttons on port C
		keyboard.buttons[BTN_OK].pin = LL_GPIO_PIN_15;
		keyboard.buttons[BTN_OK].port = GPIOC;

		keyboard.buttons[BTN_ESC].pin = LL_GPIO_PIN_14;
		keyboard.buttons[BTN_ESC].port = GPIOC;


		// buttons on port G
		keyboard.buttons[BTN_UP].pin = LL_GPIO_PIN_0;
		keyboard.buttons[BTN_UP].port = GPIOG;

		keyboard.buttons[BTN_DOWN].pin = LL_GPIO_PIN_1;
		keyboard.buttons[BTN_DOWN].port = GPIOG;

		keyboard.buttons[BTN_LEFT].pin = LL_GPIO_PIN_6;
		keyboard.buttons[BTN_LEFT].port = GPIOG;

		keyboard.buttons[BTN_RIGHT].pin = LL_GPIO_PIN_8;
		keyboard.buttons[BTN_RIGHT].port = GPIOG;

		keyboard.buttons[BTN_JOY].pin = LL_GPIO_PIN_13;
		keyboard.buttons[BTN_JOY].port = GPIOC;

	// 2. initialize the default handle values on the system level
		for(int i=0; i < NUM_OF_BTNS; i++)
		{
			keyboard.buttons[i].state_new = BTN_VALUE_OFF;		// default button state is high due to the pull-up
			keyboard.buttons[i].state_old = BTN_VALUE_OFF;		// default button state is high due to the pull-up
		}


	// 3. initialize the keyboard buffer
		BUF_init( &kbd_buf_handle, kbd_buffer, KBD_BUF_LEN);


	// 4. initialize the timer used to trigger keyboard scanning
		keyboard.timer = TIM6;						// define the timer used for keyboard scanning

		LL_TIM_EnableIT_UPDATE(keyboard.timer);		// enable the timer update interrupt
		LL_TIM_EnableCounter(keyboard.timer);		// start the timer

}



// Scan the current states of the keyboard buttons, determine which buttons
// were recently pressed and store this information in a keyboard buffer.
void KBD_scan(void)
{
	// For each button, read the new state and determine if pressed.
	// If pressed, save that information in the keyboard buffer.
	// Note that the order in which the buttons are scanned for the pressing is the
	// order in which the button names were defined in BTNs_enum_t type. Hence, this
	// order also defines the button priority.
	for(int i=0; i < NUM_OF_BTNS; i++)
	{
		// The new state becomes the old state
		keyboard.buttons[i].state_old = keyboard.buttons[i].state_new;

		// and the new state is read from the GPIO pin.
		keyboard.buttons[i].state_new = LL_GPIO_IsInputPinSet( keyboard.buttons[i].port, keyboard.buttons[i].pin );

		// If the old state is BTN_VALUE_OFF and the new state is BTN_VALUE_ON, then a change occurred and
		// the button was pressed.
		if (  ( keyboard.buttons[i].state_old == BTN_VALUE_OFF) && ( keyboard.buttons[i].state_new == BTN_VALUE_ON ) )
		{

			// Save this information in the keyboard buffer.
			// We save the enumerated value that corresponds to the button pressed.
			BUF_store_byte(&kbd_buf_handle, i);

		}

	}

}


// Returns the information about the next pressed key waiting
// to be processed.
buttons_enum_t KBD_get_pressed_key(void)
{
	// Auxiliary variable to help us prepare the return value.
	buttons_enum_t pressed_button;

	if ( BUF_get_byte(&kbd_buf_handle, &pressed_button) == BUFFER_OK )
		return pressed_button;
	else
		return BTN_NONE;

}


// Returns the current state of the button.
button_sig_value_t KBD_get_button_state(buttons_enum_t button)
{
	return keyboard.buttons[button].state_new;
}





// ------- Interrupt callbacks  ---------


inline void KBD_scan_Callback(void)
{
	KBD_scan();
}







// ------- Test functions ---------



void KBD_demo(void)
{

	buttons_enum_t pressed_key;


	// scan keyboard buttons
	KBD_scan();

	// get next pressed button
	pressed_key = KBD_get_pressed_key();


	// read all pressed keys from the keyboard buffer
	while (pressed_key != BTN_NONE)
	{
		// toggle LEDs according to pressed keys
		switch (pressed_key)
		{
		  case BTN_OK:
			  LED_toggle(LED0);
			  break;

		  case BTN_ESC:
			  LED_toggle(LED1);
			  break;

		  case BTN_UP:
			  LED_toggle(LED2);
			  break;

		  case BTN_DOWN:
			  LED_toggle(LED3);
			  break;

		  case BTN_LEFT:
			  LED_toggle(LED4);
			  break;

		  case BTN_RIGHT:
			  LED_toggle(LED5);
			  break;

		  default:
			  break;
		}

		// get next pressed button
		pressed_key = KBD_get_pressed_key();

	}

	// test get button state
	LED_set(LED7, KBD_get_button_state(BTN_OK));


	// scan keyboard period
	HAL_Delay(10);

}








void KBD_demo_Timer_IRQ_scan(void)
{

	buttons_enum_t pressed_key;


	// NO NEED TO scan keyboard buttons! It is done during Timer IRQ routine.
	// KBD_scan();

	// get next pressed button
	pressed_key = KBD_get_pressed_key();


	// read all pressed keys from the keyboard buffer
	while (pressed_key != BTN_NONE)
	{
		// toggle LEDs according to pressed keys
		switch (pressed_key)
		{
		  case BTN_OK:
			  LED_toggle(LED0);
			  break;

		  case BTN_ESC:
			  LED_toggle(LED6);
			  break;

		  case BTN_UP:
			  LED_toggle(LED5);
			  break;

		  case BTN_DOWN:
			  LED_toggle(LED2);
			  break;

		  case BTN_LEFT:
			  LED_toggle(LED4);
			  break;

		  case BTN_RIGHT:
			  LED_toggle(LED3);
			  break;

		  case BTN_JOY:
			  LED_toggle(LED7);
			  break;

		  default:
			  break;
		}

		// get next pressed button
		pressed_key = KBD_get_pressed_key();

	}

	// test get button state
	LED_set(LED7, KBD_get_button_state(BTN_OK));

}







// -------------- Private function implementations -------------


