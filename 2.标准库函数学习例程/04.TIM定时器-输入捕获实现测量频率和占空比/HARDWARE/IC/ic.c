#include "ic.h"

//输入捕获 驱动代码

//定时器TIM2的通道1 TIM2_CH1默认接在PA0,这里采用TIM2_CH1捕获频率，并且TIM2_CH2捕获占空比
void ic_init(uint16_t arr,uint16_t psc)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStruct;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);         //TIM2时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);        //使能PA端口时钟
	
	//PA0初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 	             //上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//定时器TIM2初始化
	TIM_TimeBaseStructure.TIM_Period = arr;                      //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc;                    //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);              //根据指定的参数初始化TIMx的时间基数单位
	
	//设置TIM2_CH1和CH2同时捕获PA0的输入并使能
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStruct.TIM_ICFilter = 0xF;                         //输入捕获滤波器，用于消除输入信号的毛刺和噪声
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;           //分频系数
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI; //选择直连通道
	TIM_PWMIConfig(TIM2, &TIM_ICInitStruct);                     //自动配置TIM2_CH2为CH1相反的配置
	
	TIM_SelectInputTrigger(TIM2,TIM_TS_TI1FP1);                  //主模式触发源选择
	TIM_SelectSlaveMode(TIM2,TIM_SlaveMode_Reset);               //从模式操作选择复位，TI1产生上升沿时，会触发CNT归零
	
	TIM_Cmd(TIM2, ENABLE);                                       //使能TIM2
}
