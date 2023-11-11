/********************************************************************************
  * �� �� ��: bsp_stepper_motor.c
  * �� �� ��: ����
  * �޸�����: LC
  * �޸�����: 2023��04��06��
  * ���ܽ���:          
  ******************************************************************************
  * ע������: 
*********************************************************************************/
#include "motor.h"
#include "stdio.h"
#include "math.h"
#include "hlk-v20.h"
#include "mode.h"
#include "adc_rain.h"
#include "adc_light.h"

unsigned char v20_open[5]={0xAA,0x55,0x03,0x55,0xAA};
unsigned char v20_clos[5]={0xAA,0x55,0x04,0x55,0xAA};

uint8_t phasecw[8] =  {0x08, 0x0c, 0x04, 0x06, 0x02, 0x03, 0x01, 0x09};
uint8_t phaseccw[8] = {0x09, 0x01, 0x03, 0x02, 0x06, 0x04, 0x0c, 0x08};

uint8_t motor_cw_flag = 0;
uint8_t motor_ccw_flag = 0;
uint16_t step_count = 0;
uint16_t step_max = 600;
uint16_t step_auto = 600;




/******************************************************************
 * �� �� �� �ƣ�stepper_motor_timer_config
 * �� �� ˵ ������������������Ƶ�ʶ�ʱ����ʼ��
 * �� �� �� �Σ���
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע������Ϊ2ms����ÿ2ms����һ������
******************************************************************/
void stepper_motor_timer_config(void)
{
	/* һ�����ڵ�ʱ��T = 1/f, ��ʱʱ��time = T * ����
    ��Ԥ��Ƶֵλpre,����λper
    time = (pre + 1) * (per + 1) / psc_clk
	*/
	timer_parameter_struct timere_initpara; 				// ���嶨ʱ���ṹ��
	/* ����ʱ�� */
	rcu_periph_clock_enable(RCU_TIMER5); 					// ������ʱ��ʱ��
	/* CK_TIMERx = 4 x CK_APB1  = 4x50M = 200MHZ */
	rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);   // ���ö�ʱ��ʱ��
	timer_deinit(TIMER5);								    // ��λ��ʱ��
	/* ���ö�ʱ������ */
	timere_initpara.prescaler = 2000-1;                     //  ʱ��Ԥ��Ƶֵ 0-65535,10us   
	timere_initpara.alignedmode = TIMER_COUNTER_EDGE;       // ��Ե����                  
	timere_initpara.counterdirection = TIMER_COUNTER_UP;    // ���ϼ���    
	timere_initpara.period = 500-1;                         // ����  
	timere_initpara.clockdivision = TIMER_CKDIV_DIV1;       // ��Ƶ����         
	timere_initpara.repetitioncounter = 0;					// �ظ������� 0-255  
	timer_init(TIMER5,&timere_initpara);					// ��ʼ����ʱ��
	
	/* �����ж����ȼ� */
	nvic_irq_enable(TIMER5_DAC_IRQn,1,2); 					// �����ж����ȼ�Ϊ 3,2
	/* ʹ���ж� */
	timer_interrupt_enable(TIMER5,TIMER_INT_UP);            // ʹ�ܸ����¼��ж� 
	/* ʹ�ܶ�ʱ�� */
	timer_enable(TIMER5);
}

/******************************************************************
 * �� �� �� �ƣ�stepper_motor_config
 * �� �� ˵ �����Բ���������ų�ʼ��
 * �� �� �� �Σ���
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
******************************************************************/
void stepper_motor_config(void)
{
	rcu_periph_clock_enable(AP_RCU);   // ����ʱ��
    rcu_periph_clock_enable(AM_RCU);   // ����ʱ��
	rcu_periph_clock_enable(BP_RCU);   // ����ʱ��
    rcu_periph_clock_enable(BM_RCU);   // ����ʱ��

	/* ����A+�������ģʽ ����ģʽ */
	gpio_mode_set(AP_PORT,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,AP_PIN);
	gpio_output_options_set(AP_PORT,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,AP_PIN);
    
	/* ����A-�������ģʽ ����ģʽ */
	gpio_mode_set(AM_PORT,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,AM_PIN);
	gpio_output_options_set(AM_PORT,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,AM_PIN);    
    
    /* ����B+�������ģʽ ����ģʽ */
	gpio_mode_set(BP_PORT,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,BP_PIN);
	gpio_output_options_set(BP_PORT,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,BP_PIN);
    
    /* ����B-�������ģʽ ����ģʽ */
	gpio_mode_set(BM_PORT,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,BM_PIN);
	gpio_output_options_set(BM_PORT,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,BM_PIN);
    
    AP(0);
    BP(0);
    AM(0);
    BM(0);    
}

