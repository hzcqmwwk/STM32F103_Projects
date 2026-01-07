#include "pwm.h"

//PWM 驱动代码

//定时器TIM3的通道2 TIM3_CH2默认接在PA7,这里采用TIM3_CH2输出PWM
void pwm_init(uint16_t arr,uint16_t psc)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStruct;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);         //TIM3时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);        //使能PA端口时钟
	
	//PA7初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 	         //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr;                      //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc;                    //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);              //根据指定的参数初始化TIMx的时间基数单位
	
	//设置TIM3_CH2的PWM模式，使能TIM3的CH2输出
	TIM_OCStructInit(&TIM_OCInitStruct);                         //给TIM_OCInitStruct赋默认初始值，然后修改用到的参数
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;       //输出极性高
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = arr/2;                          //CCR的比较值,此处默认50%的占空比
	TIM_OC2Init(TIM3,&TIM_OCInitStruct);
	
	TIM_Cmd(TIM3, ENABLE);                                       //使能TIM3			 
}
