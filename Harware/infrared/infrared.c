#include "infrared.h"

 /********************************************************************************
   * ����Ӳ������������ɽ�ɿ�����GD32F470ZGT6    ʹ����Ƶ200Mhz    ����25Mhz
   * �� �� ��: V1.0
   * �޸�����: LCKFB
   * �޸�����: 2023��06��12��
   * ���ܽ���:      
   ******************************************************************************
   * ��ɽ����Ӳ�������������չ����Ӳ�����Ϲ���ȫ����Դ  
   * �����������www.lckfb.com   
   * ����֧�ֳ�פ��̳���κμ������⻶ӭ��ʱ����ѧϰ  
   * ������̳��club.szlcsc.com   
   * ����ģ����ֲ�ֲ᣺https://dri8c0qdfb.feishu.cn/docx/EGRVdxunnohkrNxItYTcrwAnnHe
   * ��עbilibili�˺ţ������������塿���������ǵ����¶�̬��
   * ��������׬Ǯ���������й�����ʦΪ����
 *********************************************************************************/
 
#include "bsp_usart.h"
#include "stdio.h"
#include "systick.h"
#include "adc_rain.h"
#include "adc_light.h"
#include "motor.h"
#include "mode.h"
#include "hlk-v20.h"


typedef struct INFRARED_DATA{
    
    uint8_t AddressCode;            //��ַ��
    uint8_t AddressInverseCode;     //��ַ����
    uint8_t CommandCode;            //������
    uint8_t CommandInverseCode;     //�����
    
}_INFRARED_DATA_STRUCT_;

unsigned char v20_auto[5]={0xAA,0x55,0x01,0x55,0xAA};
unsigned char v20_manu[5]={0xAA,0x55,0x02,0x55,0xAA};


_INFRARED_DATA_STRUCT_ InfraredData;

//�������ų�ʼ��
void infrared_goio_config(void)
{
    //��������ʱ��
    rcu_periph_clock_enable(IR_RCU);   
    //����ϵͳ����ʱ��
    rcu_periph_clock_enable(RCU_SYSCFG);    
    
    //��������Ϊ��������ģʽ
    gpio_mode_set(IR_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, IR_PIN);
	//��֤������Ϊ�ߵ�ƽ�����������
	//����ԭ��ͼ�в�ȡ�����������Բ�ʹ���������
        
    /* ʹ��NVIC�ж� �жϷ���Ϊ2λ��ռ���ȼ���2λ�����ȼ� */
    nvic_irq_enable(EXTI_IRQ,2U,2U);  // ��ռ���ȼ�2�������ȼ�2
    /* �����ж��ߵ�GPIO */
    syscfg_exti_line_config(EXTI_SOURCE_PORT,EXTI_SOURCE_PIN);
    /* ��ʼ���ж����½��ش��� */
    exti_init(EXTI_X,EXTI_INTERRUPT,EXTI_TRIG_FALLING);        
    /* ʹ���ж� */
    exti_interrupt_enable(EXTI_X);
    /* ����жϱ�־λ */
    exti_interrupt_flag_clear(EXTI_X);
}




//��ȡ����͵�ƽʱ��
//��΢��us��Ϊʱ��ο�
void get_infrared_low_time( uint32_t *low_time )
{
    uint32_t time_val = 0;

    while( gpio_input_bit_get(IR_PORT, IR_PIN) == 0 )
    {
        if( time_val>= 500 ) 
        {
            break;
        } 
        delay_1us(20);           
        time_val++;
    }
    *low_time = time_val;
}

//��ȡ����ߵ�ƽʱ��
//��΢��us��Ϊʱ��ο�
void get_infrared_high_time(uint32_t *high_time)
{
    uint32_t time_val = 0;
    while( gpio_input_bit_get(IR_PORT, IR_PIN) == 1 )
    {
        if( time_val >= 250 ) 
        {
            break;
        } 
        delay_1us(20);
        time_val++;
    }
    *high_time = time_val;
}

/******************************************************************
 * �� �� �� �ƣ�guide_and_repeat_code_judgment
 * �� �� ˵ �������� �� �ظ� �� �ж�
 * �� �� �� �Σ���
 * �� �� �� �أ�1������������   2���ظ���  0��������
 * ��       �ߣ�LC
 * ��       ע����20΢��us��Ϊʱ��ο�
                �����룺��һ�� 9ms �ĵ͵�ƽ��һ�� 4.5ms �ĸߵ�ƽ���
                �ظ��룺��һ�� 9ms �ĵ͵�ƽ��һ�� 2.5ms �ĸߵ�ƽ���
******************************************************************/
uint8_t guide_and_repeat_code_judgment(void)
{
    uint32_t out_time=0;
    get_infrared_low_time(&out_time);
	//����ʱΪ�ߵ�ƽ������⵽�͵�ƽ��ʼ
    //time>10ms             time <8ms
    if((out_time > 500) || (out_time < 400))
    {
        return 1;
    } 
    get_infrared_high_time(&out_time);
    // x>5ms  ���� x<2ms
    if((out_time > 250) || (out_time < 100))
    {
        return 1;
    } 
    
    //������ظ���  2ms < time < 3ms 
    if((out_time > 100) && (out_time < 150))
    {
        return 2;
    } 
    
    return 0;
}

