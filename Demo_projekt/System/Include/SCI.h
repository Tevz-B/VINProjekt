/*
 * SCI.h
 *
 *  Created on: Jan 21, 2022
 *      Author: Gasper
 */

#ifndef INCLUDE_SCI_H_
#define INCLUDE_SCI_H_


// ----------- Include other modules (for public) -------------

#include "stm32g4xx_ll_usart.h"

// We will need the following LL functions:
//		- LL_USART_TransmitData8(USARTx, Value)
//		- uint8_t LL_USART_ReceiveData8(USARTx)
//		- uint32_t LL_USART_IsActiveFlag_TC (USART_TypeDef * USARTx)
//		- uint32_t LL_USART_IsActiveFlag_RXNE_RXFNE (USART_TypeDef * USARTx)

// From the functions above we can see that the handle structure requires only one parameter
// that specifies which USARTx we will use.



// -------------------- Public definitions --------------------



// ---------------- Public function prototypes ----------------

void SCI_init(void);

void SCI_send_char(char c);
void SCI_send_byte(uint8_t data);

uint8_t SCI_is_data_waiting(void);

int SCI_read_char(char *c);
int SCI_read_byte(uint8_t *data);


void SCI_send_string(char *str);
void SCI_send_bytes(uint8_t *data, uint32_t size);

void SCI_send_string_IT(char *str);
void SCI_send_bytes_IT(uint8_t *data, uint32_t size);

void SCI_receive_char_Callback(void);
void SCI_transmit_char_Callback(void);



void SCI_demo_Hello_world(void);
void SCI_demo_echo_with_polling(void);
void SCI_demo_Hello_world_printf(void);

void SCI_demo_receive_with_interrupts(void);
void SCI_demo_transmit_with_interrupts(void);
void SCI_demo_echo_with_interrupts(void);


#endif /* INCLUDE_SCI_H_ */
