/*
 * SCI.c
 *
 *  Created on: Jan 21, 2022
 *      Author: Gasper
 */

/* **************** MODULE DESCRIPTION *************************

This module implements the system Serial Communication Interface (SCI)
which provides a serial connection to the PC via the USART periphery.

************************************************************* */




// ----------- Include other modules (for private) -------------
#include "SCI.h"

#include <stdio.h>			// add printf() functionality
#include "buf.h"			// add buffer to SCI



// ---------------------- Private definitions ------------------


// ------ SCI definitions -------

// Define the handle structure that we will use to operate the SCI.
// We only need to specify which USART instance is going to be used.
typedef struct
{
	USART_TypeDef *USART;
} SCI_handle_t;


// And now define the private SCI handle variable.
SCI_handle_t SCI;


// Also, let's define the SCI return codes that will help us with error
// reports for functions dealing with receiving or transmitting data.
typedef enum { SCI_ERROR=-1, SCI_NO_ERROR=0} SCI_rtrn_codes_t;





// ------ printf() customization definitions -----

// Declare the customized _write() function used by printf() to print strings.
int _write(int file, char *ptr, int len);




// ------ SCI receive (RX) buffer definitions -------

// We must also prepare the data structures that will implement the SCI buffer.

// Define the length of the SCI buffer.
#define SCI_RX_BUF_LEN 	500

// And define the data structures required to implement the circular buffer:
uint8_t SCI_RX_buffer[SCI_RX_BUF_LEN];	// the buffer data array
BUF_handle_t SCI_RX_buf_handle;			// the buffer handle structure



// ------ SCI transmit (TX) buffer definitions -------

// We must also prepare the data structures that will implement the SCI buffer.

// Define the length of the SCI buffer.
#define SCI_TX_BUF_LEN 	500

// And define the data structures required to implement the circular buffer:
uint8_t SCI_TX_buffer[SCI_TX_BUF_LEN];	// the buffer data array
BUF_handle_t SCI_TX_buf_handle;			// the buffer handle structure







// -------------- Public function implementations --------------

void SCI_init(void)
{
	// 1. Specify which USARTx instance is going to be used to implement the SCI.
		SCI.USART = USART3;

	// 2. printf() customization
		// Initialize the printf() buffer to zero length
		// to achieve the best response time.
		setvbuf(stdout, NULL, _IONBF, 0);

		// Note: use setvbuf to specify what kind of buffering you want for the file or stream.
		// Use the predefined mode argument:
		// _IONBF -> Do not use a buffer: send output directly to the host system for the file or
		// stream identified by fp.


	// 3. Initialize the SCI buffers (RX and TX)
		BUF_init( &SCI_RX_buf_handle, SCI_RX_buffer, SCI_RX_BUF_LEN);
		BUF_init( &SCI_TX_buf_handle, SCI_TX_buffer, SCI_TX_BUF_LEN);


    // 4. Initialize the USART interrupts
		LL_USART_EnableIT_RXNE_RXFNE(SCI.USART);

		// Mind that the RXNE IRQ is always on, while the TX IRQ should be enabled
		// only if there is some data to be transmitted.

}



// ------ Sending one byte -------

// Send one character (useful when working with text strings).
void SCI_send_char(char c)
{
	// make sure that the previous transmission is complete
	while( !LL_USART_IsActiveFlag_TC(SCI.USART) );

	// and then send the character
	LL_USART_TransmitData8(SCI.USART, c);

}


// Send one byte (useful when working with binary data).
void SCI_send_byte(uint8_t data)
{
	// make sure that the previous transmission is complete
	while( !LL_USART_IsActiveFlag_TC(SCI.USART) );

	// and then send the byte
	LL_USART_TransmitData8(SCI.USART, data);
}




// ------ Reading one byte -------


uint8_t SCI_is_data_waiting(void)
{
	return ( (uint8_t) LL_USART_IsActiveFlag_RXNE_RXFNE (SCI.USART));
}



int SCI_read_char(char *c)
{
	if ( SCI_is_data_waiting() )
	{
		// save the received character in the provide destination
		*c = (char) LL_USART_ReceiveData8(SCI.USART);

		return SCI_NO_ERROR;	// data was available -> no error
	}
	else
		return SCI_ERROR;		// no data // data was available -> error!

}


int SCI_read_byte(uint8_t *data)
{
	if ( SCI_is_data_waiting() )
		{
			// save the received character in the provide destination
			*data = (uint8_t) LL_USART_ReceiveData8(SCI.USART);

			return SCI_NO_ERROR;	// data was available -> no error
		}
		else
			return SCI_ERROR;		// no data // data was available -> error!
}









// ------ Sending more bytes -------

// Send a string of characters.
void SCI_send_string(char *str)
{
	uint32_t i = 0;

	// send character by character until "0" value is reached
	while(str[i] != 0)
	{
		SCI_send_char( str[i] );
		i++;
	}


}

// Send several bytes from a given location
void SCI_send_bytes(uint8_t *data, uint32_t size)
{
	for(uint32_t i=0; i < size ; i++ )
	{
		SCI_send_byte( data[i] );
	}
}





// ------ Sending more bytes using interrupts -------


