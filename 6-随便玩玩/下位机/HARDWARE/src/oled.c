#include "oled.h"
#include "oledfont.h"

////几个变量声明
//uint8_t **Hzk;

//初始化命令
uint8_t CMD_Data[]={
0xAE, 0x00, 0x10, 0x40, 0xB0, 0x81, 0xFF, 0xA1, 0xA6, 0xA8, 0x3F,
					
0xC8, 0xD3, 0x00, 0xD5, 0x80, 0xD8, 0x05, 0xD9, 0xF1, 0xDA, 0x12,
					
0xD8, 0x30, 0x8D, 0x14, 0xAF};
void WriteCmd()
{
	uint8_t i = 0;
	for(i=0; i<27; i++){
		HAL_I2C_Mem_Write(&hi2c1 ,0x78,0x00,I2C_MEMADD_SIZE_8BIT,CMD_Data+i,1,0x100);
	}
}
//向设备写控制命令
void OLED_WR_CMD(uint8_t cmd)
{
	HAL_I2C_Mem_Write(&hi2c1 ,0x78,0x00,I2C_MEMADD_SIZE_8BIT,&cmd,1,0x100);
}
//向设备写数据
void OLED_WR_DATA(uint8_t data)
{
	HAL_I2C_Mem_Write(&hi2c1 ,0x78,0x40,I2C_MEMADD_SIZE_8BIT,&data,1,0x100);
}
//初始化oled屏幕
void OLED_Init(void)
{ 	
	HAL_Delay(200);
	WriteCmd();
	OLED_Display_On();//开启OLED显示 
	OLED_Clear();//清屏
}
//清屏
void OLED_Clear()
{
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_CMD(0xb0+i);
		OLED_WR_CMD (0x00); 
		OLED_WR_CMD (0x10); 
		for(n=0;n<128;n++)
			OLED_WR_DATA(0);
	} 
}
//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_CMD(0X8D);  //SET DCDC命令
	OLED_WR_CMD(0X14);  //DCDC ON
	OLED_WR_CMD(0XAF);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_CMD(0X8D);  //SET DCDC命令
	OLED_WR_CMD(0X10);  //DCDC OFF
	OLED_WR_CMD(0XAE);  //DISPLAY OFF
}		   			 
void OLED_Set_Pos(uint8_t x, uint8_t y) 
{ 	
	OLED_WR_CMD(0xb0+y);
	OLED_WR_CMD(((x&0xf0)>>4)|0x10);
	OLED_WR_CMD(x&0x0f);
} 
 
void OLED_On(void)  
{  
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_CMD(0xb0+i);    //设置页地址（0~7）
		OLED_WR_CMD(0x00);      //设置显示位置—列低地址
		OLED_WR_CMD(0x10);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)
			OLED_WR_DATA(1); 
	} //更新显示
}
unsigned int oled_pow(uint8_t m,uint8_t n)
{
	unsigned int result=1;	 
	while(n--)result*=m;    
	return result;
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size)
{      	
	unsigned char c=0,i=0;	
		c=chr-' ';//得到偏移后的值			
		if(x>128-1){x=0;y=y+2;}
		if(Char_Size ==16)
			{
			OLED_Set_Pos(x,y);	
			for(i=0;i<8;i++)
			OLED_WR_DATA(F8x16[c*16+i]);
			OLED_Set_Pos(x,y+1);
			for(i=0;i<8;i++)
			OLED_WR_DATA(F8x16[c*16+i+8]);
			}
			else {	
				OLED_Set_Pos(x,y);
				for(i=0;i<6;i++)
				OLED_WR_DATA(F6x8[c][i]);
				
			}
}
 //显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(uint8_t x,uint8_t y,unsigned int num,uint8_t len,uint8_t size2)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ',size2);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2); 
	}
} 
//显示一个字符号串
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t Char_Size)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		OLED_ShowChar(x,y,chr[j],Char_Size);
			x+=8;
		if(x>120){x=0;y+=2;}
			j++;
	}
}
//显示汉字
//hzk 用取模软件得出的数组
void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no)
{      			    
	uint8_t t,adder=0;
	OLED_Set_Pos(x,y);	
    for(t=0;t<16;t++)
		{
				OLED_WR_DATA(Hzk[2*no][t]);
				adder+=1;
     }	
		OLED_Set_Pos(x,y+1);	
    for(t=0;t<16;t++)
			{	
				OLED_WR_DATA(Hzk[2*no+1][t]);
				adder+=1;
      }					
}

