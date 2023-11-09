#include "stm32f10x.h"
#include "1602.h"
#include "DS18B20.h"
#include "math.h" 
#include "esp8266.h"
#include "MqttKit.h"
#include "onenet.h"

typedef unsigned   char    u8_t;
typedef signed     char    s8_t;
typedef unsigned   short   u16_t;
typedef signed     short   s16_t;
typedef unsigned   long    u32_t;
typedef signed     long    s32_t;

//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

//以下为汇编函数
void WFI_SET(void);		//执行WFI指令
void INTX_DISABLE(void);//关闭所有中断
void INTX_ENABLE(void);	//开启所有中断
void MSR_MSP(u32 addr);	//设置堆栈地址


#define EnableINT()   INTX_ENABLE() 


#define DisableINT()  INTX_DISABLE()

static u8  fac_us=0;							//us延时倍乘数			   
static u16 fac_ms=0;							//ms延时倍乘数,在ucos下,代表每个节拍的ms数
//void delay_init()
//{
//#if SYSTEM_SUPPORT_OS  							//如果需要支持OS.
//	u32 reload;
//#endif
//	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8
//	fac_us=SystemCoreClock/8000000;				//为系统时钟的1/8  
//#if SYSTEM_SUPPORT_OS  							//如果需要支持OS.
//	reload=SystemCoreClock/8000000;				//每秒钟的计数次数 单位为M  
//	reload*=1000000/delay_ostickspersec;		//根据delay_ostickspersec设定溢出时间
//												//reload为24位寄存器,最大值:16777216,在72M下,约合1.86s左右	
//	fac_ms=1000/delay_ostickspersec;			//代表OS可以延时的最少单位	   

//	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//开启SYSTICK中断
//	SysTick->LOAD=reload; 						//每1/delay_ostickspersec秒中断一次	
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//开启SYSTICK    

//#else
//	fac_ms=(u16)fac_us*1000;					//非OS下,代表每个ms需要的systick时钟数   
//#endif
//}	
////延时nus
////nus为要延时的us数.		    								   
//void delay_us(u32 nus)
//{		
//	u32 temp;	    	 
//	SysTick->LOAD=nus*fac_us; 					//时间加载	  		 
//	SysTick->VAL=0x00;        					//清空计数器
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数	  
//	do
//	{
//		temp=SysTick->CTRL;
//	}while((temp&0x01)&&!(temp&(1<<16)));		//等待时间到达   
//	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
//	SysTick->VAL =0X00;      					 //清空计数器	 
//}
////延时nms
////注意nms的范围
////SysTick->LOAD为24位寄存器,所以,最大延时为:
////nms<=0xffffff*8*1000/SYSCLK
////SYSCLK单位为Hz,nms单位为ms
////对72M条件下,nms<=1864 
//void delay_ms(u16 nms)
//{	 		  	  
//	u32 temp;		   
//	SysTick->LOAD=(u32)nms*fac_ms;				//时间加载(SysTick->LOAD为24bit)
//	SysTick->VAL =0x00;							//清空计数器
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数  
//	do
//	{
//		temp=SysTick->CTRL;
//	}while((temp&0x01)&&!(temp&(1<<16)));		//等待时间到达   
//	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
//	SysTick->VAL =0X00;       					//清空计数器	  	    
//} 

int  wendu1=0,wendu2=0;	//温度1温度2 
int  yw1=0,yw2=0;//yw1yw2  
int hy1=0,hy2=0;//0无火  1有火
int  wenduSD=45,yw=60;//保存温度 CO 设定值
int  moshi=0;						//0-温度 1CO


void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);	 //ENABLEPBCLK
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE); //Disable jtag
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	//LCD1602 管脚			 D0~D7 双向I/O
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15| GPIO_Pin_3| GPIO_Pin_4| GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8| GPIO_Pin_11| GPIO_Pin_12| GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
	//LCD1602 管脚	    控制 推挽模式
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 |GPIO_Pin_13|GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	//蜂鸣器
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	//按键
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOB, &GPIO_InitStructure);	
}


