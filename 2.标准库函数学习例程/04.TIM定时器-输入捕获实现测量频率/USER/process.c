#include "process.h"

void process(void)
{
	while(1)
	{
		OLED_ShowNum(40,16,(SystemCoreClock/(TIM2->PSC+1))/TIM_GetCapture1(TIM2),5,OLED_8X16);
		OLED_Update();
	}
}
