//不支持任何OS
#include "delay.h"

static uint8_t  fac_us=0;							//us延时倍乘数			   
		   
//初始化延迟函数，SYSTICK的时钟固定为HCLK时钟
void delay_init()
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);	    //选择外部时钟HCLK=72MHz
	fac_us=SystemCoreClock/1000000;				            //fac_us=72  
}								    

//延时nus,SysTick->LOAD为24位寄存器，最大值为16777215，则	nus最大233015
void delay_us(uint32_t nus)
{			    	 
	SysTick->LOAD = nus*fac_us; 					//设置定时器重装值
	SysTick->VAL = 0x00;        					//清空计数器
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;	    //启动计数器
	while(!(SysTick->CTRL & 0x00010000));		    //等待计数到0
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	    //关闭计数器
	SysTick->VAL = 0x00;      					    //清空计数器	 
}

//延时nms
void delay_ms(uint32_t nms)
{	 		  	  		   
	while(nms--)
		delay_us(1000);
} 
