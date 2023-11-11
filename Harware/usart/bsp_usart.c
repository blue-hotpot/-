#include "bsp_usart.h"
#include <stdio.h>
#include "bsp_dma.h"


uint8_t recive_buff[USART_RECIVE_LENGTH];
uint16_t recive_length =0;
uint8_t recive_complete =0;//0未完成，1完成




void usart_config(uint32_t baudrate)
{
	rcu_periph_clock_enable(RCU_USART0);//时钟使能
	rcu_periph_clock_enable(BSP_USART_RX_RCU);//GPIOA时钟使能
	rcu_periph_clock_enable(BSP_USART_TX_RCU);
	
	gpio_af_set(BSP_USART_TX_PORT,BSP_USART_AF,BSP_USART_TX_PIN);//复用功能设置(TXD--PA9)
	gpio_af_set(BSP_USART_RX_PORT,BSP_USART_AF,BSP_USART_RX_PIN);
	
	gpio_mode_set(BSP_USART_TX_PORT,GPIO_MODE_AF,GPIO_PUPD_PULLUP,BSP_USART_TX_PIN);//uart空闲高电平
	gpio_mode_set(BSP_USART_RX_PORT,GPIO_MODE_AF,GPIO_PUPD_PULLUP,BSP_USART_RX_PIN);

	gpio_output_options_set(BSP_USART_TX_PORT,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,BSP_USART_TX_PIN);//PP推挽PD开漏
	gpio_output_options_set(BSP_USART_RX_PORT,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,BSP_USART_RX_PIN);
	
	usart_deinit(USART0);//初始化
	usart_baudrate_set(USART0,baudrate);//波特率
	usart_parity_config(USART0,USART_PM_NONE);//校验位
	usart_word_length_set(USART0,USART_WL_8BIT);//数据位
	usart_stop_bit_set(USART0,USART_STB_1BIT);//停止位
	
	usart_enable(USART0);
	usart_transmit_config(USART0,USART_TRANSMIT_ENABLE);
	usart_receive_config(USART0,USART_RECEIVE_ENABLE);//接收使能
	
	nvic_irq_enable(USART0_IRQn,2,2);
	#if !USE_USART_DMA
	usart_interrupt_enable(USART0,USART_INT_RBNE);
	#endif
	usart_interrupt_enable(USART0,USART_INT_IDLE);
	

	
}

void usart_send (uint32_t data)
{
	usart_data_transmit(USART0,(uint32_t)data);
	while(RESET==usart_flag_get(USART0,USART_FLAG_TBE))//不为空则在此循环
	{}
}

void usart_send_string(uint8_t *string)
{
	while(string && *string)
	{
		usart_send(*string++);
	}
}

int fputc (int ch,FILE *f)
{
	usart_send(ch);
	return ch;
}

void USART0_IRQHandler(void)
{
	#if !USE_USART_DMA
	if(SET == usart_interrupt_flag_get(USART0,USART_INT_FLAG_RBNE))
	{
		recive_buff[recive_length++]=usart_data_receive(USART0);//串口接收自带清除标志位
	}
	#endif
	if(SET == usart_interrupt_flag_get(USART0,USART_INT_FLAG_IDLE))//接收完成
	{
		usart_data_receive(USART0);//调用串口接收函数清除标志位
		#if USE_USART_DMA
		recive_length=USART_RECIVE_LENGTH-dma_transfer_number_get(DMA1,DMA_CH2);
		
		dma_channel_disable(DMA1,DMA_CH2);
		dma1_config();
		#endif
		recive_buff[recive_length]='\0';
		recive_complete = 1;
		
	}
}

