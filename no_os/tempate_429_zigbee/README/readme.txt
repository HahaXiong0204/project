
使用cubemx重新创建项目 必须在middlewares/Rt-thread/RTOS/kernal的board.c中将static int uart_init(void)的串口改成usart1


v1.0
	使用 TIM1 做系统时钟
	
	TIM2 做一秒定时，可以给同步时钟做定时   中断优先级为  3   2
	
	// 传感器数据读取   
	void sensor_thread_entry(void *pargem);    20
	// 连接wifi的入口函数
	void connect_thread_entery(void *pargem);   10
	
	添加了lcd的驱动

