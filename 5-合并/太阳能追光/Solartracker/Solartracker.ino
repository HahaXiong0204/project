#include <Servo.h> 

#define SERVOPINH  5 //水平舵机
#define SERVOPINV  6 //垂直舵机

#define dtime   30   //延时参数，数值越小相应速度越快，反之相应慢   单位毫秒 一般取值（10~100） 
#define tol   40     //照度的相应范围，越小越敏感，反之迟缓  （取值10~100 根据环境光强度不同敏感度也不同，室内光源变化幅度大，阳光下变化小）
/*以上2参数太小，会对光线的细微变化极其敏感，进而产生抖动，
  为消除抖动，可以使用滤波处理，或者调节参数，减慢反应时间或敏感度来应对。 */

          
// 水平舵机的设置
Servo horizontal;            //水平舵机
int servoh = 90;             //默认角度
int servohLimitHigh = 175;   //水平转动最大角度
int servohLimitLow = 5;      //水平转动最小角度

// 垂直舵机的设置
Servo vertical;              //垂直舵机
int servov = 90;             //默认角度
int servovLimitHigh = 180;   //垂直转动最大角度
int servovLimitLow = 90;     //垂直转动最小角度

// 4个光敏电阻模块的接线口   
const int ldrlt = A0;   //左上
const int ldrrt = A1;   //右上
const int ldrld = A2;   //左下
const int ldrrd = A3;   //右下

void setup()
{
  horizontal.attach(SERVOPINH); 
  vertical.attach(SERVOPINV);
  delay(100);
}

void loop() 
{
  //分别读取4个光敏电阻模块的照度值
  int lt = analogRead(ldrlt); //左上
  int rt = analogRead(ldrrt); //右上
  int ld = analogRead(ldrld); //左下
  int rd = analogRead(ldrrd); //右下
  
  //将邻近光敏电阻模块的读数平均    
  int avt = (lt + rt) / 2; 
  int avd = (ld + rd) / 2; 
  int avl = (lt + ld) / 2; 
  int avr = (rt + rd) / 2; 
  
  //再计算上下行和左右排平均值的差值
  int dvert = avt - avd;    //上下行 
  int dhoriz = avl - avr;

   //检查差异是否在公差范围内，否则改变垂直角度    
   if (-1*tol > dvert || dvert > tol) 
   {
      if (avt < avd)
      {
        servov = ++servov;
         if (servov > servovLimitHigh) 
         { 
           servov = servovLimitHigh;
         }
      }
      else if (avt > avd)
      {
        servov= --servov;
        if (servov < servovLimitLow)
        {
          servov = servovLimitLow;
        }
      }
      vertical.write(servov); //舵机旋转角度和光线相反的话 用(180- servov)或 (servov) 调换方向即可
    }

  //检查差异是否在公差范围内，否则改变水平角度  
  if (-1*tol > dhoriz || dhoriz > tol) 
  {
    if (avl < avr)
    {
      servoh = --servoh;
      if (servoh < servohLimitLow)
      {
        servoh = servohLimitLow;
      }
    }
    else if (avl > avr)
    {
       servoh = ++servoh;
       if (servoh > servohLimitHigh)
       {
         servoh = servohLimitHigh;
       }
    }
    horizontal.write(servoh); //舵机旋转角度和光线相反的话 用(180- servoh) 或 (servoh) 调换方向即可
  }
   delay(dtime);
}
