#ifndef _BSP_USART_H
#define _BSP_USART_H

#include "gd32f4xx.h"
#include "systick.h"

#define BSP_USART_TX_RCU RCU_GPIOA//PA9
#define BSP_USART_RX_RCU RCU_GPIOA//PA10
#define BSP_USART_TX_PIN GPIO_PIN_9
#define BSP_USART_RX_PIN GPIO_PIN_10
#define BSP_USART_TX_PORT GPIOA
#define BSP_USART_RX_PORT GPIOA
#define BSP_USART_AF GPIO_AF_7

#define USART_RECIVE_LENGTH 1024

extern uint8_t recive_buff[USART_RECIVE_LENGTH];
extern uint16_t recive_length ;
extern uint8_t recive_complete ;


#endif



void usart_send (uint32_t data);
void usart_config(uint32_t baudrate);
void usart_send_string(uint8_t *string);



