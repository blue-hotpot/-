#ifndef BSP_MOTOR_H
#define BSP_MOTOR_H

#include "systick.h"
#include "gd32f4xx.h"

#define AP_RCU      RCU_GPIOE//PE5
#define AP_PORT     GPIOE
#define AP_PIN      GPIO_PIN_5

#define AM_RCU      RCU_GPIOE//PE6
#define AM_PORT     GPIOE
#define AM_PIN      GPIO_PIN_6

#define BP_RCU      RCU_GPIOB//PB9
#define BP_PORT     GPIOB
#define BP_PIN      GPIO_PIN_9

#define BM_RCU      RCU_GPIOB//PB8
#define BM_PORT     GPIOB
#define BM_PIN      GPIO_PIN_8

#define AP(X)   gpio_bit_write(AP_PORT, AP_PIN, X?SET:RESET)//A+    
#define AM(X)   gpio_bit_write(AM_PORT, AM_PIN, X?SET:RESET)//A-      
#define BP(X)   gpio_bit_write(BP_PORT, BP_PIN, X?SET:RESET)//B+  
#define BM(X)   gpio_bit_write(BM_PORT, BM_PIN, X?SET:RESET)//B-   


extern uint8_t motor_cw_flag;
extern uint8_t motor_ccw_flag;



void stepper_motor_config(void);
void stepper_motor_timer_config(void);
uint16_t get_step_count(void);
void motor_auto(uint16_t step_temp);
void set_step_auto(uint16_t num);



#endif


