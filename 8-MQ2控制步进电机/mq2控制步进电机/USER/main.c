//=============================================================================
//文件名称：main.h
//功能概要：STM32F103C8核心检测
//版权所有：源地工作室www.vcc-gnd.com
//版权更新：2013-02-20
//调试方式：J-Link OB ARM SW方式 5MHz
//=============================================================================

//头文件
#include "stm32f10x.h"
#include "GPIOLIKE51.h"

#include "delay.h"
#include "sys.h"
#include "usart.h"

#include "oled.h"

//网络协议层
#include "onenet.h"
//网络设备
#include "esp8266.h"

#include "stdio.h"
#include "string.h"
#include "adc.h"
//函数声明
void GPIO_Configuration(void);


u16 ADC_Value[10];
extern uint8_t net_connect;

u8 buf1[10]={0};

u16 i;   //for循环用到的变量而已
u16 fire_value;
u16 timeCount = 0;	//发送间隔变量
float value1 = 0,Gas;
float temp1;
u16 n=0;
u8 flag = 2;    // 2 为关门  1 为开门
u8 mode = 1;   // 1 为自动  2 为手动
u8 th_Gas = 0 ;
u8 num = 0;
//=============================================================================
//文件名称：Delay
//功能概要：延时
//参数说明：nCount：延时长短
//函数返回：无
//=============================================================================

void Delay(uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}


//========================================
u8 phasecw[8] ={0x08,0x0c,0x04,0x06,0x02,0x03,0x01,0x09};// 逆时针
//1000,1100,0100,0110,0010,0011,0001,1001
u8 phaseccw[8]={0x09,0x01,0x03,0x02,0x06,0x04,0x0c,0x08};// 顺时针
//1001,0001,0011,0010,0110,0100,1100,1000

//引脚初始化
void Step_Motor_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
    GPIO_Init(GPIOA,&GPIO_InitStructure);   
}
//引脚映射
void SetMotor(unsigned char InputData)//根据传送的数据Inputdata设置脉冲引脚输出
{//根据传送的数据Inputdata设置PB5-PB8引脚输出
    if(InputData&0x01)
    {
        GPIO_SetBits(GPIOA,GPIO_Pin_5);
    }
    else
    {
        GPIO_ResetBits(GPIOA,GPIO_Pin_5);
    }
    if(InputData&0x02)
    {
        GPIO_SetBits(GPIOA,GPIO_Pin_6);
    }
    else
    {
        GPIO_ResetBits(GPIOA,GPIO_Pin_6);

    }
    if(InputData&0x04)
    {
        GPIO_SetBits(GPIOA,GPIO_Pin_7);
    }
    else
    {
        GPIO_ResetBits(GPIOA,GPIO_Pin_7);
    }
    if(InputData&0x08)
    {
        GPIO_SetBits(GPIOA,GPIO_Pin_4);
    }
    else
    {
        GPIO_ResetBits(GPIOA,GPIO_Pin_4);
    }
}
//步距角5.625 360/5.625=64 减速比1/64 
//故64*64个脉冲转一圈
//n圈数 
//position 方向
void motorNcircle(int n,u8 position)//n为步进电机转动的圈数，position为正转或者反转
{
    int i,j,k=0;
    for(j=0;j<n;j++)
    {
       for(i=0;i<64*8;i++)
       // for(i=0;i<8;i++)
        {//4相单双8拍
					for(k=0;k<8;k++)
            {
                if(1 == position)
                {
                    SetMotor(phasecw[k]);
                }
                else
                {
                    SetMotor(phaseccw[k]);
                }
                delay_ms(1);//延时：速度控制，数值越大速度越慢。电流0.65A
            }
        }
    }
}






