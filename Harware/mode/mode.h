#ifndef BSP_MODE_H
#define BSP_MODE_H

#include "gd32f4xx.h"
#include "systick.h"



typedef enum
{
	INIT,
	IFMT,
	FIRE,
}ITFC;

extern ITFC interface;
extern unsigned char bsp_mode_f;


void lcd_interface_init(void);
void lcd_interface_fire(void);
void lcd_interface_information(void);



#endif

