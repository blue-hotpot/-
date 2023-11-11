#include "fire.h"
#include "mode.h"


/******************************************************************
 * 函 数 名 称：ADC_DMA_Init
 * 函 数 说 明：初始化ADC+DMA功能
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void ADC_DMA_Init(void)
{
	
	/* 使能引脚时钟 */
    rcu_periph_clock_enable(RCU_FLAME_GPIO_AO);    		
	rcu_periph_clock_enable(RCU_FLAME_GPIO_DO);
	rcu_periph_clock_enable(RCU_SYSCFG);	
	/* 使能ADC时钟 */
    rcu_periph_clock_enable(RCU_FLAME_ADC);		
  
 
	/*	配置ADC时钟	*/ 
    adc_clock_config(ADC_ADCCK_PCLK2_DIV4);	
   /*        配置DO为输入模式        */
    gpio_mode_set(PORT_FLAME_DO, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_FLAME_DO);
	nvic_irq_enable(EXTI1_IRQn,0,0);
	syscfg_exti_line_config(EXTI_SOURCE_GPIOA,EXTI_SOURCE_PIN1);
	
	exti_init(EXTI_1,EXTI_INTERRUPT,EXTI_TRIG_FALLING);
	exti_interrupt_enable(EXTI_1);
	exti_interrupt_flag_clear(EXTI_1);
	/*	配置AO为浮空模拟输入模式	*/
    gpio_mode_set(PORT_FLAME_AO, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_FLAME_AO); // PC1 : ADC012_IN11 
 
	/*	配置ADC为独立模式	*/
    adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);
	 
    /*	使能扫描模式	*/
    adc_special_function_config(PORT_ADC, ADC_SCAN_MODE, ENABLE);
   
	/*	数据右对齐	*/        
    adc_data_alignment_config(PORT_ADC, ADC_DATAALIGN_RIGHT);
    
    /*	ADC0设置为规则组  一共使用 CHANNEL_NUM 个通道		*/  
    adc_channel_length_config(PORT_ADC, ADC_ROUTINE_CHANNEL, 1);
		
    /*	ADC规则通道配置：ADC0的通道11的扫描顺序为0；采样时间：15个周期		*/  
	/*	DMA开启之后 gt_adc_val[x][0] = PC1的数据   */
    adc_routine_channel_config(PORT_ADC, 0, CHANNEL_ADC, ADC_SAMPLETIME_15);//PC1
 
	/*	ADC0设置为12位分辨率		*/  
	adc_resolution_config(PORT_ADC, ADC_RESOLUTION_12B); 
	
	/*	ADC外部触发禁用, 即只能使用软件触发		*/  
    adc_external_trigger_config(PORT_ADC, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_DISABLE);
	
    //ADC0使能
	adc_enable(PORT_ADC);

	//开启ADC自校准
	adc_calibration_enable(PORT_ADC); 
	
	
}
 

/******************************************************************
 * 函 数 名 称：Get_Adc_Dma_Value
 * 函 数 说 明：对DMA保存的数据进行平均值计算后输出
 * 函 数 形 参：CHx 第几个扫描的数据
 * 函 数 返 回：对应扫描的ADC值
 * 作       者：LC
 * 备       注：无
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
 * 函 数 名 称：Get_FLAME_Percentage_value
 * 函 数 说 明：读取火焰AO值，并且返回百分比
 * 函 数 形 参：无
 * 函 数 返 回：返回百分比
 * 作       者：LC
 * 备       注：无
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
 * 函 数 名 称：Get_FLAME_Do_value
 * 函 数 说 明：读取火焰DO值，返回0或者1
 * 函 数 形 参：无
 * 函 数 返 回：
 * 作       者：LC
 * 备       注：无
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
