/********************************************************************************
  * 文 件 名: bsp_hlkv20.c
  * 版 本 号: 初版
  * 修改作者: LC
  * 修改日期: 2023年06月06日
  * 功能介绍: HLK-V20语音识别模块的底层驱动         
  ******************************************************************************
  * 开发板官网：www.lckfb.com        
*********************************************************************************/

#include "hlk-v20.h"
#include "stdio.h"
#include "string.h"
#include "motor.h"
#include "mode.h"


unsigned char HLK_RX_BUFF[HLK_RX_LEN_MAX];     
unsigned char HLK_RX_FLAG = 0;
unsigned char HLK_RX_LEN = 0;

unsigned char rx_data = 0;
unsigned char rx_flag = 0;

/************************************************************
 * 函数名称：HLK_USART_Init
 * 函数说明：连接HLK的初始化
 * 型    参：bund=串口波特率
 * 返 回 值：无
 * 备    注：波特率根据你设置的内容来决定
*************************************************************/
void HLK_USART_Init(unsigned int bund)
{
	/* 使能 HLK_USART 的时钟 */
	rcu_periph_clock_enable(RCU_HLK_USART);
	/* 使能时钟 */
	rcu_periph_clock_enable(RCU_HLK_TX);
	rcu_periph_clock_enable(RCU_HLK_RX);
	/*	配置引脚为复用功能 */
	gpio_af_set(PORT_HLK_TX, BSP_HLK_AF, GPIO_HLK_TX);
	
	/*	配置引脚为复用功能 */
	gpio_af_set(PORT_HLK_RX, BSP_HLK_AF, GPIO_HLK_RX);
	
	/*	配置TX引脚为复用上拉模式 */
	gpio_mode_set(PORT_HLK_TX, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_HLK_TX);
	
	/*	配置RX引脚为复用上拉模式 */
	gpio_mode_set(PORT_HLK_RX, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_HLK_RX);
	
	/*	配置PA2引脚为为输出模式 */
	gpio_output_options_set(PORT_HLK_TX, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_HLK_TX);
	
	/*	配置PA3引脚为为输出模式 */
	gpio_output_options_set(PORT_HLK_RX, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_HLK_RX);
	
	/*	设置HLK_USART的波特率为115200 */
	usart_baudrate_set(HLK_USART, bund);
	
	/*	设置HLK_USART的校验位为无 */
	usart_parity_config(HLK_USART, USART_PM_NONE);
	
	/*	设置HLK_USART的数据位为8位 */
	usart_word_length_set(HLK_USART, USART_WL_8BIT);
	
	/*	设置HLK_USART的停止位为1位 */
	usart_stop_bit_set(HLK_USART, USART_STB_1BIT);
	
	/*	使能串口1 */
	usart_enable(HLK_USART);
	
	/*	使能HLK_USART传输 */
	usart_transmit_config(HLK_USART, USART_TRANSMIT_ENABLE);
	
	/*	使能HLK_USART接收 */
	usart_receive_config(HLK_USART, USART_RECEIVE_ENABLE);
	
	/*	使能HLK_USART接收中断标志位  */
	usart_interrupt_enable(HLK_USART, USART_INT_RBNE);   
	
    /*	使能HLK_USART空闲中断标志位  */
	usart_interrupt_enable(HLK_USART, USART_INT_IDLE); // DLE 线检测中断

	/* 配置中断优先级 */
	nvic_irq_enable(HLK_USART_IRQ, 2, 2); // 配置中断优先级
}

