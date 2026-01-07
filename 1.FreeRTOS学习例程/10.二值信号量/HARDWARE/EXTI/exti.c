#include "exti.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "usart.h"

//外部中断 驱动代码
//extix_init中根据需要删减，最下面三个外部中断服务程序根据需要重写或删减，在FreeRTOS中优先级需在5-15之间
//外部中断0服务程序
void extix_init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	
	key_init();

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	          //使能复用功能时钟

    //GPIOE.3 KEY1	  中断线以及中断初始化配置 下降沿触发
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource3);    //将GPIO引脚与外部中断线（EXTI Line）关联起来
  	EXTI_InitStructure.EXTI_Line=EXTI_Line3;                      //选择中断线3
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;           //配置为中断模式（而非事件模式）
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;                     //使能该中断线
  	EXTI_Init(&EXTI_InitStructure);	  	                          //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

    //GPIOE.4 KEY0	  中断线以及中断初始化配置  下降沿触发
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource4);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);

    //GPIOA.0 WK_UP	  中断线以及中断初始化配置 上升沿触发
 	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0); 
  	EXTI_InitStructure.EXTI_Line=EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			  //使能按键WK_UP所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x05;  //抢占优先级5，在FreeRTOS中需在5-15之间
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;		  //子优先级无
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				  //使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);                               //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x05;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);  	                          

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x05;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
}

//外部中断0服务程序
void EXTI0_IRQHandler(void)
{
	volatile uint32_t i = 0;
	//delay_ms(10);中断中不能用delay延时，会报错卡死
	//简单的硬件消抖（短时间内两次检测高电平）
	if(WK_UP==1)
	{
		for(i; i<10000; i++);  //短延时
		if(WK_UP==1)  //二次确认
		{
			//具体操作
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line0);  //清除LINE0上的中断标志位
}
 
//外部中断3服务程序
void EXTI3_IRQHandler(void)
{
	volatile uint32_t i = 0;
	//delay_ms(10);中断中不能用delay延时，会报错卡死
	//简单的硬件消抖（短时间内两次检测高电平）
	if(KEY1==0)
	{
		for(i; i<10000; i++);  //短延时
		if(KEY1==0)  //二次确认
		{
			//具体操作
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line3);  //清除LINE3上的中断标志位  
}

void EXTI4_IRQHandler(void)
{
	volatile uint32_t i = 0;
	//delay_ms(10);中断中不能用delay延时，会报错卡死
	//简单的硬件消抖（短时间内两次检测高电平）
	if(KEY0==0)
	{
		for(i; i<10000; i++);  //短延时
		if(KEY0==0)  //二次确认
		{
			//具体操作
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line4);  //清除LINE4上的中断标志位  
}
