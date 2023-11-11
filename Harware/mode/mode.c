#include "mode.h"
#include "lcd.h"
#include "lcd_init.h"
#include "systick.h"
#include "adc_rain.h"
#include "adc_light.h"
#include "fire.h"
#include "motor.h"


ITFC interface=INIT;
unsigned char bsp_mode_f=1;//0自动



void lcd_interface_init(void)
{
	LCD_Fill(0,0,LCD_W,LCD_H,BLACK);
	LCD_DrawPoint(80,40,WHITE);//鼻子
	
	Draw_Circle(40,20,10,WHITE);//左眼睛
	Draw_Circle1(40,20,10,WHITE);
	Draw_Circle2(40,20,3,WHITE);
	
	Draw_Circle(120,20,10,WHITE);//右眼睛
	Draw_Circle1(120,20,10,WHITE);
	Draw_Circle2(120,20,3,WHITE);
	
	LCD_DrawLine(70,60,90,60,WHITE);
	
	delay_1ms(1000);
	
	LCD_Fill(0,0,LCD_W,LCD_H,BLACK);
	LCD_DrawPoint(80,40,WHITE);//鼻子
	
	LCD_DrawLine(20,20,60,20,WHITE);
	
	LCD_DrawLine(100,20,140,20,WHITE);
	
	LCD_DrawLine(70,60,90,60,WHITE);
	
	delay_1ms(500);
}



void lcd_interface_fire(void)
{
	LCD_Fill(0,0,LCD_W,LCD_H,RED);
	delay_1ms(500);
	LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
	delay_1ms(500);
}



void lcd_interface_information(void)
{
	LCD_Fill(0,0,LCD_W,LCD_H,BLACK);
	LCD_ShowString(0,16,"Rain:   %",WHITE,BLACK,16,0);
	//字号16（字长为8，字高16）横屏模式
	LCD_ShowIntNum(40,16,get_raindrop_percentage_value(),3,WHITE,BLACK,16);
	LCD_ShowString(80,16,"Light:   %",WHITE,BLACK,16,0);
	LCD_ShowIntNum(128,16,get_light_percentage_value(),3,WHITE,BLACK,16);
	LCD_ShowString(0,16*2,"Fire:  %",WHITE,BLACK,16,0);
	LCD_ShowIntNum(40,16*2,Get_FLAME_Percentage_value(),2,WHITE,BLACK,16);
	LCD_ShowString(80,16*2,"MODE:",WHITE,BLACK,16,0);
	if(bsp_mode_f==0)
		LCD_ShowString(120,16*2,"AUTO",WHITE,BLACK,16,0);
	else
		LCD_ShowString(120,16*2,"MANU",WHITE,BLACK,16,0);
	LCD_ShowString(0,16*3,"Step:  ",WHITE,BLACK,16,0);
	LCD_ShowIntNum(40,16*3,get_step_count(),3,WHITE,BLACK,16);
	delay_1ms(1000);

	
}


