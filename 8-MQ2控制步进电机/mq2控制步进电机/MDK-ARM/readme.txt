led beep -> PC13

28BYJ48 ->  PA4 PA5 PA6 PA7

esp2866 ->  RX-PA2		TX-PA3

Oled -> PB6 -- SCL; PB7 -- SDA

MQ2 -> PA0

控制命令：

    mode                1 为自动模式  2 为手动模式
    TH_Gas              {"TH_Gas","40"}  将阈值改成40
    flag                1为正转开门   2为反转关门
    
接收的：
    TH_Gas              阈值
    Gas                 空气值
    mode                模式
    flag                1 开门    2 关门
    
    