int RX1_Num=0;//保存接收的数据
int RX1_JS=0,RX1_KG=0;//保存接收的数据
int RX1_OK=0;//1表明接收到数据 0表明没有收到数据
unsigned char RX1_BUFF[200];
unsigned char TX1_BUFF[200];	
void USART1_init(void)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_AFIO   , ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	
	USART_InitStructure.USART_BaudRate = 9600; 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 
	USART_InitStructure.USART_Parity = USART_Parity_No; 
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; 
 
	//USART1
	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE); 	
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_PE, ENABLE);  
	USART_ITConfig(USART1, USART_IT_ERR, ENABLE);		
	
		USART1->DR = (0X55 & (uint16_t)0x01FF);
		while((USART1->SR&0X40)==0);		
		
	
 	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}
/*----------------------------
发送字符串
----------------------------*/
void SendString(char *s)
{
    while (*s)                  //检测字符串结束标志
    {
     USART1->DR = (*s++& (uint16_t)0x01FF);
		 while((USART1->SR&0X40)==0);
    }
}


void RX1_CHULI(void)
{
	long int *p;
	int  shumu=0;
	int  wz=0;	
	char i=0;
	
	if(RX1_OK==1)
	{
		RX1_OK = 0;
		shumu = RX1_Num;
		RX1_Num = 0;	
		//开始处理接受到的数据
		//判断接受到的数据长度			 
        if(shumu==5)
		{
			//从机1发送
			if(RX1_BUFF[0]==0x01  && RX1_BUFF[4]==0x55)
			{   
				wendu1=RX1_BUFF[1];yw1=RX1_BUFF[2];hy1=RX1_BUFF[3];
			}
			else if(RX1_BUFF[0]==0x02  && RX1_BUFF[4]==0x66)
			{
			 wendu2=RX1_BUFF[1];yw2=RX1_BUFF[2];hy2=RX1_BUFF[3];				
			}			
		}
			
		
		
		for(i=0;i<shumu;i++)
		{
		 RX1_BUFF[i]=0;
		}						
	}
}	
void chuli1(void)
{
	 if(RX1_KG==1)
	 {
		 RX1_JS++;
		 if(RX1_JS>=3)
		 {
			 RX1_KG = 0;
			 RX1_JS = 0;
			 RX1_OK = 1;
			 
	 
		 }
	 }
}
//串口1接收中断
void USART1_IRQHandler(void)  
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{	
		  USART_ClearITPendingBit(USART1, USART_IT_RXNE);	
		  //保存收到的数据
		  RX1_BUFF[RX1_Num++]=USART_ReceiveData(USART1);	//读取接收到的数据
		  if(RX1_Num>=200)
			{
				RX1_Num=0;
			}
			 RX1_KG = 1;
			 RX1_JS = 0;

	}
	

	if(USART_GetITStatus(USART1, USART_IT_PE) != RESET)
	{	
		USART_ClearITPendingBit(USART1, USART_IT_PE);
	}		
	if (USART_GetFlagStatus(USART1, USART_IT_LBD) != RESET) 
	{
    USART_ClearITPendingBit(USART1, USART_IT_LBD);           
  }		
	if(USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
	{
		USART_ReceiveData(USART1);
		USART_ClearFlag(USART1, USART_FLAG_ORE);
	}		
	if(USART_GetFlagStatus(USART1, USART_FLAG_NE) != RESET)
	{
		USART_ClearFlag(USART1, USART_FLAG_NE);
	}
	if(USART_GetFlagStatus(USART1, USART_FLAG_FE) != RESET)
	{
		USART_ClearFlag(USART1, USART_FLAG_FE);
	}
	if(USART_GetFlagStatus(USART1, USART_FLAG_PE) != RESET)
	{
		USART_ClearFlag(USART1, USART_FLAG_PE);
	}	
	if (USART_GetITStatus(USART1, USART_IT_TC) != RESET) 
  {
    USART_ClearITPendingBit(USART1, USART_IT_TC);    
  }	
}
void UsartSendData1(unsigned char *send_buff,unsigned long int length)
{
 	unsigned long int i = 0;


	delay_ms(1);
	for(i = 0;i < length;i ++)
	{	
		USART1->DR = (send_buff[i] & (uint16_t)0x01FF);
		while((USART1->SR&0X40)==0);	
	}	
		delay_ms(1);
	
}

