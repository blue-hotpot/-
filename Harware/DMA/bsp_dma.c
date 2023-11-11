#include "bsp_dma.h"
#include "bsp_usart.h"




void dma1_config(void)
{
	dma_single_data_parameter_struct dma_init_struct;//dma结构体
	
	dma_init_struct.periph_addr = (uint32_t)&USART_DATA(USART0);
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;//串口的增长模式是固定的
	dma_init_struct.memory0_addr = (uint32_t)recive_buff;
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
	dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
	dma_init_struct.circular_mode = DMA_CIRCULAR_MODE_DISABLE;                         
    dma_init_struct.direction = DMA_PERIPH_TO_MEMORY;                            
    dma_init_struct.number = USART_RECIVE_LENGTH;                               
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
	
	rcu_periph_clock_enable(RCU_DMA1);
	
	dma_deinit(DMA1,DMA_CH2);
	
	dma_single_data_mode_init(DMA1,DMA_CH2,&dma_init_struct);
	
	
	dma_channel_subperipheral_select(DMA1,DMA_CH2,DMA_SUBPERI4);//根据表格选择，DMA_SUBPERI4确定行，DMA_CH2确定列
	dma_channel_enable(DMA1,DMA_CH2);
	nvic_irq_enable(DMA1_Channel2_IRQn,2,1);
	dma_interrupt_enable(DMA1,DMA_CH2,DMA_CHXCTL_FTFIE);
	
	usart_dma_receive_config(USART0,USART_RECEIVE_DMA_ENABLE);

}


void DMA1_Channel2_IRQHandler(void)
{
	if(dma_interrupt_flag_get(DMA1,DMA_CH2,DMA_INT_FLAG_FTF)==SET)
	{
		dma_interrupt_flag_clear(DMA1,DMA_CH2,DMA_INT_FLAG_FTF);
		//recive_complete = 1;
		
	}
}



