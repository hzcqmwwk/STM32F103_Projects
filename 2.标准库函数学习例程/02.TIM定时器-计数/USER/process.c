#include "process.h"

uint16_t num;

void process(void)
{
	OLED_ShowString(0,16,"seconds:",OLED_8X16);
	//OLED_ShowString(0,32,"counter:",OLED_8X16);
	while(1)
	{
		OLED_ShowNum(64,16,num,5,OLED_8X16);
		//OLED_ShowNum(64,32,TIM_GetCounter(TIM3),5,OLED_8X16);
		OLED_Update();
	}

}

//定时器3中断服务函数，视具体情况改写
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{
		num++;
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
}
