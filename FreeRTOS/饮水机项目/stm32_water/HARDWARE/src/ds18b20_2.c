#include "DS18B20_2.h"
#include "delay.h"


//复位DS18B20_2
void DS18B20_2_Rst(void)	   
{                 
	DS18B20_2_IO_OUT();   //设置为输出
  DS18B20_2_DQ_OUT=0;  //拉低DQ
  delay_us(750);      //拉低750us
  DS18B20_2_DQ_OUT=1;  //DQ=1 
	delay_us(15);       //15US
}

//等待DS18B20_2的回应
//返回1:未检测到DS18B20_2的存在
//返回0:存在
uint8_t DS18B20_2_Check(void) 	   
{   
	uint8_t retry=0;
	DS18B20_2_IO_IN();    //设置为输入
    while (DS18B20_2_DQ_IN&&retry<200)
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=200)return 1;
	else retry=0;
    while (!DS18B20_2_DQ_IN&&retry<240)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)return 1;	    
	return 0;
}

//从DS18B20_2读取一个位
//返回值：1/0
uint8_t DS18B20_2_Read_Bit(void) 
{
  uint8_t data;
	DS18B20_2_IO_OUT();   //设置为输出
  DS18B20_2_DQ_OUT=0; 
	delay_us(2);
  DS18B20_2_DQ_OUT=1; 
	DS18B20_2_IO_IN();    //设置为输入
	delay_us(12);
	if(DS18B20_2_DQ_IN)data=1;
  else data=0;	 
  delay_us(50);           
  return data;
}

//从DS18B20_2读取一个字节
//返回值：读到的数据
uint8_t DS18B20_2_Read_Byte(void)   
{        
  uint8_t i,j,dat;
  dat=0;
	for (i=1;i<=8;i++) 
	{
        j=DS18B20_2_Read_Bit();
        dat=(j<<7)|(dat>>1);
    }						    
  return dat;
}

//写一个字节到DS18B20_2
//dat：要写入的字节
void DS18B20_2_Write_Byte(uint8_t dat)     
 {             
    uint8_t j;
    uint8_t testb;
    DS18B20_2_IO_OUT();     //设置为输出
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if(testb)       // 写1
        {
            DS18B20_2_DQ_OUT=0;
            delay_us(2);                            
            DS18B20_2_DQ_OUT=1;
            delay_us(60);             
        }
        else            //写0
        {
            DS18B20_2_DQ_OUT=0;
            delay_us(60);             
            DS18B20_2_DQ_OUT=1;
            delay_us(2);                          
        }
    }
}
 
//开始温度转换
void DS18B20_2_Start(void)
{   						               
    DS18B20_2_Rst();	   
    DS18B20_2_Check();	 
    DS18B20_2_Write_Byte(0xcc);// skip rom
    DS18B20_2_Write_Byte(0x44);// convert
}

//初始化DS18B20_2的IO口 DQ 同时检测DS的存在
//返回1:不存在
//返回0:存在    	 
uint8_t DS18B20_2_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOH_CLK_ENABLE();			//开启GPIOB时钟
	
    GPIO_Initure.Pin=GPIO_PIN_9;           //PB12
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);     //初始化
 
 	  DS18B20_2_Rst();
	  return DS18B20_2_Check();
}

//从DS18B20_2得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250） 
short DS18B20_2_Get_Temp(void)
{
    uint8_t temp;
    uint8_t TL,TH;
    short tem;
    DS18B20_2_Start ();           //开始转换
    DS18B20_2_Rst();
    DS18B20_2_Check();	 
    DS18B20_2_Write_Byte(0xcc);   // skip rom
    DS18B20_2_Write_Byte(0xbe);   // convert	    
    TL=DS18B20_2_Read_Byte();     // LSB   
    TH=DS18B20_2_Read_Byte();     // MSB   
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;//温度为负  
    }else temp=1;//温度为正	  	  
    tem=TH; //获得高八位
    tem<<=8;    
    tem+=TL;//获得底八位
    tem=(double)tem*0.625;//转换     
	if(temp)return tem; //返回温度值
	else return -tem;    
}
