/********************************************************************************
  * �� �� ��: bsp_hlkv20.c
  * �� �� ��: ����
  * �޸�����: LC
  * �޸�����: 2023��06��06��
  * ���ܽ���: HLK-V20����ʶ��ģ��ĵײ�����         
  ******************************************************************************
  * �����������www.lckfb.com        
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
 * �������ƣ�HLK_USART_Init
 * ����˵��������HLK�ĳ�ʼ��
 * ��    �Σ�bund=���ڲ�����
 * �� �� ֵ����
 * ��    ע�������ʸ��������õ�����������
*************************************************************/
void HLK_USART_Init(unsigned int bund)
{
	/* ʹ�� HLK_USART ��ʱ�� */
	rcu_periph_clock_enable(RCU_HLK_USART);
	/* ʹ��ʱ�� */
	rcu_periph_clock_enable(RCU_HLK_TX);
	rcu_periph_clock_enable(RCU_HLK_RX);
	/*	��������Ϊ���ù��� */
	gpio_af_set(PORT_HLK_TX, BSP_HLK_AF, GPIO_HLK_TX);
	
	/*	��������Ϊ���ù��� */
	gpio_af_set(PORT_HLK_RX, BSP_HLK_AF, GPIO_HLK_RX);
	
	/*	����TX����Ϊ��������ģʽ */
	gpio_mode_set(PORT_HLK_TX, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_HLK_TX);
	
	/*	����RX����Ϊ��������ģʽ */
	gpio_mode_set(PORT_HLK_RX, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_HLK_RX);
	
	/*	����PA2����ΪΪ���ģʽ */
	gpio_output_options_set(PORT_HLK_TX, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_HLK_TX);
	
	/*	����PA3����ΪΪ���ģʽ */
	gpio_output_options_set(PORT_HLK_RX, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_HLK_RX);
	
	/*	����HLK_USART�Ĳ�����Ϊ115200 */
	usart_baudrate_set(HLK_USART, bund);
	
	/*	����HLK_USART��У��λΪ�� */
	usart_parity_config(HLK_USART, USART_PM_NONE);
	
	/*	����HLK_USART������λΪ8λ */
	usart_word_length_set(HLK_USART, USART_WL_8BIT);
	
	/*	����HLK_USART��ֹͣλΪ1λ */
	usart_stop_bit_set(HLK_USART, USART_STB_1BIT);
	
	/*	ʹ�ܴ���1 */
	usart_enable(HLK_USART);
	
	/*	ʹ��HLK_USART���� */
	usart_transmit_config(HLK_USART, USART_TRANSMIT_ENABLE);
	
	/*	ʹ��HLK_USART���� */
	usart_receive_config(HLK_USART, USART_RECEIVE_ENABLE);
	
	/*	ʹ��HLK_USART�����жϱ�־λ  */
	usart_interrupt_enable(HLK_USART, USART_INT_RBNE);   
	
    /*	ʹ��HLK_USART�����жϱ�־λ  */
	usart_interrupt_enable(HLK_USART, USART_INT_IDLE); // DLE �߼���ж�

	/* �����ж����ȼ� */
	nvic_irq_enable(HLK_USART_IRQ, 2, 2); // �����ж����ȼ�
}

/******************************************************************
 * �� �� �� �ƣ�HLK_USART_Send_Bit
 * �� �� ˵ ������HLKģ�鷢�͵����ַ�
 * �� �� �� �Σ�ch=�ַ�
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
******************************************************************/
void HLK_USART_Send_Bit(unsigned char ch)
{
	//�����ַ�
	usart_data_transmit(HLK_USART, ch);
	// �ȴ��������ݻ�������־�Զ���λ
	while(RESET == usart_flag_get(HLK_USART, USART_FLAG_TBE) );
}  


/******************************************************************
 * �� �� �� �ƣ�HLK_USART_send_String
 * �� �� ˵ ������HLKģ�鷢���ַ���
 * �� �� �� �Σ�str=���͵��ַ���
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
******************************************************************/
void HLK_USART_send_String(unsigned char *str)
{
	while( str && *str ) // ��ַΪ�ջ���ֵΪ������
	{	
		HLK_USART_Send_Bit(*str++);
	}	
}

void HLK_USART_send_HEX(unsigned char *buff,unsigned int len)
{
	while( len) // ��ַΪ�ջ���ֵΪ������
	{	
		HLK_USART_Send_Bit(*buff);
		buff++;
		len--;
	}	
}
//������ڽ��յ�����
/******************************************************************
 * �� �� �� �ƣ�Clear_HLK_RX_BUFF
 * �� �� ˵ �������HLK������������
 * �� �� �� �Σ���
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
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
 * �� �� �� �ƣ�Anakysis_Data
 * �� �� �� �ܣ���������ʶ��ģ�鷢�͹���������
 * �� �� �� ������
 * �� �� �� �أ�1=���յ���������    0=û�н��յ���������
 * ��       �ߣ�LC
 * ��       ע��AA 03 55
**********************************************************/
unsigned char Anakysis_Data(void)
{
    unsigned char ret = 0;
	if( rx_flag == 1 )//���յ���������
	{
		rx_flag = 0;
		if((interface!=FIRE))
		{
			switch( rx_data )//������������ȷ����Ӧ�Ķ���
			{
				case 0x01://����
					bsp_mode_f=1;
					motor_ccw_flag=0;
					motor_cw_flag=1;
					break;
				case 0x02://����
					bsp_mode_f=1;
					motor_ccw_flag=1;
					motor_cw_flag=0;
					break;
				case 0x03://����
					bsp_mode_f=0;
					break;
				case 0x04://�ص�
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
 * �� �� �� �ƣ�HLK_USART_IRQHandler
 * �� �� ˵ ��������HLK�Ĵ����жϷ�����
 * �� �� �� �Σ���
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
******************************************************************/
void HLK_USART_IRQHandler(void)
{
	if(usart_interrupt_flag_get(HLK_USART,USART_INT_FLAG_RBNE) != RESET) // ���ջ�������Ϊ��
	{
        //��������
		HLK_RX_BUFF[ HLK_RX_LEN ] = usart_data_receive(HLK_USART);
        
#if DEBUG
        //���ԣ��鿴���յ���ʲô����
        printf("%c", HLK_RX_BUFF[ HLK_RX_LEN ]);
#endif
		
		//0XAA 0X01 0X55
		if( HLK_RX_BUFF[HLK_RX_LEN] == 0X55 )//���յ�֡β
		{
			if( HLK_RX_BUFF[HLK_RX_LEN-2] == 0XAA)//���յ�֡ͷ��ȷ�����ݸ�ʽ��ȷ
			{
				rx_data = HLK_RX_BUFF[HLK_RX_LEN-1];//��������
				rx_flag = 1;
			}
		}
				
		
		//���ճ�������
        HLK_RX_LEN = ( HLK_RX_LEN + 1 ) % HLK_RX_LEN_MAX;
	}
	if(usart_interrupt_flag_get(HLK_USART,USART_INT_FLAG_IDLE) == SET) // ��⵽�����ж�
	{
		usart_data_receive(HLK_USART); // ����Ҫ������������ֵ����Ҫ
		HLK_RX_BUFF[HLK_RX_LEN] = '\0'; //�ַ�����β�� '\0'
		HLK_RX_FLAG = SET;            // �������
	}
	Anakysis_Data();
}

