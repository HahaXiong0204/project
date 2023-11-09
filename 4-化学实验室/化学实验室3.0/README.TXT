PA4 光敏   第一个读取 （ADC1_4）

PA5 MQ135    第二个读取(ADC1_5)

PB1 Mq2     第三个读取(ADC1_9)

PA6 火焰   第四个读取(ADC1_6)

DMA同时读取4个ADC的通道


蜂鸣器 PB10  推挽输出 低电平有效 

LED灯 PC13 推挽输出

DHT11 PB0 


USART1 PA9（TX） PA10（RX）给串口打印

USART2 PA2（TX） PA3（RX） 给wifi使用

风扇的继电器 PA12 推挽输出


OneNet平台

1、更改\NET\esp8266.c的wifi名和密码
2、更改\NET\onenet.c的PROID、AUTH_INFO、DEVID，将其改成自己的id和鉴权信息
3、更改main.c的const char *topics[]，为自己需要订阅的主题
4、更改\NET\MqttKit.c的const int8 Publish_Buf[2][16]，为自己需要发布的主题


上位机设置下位机的阈值

* 采用json格式
一次发送{"mode":"1","TH_humidity":"220"....}

其中：
    fan: 1开启  0关闭
    mode ：模式  1为自动 2为手动
    TH_temp :温度
    TH_humidity: 湿度
    TH_fire :火焰值
    TH_MQ135: 空气质量阈值
    TH_MQ2 : 烟雾阈值


fan_flag  风扇开关
temperature 温度
humidity 湿度
Lighting  亮度
MQ_135  空气质量
MQ_2 烟雾浓度
Mode 模式
