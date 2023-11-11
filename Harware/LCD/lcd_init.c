#include "lcd_init.h"
#include "systick.h"
#include "gd32f4xx.h"
#define  delay_ms  delay_1ms

void LCD_GPIO_Init(void)
{
    spi_parameter_struct spi_init_struct;
          /* 开启各引脚时钟 */
        rcu_periph_clock_enable(RCU_LCD_SCL);
        rcu_periph_clock_enable(RCU_LCD_SDA);
        rcu_periph_clock_enable(RCU_LCD_CS);
        rcu_periph_clock_enable(RCU_LCD_DC);
        rcu_periph_clock_enable(RCU_LCD_RES);
        rcu_periph_clock_enable(RCU_LCD_BLK);
          /* 使能SPI */
        rcu_periph_clock_enable(RCU_SPI_HARDWARE); 

          /* 配置 SPI的SCK GPIO */
        gpio_af_set(PORT_LCD_SCL, LINE_AF_SPI, GPIO_LCD_SCL);
        gpio_mode_set(PORT_LCD_SCL, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_LCD_SCL);
        gpio_output_options_set(PORT_LCD_SCL, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_LCD_SCL);
        gpio_bit_set(PORT_LCD_SCL,GPIO_LCD_SCL);
          /* 配置 SPI的MOSI  GPIO */
        gpio_af_set(PORT_LCD_SDA, LINE_AF_SPI, GPIO_LCD_SDA);
        gpio_mode_set(PORT_LCD_SDA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_LCD_SDA);
        gpio_output_options_set(PORT_LCD_SDA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_LCD_SDA);
        gpio_bit_set(PORT_LCD_SDA, GPIO_LCD_SDA);         
        
           /* 配置DC */
        gpio_mode_set(PORT_LCD_DC,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_LCD_DC);
        gpio_output_options_set(PORT_LCD_DC,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_LCD_DC);
        gpio_bit_write(PORT_LCD_DC, GPIO_LCD_DC, SET);
           /* 配置RES */
        gpio_mode_set(PORT_LCD_RES,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_LCD_RES);
        gpio_output_options_set(PORT_LCD_RES,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_LCD_RES);
        gpio_bit_write(PORT_LCD_RES, GPIO_LCD_RES, SET);
           /* 配置BLK */
        gpio_mode_set(PORT_LCD_BLK, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_LCD_BLK);
        gpio_output_options_set(PORT_LCD_BLK, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_LCD_BLK);
        gpio_bit_write(PORT_LCD_BLK, GPIO_LCD_BLK, SET);
           /* 配置CS */
        gpio_mode_set(PORT_LCD_CS,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO_LCD_CS);
        gpio_output_options_set(PORT_LCD_CS,GPIO_OTYPE_PP,GPIO_OSPEED_50MHZ,GPIO_LCD_CS);
        gpio_bit_write(PORT_LCD_CS, GPIO_LCD_CS, SET);

        /* 配置 SPI 参数 */
        spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;  // 传输模式全双工
        spi_init_struct.device_mode          = SPI_MASTER;   // 配置为主机
        spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT; // 8位数据
        spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
        spi_init_struct.nss                  = SPI_NSS_SOFT;  // 软件cs
		//片选信号
        spi_init_struct.prescale             = SPI_PSC_2;//2分频
        spi_init_struct.endian               = SPI_ENDIAN_MSB;
		//大端方式
        spi_init(PORT_SPI, &spi_init_struct);

        /* 使能 SPI */
        spi_enable(PORT_SPI);

}


/******************************************************************************
      函数说明：LCD串行数据写入函数
      入口数据：dat  要写入的串行数据
      返回值：  无
******************************************************************************/
void LCD_Writ_Bus(u8 dat) 
{	
	 LCD_CS_Clr();//？？？？？为什么关片选

        while(RESET == spi_i2s_flag_get(PORT_SPI, SPI_FLAG_TBE))
		{
		}
		spi_i2s_data_transmit(PORT_SPI, dat);
        while(RESET == spi_i2s_flag_get(PORT_SPI, SPI_FLAG_RBNE))
		{
		}
        spi_i2s_data_receive(PORT_SPI);
                        
    LCD_CS_Set();        
}


/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
	LCD_Writ_Bus(dat>>8);
	//大端方式，先传高位数据
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      函数说明：LCD写入命令
      入口数据：dat 写入的命令
      返回值：  无
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
	LCD_DC_Clr();//写命令
	LCD_Writ_Bus(dat);
	LCD_DC_Set();//写数据
}


/******************************************************************************
      函数说明：设置起始和结束地址
      入口数据：x1,x2 设置列的起始和结束地址
                y1,y2 设置行的起始和结束地址
      返回值：  无
******************************************************************************/
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
	if(USE_HORIZONTAL==0)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+26);
		LCD_WR_DATA(x2+26);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+1);
		LCD_WR_DATA(y2+1);
		LCD_WR_REG(0x2c);//储存器写
	}
	else if(USE_HORIZONTAL==1)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+26);
		LCD_WR_DATA(x2+26);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+1);
		LCD_WR_DATA(y2+1);
		LCD_WR_REG(0x2c);//储存器写
	}
	else if(USE_HORIZONTAL==2)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+1);
		LCD_WR_DATA(x2+1);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+26);
		LCD_WR_DATA(y2+26);
		LCD_WR_REG(0x2c);//储存器写
	}
	else
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+1);
		LCD_WR_DATA(x2+1);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+26);
		LCD_WR_DATA(y2+26);
		LCD_WR_REG(0x2c);//储存器写
	}
}

