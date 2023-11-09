

#include "tft.h"
#include "delay.h"

//	   用到的GPIO  PB为数据口	  CS PC.8    RS PC.9    WR PC.10     RD PA.15	  RES 采用系统复位
//
//

//tft  io口 配置
void Lcd_GPIO_Init(void)
{
   GPIO_InitTypeDef  GPIO_InitStructure;					 //定义GPIO结构体


 //PB 0-15做为数据口 所以要关掉 PB上的JTAG仿真 	只开启SWJ仿真
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	  //启动AFIO时钟
// GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); //配置JTAG-DP Disabled and SW-DP Enabled 
 
 GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE); //JTAG and SWD 全部禁用

 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);	//使能PC端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;			 //端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOC.13
 GPIO_SetBits(GPIOB,GPIO_Pin_All);						 //PB.5 输出高


 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
 GPIO_Init(GPIOC, &GPIO_InitStructure);

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; 
 GPIO_Init(GPIOA, &GPIO_InitStructure);

}


//库函数方法
//库函数在调用上 相对寄存器要慢很多 

////引脚定义
//void ILI9325_RS(unsigned int d)		   //命令/数据,0=命令,1=数据
//{
// if(d==1)
// GPIO_SetBits(GPIOC,GPIO_Pin_7);// GPIOC->BSRR|=pin7;
// else if(d==0)
//  GPIO_ResetBits(GPIOC,GPIO_Pin_7); //GPIOC->BRR|=pin7;
//} 
//
//void ILI9325_WR(unsigned int d)		   //写数据
//{
// if(d==1)
// GPIO_SetBits(GPIOC,GPIO_Pin_8);// GPIOC->BSRR|=pin8;
// else if(d==0)
//  GPIO_ResetBits(GPIOC,GPIO_Pin_8);//GPIOC->BRR|=pin8;
//}                  
//
//void ILI9325_RD(unsigned int d)		   //读数据
//{									   
// if(d==1)
// GPIO_SetBits(GPIOC,GPIO_Pin_9);// GPIOC->BSRR|=pin9;
// else if(d==0)
//  GPIO_ResetBits(GPIOC,GPIO_Pin_9);//GPIOC->BRR|=pin9;
//}                    
//void ILI9325_CS(unsigned int d)		   //片选
//{
// if(d==1)
//  GPIO_SetBits(GPIOC,GPIO_Pin_6);//GPIOC->BSRR|=pin6;
// else if(d==0)
//  GPIO_ResetBits(GPIOC,GPIO_Pin_6);//GPIOC->BRR|=pin6;
//}                 
//void ILI9325_RST(unsigned int d)	   //复位
//{
// if(d==1)
// GPIO_SetBits(GPIOC,GPIO_Pin_10);// GPIOC->BSRR|=pin10;
// else if(d==0)
// GPIO_ResetBits(GPIOC,GPIO_Pin_10);// GPIOC->BRR|=pin10;
//}                   
//void ILI9325_DATA(unsigned int data)   //数据接PD口
//{
//   GPIO_ResetBits(GPIOB,GPIO_Pin_All);//GPIOB->BRR=0xffff;
//   GPIO_SetBits(GPIOB,data);//GPIOB->BSRR=data;
//}  




//寄存器方法

//引脚定义
void TFT_RS(u8 d)		   //命令/数据,0=命令,1=数据
{
 if(d==1)
  GPIOC->BSRR|=GPIO_Pin_9;
 else if(d==0)
  GPIOC->BRR|=GPIO_Pin_9;
}     
          					   
void TFT_WR(u8 d)		   //写数据
{
 if(d==1)
  GPIOC->BSRR|=GPIO_Pin_10;
 else if(d==0)
  GPIOC->BRR|=GPIO_Pin_10;
}                  
void TFT_RD(u8 d)		   //读数据
{									   
 if(d==1)
  GPIOA->BSRR|=GPIO_Pin_15;
 else if(d==0)
  GPIOA->BRR|=GPIO_Pin_15;
}                    
void TFT_CS(u8 d)		   //片选
{
 if(d==1)
  GPIOC->BSRR|=GPIO_Pin_8;
 else if(d==0)
  GPIOC->BRR|=GPIO_Pin_8;
}                 
//void TFT_RES(unsigned int d)	   //复位
//{
// if(d==1)
//  GPIOC->BSRR|=GPIO_Pin_10;
// else if(d==0)
//  GPIOC->BRR|=GPIO_Pin_10;
//}                   
void TFT_DATA(u16 data)   //数据接PD口
{
  GPIOB->BRR=GPIO_Pin_All;
  GPIOB->BSRR=data;
}                          



//显示位置寄存器定义
// 0x0050   //X轴起始位置
// 0x0051   //X轴结束位置
// 0x0052   //Y轴起始位置
// 0x0053   //Y轴结束位置
// 0x0020   //当前坐标X轴位置
// 0x0021   //当前坐标Y轴位置



//写命令
void Lcd_WR_Reg(u16 data)
{
 TFT_RS(0);       //选择命令
 TFT_DATA(data);  //赋值
 TFT_WR(0);
 TFT_WR(1);       //写入拉高电平
}

//写数据
void Lcd_Write_Data(u16 data)
{
 TFT_RS(1);       //选择数据
 TFT_DATA(data);  //赋值
 TFT_WR(0);
 TFT_WR(1);       //写入拉高电平
}

//在某一个地址或者寄存器里写入一个16位值
void Lcd_WriteReg(u16 com,u16 val)	
{
 Lcd_WR_Reg(com);
 Lcd_Write_Data(val);
}


