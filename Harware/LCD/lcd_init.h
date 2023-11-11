#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "gd32f4xx.h"

#define USE_HORIZONTAL 2  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏


#ifndef u8
#define u8 uint8_t
#endif

#ifndef u16
#define u16 uint16_t
#endif

#ifndef u32
#define u32 uint32_t
#endif

#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 80
#define LCD_H 160

#else
#define LCD_W 160
#define LCD_H 80
#endif



//-----------------LCD端口定义---------------- 

#define LCD_SCLK_Clr() gpio_bit_reset(GPIOC,GPIO_PIN_10)//SCL=SCLK
#define LCD_SCLK_Set() gpio_bit_set(GPIOC,GPIO_PIN_10)//PC10

#define LCD_MOSI_Clr() gpio_bit_reset(GPIOD,GPIO_PIN_0)//SDA=MOSI
#define LCD_MOSI_Set() gpio_bit_set(GPIOD,GPIO_PIN_0)//PD0

#define LCD_RES_Clr()  gpio_bit_reset(GPIOC,GPIO_PIN_7)//RES
#define LCD_RES_Set()  gpio_bit_set(GPIOC,GPIO_PIN_7)//PC7

#define LCD_DC_Clr()   gpio_bit_reset(GPIOG,GPIO_PIN_7)//DC
#define LCD_DC_Set()   gpio_bit_set(GPIOG,GPIO_PIN_7)//PG7
 		     
#define LCD_CS_Clr()   gpio_bit_reset(GPIOB,GPIO_PIN_15)//CS
#define LCD_CS_Set()   gpio_bit_set(GPIOB,GPIO_PIN_15)//PB15

#define LCD_BLK_Clr()  gpio_bit_reset(GPIOD,GPIO_PIN_4)//BLK
#define LCD_BLK_Set()  gpio_bit_set(GPIOD,GPIO_PIN_4)//PD4



#define RCU_LCD_SCL     RCU_GPIOC//SCK
#define PORT_LCD_SCL    GPIOC
#define GPIO_LCD_SCL    GPIO_PIN_10

#define RCU_LCD_SDA     RCU_GPIOD//MOSI
#define PORT_LCD_SDA    GPIOD
#define GPIO_LCD_SDA    GPIO_PIN_0

#define RCU_LCD_CS      RCU_GPIOB//NSS
#define PORT_LCD_CS     GPIOB
#define GPIO_LCD_CS     GPIO_PIN_15

#define RCU_LCD_DC      RCU_GPIOG //DC
#define PORT_LCD_DC     GPIOG
#define GPIO_LCD_DC     GPIO_PIN_7

#define RCU_LCD_RES     RCU_GPIOC//RES
#define PORT_LCD_RES    GPIOC
#define GPIO_LCD_RES    GPIO_PIN_7

#define RCU_LCD_BLK     RCU_GPIOD//BLK
#define PORT_LCD_BLK    GPIOD
#define GPIO_LCD_BLK    GPIO_PIN_4

#define RCU_SPI_HARDWARE RCU_SPI2
#define PORT_SPI         SPI2
#define LINE_AF_SPI      GPIO_AF_6



void LCD_GPIO_Init(void);//初始化GPIO
void LCD_Writ_Bus(u8 dat);//模拟SPI时序
void LCD_WR_DATA8(u8 dat);//写入一个字节
void LCD_WR_DATA(u16 dat);//写入两个字节
void LCD_WR_REG(u8 dat);//写入一个指令
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);//设置坐标函数
void LCD_Init(void);//LCD初始化
#endif




