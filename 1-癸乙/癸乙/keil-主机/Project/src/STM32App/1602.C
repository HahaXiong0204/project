#include "stm32f10x.h"

#include "1602.h"

#define uchar unsigned char
#define uint unsigned int

uchar  AsciiCode[]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39};//2

/*
PA8~PA15为数据口
*/
#define LCD1602_RS_0		GPIO_ResetBits(GPIOB , GPIO_Pin_12)			
#define LCD1602_RS_1    GPIO_SetBits(GPIOB , GPIO_Pin_12)	

#define LCD1602_RW_0		GPIO_ResetBits(GPIOB , GPIO_Pin_13)	  
#define LCD1602_RW_1    GPIO_SetBits(GPIOB , GPIO_Pin_13)		

#define LCD1602_EN_0		GPIO_ResetBits(GPIOB , GPIO_Pin_14)	
#define LCD1602_EN_1    GPIO_SetBits(GPIOB , GPIO_Pin_14)		



#define LCD1602_D0_0		GPIO_ResetBits(GPIOB , GPIO_Pin_15)	
#define LCD1602_D0_1    GPIO_SetBits(GPIOB , GPIO_Pin_15)	

#define LCD1602_D1_0		GPIO_ResetBits(GPIOA , GPIO_Pin_8)	
#define LCD1602_D1_1    GPIO_SetBits(GPIOA , GPIO_Pin_8)	

#define LCD1602_D2_0		GPIO_ResetBits(GPIOA , GPIO_Pin_11)	
#define LCD1602_D2_1    GPIO_SetBits(GPIOA , GPIO_Pin_11)	

#define LCD1602_D3_0		GPIO_ResetBits(GPIOA , GPIO_Pin_12)	
#define LCD1602_D3_1    GPIO_SetBits(GPIOA , GPIO_Pin_12)	

#define LCD1602_D4_0		GPIO_ResetBits(GPIOA , GPIO_Pin_15)	
#define LCD1602_D4_1    GPIO_SetBits(GPIOA , GPIO_Pin_15)	

#define LCD1602_D5_0		GPIO_ResetBits(GPIOB , GPIO_Pin_3)	
#define LCD1602_D5_1    GPIO_SetBits(GPIOB , GPIO_Pin_3)	

#define LCD1602_D6_0		GPIO_ResetBits(GPIOB , GPIO_Pin_4)	
#define LCD1602_D6_1    GPIO_SetBits(GPIOB , GPIO_Pin_4)	

#define LCD1602_D7_0		GPIO_ResetBits(GPIOB , GPIO_Pin_5)	
#define LCD1602_D7_1    GPIO_SetBits(GPIOB , GPIO_Pin_5)	




#define GET_LCD1602_D0  GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_15) 
#define GET_LCD1602_D1  GPIO_ReadInputDataBit(GPIOA , GPIO_Pin_8) 
#define GET_LCD1602_D2  GPIO_ReadInputDataBit(GPIOA , GPIO_Pin_11)  
#define GET_LCD1602_D3  GPIO_ReadInputDataBit(GPIOA , GPIO_Pin_12)
#define GET_LCD1602_D4  GPIO_ReadInputDataBit(GPIOA , GPIO_Pin_15) 
#define GET_LCD1602_D5  GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_3)
#define GET_LCD1602_D6  GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_4)
#define GET_LCD1602_D7  GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_5) 



void SetLCD1602DValue(unsigned char value)
{
	if(value&0x01)
	{
		LCD1602_D0_1;
	}
	else
	{
		LCD1602_D0_0;
	}
	
	if(value&0x02)
	{
		LCD1602_D1_1;
	}
	else
	{
		LCD1602_D1_0;
	}
		
	if(value&0x04)
	{
		LCD1602_D2_1;
	}
	else
	{
		LCD1602_D2_0;
	}
	
	if(value&0x08)
	{
		LCD1602_D3_1;
	}
	else
	{
		LCD1602_D3_0;
	}
			
	if(value&0x10)
	{
		LCD1602_D4_1;
	}
	else
	{
		LCD1602_D4_0;
	}
		
	if(value&0x20)
	{
		LCD1602_D5_1;
	}
	else
	{
		LCD1602_D5_0;
	}	
	
	if(value&0x40)
	{
		LCD1602_D6_1;
	}
	else
	{
		LCD1602_D6_0;
	}	
	
	if(value&0x80)
	{
		LCD1602_D7_1;
	}
	else
	{
		LCD1602_D7_0;
	}	
	
}

