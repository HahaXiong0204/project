#include "stm32f10x.h"
#include "MQ_Sensor.h"
#include <stdio.h>
#include "oled.h"

	u16 ADC_Val = 0;
/**************************** ADC模块初始化函数 ****************************
功  能：初始ADC_1模块
参  数：无
返回值：无
硬件连接：
			MQ2（可燃气体）   ---> ADC1_IN2（AO） --->PA2
***************************************************************************/
void Adc_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	/* 开启GPIO口和ADC的时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1,ENABLE);
	/* 设置ADC模块工作频率 72/6 = 12M */
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	/* 设置ADC通道管脚对应的GPIO */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;                          //模拟输入引脚
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	/* 设置ADC模块工作模式 */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                      //ADC独立模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                     //单次循环模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;                           //单通道
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;     //打开软件触发
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                  //数据右对齐 -- 低12位
	ADC_InitStructure.ADC_NbrOfChannel = 1;                                 //顺序转换的规则组的通道数目
	ADC_Init(ADC1,&ADC_InitStructure);                                      //给相关寄存器赋值
	
	/* 使能ADC模块 */
	ADC_Cmd(ADC1,ENABLE);                                                   //开启ADC1
	
	/* ADC模块校验 */
	ADC_ResetCalibration(ADC1);						//重置ADC1的校准寄存器
	while(ADC_GetResetCalibrationStatus(ADC1));		//获取ADC1重置校准寄存器的状态,设置状态则等待

	ADC_StartCalibration(ADC1);					    //开始ADC1的校准状态
	while(ADC_GetCalibrationStatus(ADC1));		    //等待校准完成
}

/**************************** ADC转换值获取函数 ****************************
功  能：获取ADC模块转换后的数值
参  数：无
返回值：存放ADC转换值
***************************************************************************/
u16 Get_adcvalue(void)
{
	u16 Buff[10] = {0};
	u16 temp = 0;
	u8 i,j;
	
	ADC_RegularChannelConfig(ADC1,2,1,ADC_SampleTime_239Cycles5);   //设置指定ADC的规则组通道，一个序列，采样时间
	
	for(i = 0; i < 10; i++)
	{
		ADC_SoftwareStartConvCmd(ADC1,ENABLE);               //软件启动ADC转换
		while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) != SET)
		{
			/* 等待转换完成 */
		}
		Buff[i] = ADC_GetConversionValue(ADC1);             //读取ADC数据寄存器中的值
	}
	/* ADC软件滤波 */     /*冒泡算法*/
	for(i = 0; i < 9; i++)
	{
		for(j = i + 1; j < 10; j++)
		{
			if(Buff[i] > Buff[j])
			{
				temp = Buff[i];
				Buff[i] = Buff[j];
				Buff[j] = temp;
			}
		}
	}
	temp = 0;
	for(i = 1; i < 9; i++)
	{
		temp += Buff[i];
	}
	return (temp / 8);
}

/*************************** ADC转换结果显示函数 **************************
函数功能：ADC最终结果函数
参数：
	Page：显示结果的横坐标
	Colum：显示结果的纵坐标
返回值：无
**************************************************************************/
void ADC_Val_Disp(u8 Page,u8 Colum)
{
	u16 ADC_Temp = 0;                     //ADC转换后的数字量
	//float ADC_TT = 0;
//	char Buff[20];
	
	/* 获取ADC转换值 */
	ADC_Temp = Get_adcvalue();        //模拟量=参考量/ADC位数*数字量     丁烷300-5000ppm  故:
	ADC_Val = (u16)((((5000.0 - 300) / 4096) * ADC_Temp) + 300);			
//	sprintf(Buff,"gas:%dppm",ADC_Val);
//	printf("\r\nGas:%dppm\r\n",ADC_Val);
	/* 液晶屏显示 */




	

}
