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
u8 temperature;
u8 humidity;               //ʪ��
u16 ADC_Value[40];         //adc���ݻ�����
u16 value1,value2,value3,value4;  //DMA���ݴ���������м�������ֱ���ȡ���գ����������������
u16 n=0;
float Distance;            //����
u16 Lighting,Gas,Gas2;     //���գ����������������
float temp1,temp2;
extern u8 LED_value;              //LED�Ƶ�ֵ��0��1
u8 buf1[10]={0};
u8 buf2[10]={0};
u8 buf3[10]={0};
u8 buf4[10]={0};
u8 buf5[10]={0};
u8 buf6[10]={0};
u16 i;   //forѭ���õ��ı�������
u16 fire_value;
unsigned short timeCount = 0;	//���ͼ������


//��ֵ
u8 th_temp,th_humidity,th_fire,th_MQ135,th_MQ2,th_Lighting;
u8 mode = 1;   // 1�Զ� 2�ֶ�




 int main(void)
{
	

	unsigned char *dataPtr = NULL;
    const char *topics[] = {"two","$creq"};
//	Hardware_Init();				//��ʼ����ΧӲ��
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����
	delay_init();			          //��ʱ������ʼ��
    LED_Init();
	Buzzer_Init();              //��������ʼ��
    Usart1_Init(115200);							//����1����ӡ��Ϣ��
	Usart2_Init(115200);
	OLED_Init(); 	              //OLED��ʼ��  
	DHT11_Init();               //��ʼ����ʪ��ģ��
	ADCx_Init();                //��ʼ��ADC�����ڲɼ����������գ�����Ũ��
	Relay_Init();
    Buzzer_Init();              //��������ʼ��
	show();                     //OLED��ʾ������

	ESP8266_Init();					//��ʼ��ESP8266
	
	while(OneNet_DevLink())			//����OneNET
		delay_ms(500);
	
	OneNet_Subscribe(topics, 1);
    //��Ĭ��ֵ
    th_fire = 50;
    th_humidity = 100;
    th_temp = 30;
    th_MQ135 = 50;
    th_MQ2 = 50;
    th_Lighting = 50;
    
//	

	while(1) 
	{
        

        
//=====================================��ʪ��ģ��===================================================	
        if(DHT11_Read_Data(&temperature,&humidity) == 0)
		{
			sprintf((char *)buf1,":%d",temperature);
			sprintf((char *)buf2,":%d%%RH",humidity);
			OLED_ShowStr(32,0,buf1,2);
			OLED_ShowStr(32,2,buf2,2);
		}
		
//=======================�����գ��������к����壩�������������ݵĻ�ȡ��ͨ��ADC����ͨ����ѯ��ȡ========		
        for(i=0,value1=0,value2=0,value3=0;i<40;)
		{
			value1+=ADC_Value[i++];	
			value2+=ADC_Value[i++];
			value3+=ADC_Value[i++];
            value4+=ADC_Value[i++];
		}	
//printf("val1 = %d   val2 = %d  val3= %d \r\n",value1,value2,value3);		
		Lighting=(4096-value1/10)*100/4096; //����
		temp1=(float)(value2/10)*(3.3/4096);//mq135
        temp2=(float)(value3/10)*(3.3/4096);  // mq2
        fire_value=(4096-value4/10)/41.0; //����ֵ
//        printf("val1 = %d   val2 = %d  val3= %d val4 = %d\r\n",value1,value2,value3,value4);
//        printf("LIGHRT = %d    Tu = %d",Lighting,tu_si);
//        printf("temp1 = %f         temp2 = %f   \r\n",temp1,temp2);
//		
		if(temp1<0.9)
        {
            n=temp1/0.001;
            Gas = n*0.012;
//				printf("GASһ����̼Ũ��:%dppm\r\n",Gas);
        }
        else
        {
            Gas = ((temp1-0.9)/0.1)*100;
            Gas =Gas+9;
//				printf("GASһ����̼Ũ��:%dppm\r\n",Gas);
        }
        
        
        if(temp2<0.9)
        {
            n=temp2/0.001;
            Gas2 = n*0.012;
//				printf("GAS2һ����̼Ũ��:%dppm\r\n",Gas2);
        }
        else
        {
            Gas2 = ((temp2-0.9)/0.1)*100;
            Gas2 =Gas2+9;
//				printf("GAS2һ����̼Ũ��:%dppm\r\n",Gas2);
        }
        if(Gas>=100){
            Gas = 99;
        }
        if(Gas2>=100){
            Gas2 = 99;
        }
        
		sprintf((char *)buf1,":%d%%",Lighting);
        sprintf((char *)buf2,":%d%%",Gas);
		sprintf((char *)buf3,":%d%%",Gas2);
        sprintf((char *)buf4,":%d",fire_value);
//        printf("���棺%d  \r\n",fire_value);
		OLED_ShowStr(32,4,buf1,2);//OLED��ʾ����
        OLED_ShowStr(32,6,buf2,2);//OLED��ʾ��������1
		OLED_ShowStr(96,6,buf3,2);//OLED��ʾ��������2
        OLED_ShowStr(106,4,buf4,2);//OLED��ʾ����
        
        value1=0;
		value2=0;
		value3=0;
        value4=0;
        

        
        
        
////===========================================================����������===================================
        if(mode == 1)   // �Զ�
        {
            if(th_temp<temperature||th_fire<fire_value||th_humidity<humidity||th_MQ135<Gas||th_MQ2<Gas2)
            {
                Buzzer = 0;
//                LED = LED_ON;
            }
            else{
                Buzzer = 1;
//                LED = LED_OFF;
            }
            
            if(th_temp<temperature)
            {
                Relay_Fan_On();
            }
            else
            {
                Relay_Fan_Off();
            }
            
            if(th_Lighting>=Lighting)
            {
                LED = LED_ON;
                LED_value = 1;
            }
            else
            {
                LED = LED_OFF;
                LED_value = 0;
            }
            
            
        
        }
//		
        
        timeCount ++;
		if(timeCount >= 25)	//���ͼ��3S
		{	
			timeCount = 0;
			OneNet_SendData();	//��������
			ESP8266_Clear();    //��ջ���
            printf("mode = %d   th_temp = %d  th_hum = %d  th_Mq135= %d   th_mq2 = %d   th_fire= %d  th_Lighting = %d \r\n",mode,th_temp,th_humidity,th_MQ135,th_MQ2,th_fire,th_Lighting);
		}
		
//==============================================����OneNet���������������ݣ����Բ��ùܣ�===============================			
		dataPtr = ESP8266_GetIPD(0);
		if(dataPtr != NULL)
		{
			OneNet_RevPro(dataPtr);
		}
        
        

	
	
	}


}
