//���������Ƿ���ȷ�ж�
uint8_t infrared_data_true_judgment(uint8_t *value)
{
    //�жϵ�ַ���Ƿ���ȷ
    if( value[0] != (uint8_t)(~value[1]) )  return 0;
    //�ж��������Ƿ���ȷ
    if( value[2] != (uint8_t)(~value[3]) )  return 1;


    printf("%x %x %x %x\r\n",value[0],value[1],value[2],value[3]);
    //������ȷ����
    InfraredData.AddressCode        = value[0];
    InfraredData.AddressInverseCode = value[1];
    InfraredData.CommandCode        = value[2];
    InfraredData.CommandInverseCode = value[3];
	if((InfraredData.CommandCode==0x22)&&(interface!=FIRE))//0x22--<<
	{
		motor_ccw_flag=0;
		motor_cw_flag=1;
	}
	
	if(InfraredData.CommandCode==0x02&&(interface!=FIRE))
	{
		motor_ccw_flag=1;
		motor_cw_flag=0;
	}
	if(InfraredData.CommandCode==0xc2&&(interface!=FIRE))
	{
		motor_ccw_flag=0;
		motor_cw_flag=0;
	}
	//0x90--EQ
	if(InfraredData.CommandCode==0x90&&(interface!=FIRE))
	{
		bsp_mode_f=!bsp_mode_f;
		if(bsp_mode_f==0)//�Զ�ģʽ
		{
			HLK_USART_send_HEX(v20_auto,5);

		}
		if(bsp_mode_f==1)
		{
			HLK_USART_send_HEX(v20_manu,5);
		}
	}
	//0xa2--ch-
	if(InfraredData.CommandCode==0xa2&&(interface!=FIRE))
	{
		interface=IFMT;
	}
	
	if(InfraredData.CommandCode==0xe2&&(interface!=FIRE))
	{
		interface=INIT;
	}
	
	if(get_infrared_command()==0x62)
	{
		clear_infrared_command();
		interface=INIT;
	}
	return 2;
}

//���պ�������
void receiving_infrared_data(void)
{
    uint16_t group_num = 0,data_num = 0;
    uint32_t time=0;
    uint8_t bit_data = 0;
    uint8_t ir_value[5] = {0};
    
    uint8_t guide_and_repeat_code = 0;
    
    //�ȴ�������
    guide_and_repeat_code = guide_and_repeat_code_judgment();
    //����������������������
    if(  guide_and_repeat_code == 1 ) 
    {
        printf("err\r\n");
        return;
    }

    //����4������
    //��ַ��+��ַ����+������+�����
    for(group_num = 0; group_num < 4; group_num++ )		
	{				
        //����һ��8λ������
        for( data_num = 0; data_num < 8; data_num++ )	
        {
            //���յ͵�ƽ
            get_infrared_low_time(&time);
            //�������0.56ms�ڵĵ͵�ƽ,���ݴ���
            if((time > 60) || (time < 20))//time>1.2ms,time<0.4ms
            {
                return ;
            }
            
            time = 0;//ʱ���ȡ������ֱ�Ӵӵ�ַ��ʹ��time++������ÿ�ζ�Ҫ��ֵ0��
            //���ոߵ�ƽ
            get_infrared_high_time(&time);
            //�������1200us<t<2000us��Χ�����ж�Ϊ1
            if((time >=60) && (time < 100))   
            {
                bit_data = 1;
            }
            //�������200us<t<1000us��Χ�����ж�Ϊ0
            else if((time >=10) && (time < 50))  
            {
                bit_data = 0;
            }
                
            //groupNum��ʾ�ڼ�������
            ir_value[ group_num ] <<= 1;	 
            
            //���յĵ�1����Ϊ�ߵ�ƽ;�ڵڶ���forѭ���У����ݻ�������8��
            ir_value[ group_num ] |= bit_data;   
            
            //����ʱ��Ҫ���¸�ֵ	
            time=0;				
        }
    }
    //�ж������Ƿ���ȷ����ȷ�򱣴�����
    infrared_data_true_judgment(ir_value);
}

//��ȡ���ⷢ�͹���������
uint8_t get_infrared_command(void)
{
    return InfraredData.CommandCode;
}
//������ⷢ�͹���������
void clear_infrared_command(void)
{
    InfraredData.CommandCode = 0x00;
}


void EXTI5_9_IRQHandler(void)
{
  if(exti_interrupt_flag_get(EXTI_X) == SET)   // �жϱ�־λΪ1
	{
		if(gpio_input_bit_get(IR_PORT,IR_PIN) == RESET)  // ����ǵ͵�ƽ
		{
            //����һ�κ�������
            receiving_infrared_data();
			
		}
	  exti_interrupt_flag_clear(EXTI_X);            // ���жϱ�־λ
	}
}

//0x68--0...0110 1000
//0x30--1...0011 0000
//0x18--2...0001 1000
//0x7a--3...0111 1010
//0x10--4
//0x38--5
//0x5a--6
//0x42--7
//0x4a--8
//0x52--9
//0x98--100+
//0xb0--200+
//0xe0---
//0xa8--+
//0x90--EQ
//0x22--<<
//0x02-->>
//0xc2--|>
//0xa2--ch-
//0x62--ch
//0xe2--ch+







