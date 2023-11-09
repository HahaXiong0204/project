#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "dht11.h"
#include "oled.h"

//网络协议层
#include "onenet.h"
//网络设备
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
u16 ADC_Value[10];         //adc数据缓冲区
float temperature;
u16 water_threshold,tem_threshold;    // 阈值
u16 thre_temp;
u8 tea;
u8 water_flag,fan_flag,heat_flag,xia_water_flag;     // 水泵、风扇、加热器的标志位
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
	unsigned short timeCount = 0;	//发送间隔变量
	unsigned char *dataPtr = NULL;
    const char *topics[] = {"TWO_T"};
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组
	delay_init();			          //延时函数初始化
	Usart1_Init(115200);							//串口1，打印信息用
	Usart2_Init(115200);
	OLED_Init(); 	              //OLED初始化  
    relay_init();
//    while(1){}
    
    Buzzer_Init();
	Water_Init();
    
	KEY_Init();
  
    flag=0;  
	DS18B20_Init();

    ESP8266_Init();					//初始化ESP8266

    
	while(OneNet_DevLink())			//接入OneNET
		delay_ms(500);
	
	OneNet_Subscribe(topics, 1);

//    relay_shui_on();
    delay_ms(1000);
    Init_HX711pin();
    Get_Maopi();				//称毛皮重量
    delay_ms(1000);
	delay_ms(1000);
	Get_Maopi();				//重新获取毛皮重量
//    relay_shui_off();
//	
    tem_threshold = 20;
    water_threshold = 20;
    
    Buzzer = Buzzer_OFF;
    
	while(1) 
	{
        key_set();

        show();                     //OLED显示主界面
        
        timeCount ++;
        delay_ms(50);
        temperature=DS18B20_GetTemperture();
        water_level = Get_Water_Adc_Average(10)*40/4096;
        Get_Weight();
        if(timeCount%10==0)
        {      
//==============================================oled显示======================================================================
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
                OLED_ShowStr(32,2,buf3,2); // 显示重量
                OLED_ShowStr(32,0,buf2,2);//OLED显示温度
                OLED_ShowStr(32,4,buf1,2);//OLED显示水位

                
            }
        }
		if(timeCount >= 30)	//发送间隔3S
		{
            timeCount = 0;
            OneNet_SendData();	//发送数据
            ESP8266_Clear();    //清空缓存
		}

		
//==============================================阈值控制=================================================================//
        // 水位控制
        if(water_level<=water_threshold)
        {
            relay_shui_on();
            printf("开启水泵\r\n");
            Buzzer = Buzzer_ON;
            
        }
        if(water_level>=water_threshold){
            relay_shui_off();
            Buzzer = Buzzer_OFF;
            printf("关闭水泵\r\n");
        }
       // 温度控制
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
        
        
//==============================================接受OneNet服务器发来的数据（可以不用管）===============================			
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
        // 进入
        if(key==1)    //设置  确定
        {
           

            if(flag == 3||flag ==4)
            {
                thre_temp++;
            }
            if(flag == 1){
                flag = 3;    // 进入温度设置
                thre_temp = tem_threshold;
                OLED_CLS();
            }
            if(flag ==0){
                OLED_CLS();
                flag = 1;   //设置状态

            }
            
            if(flag==5)
            {
                tea = 1;
                OLED_CLS();
                flag = 0;   //设置状态
            }
            
        }
        else if(flag != 0&&key==2)
        {

            if(flag == 3||flag ==4)        //加减
            {
                thre_temp--;
            }
            if(flag==1){
                thre_temp = water_threshold;
                flag = 4;   // 进入水位设置
                OLED_CLS();
            }    

            if(flag==5)
            {
                tea = 2;
                OLED_CLS();
                flag = 0;   //设置状态
            }
        }
        else if(flag != 0&&key==3)
        {
            if(flag==5)
            {
                tea = 3;
                OLED_CLS();
                flag = 0;   //设置状态
                
            }
            
            if(flag ==1)
            {
                flag = 5;    // 进入选择茶类
                OLED_CLS();
            }
            if(flag == 3||flag == 4)   // 确定
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
            if(flag == 3||flag == 4||flag==5)    // 返回
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
