void LCD_Init(void)
{
	LCD_GPIO_Init();//初始化GPIO
	
	LCD_RES_Clr();//复位
	delay_ms(100);
	LCD_RES_Set();
	delay_ms(100);
	
	LCD_BLK_Set();//打开背光
  delay_ms(100);
	
	LCD_WR_REG(0x11);     //Sleep out
	delay_ms(120);                //Delay 120ms
	LCD_WR_REG(0xB1);     //Normal mode
	LCD_WR_DATA8(0x05);   
	LCD_WR_DATA8(0x3C);   
	LCD_WR_DATA8(0x3C);   
	LCD_WR_REG(0xB2);     //Idle mode
	LCD_WR_DATA8(0x05);   
	LCD_WR_DATA8(0x3C);   
	LCD_WR_DATA8(0x3C);   
	LCD_WR_REG(0xB3);     //Partial mode
	LCD_WR_DATA8(0x05);   
	LCD_WR_DATA8(0x3C);   
	LCD_WR_DATA8(0x3C);   
	LCD_WR_DATA8(0x05);   
	LCD_WR_DATA8(0x3C);   
	LCD_WR_DATA8(0x3C);   
	LCD_WR_REG(0xB4);     //Dot inversion
	LCD_WR_DATA8(0x03);   
	LCD_WR_REG(0xC0);     //AVDD GVDD
	LCD_WR_DATA8(0xAB);   
	LCD_WR_DATA8(0x0B);   
	LCD_WR_DATA8(0x04);   
	LCD_WR_REG(0xC1);     //VGH VGL
	LCD_WR_DATA8(0xC5);   //C0
	LCD_WR_REG(0xC2);     //Normal Mode
	LCD_WR_DATA8(0x0D);   
	LCD_WR_DATA8(0x00);   
	LCD_WR_REG(0xC3);     //Idle
	LCD_WR_DATA8(0x8D);   
	LCD_WR_DATA8(0x6A);   
	LCD_WR_REG(0xC4);     //Partial+Full
	LCD_WR_DATA8(0x8D);   
	LCD_WR_DATA8(0xEE);   
	LCD_WR_REG(0xC5);     //VCOM
	LCD_WR_DATA8(0x0F);   
	LCD_WR_REG(0xE0);     //positive gamma
	LCD_WR_DATA8(0x07);   
	LCD_WR_DATA8(0x0E);   
	LCD_WR_DATA8(0x08);   
	LCD_WR_DATA8(0x07);   
	LCD_WR_DATA8(0x10);   
	LCD_WR_DATA8(0x07);   
	LCD_WR_DATA8(0x02);   
	LCD_WR_DATA8(0x07);   
	LCD_WR_DATA8(0x09);   
	LCD_WR_DATA8(0x0F);   
	LCD_WR_DATA8(0x25);   
	LCD_WR_DATA8(0x36);   
	LCD_WR_DATA8(0x00);   
	LCD_WR_DATA8(0x08);   
	LCD_WR_DATA8(0x04);   
	LCD_WR_DATA8(0x10);   
	LCD_WR_REG(0xE1);     //negative gamma
	LCD_WR_DATA8(0x0A);   
	LCD_WR_DATA8(0x0D);   
	LCD_WR_DATA8(0x08);   
	LCD_WR_DATA8(0x07);   
	LCD_WR_DATA8(0x0F);   
	LCD_WR_DATA8(0x07);   
	LCD_WR_DATA8(0x02);   
	LCD_WR_DATA8(0x07);   
	LCD_WR_DATA8(0x09);   
	LCD_WR_DATA8(0x0F);   
	LCD_WR_DATA8(0x25);   
	LCD_WR_DATA8(0x35);   
	LCD_WR_DATA8(0x00);   
	LCD_WR_DATA8(0x09);   
	LCD_WR_DATA8(0x04);   
	LCD_WR_DATA8(0x10);
		 
	LCD_WR_REG(0xFC);    
	LCD_WR_DATA8(0x80);  
		
	LCD_WR_REG(0x3A);     
	LCD_WR_DATA8(0x05);   
	LCD_WR_REG(0x36);
	if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x08);
	else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC8);
	else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x78);
	else LCD_WR_DATA8(0xA8);   
	LCD_WR_REG(0x21);     //Display inversion
	LCD_WR_REG(0x29);     //Display on
	LCD_WR_REG(0x2A);     //Set Column Address
	LCD_WR_DATA8(0x00);   
	LCD_WR_DATA8(0x1A);  //26  
	LCD_WR_DATA8(0x00);   
	LCD_WR_DATA8(0x69);   //105 
	LCD_WR_REG(0x2B);     //Set Page Address
	LCD_WR_DATA8(0x00);   
	LCD_WR_DATA8(0x01);    //1
	LCD_WR_DATA8(0x00);   
	LCD_WR_DATA8(0xA0);    //160
	LCD_WR_REG(0x2C); 
}








