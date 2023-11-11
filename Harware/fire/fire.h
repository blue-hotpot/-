#ifndef BSP_FIRE_H
#define BSP_FIRE_H

#include "gd32f4xx.h"
 
 
#define RCU_FLAME_GPIO_AO    RCU_GPIOF//PF8
#define RCU_FLAME_GPIO_DO    RCU_GPIOA//PA1

#define RCU_FLAME_ADC     RCU_ADC2//ADC2-6
#define PORT_ADC        ADC2
#define CHANNEL_ADC     ADC_CHANNEL_6

#define PORT_FLAME_AO     GPIOF
#define GPIO_FLAME_AO     GPIO_PIN_8

#define PORT_FLAME_DO     GPIOA
#define GPIO_FLAME_DO     GPIO_PIN_1

 

void ADC_DMA_Init(void);
unsigned int Get_Adc_Dma_Value(char CHx);
unsigned int Get_FLAME_Percentage_value(void);
unsigned char Get_FLAME_Do_value(void);

#endif



