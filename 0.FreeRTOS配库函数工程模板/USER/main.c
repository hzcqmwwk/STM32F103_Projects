#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "exti.h"
#include "sys.h"
#include "process.h"

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);     //设置系统中断优先级分组4
	uart_init(115200);			//初始化串口
	delay_init();	    		//延时函数初始化
	led_init();		  			//初始化led
	key_init();                 //初始化key
	extix_init();               //初始化外部中断
	
	process();
}
