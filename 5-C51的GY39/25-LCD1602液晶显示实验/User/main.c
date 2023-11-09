/**************************************************************************************
���������пƼ����޹�˾��PRECHIN ���У�
����֧�֣�www.prechin.net

ʵ�����ƣ�LCD1602Һ����ʾʵ��
����˵����	
ʵ���������س����LCD1602����ʾ�ַ���Ϣ
ע�����																				  
***************************************************************************************/
#include "public.h"
#include "lcd1602.h"
#include "stdio.h"
#include "string.h"

#define uint32_t unsigned long
#define uint16_t unsigned int
#define uint8_t unsigned char
   
sbit LED0 = P0^0;
sbit LED1 = P0^1;

u8 buff1[3] = {0XA5,0x81,0x26};    // ����
u8 buff2[3] = {0XA5,0x82,0x27};
u8 Receive_ok;
u8 raw_data[15]={0};




void uart_init(u8 baud)
{
	TMOD|=0X20;	//���ü�����������ʽ2
	SCON=0X50;	//����Ϊ������ʽ1
	PCON=0X80;	//�����ʼӱ�
	TH1=baud;	//��������ʼֵ����
	TL1=baud;
	ES=1;		//�򿪽����ж�
	EA=1;		//�����ж�
	TR1=1;		//�򿪼�����		
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
    uint16_t data_16[2]={0};
    uint32_t HUM,TE,Lux;
    u8 sum=0,i=0;
    uart_init(0XFA);//������Ϊ9600
	LCD_Init();//LCD1602��ʼ��
//    
//	LCD_ShowString(2,10,"World!");//��һ����ʾ
        
    

//	LCD_ShowString(2,1,"0123456789");//�ڶ�����ʾ
        
	while(1)
	{
        send_buff(buff1,3);
		delay_ms(10);
		   if(Receive_ok==2)
        {
//            send_buff(raw_data,9);
            Lux = (raw_data[4]<<24)|(raw_data[5]<<16)|(raw_data[6]<<8)|raw_data[7];
            LCD_ShowString(2,1,"Lux:");
            LCD_ShowNum(2,5,Lux/100,3);
            LCD_ShowString(2,8,".");
            LCD_ShowNum(2,9,Lux%100,2);
//						LCD_ShowString(2,1,".");
					
            
						LED1 = !LED1;
            Receive_ok = 0;
        }
        delay_ms(500);
				
        send_buff(buff2,3);
				delay_ms(10);
        if(Receive_ok==1)
        {
           
//            
//            for(sum=0,i=0;i<(raw_data[3]+4);i++)//rgb_data[3]=3
//					sum+=raw_data[i];
//					if(sum==raw_data[i])//У����ж�
//					{
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

//			}
//            
            
            
            LED0 = !LED0;
            Receive_ok = 0;
        }
        delay_ms(500);
	}	
}


//�����ж�
void USART_IRQHandler(void)interrupt 4
{
	static uint8_t i=0,rebuf[15]={0};
	uint8_t sum=0;
    
    
    
//    if(RI)			//��������жϱ�־λ
//	{
//        rebuf[i++]=SBUF;	//�洢���յ�������
//                RI = 0;
////        send_buff(rebuf,i);
//        SBUF = rebuf[i-1];
//        while(!TI);		//�ȴ������������
//        TI=0;			//���������ɱ�־λ		

//    }
    
    
	if(RI)//������ɱ�־
	{
		rebuf[i++]=SBUF;
        RI=0;//���жϽ��ձ�־
        
        
//        SBUF = rebuf[i-1];
//        while(!TI);		//�ȴ������������
//        TI=0;			//���������ɱ�־λ	
//		
        
        
        
		if (rebuf[0]!=0x5a)//֡ͷ����
			i=0;	
		if ((i==2)&&(rebuf[1]!=0x5a))//֡ͷ����
			i=0;
	
		if(i>3)//i����4ʱ���Ѿ����յ��������ֽ�rebuf[3]
		{

			if(i!=(rebuf[3]+5))//�ж��Ƿ����һ֡�������
				return;	
			switch(rebuf[2])//������Ϻ���
			{
				case 0x45:
					if(!Receive_ok)//�����ݴ�����ɺ�Ž����µ�����
					{
						memcpy(raw_data,rebuf,15);//�������յ�������
						Receive_ok=1;//������ɱ�־

                        
					}
					break;
				case 0x15:
                    if(!Receive_ok)//�����ݴ�����ɺ�Ž����µ�����
					{
						memcpy(raw_data,rebuf,9);//�������յ�������
						Receive_ok=2;//������ɱ�־
              
					}
                    break;//ԭʼ���ݽ��գ���ģ��0x45�ķ�ʽ
				case 0x35:break;
			}
			i=0;//������0
		}
	
	}

}
