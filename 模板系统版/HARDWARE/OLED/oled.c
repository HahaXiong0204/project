
#include "oled.h"
#include "oledfont.h"  	 
#include "delay.h"


//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 			   

//向SSD1306写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 dat,u8 cmd)
{	
	u8 i;			  
	if(cmd)
	  OLED_DC_Set();
	else 
	  OLED_DC_Clr();	 
	OLED_CS_Clr();
	for(i=0;i<8;i++)
	{			  
		OLED_SCLK_Clr();
		if(dat&0x80)
			{
		   OLED_SDIN_Set();
			}
        else
		   OLED_SDIN_Clr();
	    OLED_SCLK_Set();

		dat<<=1;   
	}				 		  
	OLED_CS_Set();
	OLED_DC_Set();   	  
} 



void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); 
}   	  


//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}


//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   


			 
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
	} //更新显示
}


//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr)
{      	
	unsigned char c=0,i=0;	
		c=chr-' ';//得到偏移后的值			
		if(x>Max_Column-1){x=0;y=y+2;}
		if(SIZE ==16)
			{
			OLED_Set_Pos(x,y);	
			for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
			OLED_Set_Pos(x,y+1);
			for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
			}
			else {	
				OLED_Set_Pos(x,y+1);
				for(i=0;i<6;i++)
				OLED_WR_Byte(F6x8[c][i],OLED_DATA);
				
			}
}



//m^n函数
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}	

			  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ');
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0'); 
	}
} 



//显示一个字符号串
void OLED_ShowString(u8 x,u8 y,u8 *chr)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		OLED_ShowChar(x,y,chr[j]);
			x+=8;
		if(x>120){x=0;y+=2;}
			j++;
	}
}



//显示汉字
void OLED_ShowCHinese(u8 x,u8 y,u8 no)
{      			    
	u8 t,adder=0;
	OLED_Set_Pos(x,y);	
    for(t=0;t<16;t++)
		{
				OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
				adder+=1;
     }	
		OLED_Set_Pos(x,y+1);	
    for(t=0;t<16;t++)
			{	
				OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
				adder+=1;
      }					
}






/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
 unsigned int j=0;
 unsigned char x,y;
  
  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
	    {      
	    	OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
	    }
	}
} 


