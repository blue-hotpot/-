#include "fire.h"
#include "mode.h"


/******************************************************************
 * �� �� �� �ƣ�ADC_DMA_Init
 * �� �� ˵ ������ʼ��ADC+DMA����
 * �� �� �� �Σ���
 * �� �� �� �أ���
 * ��       �ߣ�LC
 * ��       ע����
******************************************************************/
void ADC_DMA_Init(void)
{
	
	/* ʹ������ʱ�� */
    rcu_periph_clock_enable(RCU_FLAME_GPIO_AO);    		
	rcu_periph_clock_enable(RCU_FLAME_GPIO_DO);
	rcu_periph_clock_enable(RCU_SYSCFG);	
	/* ʹ��ADCʱ�� */
    rcu_periph_clock_enable(RCU_FLAME_ADC);		
  
 
	/*	����ADCʱ��	*/ 
    adc_clock_config(ADC_ADCCK_PCLK2_DIV4);	
   /*        ����DOΪ����ģʽ        */
    gpio_mode_set(PORT_FLAME_DO, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_FLAME_DO);
	nvic_irq_enable(EXTI1_IRQn,0,0);
	syscfg_exti_line_config(EXTI_SOURCE_GPIOA,EXTI_SOURCE_PIN1);
	
	exti_init(EXTI_1,EXTI_INTERRUPT,EXTI_TRIG_FALLING);
	exti_interrupt_enable(EXTI_1);
	exti_interrupt_flag_clear(EXTI_1);
	/*	����AOΪ����ģ������ģʽ	*/
    gpio_mode_set(PORT_FLAME_AO, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_FLAME_AO); // PC1 : ADC012_IN11 
 
	/*	����ADCΪ����ģʽ	*/
    adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
	 
    /*	ʹ��ɨ��ģʽ	*/
    adc_special_function_config(PORT_ADC, ADC_SCAN_MODE, ENABLE);
   
	/*	�����Ҷ���	*/        
    adc_data_alignment_config(PORT_ADC, ADC_DATAALIGN_RIGHT);
    
    /*	ADC0����Ϊ������  һ��ʹ�� CHANNEL_NUM ��ͨ��		*/  
    adc_channel_length_config(PORT_ADC, ADC_ROUTINE_CHANNEL, 1);
		
    /*	ADC����ͨ�����ã�ADC0��ͨ��11��ɨ��˳��Ϊ0������ʱ�䣺15������		*/  
	/*	DMA����֮�� gt_adc_val[x][0] = PC1������   */
    adc_routine_channel_config(PORT_ADC, 0, CHANNEL_ADC, ADC_SAMPLETIME_15);//PC1
 
	/*	ADC0����Ϊ12λ�ֱ���		*/  
	adc_resolution_config(PORT_ADC, ADC_RESOLUTION_12B); 
	
	/*	ADC�ⲿ��������, ��ֻ��ʹ���������		*/  
    adc_external_trigger_config(PORT_ADC, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_DISABLE);
	
    //ADC0ʹ��
	adc_enable(PORT_ADC);

	//����ADC��У׼
	adc_calibration_enable(PORT_ADC); 
	
	
}
 

/******************************************************************
 * �� �� �� �ƣ�Get_Adc_Dma_Value
 * �� �� ˵ ������DMA��������ݽ���ƽ��ֵ��������
 * �� �� �� �Σ�CHx �ڼ���ɨ�������
 * �� �� �� �أ���Ӧɨ���ADCֵ
 * ��       �ߣ�LC
 * ��       ע����
******************************************************************/
unsigned int Get_Adc_Dma_Value(char CHx)
{
	unsigned int AdcValue = 0;
    adc_software_trigger_enable(PORT_ADC, ADC_ROUTINE_CHANNEL); 
    while(adc_flag_get(PORT_ADC,ADC_FLAG_EOC)==RESET)
	{
	}
	AdcValue=adc_routine_data_read(PORT_ADC);
	return AdcValue;
}

/******************************************************************
 * �� �� �� �ƣ�Get_FLAME_Percentage_value
 * �� �� ˵ ������ȡ����AOֵ�����ҷ��ذٷֱ�
 * �� �� �� �Σ���
 * �� �� �� �أ����ذٷֱ�
 * ��       �ߣ�LC
 * ��       ע����
******************************************************************/
unsigned int Get_FLAME_Percentage_value(void)
{
    int adc_max = 4095;
    int adc_new = 0;
    int Percentage_value = 0;
    
    adc_new = Get_Adc_Dma_Value(0);
    
    Percentage_value = (1-((float)adc_new/adc_max)) * 100;
    return Percentage_value;
}
/******************************************************************
 * �� �� �� �ƣ�Get_FLAME_Do_value
 * �� �� ˵ ������ȡ����DOֵ������0����1
 * �� �� �� �Σ���
 * �� �� �� �أ�
 * ��       �ߣ�LC
 * ��       ע����
******************************************************************/
unsigned char Get_FLAME_Do_value(void)
{
    return gpio_input_bit_get(PORT_FLAME_DO,GPIO_FLAME_DO);
}


void EXTI1_IRQHandler(void)
{
	if(exti_interrupt_flag_get(EXTI_1)==SET)
	{
		if(Get_FLAME_Do_value()==0)
		{
			interface=FIRE;
			
		}
		exti_interrupt_flag_clear(EXTI_1);
	}
}
