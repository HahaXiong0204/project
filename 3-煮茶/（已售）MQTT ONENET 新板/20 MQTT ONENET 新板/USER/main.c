#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "oled.h"
#include "usart.h"
#include "dht11.h"
#include "esp8266.h"



//����Э���
#include "onenet.h"

//�����豸
#include "esp8266.h"
//c��
#include "string.h"


	

//dht11��ӱ���
u8 humidityH;	  //ʪ����������
u8 humidityL;	  //ʪ��С������
u8 temperatureH;   //�¶���������
u8 temperatureL;   //�¶�С������
extern u8  flag;
//�Ƶ�״̬
// u8 LED0_FLAG;
// u8 LED1_FLAG;
	
	DHT11_Data_TypeDef DHT11_Data;
 int main(void)
 {	

	unsigned char *dataPtr = NULL;
	 
	unsigned short timeCount = 0;	//���ͼ������
	 
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
 	
	OLED_Init();			//��ʼ��OLED   
	OLED_Clear();    
	LED_Init();			     //LED�˿ڳ�ʼ��
	 
	 
	uart_init(115200);//����1��ʼ��
	uart2_init(115200);//����2��ʼ��
	 
	DHT11_Init();//DHT11��ʼ��
	 
	 
	ESP8266_Init();					//��ʼ��ESP8266
	 
	 
	 
	printf("8266��ʼ���ɹ�\n");
	LED1=1;
	LED0=1;
	 
	while(OneNet_DevLink())			//����OneNET
		 
		 	OLED_ShowCHinese(0,3,9);//������
			OLED_ShowCHinese(18,3,10);
			OLED_ShowCHinese(36,3,11);
			OLED_ShowCHinese(54,3,12);
			OLED_ShowCHinese(72,3,13);

	delay_ms(900);
	
	printf("����onenet�ɹ�");
	
	  OLED_Clear();//����


	
	
	while(1) 
	{		
 	
		if(++timeCount >= 500)		//ʱ����5s
		{
				
			if( Read_DHT11(&DHT11_Data)==SUCCESS)										 //��\����ʾת��һ����
			{
				//printf("temp %d hum %d",DHT11_Data.temp_int,DHT11_Data.humi_int);
				
				OLED_ShowCHinese(0,4,14);//�¶�
				OLED_ShowCHinese(18,4,15);
                OLED_ShowString(34,4,":"); 
				
				
				OLED_ShowNum(48,4,DHT11_Data.temp_int,2,16);
				OLED_ShowChar(70,4,'.');
				OLED_ShowNum(85,4,DHT11_Data.temp_deci,1,16);
				
				//printf("temp=%d.%d\n",DHT11_Data.temp_int,DHT11_Data.temp_deci);
				
				printf("hum=%d.%d\n",DHT11_Data.humi_int,DHT11_Data.humi_deci);
				
				
				OLED_ShowCHinese(0,2,16);//ʪ��
				OLED_ShowCHinese(18,2,15);
				OLED_ShowString(34,2,":"); 
				
				OLED_ShowNum(48,2,DHT11_Data.humi_int,2,16);
				OLED_ShowChar(70,2,'%'); 
				delay_ms(100);
			
				//��Ҫ���������ϴ�ʹ��
				humidityH=DHT11_Data.humi_int;	  //ʪ����������
				humidityL=DHT11_Data.humi_deci;	  //ʪ��С������
				temperatureH=DHT11_Data.temp_int;   //�¶���������
				temperatureL=DHT11_Data.temp_deci;   //�¶�С������
				printf("hum temp=%d .%d %d .%d\r\n",humidityH,humidityL,temperatureH,temperatureL);
				
				
			}
                printf( "��ʼ��������\r\n");//ͨ������1������ʾ��Ϣ��Ҫ��ʼ���������ˣ�
					
				OneNet_SendData();//�������ݸ�onenet
				
				printf("�������ݽ���\n");
				timeCount = 0;
				
				ESP8266_Clear();
		}
		
		dataPtr = ESP8266_GetIPD(0);//��ȡƽ̨���ص�����
		if(dataPtr != NULL)//����������ݲ�Ϊ��
			OneNet_RevPro(dataPtr);//ƽ̨�������ݼ��
		
		delay_ms(10);
		
// 	  if(flag==1)
// 			LED0=1;
// 		else if(flag==2)
// 			LED0=0;
// 		else if(flag==3)
// 			LED1=1;
// 		else if(flag==4)
// 			LED1=0;
// 		
}

}
