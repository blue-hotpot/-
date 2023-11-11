#ifndef _BSP_HLKV20_H_
#define _BSP_HLKV20_H_

#include "gd32f4xx.h" 
#include "string.h"  
#include "systick.h"

/****************************   ��������    ****************************/
#define RCU_HLK_TX          RCU_GPIOD   // ����TX�Ķ˿�ʱ��
#define RCU_HLK_RX          RCU_GPIOD   // ����RX�Ķ˿�ʱ��
#define RCU_HLK_USART       RCU_USART1  // ����1��ʱ��

#define PORT_HLK_TX         GPIOD		// ����TX�Ķ˿�
#define PORT_HLK_RX         GPIOD		// ����RX�Ķ˿�
#define GPIO_HLK_TX         GPIO_PIN_5  // ����TX������
#define GPIO_HLK_RX         GPIO_PIN_6  // ����RX������
#define BSP_HLK_AF 			GPIO_AF_7   // ����1�ĸ��ù���

#define HLK_USART 			     USART1      		// ����1
#define HLK_USART_IRQ     	     USART1_IRQn 		// ����1�ж�
#define HLK_USART_IRQHandler     USART1_IRQHandler	// ����1�жϷ�����


#define     HLK_RX_LEN_MAX     200       //���ڽ�����󳤶�

void HLK_USART_Init(unsigned int bund);
unsigned char Anakysis_Data(void);
void HLK_USART_send_HEX(unsigned char *buff,unsigned int len);
#endif

