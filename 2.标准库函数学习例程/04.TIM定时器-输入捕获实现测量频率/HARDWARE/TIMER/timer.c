#include "timer.h"
#include "usart.h"

//定时器 驱动代码

//这里时钟默认选择为APB1的36MHz的2倍=72MHz，可以使用
//arr：自动重装值，psc：时钟预分频数，用法示例：TIM3_Int_Init(10000-1,7200-1);定时1s
void TIM3_Int_Init(uint16_t arr,uint16_t psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);         //TIM3时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr;                      //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc;                    //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);              //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);                         //手动清除更新中断标志位，可以避免刚初始化就进一次中断，在这个例程中num就可以从0开始计数，而不是1
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE );                    //使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;              //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;    //抢占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;           //子优先级1级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;              //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);                              //初始化NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);                                       //使能TIM3			 
}

/*
//可以在哪里用就放哪里，这样就不需要用extern去声明跨文件调用的变量
//定时器3中断服务函数，视具体情况改写
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{
		//中断操作
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
}
*/
