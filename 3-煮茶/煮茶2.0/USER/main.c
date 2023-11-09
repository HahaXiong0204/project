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
#include "Water.h"
#include "HX711.h"
#include "bo_key.h"
#include "ds18b20.h"
u16 water_level;
extern s32 Weight_Shiwu;
u16 ADC_Value[10];         //adc���ݻ�����
float temperature;
u16 water_threshold,tem_threshold;    // ��ֵ
u16 thre_temp;
u8 tea;
u8 water_flag,fan_flag,heat_flag,xia_water_flag;     // ˮ�á����ȡ��������ı�־λ
u8 flag = 0;
u8 buf1[10]={0};
u8 buf2[10]={0};
u8 buf3[10]={0};
u8 buf4[10]={0};
u8 buf5[10]={0};
u8 buf6[10]={0};


void relay_init(void);
void relay_fan_off(void);
void relay_fan_on(void);
void relay_shui_off(void);
void relay_shui_on(void);
void relay_xia_shui_off(void);
void relay_xia_shui_on(void);
void relay_heat_on(void);
void relay_heat_off(void);

void key_set(void);

float temper = 0.0;
int main(void)
{
	u8 key;
	unsigned short timeCount = 0;	//���ͼ������
	unsigned char *dataPtr = NULL;
    const char *topics[] = {"TWO_T"};
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����
	delay_init();			          //��ʱ������ʼ��
	Usart1_Init(115200);							//����1����ӡ��Ϣ��
	Usart2_Init(115200);
	OLED_Init(); 	              //OLED��ʼ��  
    relay_init();
//    while(1){}
    
    Buzzer_Init();
	Water_Init();
    
	KEY_Init();
  
    flag=0;  
	DS18B20_Init();

    ESP8266_Init();					//��ʼ��ESP8266

    
	while(OneNet_DevLink())			//����OneNET
		delay_ms(500);
	
	OneNet_Subscribe(topics, 1);

//    relay_shui_on();
    delay_ms(1000);
    Init_HX711pin();
    Get_Maopi();				//��ëƤ����
    delay_ms(1000);
	delay_ms(1000);
	Get_Maopi();				//���»�ȡëƤ����
//    relay_shui_off();
//	
    tem_threshold = 20;
    water_threshold = 20;
    
    Buzzer = Buzzer_OFF;
    
	while(1) 
	{
        key_set();

        show();                     //OLED��ʾ������
        
        timeCount ++;
        delay_ms(50);
        temperature=DS18B20_GetTemperture();
        water_level = Get_Water_Adc_Average(10)*40/4096;
        Get_Weight();
        if(timeCount%10==0)
        {      
//==============================================oled��ʾ======================================================================
            memset(buf1,0,sizeof(buf1));
            memset(buf2,0,sizeof(buf2));
            memset(buf3,0,sizeof(buf3));
            sprintf((char *)buf1,":%d%",water_level);        
            sprintf((char *)buf2,":%.3f",(float)(temperature));
            sprintf((char *)buf3,":%dg",Weight_Shiwu);
            printf("water_level = %d   temperature  = %.3f Weight_Shiwu = %dg \r\n",water_level,temperature,Weight_Shiwu);
            if(flag == 0)
            {
                OLED_CLS();
                show();
                OLED_ShowStr(32,2,buf3,2); // ��ʾ����
                OLED_ShowStr(32,0,buf2,2);//OLED��ʾ�¶�
                OLED_ShowStr(32,4,buf1,2);//OLED��ʾˮλ

                
            }
        }
		if(timeCount >= 30)	//���ͼ��3S
		{
            timeCount = 0;
            OneNet_SendData();	//��������
            ESP8266_Clear();    //��ջ���
		}

		
//==============================================��ֵ����=================================================================//
        // ˮλ����
        if(water_level<=water_threshold)
        {
            relay_shui_on();
            printf("����ˮ��\r\n");
            Buzzer = Buzzer_ON;
            
        }
        if(water_level>=water_threshold){
            relay_shui_off();
            Buzzer = Buzzer_OFF;
            printf("�ر�ˮ��\r\n");
        }
       // �¶ȿ���
        if(temperature<=tem_threshold-4)
        {
            relay_fan_off();
            relay_heat_on();
        }else if(temperature>=tem_threshold+4){
            relay_fan_on();
            relay_heat_off();
        }else{
            relay_fan_off();
            relay_heat_off();
        }            
        
        
//==============================================����OneNet���������������ݣ����Բ��ùܣ�===============================			
		dataPtr = ESP8266_GetIPD(0);
		if(dataPtr != NULL)
		{
			OneNet_RevPro(dataPtr);
		}
        
        

	
	
	}


}

