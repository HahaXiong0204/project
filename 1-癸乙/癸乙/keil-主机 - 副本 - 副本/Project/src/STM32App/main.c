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

//λ������,ʵ��51���Ƶ�GPIO���ƹ���
//����ʵ��˼��,�ο�<<CM3Ȩ��ָ��>>������(87ҳ~92ҳ).
//IO�ڲ����궨��
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO�ڵ�ַӳ��
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
 
//IO�ڲ���,ֻ�Ե�һ��IO��!
//ȷ��n��ֵС��16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //��� 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //���� 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //���� 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //��� 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //���� 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //��� 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //���� 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //��� 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //����

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //��� 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //����

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //��� 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //����

//����Ϊ��ຯ��
void WFI_SET(void);		//ִ��WFIָ��
void INTX_DISABLE(void);//�ر������ж�
void INTX_ENABLE(void);	//���������ж�
void MSR_MSP(u32 addr);	//���ö�ջ��ַ


#define EnableINT()   INTX_ENABLE() 


#define DisableINT()  INTX_DISABLE()

static u8  fac_us=0;							//us��ʱ������			   
static u16 fac_ms=0;							//ms��ʱ������,��ucos��,����ÿ�����ĵ�ms��
//void delay_init()
//{
//#if SYSTEM_SUPPORT_OS  							//�����Ҫ֧��OS.
//	u32 reload;
//#endif
//	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//ѡ���ⲿʱ��  HCLK/8
//	fac_us=SystemCoreClock/8000000;				//Ϊϵͳʱ�ӵ�1/8  
//#if SYSTEM_SUPPORT_OS  							//�����Ҫ֧��OS.
//	reload=SystemCoreClock/8000000;				//ÿ���ӵļ������� ��λΪM  
//	reload*=1000000/delay_ostickspersec;		//����delay_ostickspersec�趨���ʱ��
//												//reloadΪ24λ�Ĵ���,���ֵ:16777216,��72M��,Լ��1.86s����	
//	fac_ms=1000/delay_ostickspersec;			//����OS������ʱ�����ٵ�λ	   

//	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//����SYSTICK�ж�
//	SysTick->LOAD=reload; 						//ÿ1/delay_ostickspersec���ж�һ��	
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//����SYSTICK    

//#else
//	fac_ms=(u16)fac_us*1000;					//��OS��,����ÿ��ms��Ҫ��systickʱ����   
//#endif
//}	
////��ʱnus
////nusΪҪ��ʱ��us��.		    								   
//void delay_us(u32 nus)
//{		
//	u32 temp;	    	 
//	SysTick->LOAD=nus*fac_us; 					//ʱ�����	  		 
//	SysTick->VAL=0x00;        					//��ռ�����
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//��ʼ����	  
//	do
//	{
//		temp=SysTick->CTRL;
//	}while((temp&0x01)&&!(temp&(1<<16)));		//�ȴ�ʱ�䵽��   
//	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//�رռ�����
//	SysTick->VAL =0X00;      					 //��ռ�����	 
//}
////��ʱnms
////ע��nms�ķ�Χ
////SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
////nms<=0xffffff*8*1000/SYSCLK
////SYSCLK��λΪHz,nms��λΪms
////��72M������,nms<=1864 
//void delay_ms(u16 nms)
//{	 		  	  
//	u32 temp;		   
//	SysTick->LOAD=(u32)nms*fac_ms;				//ʱ�����(SysTick->LOADΪ24bit)
//	SysTick->VAL =0x00;							//��ռ�����
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//��ʼ����  
//	do
//	{
//		temp=SysTick->CTRL;
//	}while((temp&0x01)&&!(temp&(1<<16)));		//�ȴ�ʱ�䵽��   
//	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//�رռ�����
//	SysTick->VAL =0X00;       					//��ռ�����	  	    
//} 

int  wendu1=0,wendu2=0;	//�¶�1�¶�2 
int  yw1=0,yw2=0;//yw1yw2  
int hy1=0,hy2=0;//0�޻�  1�л�
int  wenduSD=45,yw=60;//�����¶� CO �趨ֵ
int  moshi=0;						//0-�¶� 1CO


void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);	 //ENABLEPBCLK
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE); //Disable jtag
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	//LCD1602 �ܽ�			 D0~D7 ˫��I/O
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15| GPIO_Pin_3| GPIO_Pin_4| GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8| GPIO_Pin_11| GPIO_Pin_12| GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
	//LCD1602 �ܽ�	    ���� ����ģʽ
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 |GPIO_Pin_13|GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	//������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	//����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOB, &GPIO_InitStructure);	
}


