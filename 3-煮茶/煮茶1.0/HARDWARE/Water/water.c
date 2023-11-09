#include "Water.h"
#include "sys.h"
#include "stm32f10x.h"   //包含需要的头文件
#include "delay.h"
#include "usart.h"
void Water_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    ADC_InitTypeDef ADC_InitStruct;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStruct);
    
    
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);//72M/6<14M 分频因子
    ADC_InitStruct.ADC_ContinuousConvMode=DISABLE;//连续模式
    ADC_InitStruct.ADC_DataAlign=ADC_DataAlign_Right;//对齐设置
	ADC_InitStruct.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;//硬件触发
	ADC_InitStruct.ADC_Mode=ADC_Mode_Independent;//设置为独立模式
	ADC_InitStruct.ADC_NbrOfChannel=1;//顺序进行规则转换的ADC 通道的数目
	ADC_InitStruct.ADC_ScanConvMode=DISABLE;
    ADC_Init(ADC1,&ADC_InitStruct);
    
    ADC_Cmd(ADC1,ENABLE);  // 使能
    
    ADC_ResetCalibration(ADC1);//开启复位校准
    while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);//开启AD校准
	while(ADC_GetCalibrationStatus(ADC1));


}

u16 Water_Get()
{
    ADC_RegularChannelConfig(ADC1, 5, 1, ADC_SampleTime_239Cycles5);//配置规则通道
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//开启软件转换
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));//等待转换完成
	return ADC_GetConversionValue(ADC1);

}

u16 Get_Water_Adc_Average(u8 times)//求平均值
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Water_Get();
		delay_ms(5);
	}
    printf("temp_val=%d\r\n",temp_val/times);
	return temp_val/times;
}