void key_set(void)
{
     char i;
     u8 key = 0;
    delay_ms(100);
    key = KEY_Scan(0);
    if(key==0)
    {
        return;
    }
    else{
        // ����
        if(key==1)    //����  ȷ��
        {
           

            if(flag == 3||flag ==4)
            {
                thre_temp++;
            }
            if(flag == 1){
                flag = 3;    // �����¶�����
                thre_temp = tem_threshold;
                OLED_CLS();
            }
            if(flag ==0){
                OLED_CLS();
                flag = 1;   //����״̬

            }
            
            if(flag==5)
            {
                tea = 1;
                OLED_CLS();
                flag = 0;   //����״̬
            }
            
        }
        else if(flag != 0&&key==2)
        {

            if(flag == 3||flag ==4)        //�Ӽ�
            {
                thre_temp--;
            }
            if(flag==1){
                thre_temp = water_threshold;
                flag = 4;   // ����ˮλ����
                OLED_CLS();
            }    

            if(flag==5)
            {
                tea = 2;
                OLED_CLS();
                flag = 0;   //����״̬
            }
        }
        else if(flag != 0&&key==3)
        {
            if(flag==5)
            {
                tea = 3;
                OLED_CLS();
                flag = 0;   //����״̬
                
            }
            
            if(flag ==1)
            {
                flag = 5;    // ����ѡ�����
                OLED_CLS();
            }
            if(flag == 3||flag == 4)   // ȷ��
            {
                if(flag==3)
                {
                    tem_threshold = thre_temp;
                }else{
                    water_threshold = thre_temp;
                }
                thre_temp = 0;
                flag = 0;
                OLED_CLS();
            }

        }
        else if(key==4)
        {
            if(flag == 0)
            {
                if(xia_water_flag == 0)
                    relay_xia_shui_on();
                else
                    relay_xia_shui_off();
            }
            if(flag == 3||flag == 4||flag==5)    // ����
            {
                flag = 0;
                OLED_CLS();
            }
            if(flag == 1)
            {
                flag = 0;
                OLED_CLS();
            }
        }
        
    }
		
}


void relay_init(void)
{
    GPIO_InitTypeDef     GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_12|GPIO_Pin_14|GPIO_Pin_15|GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;//
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    
    
    GPIO_ResetBits(GPIOA,GPIO_Pin_12);
    GPIO_ResetBits(GPIOB,GPIO_Pin_5);
    GPIO_ResetBits(GPIOB,GPIO_Pin_12);
    GPIO_ResetBits(GPIOB,GPIO_Pin_14);
    GPIO_ResetBits(GPIOB,GPIO_Pin_15);
    GPIO_ResetBits(GPIOB,GPIO_Pin_9);
    water_flag = 0;
    fan_flag = 0;
    heat_flag = 0;
}        



void relay_fan_on(void)
{
	 GPIO_SetBits(GPIOB,GPIO_Pin_5);
    fan_flag = 1;
    
}

void relay_fan_off(void)
	
{ 
    GPIO_ResetBits(GPIOB,GPIO_Pin_5);
    fan_flag = 0;
}



void relay_shui_on(void)
{

	GPIO_SetBits(GPIOB,GPIO_Pin_12);
    water_flag = 1;
}

void relay_shui_off(void)
{

    GPIO_ResetBits(GPIOB,GPIO_Pin_12);
    water_flag = 0;
}


void relay_heat_on(void)
{
    GPIO_SetBits(GPIOA,GPIO_Pin_12);
    heat_flag = 1;
}

void relay_heat_off(void)
{
    GPIO_ResetBits(GPIOA,GPIO_Pin_12);
    heat_flag = 0;
}

void relay_xia_shui_off(void)
{

    GPIO_ResetBits(GPIOB,GPIO_Pin_9);
    xia_water_flag = 0;
}

void relay_xia_shui_on(void)
{
    GPIO_SetBits(GPIOB,GPIO_Pin_9);
    xia_water_flag = 1;
}
