//=================================
//=============================================================================
//文件名称：main
//功能概要：主函数
//参数说明：无
//函数返回：int
//=============================================================================
int main(void)
{
    unsigned char *dataPtr = NULL;

    const char *topics[] = {"two","$creq"};
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组
	delay_init();			          //延时函数初始化
    Usart1_Init(115200);							//串口1，打印信息用
	Usart2_Init(115200);
    ADCx_Init();

	OLED_Init(); 	              //OLED初始化  
    show();
    Step_Motor_GPIO_Init();     // 步进电机初始化

    th_Gas = 15;
	                     //OLED显示主界面
	GPIO_Configuration();
//    PBout(9)=1;
//    delay_ms(10);
//    PBout(9)=0;
//    
//    
    
//    delay_ms(10);
    PCout(13)=1;
    while(1)
    {
        if(net_connect == 1)
        {
            timeCount ++;
            
            if(mode == 1)   // 自动
            {
                OLED_ShowStr(32,6,":",2);OLED_ShowCN(42,6,73); OLED_ShowCN(58,6,74); 
            }
            else
            {
                OLED_ShowStr(32,6,":",2);OLED_ShowCN(42,6,75); OLED_ShowCN(58,6,74);
            }
            
            if(flag == 1)
            {
                OLED_ShowCN(88,4,76); OLED_ShowCN(104,4,78); // 开门
            }
            else
            {
                 OLED_ShowCN(88,4,77); OLED_ShowCN(104,4,78); // 关门
            }
//=======================（光照，声音，有害气体）三个传感器数据的获取，通过ADC规则通道查询获取========	
            if(timeCount%200 == 0)
            {   
                for(i=0,value1=0;i<40;)
                {
                    value1+=ADC_Value[i++];	
                }	
                
                Gas = ((((float)value1/10) / 4096.0f * 5.0f) - 0.4f)/ 0.4f+5;
                memset(buf1,0,sizeof(buf1));
                sprintf((char *)buf1,":%.2f%%",Gas);
                OLED_ShowStr(32,0,buf1,2);//OLED显示亮度
                
                memset(buf1,0,sizeof(buf1));
                sprintf((char *)buf1,":%d",th_Gas);
                OLED_ShowStr(64,2,buf1,2);//OLED显示亮度

                value1=0;
                      // 阈值判断
                if(mode==1)
                {
                    if(th_Gas<Gas&&num != 1)
                    {
                        PCout(13)=0;    // 亮灯
                        PBout(9)=0;
                        OLED_ShowStr(32,4,":",2); OLED_ShowCN(42,4,66); OLED_ShowCN(58,4,68); // 电机正转
                        motorNcircle(1,1);//n为步进电机转动的圈数，position为正转或者反转
                        flag = 1;    // 开门
                        OLED_ShowStr(32,4,":",2); OLED_ShowCN(42,4,79); OLED_ShowCN(58,4,80); // 电机停止
                        num = 1;   // 卡转一次
                        
                    }else if(th_Gas>=Gas&&num !=2)
                    {
                        
                        PCout(13) =  1;  // 关灯
                        PBout(9)=1;
                        OLED_ShowStr(32,4,":",2); OLED_ShowCN(42,4,67); OLED_ShowCN(58,4,68); // 电机反转
                        motorNcircle(1,0);//n为步进电机转动的圈数，position为正转或者反转
                        OLED_ShowStr(32,4,":",2); OLED_ShowCN(42,4,79); OLED_ShowCN(58,4,80); // 电机停止
                        flag = 2;   // 关门
                        num = 2;   // 卡一次
                    }
                }


            }

            
            
            
            if(timeCount > 400)	//发送间隔3S
            {	
                timeCount = 0;
                OneNet_SendData();	//发送数据
                ESP8266_Clear();    //清空缓存
//                printf("mode = %d   th_temp = %d  th_hum = %d  th_Mq135= %d   th_mq2 = %d   th_fire= %d  th_Lighting = %d \r\n",mode,th_temp,th_humidity,th_MQ135,th_MQ2,th_fire,th_Lighting);
            }
//            
////==============================================接受OneNet服务器发来的数据（可以不用管）===============================			
            dataPtr = ESP8266_GetIPD(0);
            if(dataPtr != NULL)
            {
                OneNet_RevPro(dataPtr);
            }
        }
        else
        {
            printf("开始连接！！！\r\n");
            ESP8266_Init();					//初始化ESP8266
	
            while(OneNet_DevLink())			//接入OneNET
                delay_ms(500);
            
            OneNet_Subscribe(topics, 1);
            net_connect = 1;
        
        }
    
    
    }
    

}

//=============================================================================
//文件名称：GPIO_Configuration
//功能概要：GPIO初始化
//参数说明：无
//函数返回：无
//=============================================================================
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE); 
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE);     
//=============================================================================
//LED -> PC13    BEEP ->PB9
//=============================================================================			 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 

    GPIO_Init(GPIOC, &GPIO_InitStructure);
}




