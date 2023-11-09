#include "Water.h"
#include "sys.h"
#include "stm32f10x.h"   //������Ҫ��ͷ�ļ�
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
    
    
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);//72M/6<14M ��Ƶ����
    ADC_InitStruct.ADC_ContinuousConvMode=DISABLE;//����ģʽ
    ADC_InitStruct.ADC_DataAlign=ADC_DataAlign_Right;//��������
	ADC_InitStruct.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;//Ӳ������
	ADC_InitStruct.ADC_Mode=ADC_Mode_Independent;//����Ϊ����ģʽ
	ADC_InitStruct.ADC_NbrOfChannel=1;//˳����й���ת����ADC ͨ������Ŀ
	ADC_InitStruct.ADC_ScanConvMode=DISABLE;
    ADC_Init(ADC1,&ADC_InitStruct);
    
    ADC_Cmd(ADC1,ENABLE);  // ʹ��
    
    ADC_ResetCalibration(ADC1);//������λУ׼
    while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);//����ADУ׼
	while(ADC_GetCalibrationStatus(ADC1));


}

u16 Water_Get()
{
    ADC_RegularChannelConfig(ADC1, 5, 1, ADC_SampleTime_239Cycles5);//���ù���ͨ��
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//�������ת��
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));//�ȴ�ת�����
	return ADC_GetConversionValue(ADC1);

}

u16 Get_Water_Adc_Average(u8 times)//��ƽ��ֵ
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





