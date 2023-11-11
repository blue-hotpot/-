#ifndef BSP_INFRARED_H
#define BSP_INFRARED_H

#include "gd32f4xx.h"
#include "systick.h"

#define IR_RCU              RCU_GPIOF//PF7
#define IR_PORT             GPIOF
#define IR_PIN              GPIO_PIN_7

#define EXTI_X              EXTI_7
#define EXTI_IRQ            EXTI5_9_IRQn
#define EXTI_SOURCE_PORT    EXTI_SOURCE_GPIOF
#define EXTI_SOURCE_PIN     EXTI_SOURCE_PIN7

void infrared_goio_config(void);
uint8_t get_infrared_command(void);
void clear_infrared_command(void);



#endif

