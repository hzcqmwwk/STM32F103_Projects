#include "usart.h"
#include "timer.h"
#include "delay.h"
#include "sys.h"
#include "process.h"

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);   //设置系统中断优先级分组4
	uart_init(115200);			                      //初始化串口
	delay_init();	    				              //延时函数初始化
	TIM3_Int_Init(10000-1,7200-1);                    //初始化定时器3、5 一秒中断
	TIM5_Int_Init(10000-1,7200-1);	  			  
	
	//使用两个定时器，一个优先级为4，一个优先级为6，两个定时器每一秒打印一段字符串，当关中断时停止打印，开中断时持续打印
	process();
}
