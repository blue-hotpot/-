#ifndef BSP_ADC_LIGHT_
#define BSP_ADC_LIGHT_

#include "gd32f4xx.h"
#include "systick.h"

#define BSP_LIGHT_RCU_AO       RCU_GPIOC//PC3
#define BSP_LIGHT_PORT_AO      GPIOC
#define BSP_LIGHT_PIN_AO       GPIO_PIN_3


#define BSP_ADC_RCU                 RCU_ADC0//ADC0-13
#define BSP_ADC                     ADC0
#define BSP_LIGHT_ADC_CHANNEL    ADC_CHANNEL_13

void adc_light_config(void);
unsigned int get_light_percentage_value(void);

#endif