/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void OLED_DrawBMP(void)
{ 	
 unsigned int j=0;
 unsigned char x,y;
  
  if(8%8==0) y=8/8;      
  else y=8/8+1;
	for(y=0;y<8;y++)
	{
		OLED_Set_Pos(0,y);
    for(x=0;x<128;x++)
	    {      
	    	OLED_WR_DATA(gImage_a[j++]);	    	
	    }
	}
} 

void show(void)
{

	OLED_Clear();//OLED清屏
	OLED_ShowCHinese(0,0,2); OLED_ShowCHinese(16,0,4);//温度
	OLED_ShowCHinese(64,0,3); OLED_ShowCHinese(80,0,4);//湿度
	OLED_ShowCHinese(0,2,30); OLED_ShowCHinese(16,2,31);OLED_ShowCHinese(32,2,32);OLED_ShowCHinese(48,2,24);//烟雾检测
	OLED_ShowCHinese(0,4,35); OLED_ShowCHinese(16,4,36);OLED_ShowCHinese(32,4,32);OLED_ShowCHinese(48,4,24);OLED_ShowChar(64,4,':',12);//有人检测
	
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
								OLED_WR_DATA(gImage0[j++]);
							}	
						}break;
		case 1:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage1[j++]);
							}	
						}break;
	  case 2:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage2[j++]);
							}	
						}break;
		case 3:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage3[j++]);
							}	
						}break;
	   case 4:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage4[j++]);
							}	
						}break;
		case 5:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage5[j++]);
							}	
						}break;
	  case 6:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage6[j++]);
							}	
						}break;
		case 7:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage7[j++]);
							}	
						}break;
		case 8:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage8[j++]);
							}	
						}break;
		case 9:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage9[j++]);
							}	
						}break;
	  case 10:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage10[j++]);
							}	
						}break;
		case 11:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage11[j++]);
							}	
						}break;
		case 12:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage12[j++]);
							}	
						}break;
		case 13:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage13[j++]);
							}	
						}break;
	  case 14:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage14[j++]);
							}	
						}break;
		case 15:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage15[j++]);
							}	
						}break;
    case 16:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage16[j++]);
							}	
						}break;
	  case 17:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage17[j++]);
							}	
						}break;
		case 18:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage18[j++]);
							}	
						}break;
		case 19:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage19[j++]);
							}	
						}break;
		case 20:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage20[j++]);
							}	
						}break;
	  case 21:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage21[j++]);
							}	
						}break;
		case 22:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage22[j++]);
							}	
						}break;	
    case 23:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage23[j++]);
							}	
						}break;
	  case 24:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage24[j++]);
							}	
						}break;
		case 25:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage25[j++]);
							}	
						}break;	
    case 26:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage26[j++]);
							}	
						}break;
		case 27:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage27[j++]);
							}	
						}break;
	  case 28:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage28[j++]);
							}	
						}break;
		case 29:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage29[j++]);
							}	
						}break;
	   case 30:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage30[j++]);
							}	
						}break;
		case 31:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage31[j++]);
							}	
						}break;
	  case 32:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage32[j++]);
							}	
						}break;
		case 33:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage33[j++]);
							}	
						}break;
		case 34:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage34[j++]);
							}	
						}break;
		case 35:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage35[j++]);
							}	
						}break;
	  case 36:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage36[j++]);
							}	
						}break;
		case 37:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage37[j++]);
							}	
						}break;
		case 38:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage38[j++]);
							}	
						}break;
		case 39:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage39[j++]);
							}	
						}break;
	  case 40:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage40[j++]);
							}	
						}break;
		case 41:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage41[j++]);
							}	
						}break;
    case 42:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage42[j++]);
							}	
						}break;
	  case 43:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage43[j++]);
							}	
						}break;
		case 44:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage44[j++]);
							}	
						}break;
		case 45:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage45[j++]);
							}	
						}break;
		case 46:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage46[j++]);
							}	
						}break;
	  case 47:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage47[j++]);
							}	
						}break;
		case 48:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage48[j++]);
							}	
						}break;	
    case 49:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage49[j++]);
							}	
						}break;
	  case 50:
						for(y=y0;y-y0<2;y++)
						{
							OLED_Set_Pos(0,y);
							for(x=0;x<128;x++)
							{
								OLED_WR_DATA(gImage50[j++]);
							}	
						}break;						
  }
}
