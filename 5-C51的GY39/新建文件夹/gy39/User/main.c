/**************************************************************************************
深圳市普中科技有限公司（PRECHIN 普中）
技术支持：www.prechin.net

实验名称：LCD1602液晶显示实验
接线说明：	
实验现象：下载程序后，LCD1602上显示字符信息
注意事项：																				  
***************************************************************************************/
#include "public.h"
#include "lcd1602.h"
#include "stdio.h"
#include "string.h"

#define uint32_t unsigned long
#define uint16_t unsigned int
#define uint8_t unsigned char
   
#define LED P1
sbit Buzzer=P2^5;   //
u8 buff1[3] = {0XA5,0x81,0x26};    // 光照
u8 buff2[3] = {0XA5,0x82,0x27};
u8 Receive_ok;
u8 raw_data[15]={0};




void uart_init(u8 baud)
{
	TMOD|=0X20;	//设置计数器工作方式2
	SCON=0X50;	//设置为工作方式1
	PCON=0X80;	//波特率加倍
	TH1=baud;	//计数器初始值设置
	TL1=baud;
	ES=1;		//打开接收中断
	EA=1;		//打开总中断
	TR1=1;		//打开计数器		
}

void send_buff(u8 *tem_buff,u8 buff_size)
{
    u8 i;
    for(i=0;i<buff_size;i++)
    {
        SBUF = tem_buff[i];
        while(TI==0){};
			TI = 0;
    }
}

void main()
{	
	  uint32_t TH_Light_min = 10;
	  uint16_t temp;
		uint32_t TH_Light_max = 80;
    uint16_t data_16[2]={0};
    uint32_t HUM,TE,Lux;
    u8 sum=0,i=0;
    uart_init(0XFA);//波特率为9600
  	LCD_Init();//LCD1602初始化

        
	while(1)
	{
		//===========================================
        send_buff(buff1,3);
				delay_ms(10);
		   if(Receive_ok==2)
        {

            Lux = (raw_data[4]<<24)|(raw_data[5]<<16)|(raw_data[6]<<8)|raw_data[7];
            LCD_ShowString(2,1,"Lux:");
            LCD_ShowNum(2,5,Lux/100,3);
            LCD_ShowString(2,8,".");
            LCD_ShowNum(2,9,Lux%100,2);
					
						if(Lux/100>TH_Light_max)
						{
								temp = 300;
						}
//						else
//						{
//							LCD_ShowNum(2,15,2,1);
//							Buzzer = 0;
//						}
						
						if(Lux/100<TH_Light_min)
						{
							 LED = 0x00;
						}
						else
						{
							 LED = 0xff;
						}

					

						
					
						
            Receive_ok = 0;
        }
				delay_ms(500);
		//=========================================
        send_buff(buff2,3);
				delay_ms(10);
        if(Receive_ok==1)
        {

						HUM = (raw_data[10]<<8)|raw_data[11];
						LCD_ShowString(1,1,"HU:");
						LCD_ShowNum(1,4,HUM/100,2);
						LCD_ShowString(1,6,".");
						LCD_ShowNum(1,7,HUM%100,2);
						TE =(raw_data[4]<<8)|raw_data[5];
						LCD_ShowString(1,9,"TE:");
						LCD_ShowNum(1,12,TE/100,2);
						LCD_ShowString(1,14,".");
						LCD_ShowNum(1,15,TE%100,2);

                     
            Receive_ok = 0;
        }
        delay_ms(500);
				
			
				while(temp--)
				{
					Buzzer = !Buzzer;
					delay_10us(100);
				}
				temp = 0;
				Buzzer = 0;
				
	}	
}


//串口中断
void USART_IRQHandler(void)interrupt 4
{
	static uint8_t i=0,rebuf[15]={0};
	uint8_t sum=0;
   
    
    
	if(RI)//接收完成标志
	{
		rebuf[i++]=SBUF;
        RI=0;//清中断接收标志
        
        
//        SBUF = rebuf[i-1];
//        while(!TI);		//等待发送数据完成
//        TI=0;			//清除发送完成标志位	
//		
        
        
        
		if (rebuf[0]!=0x5a)//帧头不对
			i=0;	
		if ((i==2)&&(rebuf[1]!=0x5a))//帧头不对
			i=0;
	
		if(i>3)//i等于4时，已经接收到数据量字节rebuf[3]
		{

			if(i!=(rebuf[3]+5))//判断是否接收一帧数据完毕
				return;	
			switch(rebuf[2])//接收完毕后处理
			{
				case 0x45:
					if(!Receive_ok)//当数据处理完成后才接收新的数据
					{
						memcpy(raw_data,rebuf,15);//拷贝接收到的数据
						Receive_ok=1;//接收完成标志

                        
					}
					break;
				case 0x15:
            if(!Receive_ok)//当数据处理完成后才接收新的数据
					{
						memcpy(raw_data,rebuf,9);//拷贝接收到的数据
						Receive_ok=2;//接收完成标志
              
					}
            break;//原始数据接收，可模仿0x45的方式
				case 0x35:break;
			}
			i=0;//缓存清0
		}
	
	}

}
