#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "dht11.h"
#include "oled.h"

//����Э���
#include "onenet.h"
//�����豸
#include "esp8266.h"

#include "stdio.h"
#include "string.h"
#include "adc.h"

u8 threshold_temperature;    // ��ֵ�¶�
u8 threshold_shui;       // ����ʪ��
u8 temperature;            //�¶�
u8 fan;                    //���ȿ���
u8 shui;
u8 humidity;               //ʪ��
u16 ADC_Value[30];         //adc���ݻ�����
u16 value1,value2,value3;  //DMA���ݴ���������м�������ֱ���ȡ���գ����������������
u16 n=0;
float Distance;            //����
u8 Lighting,Voice,Gas;     //���գ����������������
float temp1;
u8 LED_value;              //LED�Ƶ�ֵ��0��1
u8 buf1[10]={0};
u8 buf2[10]={0};
u8 buf3[10]={0};
u8 buf4[10]={0};
u8 buf5[10]={0};
u8 buf6[10]={0};
u16 i;   //forѭ���õ��ı�������
u16 tu_si;
/*
************************************************************
*	�������ƣ�	Hardware_Init
*
*	�������ܣ�	Ӳ����ʼ��
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		��ʼ����Ƭ�������Լ�����豸
************************************************************
*/
//void Hardware_Init(void)
//{
//	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�жϿ�������������

//	delay_init();	    	 //��ʱ������ʼ��	  
//	Usart1_Init(115200);							//����1����ӡ��Ϣ��
//	Usart2_Init(115200);							//����2��ESP8266��
//	
//	while(DHT11_Init())
//	{
//		UsartPrintf(USART_DEBUG, "DHT11 Error \r\n");	
//		delay_ms(1000);
//	}
//	
//	OLED_Init();
//	UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");	
//}

void relay_init(void);
void relay_fan_off(void);
void relay_fan_on(void);
void relay_shui_off(void);
void relay_shui_on(void);

 int main(void)
{
	
	unsigned short timeCount = 0;	//���ͼ������
	unsigned char *dataPtr = NULL;
    const char *topics[] = {"TWO_T"};
//	Hardware_Init();				//��ʼ����ΧӲ��
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����
	delay_init();			          //��ʱ������ʼ��
	Usart1_Init(115200);							//����1����ӡ��Ϣ��
	Usart2_Init(115200);
	OLED_Init(); 	              //OLED��ʼ��  
	DHT11_Init();               //��ʼ����ʪ��ģ��
	ADCx_Init();                //��ʼ��ADC�����ڲɼ����������գ�����Ũ��
	Buzzer_Init();              //��������ʼ��
	
	ESP8266_Init();					//��ʼ��ESP8266
	
	while(OneNet_DevLink())			//����OneNET
		delay_ms(500);
	
	OneNet_Subscribe(topics, 1);
    relay_init();
    threshold_shui = 20;
    threshold_temperature = 30;
//	
    show();                     //OLED��ʾ������
	while(1) 
	{
        
        
        
//	
//=====================================��ʪ��ģ��===================================================	
        if(DHT11_Read_Data(&temperature,&humidity) == 0)
		{
			sprintf((char *)buf1,":%d",temperature);
			sprintf((char *)buf2,":%d%%RH",humidity);
			OLED_ShowStr(32,0,buf1,2);
			OLED_ShowStr(32,2,buf2,2);
		}
		
//=======================�����գ��������к����壩�������������ݵĻ�ȡ��ͨ��ADC����ͨ����ѯ��ȡ========		
        for(i=0,value1=0,value2=0,value3=0;i<30;)
		{
			value1+=ADC_Value[i++];	
			value2+=ADC_Value[i++];
			value3+=ADC_Value[i++];
		}	
//printf("val1 = %d   val2 = %d  val3= %d \r\n",value1,value2,value3);		
		Lighting=(4096-value1/10)*100/4096; //����
		temp1=(float)(value2/10)*(3.3/4096);//��������
        tu_si=(4096-value3/10); //shidu
//        printf("val1 = %d   val2 = %d  val3= %d \r\n",value1,value2,value3);
//        printf("LIGHRT = %d    Tu = %d",Lighting,tu_si);

//		
		if(temp1<0.9)
        {
            n=temp1/0.001;
            Gas = n*0.012;
//				printf("һ����̼Ũ��:%dppm\n",Gas);
        }
        else
        {
            Gas = ((temp1-0.9)/0.1)*100;
            Gas =Gas+9;
//				printf("һ����̼Ũ��:%dppm\n",Gas);
        }
		sprintf((char *)buf3,":%d%%",Lighting);
        sprintf((char *)buf4,":%d%%",(4096-value3/10));
		sprintf((char *)buf5,":%d   ",Gas);
//        Gas = value2/10;
//        printf("  GAS =%d \r\n",Gas);
		OLED_ShowStr(32,4,buf3,2);//OLED��ʾ����
        OLED_ShowStr(96,4,buf4,2);//OLED��ʾʪ��
		OLED_ShowStr(64,6,buf5,2);//OLED��ʾ��������
        
        
        value1=0;
		value2=0;
		value3=0;
        
        // �����¶ȿ���
        if(temperature>threshold_temperature)
        {
            relay_fan_on();
            fan = 1;
        }else
        {
            relay_fan_off();
            fan = 0;
        }
           // �����¶ȿ���
        if(tu_si>threshold_shui)
        {
            relay_shui_on();
            shui = 1;
        }else
        {
            relay_shui_off();
            shui = 0;
        }
        
        
        
////===========================================================����������===================================
////		if(tu_si<20)
////		{
////			Buzzer=Buzzer_ON;
////            LED = LED_ON;
////		}		
////        else
////		{
////			Buzzer=Buzzer_ON;
////            LED = LED_OFF;
////		}		
//		
        
        timeCount ++;
		if(timeCount >= 50)	//���ͼ��3S
		{	
			timeCount = 0;
			OneNet_SendData();	//��������
			ESP8266_Clear();    //��ջ���
		}
		
//==============================================����OneNet���������������ݣ����Բ��ùܣ�===============================			
		dataPtr = ESP8266_GetIPD(0);
		if(dataPtr != NULL)
		{
            UsartPrintf(USART_DEBUG, "th_tem = %d\r\n",threshold_temperature);
            UsartPrintf(USART_DEBUG, "th_si = %d \r\n",threshold_shui);
			OneNet_RevPro(dataPtr);
		}
        
        

	
	
	}


}

void relay_init(void)
{
  GPIO_InitTypeDef     GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_15);
    GPIO_SetBits(GPIOB,GPIO_Pin_9);
    fan = 1;
    shui =0;
}        



void relay_fan_off(void)
{
	 GPIO_SetBits(GPIOB,GPIO_Pin_15);
     fan =0;
}

void relay_fan_on(void)
	
{ 
    GPIO_ResetBits(GPIOB,GPIO_Pin_15);
     fan = 1;
}



void relay_shui_off(void)
{

	GPIO_SetBits(GPIOB,GPIO_Pin_9);
    shui = 0;
 
}

void relay_shui_on(void)
{ 

    GPIO_ResetBits(GPIOB,GPIO_Pin_9);
    shui =1;
}
