unsigned char GetLCD1602DValue(void)
{
	unsigned char dat;
	dat = 0x00;
	if(GET_LCD1602_D0)
	{
		dat = dat |0x01;
	}
	if(GET_LCD1602_D1)
	{
		dat = dat |0x02;
	}
	if(GET_LCD1602_D2)
	{
		dat = dat |0x04;
	}
	if(GET_LCD1602_D3)
	{
		dat = dat |0x08;
	}
	if(GET_LCD1602_D4)
	{
		dat = dat |0x10;
	}
	if(GET_LCD1602_D5)
	{
		dat = dat |0x20;
	}
	if(GET_LCD1602_D6)
	{
		dat = dat |0x40;
	}
	if(GET_LCD1602_D7)
	{
		dat = dat |0x80;
	}
return(dat);
	
}


/*******************通用延时子程序**************************/
void mDelay(uint i)	 
{
	for (;i>0;i--)
		{uchar j=10;while(--j);}	
}

void LCD1602DTest(void)
{
  LCD1602_RS_1;  
	LCD1602_RW_1;				
  LCD1602_EN_1;	

	SetLCD1602DValue(0x00);
	
	mDelay(100);
	mDelay(100);
	
  LCD1602_RS_0;  
	LCD1602_RW_0;				
  LCD1602_EN_0;	

	SetLCD1602DValue(0xFF);	
	
	mDelay(100);
	mDelay(100);

  LCD1602_RS_0;  
	LCD1602_RW_0;				
  LCD1602_EN_0;	

	SetLCD1602DValue(0x00);	
}
/*************************************************************
函数名称：读忙状态子函数
全局变量：无
参数说明：无
返回说明：无
版    本：1.0
说    明：读取LCD1602忙状态
**************************************************************/
void Read1602()  //读忙状态
{
  long int i=90000;	  		//建一个循环变量避免器件发生故障停在这里
	SetLCD1602DValue(0xff);
	
  LCD1602_RS_0;  
	LCD1602_RW_1;				//设置LCD为读取数据状态
  LCD1602_EN_1;							//使能LCD，高电平
  while ((i--)&&(GetLCD1602DValue()&0x80));	//检测数据口最高位状态，为0则空闲
  LCD1602_EN_0;							//关闭使能
}  

/**************************************************************
函数名称：写操作子函数
全局变量：无
参数说明：Dat为数据，command为指令（1为写数据，0为写指令）
返回说明：无
版    本：1.0
说    明：往LCD1602写入数据、指令
***************************************************************/
void Write1602(uchar Dat,unsigned char command) 
{	  						
	Read1602();
	SetLCD1602DValue(Dat);
	if(command)
	{
			LCD1602_RS_1;		//RS为1写数据、为0写指令
	}
	else
	{
		LCD1602_RS_0;
	}

	LCD1602_RW_0;		 		//RW为低，进行写操作
	LCD1602_EN_1;
	LCD1602_EN_0;	//E端控制一个高脉冲 
}

/***************************************************************
函数名称：LCD1602初始化子函数
全局变量：无
参数说明：无
返回说明：无
版    本：1.0
说    明：设置工作模式、清屏、开显示
***************************************************************/
void Init1602(void)					 
{
	LCD1602DTest();
	
	mDelay(10);	
	mDelay(10);	
	
	Write1602(0x38,0);			 //8位点阵方式
	mDelay(10);
	Write1602(0x38,0);			
	mDelay(10);
	Write1602(0x38,0);			
	mDelay(10);
	Write1602(0x38,0);			 
	mDelay(10);
	Write1602(0x01,0);			 //清屏
	mDelay(10);
	Write1602(0x0c,0);			 //开显示，光标不显示
}
void WrByte1602_EX(uchar x,uchar y,uchar dat)	
{
	Write1602(0x80+(x<<6)+y,0); //合并生成地址
	Write1602(dat,1);		    //写入数据
}
void WrByte1602(uchar x,uchar y,uchar dat)	
{
	Write1602(0x80+(x<<6)+y,0); //合并生成地址
	Write1602(dat,1);		    //写入数据
}
/***************************************************************
函数名称：LCD1602整行字符串写入子函数
全局变量：无
参数说明：x为写入的行（0或1），*p为写入的字符串数组
返回说明：无
版    本：1.0
说    明：在LCD1602任意行写入字符串
****************************************************************/
void LCD_Write_String(uchar x1,uchar x,uchar *p)	 	
{
	Write1602(0x80+(x<<6),0); 			//合并生成行首地址
	while(*p) {Write1602(*p,1);p++;}   	//逐个字符写入
}