void SCI_send_string_IT(char *str)
{
	uint32_t i = 0;
	uint8_t c;

	// Store all of the string characters into the SCI TX buffer first
	while( str[i] != 0)		// keep storing characters until null character
	{
		BUF_store_byte( &SCI_TX_buf_handle, (uint8_t)str[i] );
		i++;
	}

	// If there is at least 1 character to be sent,
	if ( i > 0 )
	{
		// check if we can send the first character right away
		if ( LL_USART_IsActiveFlag_TC(SCI.USART) )		// transfer complete
		{
			// send the first character
			BUF_get_byte( &SCI_TX_buf_handle, &c );
			LL_USART_TransmitData8(SCI.USART, c);	// Note: writing to USART TDR register clears the TC flag.
		}


		// Also enable the USART TX complete interrupt request
		// to let the IRQ routine send the rest of the characters.
		LL_USART_EnableIT_TC(SCI.USART);

	}

}



void SCI_send_bytes_IT(uint8_t *data, uint32_t size)
{
	// temporary one-byte data storage
	uint8_t	d;

	// store all the data in the TX buffer
	for(uint32_t i=0; i < size ; i++ )
	{
		BUF_store_byte( &SCI_TX_buf_handle, data[i] );
	}


	// If there is at least 1 byte to be sent,
	if ( size > 0 )
	{
		// check if we can send the first byte right away
		if ( LL_USART_IsActiveFlag_TC(SCI.USART) )		// transfer complete
		{
			// send the first byte
			BUF_get_byte( &SCI_TX_buf_handle, &d );
			LL_USART_TransmitData8(SCI.USART, d);	// Note: writing to USART TDR register clears the TC flag.
		}


		// Also enable the USART TX complete interrupt request
		// to let the IRQ routine send the rest of the bytes.
		LL_USART_EnableIT_TC(SCI.USART);

	}

}











// ------- Interrupt callbacks  ---------

void SCI_receive_char_Callback(void)
{
	// Store the newly received byte into the SCI RX buffer.
	BUF_store_byte( &SCI_RX_buf_handle, (char) LL_USART_ReceiveData8(SCI.USART) );
}



void SCI_transmit_char_Callback(void)
{
	uint8_t c;

	// try to get the next byte from the SCI TX buffer
	if ( BUF_get_byte( &SCI_TX_buf_handle, &c ) == BUFFER_OK )
		LL_USART_TransmitData8(SCI.USART, c);	// and send it via USART

	// check if there is no data left in the buffer
	if ( BUF_get_data_size( &SCI_TX_buf_handle ) == 0)
		LL_USART_DisableIT_TC(SCI.USART);	// and disable the TC IRQ in that case

}







// ------- Test functions ---------


void SCI_demo_Hello_world(void)
{

	char text[32] = "Hello world!!!\n";

	SCI_send_string(text);

	HAL_Delay(1000);
}



void SCI_demo_echo_with_polling(void)
{
	#define N_MAX		50

	char c;

	float a_0 = 0.5;
	float a_n;
	float sum = 0;

	while(1)
	{
		// poll for new received character
		if ( SCI_read_char(&c) == SCI_NO_ERROR )
			SCI_send_char(c);	// and send it back (echo)


		// Simulate "microcontroller busy" by calculating the geometric sum.
		// Vary the N_MAX parameter and see the effect on the echo functionality.
		// Also try commenting this section.

		a_n = a_0;
		for(uint32_t n=0; n<N_MAX; n++)
		{
			 sum = sum + a_n;
			 a_n = a_n * a_0;
		}


	}

}




void SCI_demo_Hello_world_printf(void)
{

	for(int i=0; i<100; i++)
	{
		// Demonstrate the SCI formatted text functionality added by the
		// customization of the printf() function.
		printf("%2d : Hello printf() world!\n", i);

		HAL_Delay(1000);
	}


}





void SCI_demo_receive_with_interrupts(void)
{

	uint8_t data;

	while(1)
	{
		// Check if any data in the SCI RX buffer.
		if ( BUF_get_data_size( &SCI_RX_buf_handle) )
		{
			// read the data
			BUF_get_byte( &SCI_RX_buf_handle, &data );

			// and send it back in blocking mode
			SCI_send_byte(data);

		}
	}

}



void SCI_demo_transmit_with_interrupts(void)
{

	char message[100] = "This was sent using USART interrupts!\n";

	while(1)
	{

		SCI_send_string_IT(message);

		HAL_Delay(1000);

	}

}





void SCI_demo_echo_with_interrupts(void)
{



	uint8_t message[SCI_RX_BUF_LEN];
	uint32_t message_size;


	while(1)
	{


		// Check if any data in the SCI RX buffer.
		message_size = BUF_get_data_size( &SCI_RX_buf_handle);
		if ( message_size > 0 )
		{
			// store received bytes in the message
			BUF_get_bytes( &SCI_RX_buf_handle, message, message_size );

			// and transmit the message in the IRQ mode
			SCI_send_bytes_IT(message, message_size);

		}


		// Simulate "microcontroller busy" with delays.
		HAL_Delay(1);


		// Simulate "microcontroller busy" by calculating the geometric sum.
		// Vary the N_MAX parameter and see the effect on the echo functionality.
		// Also try commenting this section.


		/*

		#define N_MAX		200

		float a_0 = 0.5;
		float a_n;
		float sum = 0;

		a_n = a_0;
		for(uint32_t n=0; n<N_MAX; n++)
		{
			 sum = sum + a_n;
			 a_n = a_n * a_0;
		}
		*/


	}


}












// -------------- Private function implementations -------------



// ----------- printf() customization ----------

// Implement a customized _write() function for the printf() function
int _write(int file, char *ptr, int len)
{
	SCI_send_bytes( (uint8_t*)ptr, (uint32_t) len);

	return len;
}