int RX1_Num=0;//������յ�����
int RX1_JS=0,RX1_KG=0;//������յ�����
int RX1_OK=0;//1�������յ����� 0����û���յ�����
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
�����ַ���
----------------------------*/
void SendString(char *s)
{
    while (*s)                  //����ַ���������־
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
		//��ʼ������ܵ�������
		//�жϽ��ܵ������ݳ���			 
        if(shumu==5)
		{
			//�ӻ�1����
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
//����1�����ж�
void USART1_IRQHandler(void)  
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{	
		  USART_ClearITPendingBit(USART1, USART_IT_RXNE);	
		  //�����յ�������
		  RX1_BUFF[RX1_Num++]=USART_ReceiveData(USART1);	//��ȡ���յ�������
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

//����԰�������ɨ��
void anjiansaomiao(void)
{
	static u8_t k1,k2,k3;
	static u8_t K1Flag=0,K2Flag=0,K3Flag=0;	
	//����ɨ�裬��ȡ����״̬
	k1 = GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_6);//�����������
	k2 = GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_7);//��
	k3 = GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_8);//��
	
	//�����������
	//��������
	if(k1 == 0)
	{
		K1Flag = 1;
	}
	else
	{
		//�ͷź������Ӧ
		if(K1Flag )
		{
			K1Flag = 0;
			//
	    if(moshi == 0)
			{
				moshi = 1;//0-�¶� 1ʪ�� 2CO  
			}
	    else if(moshi == 1)
			{
				moshi = 0;
			}			
		}
	}
	//+	
	//��������
	if(k2 == 0)
	{
		K2Flag = 1;
			
	}
	else
	{
		//�ͷź������Ӧ
		if(K2Flag )
		{
			K2Flag = 0;
	    if(moshi == 0)
			{
				wenduSD++; //0-�¶� 1CO 
			}
	    if(moshi == 1)
			{
				yw++; //0-�¶� 1CO 
			}	
		
		}
	}	
	
	
	//-
	//��������
	if(k3 == 0)
	{
		K3Flag = 1;
	}
	else
	{
		//�ͷź������Ӧ
		if(K3Flag )
		{
			K3Flag = 0;
	    if(moshi == 0)
			{
				wenduSD--; //0-�¶� 1CO
			}
	    if(moshi == 1)
			{
				yw--; //0-�¶� 1CO
			}
			
		}
	}
}

/*
��LCD1602������ʾ
*/
void Display_1(int T1,int T2,int T3,int T4,int T5,int T6,int T7,int T8)
{ 
	char xx=0,yy=0;
	//T1 �¶�
	WrByte1602(yy,xx++,'W'); //.	
	WrByte1602(yy,xx++,AsciiCode[T1%100/10]);
	WrByte1602(yy,xx++,AsciiCode[T1%10]);
	WrByte1602(yy,xx++,' ');	
	//T2 ����
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
//	//�¶�
//	WrByte1602(yy,xx++,'W'); //.	
//	WrByte1602(yy,xx++,AsciiCode[T4%100/10]);
//	WrByte1602(yy,xx++,AsciiCode[T4%10]);
//	WrByte1602(yy,xx++,' ');	
//	// ����
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
	//T5 ����ֵ
	yy=1;
	xx =12;	
	WrByte1602(yy,xx++,AsciiCode[T8%100/10]);
	WrByte1602(yy,xx++,AsciiCode[T8%10]);	


}

void TIM2_Config(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
	NVIC_InitTypeDef           NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʹ��TIM2ʱ��
	//����жϱ�־λ
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//TIM_ClearFlag(TIM2,TIM_FLAG_Update);//����������ͬ
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);  //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	TIM_ITConfig(TIM2,TIM_IT_Update, ENABLE);     //ʹ��ָ����TIM2�ж�,��������ж�
 
	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
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
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//��������жϱ�־λ
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
	//��������	
	GPIO_Configuration();
	//����������	
	FMQ=1;   
	//��ʼ��LCD1602
	Init1602();
	USART1_init();	
    Usart2_Init(115200); 	      //����2����ESP8266ͨ��
	delay_ms(1000);
    
    TIM2_Config(1000-1,7200-1);
    
    ESP8266_Init();					    //��ʼ��ESP8266
	delay_ms(500);
	while(OneNet_DevLink())			//����OneNET
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
		//����LORA����	
		chuli1();			
		RX1_CHULI();	
		//����԰�������ɨ��
		anjiansaomiao();
		//���ﱨ��
		//�����¶ȳ����¶ȱ���ֵ ���� �������趨ֵ���л����򱨾�
		if(wendu1>wenduSD ||wendu2>wenduSD||yw1>yw||yw2>yw||hy1==1||hy2==1)
		{
			//��������	
            
            
			
            if((((wendu1-wenduSD)>=2&&(wendu1-wenduSD)<5)||((wendu2-wenduSD)>=2&&(wendu2-wenduSD)<5)))
            {
                FMQ_state = 2;
                TIM_Cmd(TIM2, ENABLE);     //ʹ�ܶ�ʱ��2
            }
            else if((wendu1-wenduSD>=5)||((wendu2-wenduSD>=5)))
            {
                FMQ_state = 1;
                stat = 1;
                TIM_Cmd(TIM2, ENABLE);     //ʹ�ܶ�ʱ��2
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
			//����������	
			FMQ=1; 
            TIM_Cmd(TIM2, DISABLE);
		}
		//
		ct++;
		if(ct==60)
		{
		 SendString("1111");//����ӻ�1�ϴ�����	
		}
		else if(ct>=120)
		{
			ct=0;
		 SendString("2222");//����ӻ�2�ϴ�����					
		}
        
        
        //=============================================�ϴ����ݵ�OneNet�����������Բ��ùܣ�=================================			
        delay_ms(10);
		timeCount ++;
		if(timeCount >= 100)	//���ͼ��3S
		{	
			timeCount = 0;
			OneNet_SendData();	//��������
			ESP8266_Clear();    //��ջ���
		}
		
        
        
        
	}
}



/**
  * @brief  ��ʱ��3�жϷ�����
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