//这里对按键进行扫描
void anjiansaomiao(void)
{
	static u8_t k1,k2,k3;
	static u8_t K1Flag=0,K2Flag=0,K3Flag=0;	
	//按键扫描，获取按键状态
	k1 = GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_6);//进入参数设置
	k2 = GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_7);//加
	k3 = GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_8);//减
	
	//进入参数设置
	//表明按下
	if(k1 == 0)
	{
		K1Flag = 1;
	}
	else
	{
		//释放后进行响应
		if(K1Flag )
		{
			K1Flag = 0;
			//
	    if(moshi == 0)
			{
				moshi = 1;//0-温度 1湿度 2CO  
			}
	    else if(moshi == 1)
			{
				moshi = 0;
			}			
		}
	}
	//+	
	//表明按下
	if(k2 == 0)
	{
		K2Flag = 1;
			
	}
	else
	{
		//释放后进行响应
		if(K2Flag )
		{
			K2Flag = 0;
	    if(moshi == 0)
			{
				wenduSD++; //0-温度 1CO 
			}
	    if(moshi == 1)
			{
				yw++; //0-温度 1CO 
			}	
		
		}
	}	
	
	
	//-
	//表明按下
	if(k3 == 0)
	{
		K3Flag = 1;
	}
	else
	{
		//释放后进行响应
		if(K3Flag )
		{
			K3Flag = 0;
	    if(moshi == 0)
			{
				wenduSD--; //0-温度 1CO
			}
	    if(moshi == 1)
			{
				yw--; //0-温度 1CO
			}
			
		}
	}
}

/*
在LCD1602上面显示
*/
void Display_1(int T1,int T2,int T3,int T4,int T5,int T6,int T7,int T8)
{ 
	char xx=0,yy=0;
	//T1 温度
	WrByte1602(yy,xx++,'W'); //.	
	WrByte1602(yy,xx++,AsciiCode[T1%100/10]);
	WrByte1602(yy,xx++,AsciiCode[T1%10]);
	WrByte1602(yy,xx++,' ');	
	//T2 烟雾
	WrByte1602(yy,xx++,'C'); //.		
	WrByte1602(yy,xx++,AsciiCode[T2%100/10]);
	WrByte1602(yy,xx++,AsciiCode[T2%10]);
	WrByte1602(yy,xx++,' ');
  //
  if(T3==0)
	{
	 WrByte1602(yy,xx++,'W'); //.	
	 WrByte1602(yy,xx++,'H');			
	}
	else
	{
	 WrByte1602(yy,xx++,'Y'); //.	
	 WrByte1602(yy,xx++,'H');					
	}
	
	WrByte1602(yy,xx++,' ');			
	WrByte1602(yy,xx++,'S'); //.
	WrByte1602(yy,xx++,'E'); //
	WrByte1602(yy,xx++,'T'); //.
	WrByte1602(yy,xx++,'-'); //
	WrByte1602(yy,xx++,T7); //	
	
	
//	xx=0;
//	yy=1;	
//	//温度
//	WrByte1602(yy,xx++,'W'); //.	
//	WrByte1602(yy,xx++,AsciiCode[T4%100/10]);
//	WrByte1602(yy,xx++,AsciiCode[T4%10]);
//	WrByte1602(yy,xx++,' ');	
//	// 烟雾
//	WrByte1602(yy,xx++,'C'); //.		
//	WrByte1602(yy,xx++,AsciiCode[T5%100/10]);
//	WrByte1602(yy,xx++,AsciiCode[T5%10]);
//	WrByte1602(yy,xx++,' ');
//  //
//  if(T6==0)
//	{
//	 WrByte1602(yy,xx++,'W'); //.	
//	 WrByte1602(yy,xx++,'H');			
//	}
//	else
//	{
//	 WrByte1602(yy,xx++,'Y'); //.	
//	 WrByte1602(yy,xx++,'H');					
//	}	
//	
	//T5 设置值
	yy=1;
	xx =12;	
	WrByte1602(yy,xx++,AsciiCode[T8%100/10]);
	WrByte1602(yy,xx++,AsciiCode[T8%10]);	


}