//˳ʱ�룬ת��˳��a+  b+  a-  b-
void motor_cw(void)
{
    static uint8_t i=0;
    //������˳ʱ�붯��
    if( (motor_cw_flag==1)&&(step_count<step_max) )
    {
        AP ( ( phasecw[i] >> 3 ) & 0x01 );							
        BP ( ( phasecw[i] >> 2 ) & 0x01 );
        AM ( ( phasecw[i] >> 1 ) & 0x01 );
        BM ( ( phasecw[i] >> 0 ) & 0x01 );							
        //��������
        i = ( i + 1 ) % 8; 
        //��¼��ǰ����
        step_count++;
		if(step_count==step_max)
		{
			HLK_USART_send_HEX(v20_open,5);
		}
    }
}

//��ʱ�룬ת��˳��b-  a-  b+  a+  
void motor_ccw( void )
{
    static uint8_t i=0;
    //�����������ʱ�붯��
    if( motor_ccw_flag == 1&&step_count>0 )
    {
        AP ( ( phaseccw[i] >> 3 ) & 0x01 );							
        BP ( ( phaseccw[i] >> 2 ) & 0x01 );
        AM ( ( phaseccw[i] >> 1 ) & 0x01 );
        BM ( ( phaseccw[i] >> 0 ) & 0x01 );
        i=(i+1)%8;    
        //��¼��ǰ����
        step_count--;
		if(step_count==0)
		{
			HLK_USART_send_HEX(v20_clos,5);
		}
    }
}


void motor_auto(uint16_t step_temp)
{
	if(step_count<step_temp)
	{
		motor_ccw_flag=0;
		motor_cw_flag=1;
	}
	else if(step_count>step_temp)
	{
		motor_ccw_flag=1;
		motor_cw_flag=0;
	}
	else
	{
		motor_ccw_flag=0;
		motor_cw_flag=0;
	}
}

//��ȡ��ǰ�н�����
uint16_t get_step_count(void)
{
    return  step_count;
}
//���õ�ǰ�н�����
void set_step_auto(uint16_t num)
{
    step_auto = num;
}

uint16_t max(uint16_t a,uint16_t b)
{
	if(a<b)
		return b;
	else
		return a;
}

/************************************************
�������� �� BSP_TIMER_IRQHandler
��    �� �� ������ʱ���жϷ����� 
��    �� �� ��
�� �� ֵ �� ��
��    �� �� LC
*************************************************/
void TIMER5_DAC_IRQHandler(void)
{
	uint16_t rain_value=0;
	uint16_t light_value=0;
	uint16_t step_temp;
	rain_value=max(rain_value,get_raindrop_percentage_value()/10);
	light_value=max(light_value,get_light_percentage_value()/10);
	step_temp=step_auto*rain_value/10;
	step_temp=step_temp+(step_auto-step_temp)*light_value/10;
	/* �����Ƕ�ʱ���ж� */
  if(timer_interrupt_flag_get(TIMER5,TIMER_INT_FLAG_UP) == SET)
	{
		timer_interrupt_flag_clear(TIMER5,TIMER_INT_FLAG_UP);  // ����жϱ�־λ 
        //˳ʱ����ת
		if(bsp_mode_f==0)//�Զ�ģʽ
		{
			motor_auto(step_auto-step_temp);
		}
        motor_cw();
        //��ʱ����ת
        motor_ccw();
	}
}




