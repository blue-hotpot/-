#include "adc_light.h"
#include "adc_rain.h"


void adc_light_config(void)
{
	//ʹ������ʱ��
    rcu_periph_clock_enable(BSP_LIGHT_RCU_AO);   
//    rcu_periph_clock_enable(BSP_RAINDROP_GPIO_RCU_DO);   
    //ʹ��ADCʱ��
    rcu_periph_clock_enable(BSP_ADC_RCU);
    
    //����ADCʱ��
    adc_clock_config(ADC_ADCCK_PCLK2_DIV4);
	
	//����A0����Ϊģ������ģʽ//GPIO_MODE_ANALOG analogue,ģ���
    gpio_mode_set(BSP_LIGHT_PORT_AO, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, BSP_LIGHT_PIN_AO); 
//	//����D0����Ϊ����ģʽ
//    gpio_mode_set(BSP_RAINDROP_GPIO_PORT_DO, GPIO_MODE_INPUT, GPIO_PUPD_NONE, BSP_RAINDROP_GPIO_PIN_DO); 
    
	//����ADCΪ����ģʽ      
	adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
	//����ģʽ��ͬ��ģʽ
		
	//ʹ��ɨ��ģʽ
	adc_special_function_config(BSP_ADC, ADC_SCAN_MODE, ENABLE);

	//�����Ҷ���     
	adc_data_alignment_config(BSP_ADC, ADC_DATAALIGN_RIGHT);
	//�Ҷ��벻�ᷭ��
	   
	//ADC0����Ϊ12λ�ֱ���(12λ����)2^10=1024,����12λ����Ϊ4096              
	adc_resolution_config(BSP_ADC, ADC_RESOLUTION_12B);
	
	//ADC0����Ϊ������  һ��ʹ�� 1 ��ͨ��
	adc_channel_length_config(BSP_ADC,ADC_ROUTINE_CHANNEL, 1);

	//ADC�ⲿ��������, ��ֻ��ʹ����������
	adc_external_trigger_config(BSP_ADC, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_DISABLE);

	//ADC0ʹ��
	adc_enable(BSP_ADC);

	//����ADC��У׼
	adc_calibration_enable(BSP_ADC); 
}



unsigned int get_light_percentage_value(void)
{
    int adc_max = 4095;
    int adc_new = 0;
    int Percentage_value = 0;
    
    adc_new = get_adc_value( BSP_LIGHT_ADC_CHANNEL );
    
    Percentage_value = (1-((float)adc_new/adc_max)) * 100;
    return (Percentage_value*9/8);
}




