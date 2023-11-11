#include "adc_rain.h"


void raindrop_gpio_config(void)
{
	//使能引脚时钟
    rcu_periph_clock_enable(BSP_RAINDROP_GPIO_RCU_AO);
//    rcu_periph_clock_enable(BSP_RAINDROP_GPIO_RCU_DO);   
    //使能ADC时钟
    rcu_periph_clock_enable(BSP_ADC_RCU);
    
    //配置ADC时钟
    adc_clock_config(ADC_ADCCK_PCLK2_DIV4);
	
	//配置A0引脚为模拟输入模式//GPIO_MODE_ANALOG analogue,模拟的
    gpio_mode_set(BSP_RAINDROP_GPIO_PORT_AO, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, BSP_RAINDROP_GPIO_PIN_AO); 
//	//配置D0引脚为输入模式
//    gpio_mode_set(BSP_RAINDROP_GPIO_PORT_DO, GPIO_MODE_INPUT, GPIO_PUPD_NONE, BSP_RAINDROP_GPIO_PIN_DO); 
    
	//配置ADC为独立模式      
	adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
	//独立模式和同步模式
		
	//使能扫描模式
	adc_special_function_config(BSP_ADC, ADC_SCAN_MODE, ENABLE);

	//数据右对齐     
	adc_data_alignment_config(BSP_ADC, ADC_DATAALIGN_RIGHT);
	//右对齐不会翻倍
	   
	//ADC0设置为12位分辨率(12位数据)2^10=1024,所以12位数据为4096              
	adc_resolution_config(BSP_ADC, ADC_RESOLUTION_12B);
	
	//ADC0设置为规则组  一共使用 1 个通道
	adc_channel_length_config(BSP_ADC,ADC_ROUTINE_CHANNEL, 1);

	//ADC外部触发禁用, 即只能使用软件触发
	adc_external_trigger_config(BSP_ADC, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_DISABLE);

	//ADC0使能
	adc_enable(BSP_ADC);

	//开启ADC自校准
	adc_calibration_enable(BSP_ADC); 
}

unsigned int get_adc_value(uint8_t  adc_channel_x)
{
    unsigned int adc_value = 0;
    //设置采集通道//哪个ADC，优先级（0最大），通道
    adc_routine_channel_config(BSP_ADC, 0, adc_channel_x, ADC_SAMPLETIME_15);
    //开始软件转换
    adc_software_trigger_enable(BSP_ADC, ADC_ROUTINE_CHANNEL);
    //等待 ADC 采样完成 
    while ( adc_flag_get(BSP_ADC, ADC_FLAG_EOC) == RESET ) 
    {
        ;
    }
    //读取采样值
    adc_value = adc_routine_data_read(BSP_ADC);
    //返回采样值
    return adc_value;
}


unsigned int get_raindrop_percentage_value(void)
{
    int adc_max = 4095;
    int adc_new = 0;
    int Percentage_value = 0;
    
    adc_new = get_adc_value( BSP_RAINDROP_ADC_CHANNEL );
    
    Percentage_value = (1-((float)adc_new/adc_max)) * 100;
    return (Percentage_value*2);
}



