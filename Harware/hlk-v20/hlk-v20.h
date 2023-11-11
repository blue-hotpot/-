#ifndef _BSP_HLKV20_H_
#define _BSP_HLKV20_H_

#include "gd32f4xx.h" 
#include "string.h"  
#include "systick.h"

/****************************   串口配置    ****************************/
#define RCU_HLK_TX          RCU_GPIOD   // 串口TX的端口时钟
#define RCU_HLK_RX          RCU_GPIOD   // 串口RX的端口时钟
#define RCU_HLK_USART       RCU_USART1  // 串口1的时钟

#define PORT_HLK_TX         GPIOD		// 串口TX的端口
#define PORT_HLK_RX         GPIOD		// 串口RX的端口
#define GPIO_HLK_TX         GPIO_PIN_5  // 串口TX的引脚
#define GPIO_HLK_RX         GPIO_PIN_6  // 串口RX的引脚
#define BSP_HLK_AF 			GPIO_AF_7   // 串口1的复用功能

#define HLK_USART 			     USART1      		// 串口1
#define HLK_USART_IRQ     	     USART1_IRQn 		// 串口1中断
#define HLK_USART_IRQHandler     USART1_IRQHandler	// 串口1中断服务函数


#define     HLK_RX_LEN_MAX     200       //串口接收最大长度

void HLK_USART_Init(unsigned int bund);
unsigned char Anakysis_Data(void);
void HLK_USART_send_HEX(unsigned char *buff,unsigned int len);
#endif