//y0:  y方向初始坐标(0-7)
//q:   进度条值(0-51)
void OLED_progress_bar(unsigned char y0,unsigned char q)
{
	unsigned int j=0;
	unsigned char x,y;
	
	switch(q)
	{
		case 0:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage0[j++],OLED_DATA);
							}	
						}break;
		case 1:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage1[j++],OLED_DATA);
							}	
						}break;
	  case 2:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage2[j++],OLED_DATA);
							}	
						}break;
		case 3:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage3[j++],OLED_DATA);
							}	
						}break;
	   case 4:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage4[j++],OLED_DATA);
							}	
						}break;
		case 5:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage5[j++],OLED_DATA);
							}	
						}break;
	  case 6:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage6[j++],OLED_DATA);
							}	
						}break;
		case 7:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage7[j++],OLED_DATA);
							}	
						}break;
		case 8:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage8[j++],OLED_DATA);
							}	
						}break;
		case 9:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage9[j++],OLED_DATA);
							}	
						}break;
	  case 10:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage10[j++],OLED_DATA);
							}	
						}break;
		case 11:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage11[j++],OLED_DATA);
							}	
						}break;
		case 12:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage12[j++],OLED_DATA);
							}	
						}break;
		case 13:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage13[j++],OLED_DATA);
							}	
						}break;
	  case 14:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage14[j++],OLED_DATA);
							}	
						}break;
		case 15:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage15[j++],OLED_DATA);
							}	
						}break;
    case 16:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
							OLED_WR_Byte(gImage16[j++],OLED_DATA);
							}	
						}break;
	  case 17:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage17[j++],OLED_DATA);
							}	
						}break;
		case 18:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage18[j++],OLED_DATA);
							}	
						}break;
		case 19:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage19[j++],OLED_DATA);
							}	
						}break;
		case 20:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage20[j++],OLED_DATA);
							}	
						}break;
	  case 21:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage21[j++],OLED_DATA);
							}	
						}break;
		case 22:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage22[j++],OLED_DATA);
							}	
						}break;	
    case 23:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage23[j++],OLED_DATA);
							}	
						}break;
	  case 24:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
							OLED_WR_Byte(gImage24[j++],OLED_DATA);
							}	
						}break;
		case 25:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage25[j++],OLED_DATA);
							}	
						}break;	
    case 26:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage26[j++],OLED_DATA);
							}	
						}break;
		case 27:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage27[j++],OLED_DATA);
							}	
						}break;
	  case 28:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage28[j++],OLED_DATA);
							}	
						}break;
		case 29:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage29[j++],OLED_DATA);
							}	
						}break;
	   case 30:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage30[j++],OLED_DATA);
							}	
						}break;
		case 31:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
							OLED_WR_Byte(gImage31[j++],OLED_DATA);
							}	
						}break;
	  case 32:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
							OLED_WR_Byte(gImage32[j++],OLED_DATA);
							}	
						}break;
		case 33:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage33[j++],OLED_DATA);
							}	
						}break;
		case 34:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage34[j++],OLED_DATA);
							}	
						}break;
		case 35:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage35[j++],OLED_DATA);
							}	
						}break;
	  case 36:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage36[j++],OLED_DATA);
							}	
						}break;
		case 37:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage37[j++],OLED_DATA);
							}	
						}break;
		case 38:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage38[j++],OLED_DATA);
							}	
						}break;
		case 39:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage39[j++],OLED_DATA);
							}	
						}break;
	  case 40:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage40[j++],OLED_DATA);
							}	
						}break;
		case 41:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage41[j++],OLED_DATA);
							}	
						}break;
    case 42:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage42[j++],OLED_DATA);
							}	
						}break;
	  case 43:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage43[j++],OLED_DATA);
							}	
						}break;
		case 44:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage44[j++],OLED_DATA);
							}	
						}break;
		case 45:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage45[j++],OLED_DATA);
							}	
						}break;
		case 46:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage46[j++],OLED_DATA);
							}	
						}break;
	  case 47:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage47[j++],OLED_DATA);
							}	
						}break;
		case 48:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage48[j++],OLED_DATA);
							}	
						}break;	
    case 49:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage49[j++],OLED_DATA);
							}	
						}break;
	  case 50:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_Byte(gImage50[j++],OLED_DATA);
							}	
						}break;						
  }
}

//初始化SSD1306					    
void OLED_Init(void)
{ 	
 		
 	GPIO_InitTypeDef  GPIO_InitStructure;					//定义IO配置结构体

	//要用到仿真口PA15  所以关掉JTAG仿真功能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	  //启动AFIO时钟
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); //配置JTAG-DP Disabled and SW-DP Enabled 
	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE); //JTAG and SWD 全部禁用


 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);	 //使能PC,D,G端口时钟
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_5|GPIO_Pin_7|GPIO_Pin_9;	 //PB1 3 5 7 9推挽输出  
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  //初始化GPIOB
 	GPIO_SetBits(GPIOC,GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_5|GPIO_Pin_7|GPIO_Pin_9);	// 输出高

// 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				 //PC0,1 OUT推挽输出
// 	GPIO_Init(GPIOA, &GPIO_InitStructure);
// 	GPIO_SetBits(GPIOA,GPIO_Pin_15);						 //PC0,1 OUT  输出高
//
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //PG15 OUT推挽输出	  RST
// 	GPIO_Init(GPIOB, &GPIO_InitStructure);
// 	GPIO_SetBits(GPIOB,GPIO_Pin_8);						 //PG15 OUT  输出高 

							 
    OLED_RST_Set();
	delay_ms(100);
	OLED_RST_Clr();
	delay_ms(100);
	OLED_RST_Set(); 


	OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
	OLED_WR_Byte(0xCF,OLED_CMD); // Set SEG Output Current Brightness
	OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);//-not offset
	OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
	OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
	OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
	OLED_WR_Byte(0x12,OLED_CMD);
	OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
	OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
	OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_Byte(0x02,OLED_CMD);//
	OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
	OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
	OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
	
	OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/ 
	OLED_Clear();
	OLED_Set_Pos(0,0); 	
					
}  





