/******************************************************************
 * 函 数 名 称：HLK_USART_Send_Bit
 * 函 数 说 明：向HLK模块发送单个字符
 * 函 数 形 参：ch=字符
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void HLK_USART_Send_Bit(unsigned char ch)
{
	//发送字符
	usart_data_transmit(HLK_USART, ch);
	// 等待发送数据缓冲区标志自动置位
	while(RESET == usart_flag_get(HLK_USART, USART_FLAG_TBE) );
}  


/******************************************************************
 * 函 数 名 称：HLK_USART_send_String
 * 函 数 说 明：向HLK模块发送字符串
 * 函 数 形 参：str=发送的字符串
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void HLK_USART_send_String(unsigned char *str)
{
	while( str && *str ) // 地址为空或者值为空跳出
	{	
		HLK_USART_Send_Bit(*str++);
	}	
}

void HLK_USART_send_HEX(unsigned char *buff,unsigned int len)
{
	while( len) // 地址为空或者值为空跳出
	{	
		HLK_USART_Send_Bit(*buff);
		buff++;
		len--;
	}	
}
//清除串口接收的数据
/******************************************************************
 * 函 数 名 称：Clear_HLK_RX_BUFF
 * 函 数 说 明：清除HLK发过来的数据
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void Clear_HLK_RX_BUFF(void)
{
	unsigned char i = HLK_RX_LEN_MAX-1;
	while(i)        
	{
		HLK_RX_BUFF[i--] = 0;
	}
	HLK_RX_LEN = 0;
	HLK_RX_FLAG = 0;
}

/**********************************************************
 * 函 数 名 称：Anakysis_Data
 * 函 数 功 能：接线语音识别模块发送过来的数据
 * 传 入 参 数：无
 * 函 数 返 回：1=接收到语音数据    0=没有接收到语音数据
 * 作       者：LC
 * 备       注：AA 03 55
**********************************************************/
unsigned char Anakysis_Data(void)
{
    unsigned char ret = 0;
	if( rx_flag == 1 )//接收到语音命令
	{
		rx_flag = 0;
		if((interface!=FIRE))
		{
			switch( rx_data )//根据语音命令确定对应的动作
			{
				case 0x01://开门
					bsp_mode_f=1;
					motor_ccw_flag=0;
					motor_cw_flag=1;
					break;
				case 0x02://关门
					bsp_mode_f=1;
					motor_ccw_flag=1;
					motor_cw_flag=0;
					break;
				case 0x03://开灯
					bsp_mode_f=0;
					break;
				case 0x04://关灯
					bsp_mode_f=1;
					break;			
			}
		}
        ret = 1;
		Clear_HLK_RX_BUFF();
	}
    return ret;
}




/******************************************************************
 * 函 数 名 称：HLK_USART_IRQHandler
 * 函 数 说 明：连接HLK的串口中断服务函数
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void HLK_USART_IRQHandler(void)
{
	if(usart_interrupt_flag_get(HLK_USART,USART_INT_FLAG_RBNE) != RESET) // 接收缓冲区不为空
	{
        //接收数据
		HLK_RX_BUFF[ HLK_RX_LEN ] = usart_data_receive(HLK_USART);
        
#if DEBUG
        //测试，查看接收到了什么数据
        printf("%c", HLK_RX_BUFF[ HLK_RX_LEN ]);
#endif
		
		//0XAA 0X01 0X55
		if( HLK_RX_BUFF[HLK_RX_LEN] == 0X55 )//接收到帧尾
		{
			if( HLK_RX_BUFF[HLK_RX_LEN-2] == 0XAA)//接收到帧头，确定数据格式正确
			{
				rx_data = HLK_RX_BUFF[HLK_RX_LEN-1];//接收数据
				rx_flag = 1;
			}
		}
				
		
		//接收长度限制
        HLK_RX_LEN = ( HLK_RX_LEN + 1 ) % HLK_RX_LEN_MAX;
	}
	if(usart_interrupt_flag_get(HLK_USART,USART_INT_FLAG_IDLE) == SET) // 检测到空闲中断
	{
		usart_data_receive(HLK_USART); // 必须要读，读出来的值不能要
		HLK_RX_BUFF[HLK_RX_LEN] = '\0'; //字符串结尾补 '\0'
		HLK_RX_FLAG = SET;            // 接收完成
	}
	Anakysis_Data();
}

