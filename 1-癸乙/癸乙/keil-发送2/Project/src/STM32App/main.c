#include "stm32f10x.h"
#include "1602.h"
#include "DS18B20.h"
#include "math.h" 

int wendu=0,yw=0,hy=0;
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
void delay_init()
{
#if SYSTEM_SUPPORT_OS  							//如果需要支持OS.
	u32 reload;
#endif
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8
	fac_us=SystemCoreClock/8000000;				//为系统时钟的1/8  
#if SYSTEM_SUPPORT_OS  							//如果需要支持OS.
	reload=SystemCoreClock/8000000;				//每秒钟的计数次数 单位为M  
	reload*=1000000/delay_ostickspersec;		//根据delay_ostickspersec设定溢出时间
												//reload为24位寄存器,最大值:16777216,在72M下,约合1.86s左右	
	fac_ms=1000/delay_ostickspersec;			//代表OS可以延时的最少单位	   

	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//开启SYSTICK中断
	SysTick->LOAD=reload; 						//每1/delay_ostickspersec秒中断一次	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//开启SYSTICK    

#else
	fac_ms=(u16)fac_us*1000;					//非OS下,代表每个ms需要的systick时钟数   
#endif
}	
//延时nus
//nus为要延时的us数.		    								   
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; 					//时间加载	  		 
	SysTick->VAL=0x00;        					//清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数	  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL =0X00;      					 //清空计数器	 
}
//延时nms
//注意nms的范围
//SysTick->LOAD为24位寄存器,所以,最大延时为:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK单位为Hz,nms单位为ms
//对72M条件下,nms<=1864 
void delay_ms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;				//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;							//清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL =0X00;       					//清空计数器	  	    
} 

char youren=0,kaisuo=0,baojing=0;

void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);	 //ENABLEPBCLK
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE); //Disable jtag
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	//DS18B20
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
	//配置 PA0 ADC采集模式		
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	//按键 红外
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
}


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

void SendData(char dat)
{
	USART1->DR = (dat& (uint16_t)0x01FF);
	 while((USART1->SR&0X40)==0);
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
int RX1_Num=0;//保存接收的数据
int RX1_JS=0,RX1_KG=0;//保存接收的数据
int RX1_OK=0;//1表明接收到数据 0表明没有收到数据
unsigned char RX1_BUFF[200];
unsigned char TX1_BUFF[200];	
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
    if(shumu==4)
		{
			//从机1发送
			if(RX1_BUFF[0]=='2'  && RX1_BUFF[1]=='2'&& RX1_BUFF[2]=='2'&& RX1_BUFF[3]=='2')
			{
				delay_ms(150);
				//通过LORA上传数据
				SendData(0X02);
				SendData(wendu);
				SendData(yw);
				SendData(hy);
				SendData(0X55);
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


/* ADC配置 */
void ADC_Set_0(void)
{
	ADC_InitTypeDef ADC_InitStructure;//ADC结构体变量//注意在一个语句快内变量的声明要放在可执行语句的前面，否则出错，因此要放在ADC1_GPIO_Config();前面

  ADC_Cmd(ADC1, DISABLE);//使能ADC1
  ADC_ITConfig(ADC1, ADC_IT_EOC, DISABLE);
  ADC_SoftwareStartConvCmd(ADC1, DISABLE);		
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADC1和ADC2工作在独立模式
  ADC_InitStructure.ADC_ScanConvMode =        DISABLE; //使能扫描
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//ADC转换工作在连续模式
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//由软件控制转换,不使用外部触发
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//转换数据右对齐
  ADC_InitStructure.ADC_NbrOfChannel = 1;//转换通道为1
  ADC_Init(ADC1, &ADC_InitStructure); //初始化ADC   
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_41Cycles5);


	
  ADC_Cmd(ADC1, ENABLE);//使能ADC1
//  ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
void ADC_Set_1(void)
{
	ADC_InitTypeDef ADC_InitStructure;//ADC结构体变量//注意在一个语句快内变量的声明要放在可执行语句的前面，否则出错，因此要放在ADC1_GPIO_Config();前面

  ADC_Cmd(ADC1, DISABLE);//使能ADC1
  ADC_ITConfig(ADC1, ADC_IT_EOC, DISABLE);
  ADC_SoftwareStartConvCmd(ADC1, DISABLE);	
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADC1和ADC2工作在独立模式
  ADC_InitStructure.ADC_ScanConvMode =        DISABLE; //使能扫描
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//ADC转换工作在连续模式
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//由软件控制转换,不使用外部触发
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//转换数据右对齐
  ADC_InitStructure.ADC_NbrOfChannel = 1;//转换通道为1
  ADC_Init(ADC1, &ADC_InitStructure); //初始化ADC   
  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_28Cycles5);


	
  ADC_Cmd(ADC1, ENABLE);//使能ADC1
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

int main(void)
{	
	char ct=0;
	
	float temp=0;
	SystemInit();	
	SystemCoreClockUpdate();
	delay_init();
	//引脚配置	
	GPIO_Configuration();
	//初始化LCD1602
	Init1602();
  //温度初始化
	ds18b20_init();
  //配置串口
	USART1_init();	
	delay_ms(1000);

	while (1)
	{
		 delay_ms(10);
    //采集并且计算一次烟雾
		ADC_Set_0();
		temp = ADC_GetConversionValue(ADC1);
		temp = 100.0*(temp)/(4095.00)-50;
		yw=temp;
		if(yw<0)
		{
			yw = 0;
		}
		yw = yw*2;
		if(yw>99)
		{
			yw = 99;
		}		
		//读取一次温度		
		wendu=ds18b20_read();
		if(wendu>99)
		{
			wendu=0;
		}
		//采集一次火焰状态
		if(PAin(1)==0)
		{
		 hy=1;			
		}
		else
		{
		 hy=0;				
		}

		//处理LORA数据	
		chuli1();			
		RX1_CHULI();	
		
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