//设置坐标范围		
//x1 y1  初始位置
//x2 y2  结束位置
//xq yq  起始点坐标
void Address_set(u8 x1,u16 y1,u8 x2,u16 y2,u8 xq,u16 yq)
{
	Lcd_WR_Reg(0x0050);Lcd_Write_Data(x1);		//设置要显示的区域
	Lcd_WR_Reg(0x0051);Lcd_Write_Data(x2);
	Lcd_WR_Reg(0x0052);Lcd_Write_Data(y1);
	Lcd_WR_Reg(0x0053);Lcd_Write_Data(y2);  
    Lcd_WR_Reg(0x0020);Lcd_Write_Data(xq);	  	//设置显示的初始位置
    Lcd_WR_Reg(0x0021);Lcd_Write_Data(yq); 
    Lcd_WR_Reg(0x0022);							//发送命令 
}



//TFT初始化
void Lcd_Init(void)
{
 Lcd_GPIO_Init();	//LCD 所用GPIO口配置

 //TFT_RES(1);
 //delay_ms(5);	
 //TFT_RES(0);
 //delay_ms(5) ;
 //TFT_RES(1);
 TFT_CS(1);
 TFT_RD(1);
 TFT_WR(1);
 delay_ms(5) ;
 TFT_CS(0);

//	if(Lcd_ReadReg(0x0000)==0x9325||Lcd_ReadReg(0x0000)==0x9328)led=0;	 //读93系列
	
    Lcd_WriteReg(0x0001,0x0100);	//Driver Output Contral.	
    Lcd_WriteReg(0x0002,0x0700);	//LCD Driver Waveform Contral.
	Lcd_WriteReg(0x0003,0x1030);	//LCD显示方向设置   手册56页
	Lcd_WriteReg(0x0004,0x0000);	//Scalling Contral.			
	Lcd_WriteReg(0x0008,0x0202);	//Display Contral 2.(0x0207)		
	Lcd_WriteReg(0x0009,0x0000);	//Display Contral 3.(0x0000)
	Lcd_WriteReg(0x000A,0x0000);	//Frame Cycle Contal.(0x0000)
	Lcd_WriteReg(0x000C,0x0000);	
	Lcd_WriteReg(0x000D,0x0000);	//Frame Maker Position.
	Lcd_WriteReg(0x000F,0x0000);	//Extern Display Interface Contral 2.

	Lcd_WriteReg(0x0010,0x0000);	
	Lcd_WriteReg(0x0011,0x0007);								//Power Control 2.(0x0001)				    //Power Control 3.(0x0138)
	Lcd_WriteReg(0x0012,0x0000);	
	Lcd_WriteReg(0x0013,0x0000);								//Power Control 4.
	Lcd_WriteReg(0x0007,0x0001);								//Power Control 7.
	delay_ms(50);

    Lcd_WriteReg(0x0010,0x1690);
	Lcd_WriteReg(0x0011,0x0227);
	delay_ms(50);
	Lcd_WriteReg(0x0012,0x009D);
	delay_ms(50);
	Lcd_WriteReg(0x0013,0x1900);	
	delay_ms(50);
	Lcd_WriteReg(0x0029,0x0025);	
	Lcd_WriteReg(0x002B,0x000D);
	delay_ms(50);
	Lcd_WriteReg(0x0020,0x0000);	//行地址设置 x
	Lcd_WriteReg(0x0021,0x0000);	//列地址设置 y
	delay_ms(50);
	Lcd_WriteReg(0x0030,0x0007);	
	Lcd_WriteReg(0x0031,0x0303);	
	Lcd_WriteReg(0x0032,0x0003);	
	Lcd_WriteReg(0x0035,0x0206);	
	Lcd_WriteReg(0x0036,0x0008);	
	Lcd_WriteReg(0x0037,0x0406);	
	Lcd_WriteReg(0x0038,0x0304);
	Lcd_WriteReg(0x0039,0x0007);	
	Lcd_WriteReg(0x003C,0x0601);	
	Lcd_WriteReg(0x003D,0x0008);			
				

	Lcd_WriteReg(0x0050,0x0000);	//行起始地址		  初始化设置屏幕显示范围为240x320
	Lcd_WriteReg(0x0051,0x00EF);	//行结束地址
	Lcd_WriteReg(0x0052,0x0000);	//列起始地址
	Lcd_WriteReg(0x0053,0x013F);	//列结束地址
	Lcd_WriteReg(0x0060,0xA700);	
	Lcd_WriteReg(0x0061,0x0001);	
	Lcd_WriteReg(0x006A,0x0000);	


	Lcd_WriteReg(0x0080,0x0000);	//Display Position? Partial Display 1.
	Lcd_WriteReg(0x0081,0x0000);	//RAM Address Start? Partial Display 1.
	Lcd_WriteReg(0x0082,0x0000);	//RAM Address End-Partial Display 1.
	Lcd_WriteReg(0x0083,0x0000);	//Displsy Position? Partial Display 2.
	Lcd_WriteReg(0x0084,0x0000);	//RAM Address Start? Partial Display 2.
	Lcd_WriteReg(0x0085,0x0000);	//RAM Address End? Partial Display 2.

	Lcd_WriteReg(0x0090,0x0010);	
	Lcd_WriteReg(0x0092,0x0600);	//Panel Interface Contral 2.(0x0000)

	Lcd_WriteReg(0x0007,0x0133);	//D1=D0=BASEE=1 开图像显示   GON=DTE=1   正常显示  用该命令设置显示器开关 以达到降低功耗效果
       
    Lcd_WR_Reg(0x0022);
}




