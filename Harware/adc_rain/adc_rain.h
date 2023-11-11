#ifndef BSP_ADC_RAIN_
#define BSP_ADC_RAIN_

#include "gd32f4xx.h"
#include "systick.h"

#define BSP_RAINDROP_GPIO_RCU_AO       RCU_GPIOA//PA2
#define BSP_RAINDROP_GPIO_PORT_AO      GPIOA
#define BSP_RAINDROP_GPIO_PIN_AO       GPIO_PIN_2


#define BSP_ADC_RCU                 RCU_ADC0//ADC0-2
#define BSP_ADC                     ADC0
#define BSP_RAINDROP_ADC_CHANNEL    ADC_CHANNEL_2



void raindrop_gpio_config(void);
unsigned int get_adc_value(uint8_t  adc_channel_x);
unsigned int get_raindrop_percentage_value(void);

#endif



