/********************************************************************************
  * 文 件 名: bsp_stepper_motor.c
  * 版 本 号: 初版
  * 修改作者: LC
  * 修改日期: 2023年04月06日
  * 功能介绍:          
  ******************************************************************************
  * 注意事项: 
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
 * 函 数 名 称：stepper_motor_timer_config
 * 函 数 说 明：步进电机脉冲更新频率定时器初始化
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：设置为2ms，即每2ms更新一次脉冲
******************************************************************/
void stepper_motor_timer_config(void)
{
	/* 一个周期的时间T = 1/f, 定时时间time = T * 周期
    设预分频值位pre,周期位per
    time = (pre + 1) * (per + 1) / psc_clk
	*/
	timer_parameter_struct timere_initpara; 				// 定义定时器结构体
	/* 开启时钟 */
	rcu_periph_clock_enable(RCU_TIMER5); 					// 开启定时器时钟
	/* CK_TIMERx = 4 x CK_APB1  = 4x50M = 200MHZ */
	rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL4);   // 配置定时器时钟
	timer_deinit(TIMER5);								    // 复位定时器
	/* 配置定时器参数 */
	timere_initpara.prescaler = 2000-1;                     //  时钟预分频值 0-65535,10us   
	timere_initpara.alignedmode = TIMER_COUNTER_EDGE;       // 边缘对齐                  
	timere_initpara.counterdirection = TIMER_COUNTER_UP;    // 向上计数    
	timere_initpara.period = 500-1;                         // 周期  
	timere_initpara.clockdivision = TIMER_CKDIV_DIV1;       // 分频因子         
	timere_initpara.repetitioncounter = 0;					// 重复计数器 0-255  
	timer_init(TIMER5,&timere_initpara);					// 初始化定时器
	
	/* 配置中断优先级 */
	nvic_irq_enable(TIMER5_DAC_IRQn,1,2); 					// 设置中断优先级为 3,2
	/* 使能中断 */
	timer_interrupt_enable(TIMER5,TIMER_INT_UP);            // 使能更新事件中断 
	/* 使能定时器 */
	timer_enable(TIMER5);
}

/******************************************************************
 * 函 数 名 称：stepper_motor_config
 * 函 数 说 明：对步进电机引脚初始化
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void stepper_motor_config(void)
{
	rcu_periph_clock_enable(AP_RCU);   // 开启时钟
    rcu_periph_clock_enable(AM_RCU);   // 开启时钟
	rcu_periph_clock_enable(BP_RCU);   // 开启时钟
    rcu_periph_clock_enable(BM_RCU);   // 开启时钟

	/* 配置A+推挽输出模式 上拉模式 */
	gpio_mode_set(AP_PORT,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,AP_PIN);
	gpio_output_options_set(AP_PORT,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,AP_PIN);
    
	/* 配置A-推挽输出模式 上拉模式 */
	gpio_mode_set(AM_PORT,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,AM_PIN);
	gpio_output_options_set(AM_PORT,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,AM_PIN);    
    
    /* 配置B+推挽输出模式 上拉模式 */
	gpio_mode_set(BP_PORT,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,BP_PIN);
	gpio_output_options_set(BP_PORT,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,BP_PIN);
    
    /* 配置B-推挽输出模式 上拉模式 */
	gpio_mode_set(BM_PORT,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,BM_PIN);
	gpio_output_options_set(BM_PORT,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,BM_PIN);
    
    AP(0);
    BP(0);
    AM(0);
    BM(0);    
}

//顺时针，转动顺序：a+  b+  a-  b-
void motor_cw(void)
{
    static uint8_t i=0;
    //开启了顺时针动作
    if( (motor_cw_flag==1)&&(step_count<step_max) )
    {
        AP ( ( phasecw[i] >> 3 ) & 0x01 );							
        BP ( ( phasecw[i] >> 2 ) & 0x01 );
        AM ( ( phasecw[i] >> 1 ) & 0x01 );
        BM ( ( phasecw[i] >> 0 ) & 0x01 );							
        //拍数增加
        i = ( i + 1 ) % 8; 
        //记录当前步数
        step_count++;
		if(step_count==step_max)
		{
			HLK_USART_send_HEX(v20_open,5);
		}
    }
}

//逆时针，转动顺序：b-  a-  b+  a+  
void motor_ccw( void )
{
    static uint8_t i=0;
    //如果开启了逆时针动作
    if( motor_ccw_flag == 1&&step_count>0 )
    {
        AP ( ( phaseccw[i] >> 3 ) & 0x01 );							
        BP ( ( phaseccw[i] >> 2 ) & 0x01 );
        AM ( ( phaseccw[i] >> 1 ) & 0x01 );
        BM ( ( phaseccw[i] >> 0 ) & 0x01 );
        i=(i+1)%8;    
        //记录当前步数
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

//获取当前行进步数
uint16_t get_step_count(void)
{
    return  step_count;
}
//设置当前行进步数
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
函数名称 ： BSP_TIMER_IRQHandler
功    能 ： 基本定时器中断服务函数 
参    数 ： 无
返 回 值 ： 无
作    者 ： LC
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
	/* 这里是定时器中断 */
  if(timer_interrupt_flag_get(TIMER5,TIMER_INT_FLAG_UP) == SET)
	{
		timer_interrupt_flag_clear(TIMER5,TIMER_INT_FLAG_UP);  // 清除中断标志位 
        //顺时针旋转
		if(bsp_mode_f==0)//自动模式
		{
			motor_auto(step_auto-step_temp);
		}
        motor_cw();
        //逆时针旋转
        motor_ccw();
	}
}