void TIM2_Config(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
	NVIC_InitTypeDef           NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //使能TIM2时钟
	//清除中断标志位
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//TIM_ClearFlag(TIM2,TIM_FLAG_Update);//两者作用相同
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);  //根据指定的参数初始化TIMx的时间基数单位
	TIM_ITConfig(TIM2,TIM_IT_Update, ENABLE);     //使能指定的TIM2中断,允许更新中断
 
	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);
	
	
}



uint8_t FMQ_state;
uint8_t time;
#define FMQ PBout(9)

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		time++;
        if(FMQ_state==1)
        {
            FMQ = !FMQ;
        }
        if(FMQ_state == 2)
        {
            if(time>=2)
            {
                FMQ = !FMQ;
                time  = 0;
            }
            
        }
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//清除更新中断标志位
	}
}





uint8_t timeCount;
uint8_t stat;
int main(void)
{	
	char ct=0;
	
	float temp=0;
	SystemInit();	
	SystemCoreClockUpdate();
	delay_init();
	//引脚配置	
	GPIO_Configuration();
	//蜂鸣器不响	
	FMQ=1;   
	//初始化LCD1602
	Init1602();
	USART1_init();	
    Usart2_Init(115200); 	      //串口2，与ESP8266通信
	delay_ms(1000);
    
    TIM2_Config(1000-1,7200-1);
    
    ESP8266_Init();					    //初始化ESP8266
	delay_ms(500);
	while(OneNet_DevLink())			//接入OneNET
	delay_ms(500);
    

	while (1)
	{
		delay_ms(10);
		if(moshi==0)
		{
			Display_1(wendu1,yw1,hy1,wendu2,yw2,hy2,'W',wenduSD);
		}
		else
		{
			Display_1(wendu1,yw1,hy1,wendu2,yw2,hy2,'T',yw);
		}			
		//处理LORA数据	
		chuli1();			
		RX1_CHULI();	
		//这里对按键进行扫描
		anjiansaomiao();
		//这里报警
		//任意温度超过温度报警值 或者 烟雾超过设定值，有火焰则报警
		if(wendu1>wenduSD ||wendu2>wenduSD||yw1>yw||yw2>yw||hy1==1||hy2==1)
		{
			//蜂鸣器响	
            
            
			
            if((((wendu1-wenduSD)>=2&&(wendu1-wenduSD)<5)||((wendu2-wenduSD)>=2&&(wendu2-wenduSD)<5)))
            {
                FMQ_state = 2;
                TIM_Cmd(TIM2, ENABLE);     //使能定时器2
            }
            else if((wendu1-wenduSD>=5)||((wendu2-wenduSD>=5)))
            {
                FMQ_state = 1;
                stat = 1;
                TIM_Cmd(TIM2, ENABLE);     //使能定时器2
            }
            
            if(((yw1-yw)>=2&&(yw1-yw)<5)||((yw2-yw)>=2&&(yw2-yw)<5))
            {
                if(stat!=1)
                    FMQ_state = 2;
                TIM_Cmd(TIM2, ENABLE);
            }
            else if((yw1-yw>=5)||((yw2-yw>=5)))
            {
                FMQ_state = 1;
                TIM_Cmd(TIM2, ENABLE);
            }
            
            if(hy1==1||hy2==1)
            {
                TIM_Cmd(TIM2, DISABLE);
                FMQ=0; 
            }
            stat == 0;
                
            
		}
		else
		{
			//蜂鸣器不响	
			FMQ=1; 
            TIM_Cmd(TIM2, DISABLE);
		}
		//
		ct++;
		if(ct==60)
		{
		 SendString("1111");//请求从机1上传数据	
		}
		else if(ct>=120)
		{
			ct=0;
		 SendString("2222");//请求从机2上传数据					
		}
        
        
        //=============================================上传数据到OneNet服务器（可以不用管）=================================			
        delay_ms(10);
		timeCount ++;
		if(timeCount >= 100)	//发送间隔3S
		{	
			timeCount = 0;
			OneNet_SendData();	//发送数据
			ESP8266_Clear();    //清空缓存
		}
		
        
        
        
	}
}



/**
  * @brief  定时器3中断服务函数
  * @param  TIMx: where x can be 1 to 17 to select the TIM peripheral 
  * @param  NewState: new state of the TIMx peripheral.
  *   		This parameter can be: ENABLE or DISABLE.
  * @retval None 
  */  

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   			where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
