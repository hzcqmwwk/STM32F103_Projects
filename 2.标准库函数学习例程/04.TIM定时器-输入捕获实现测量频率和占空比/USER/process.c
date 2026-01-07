#include "process.h"

void process(void)
{
	while(1)
	{
		printf("freq:%d\r\n",(SystemCoreClock/(TIM2->PSC+1))/TIM_GetCapture1(TIM2));
		printf("ccr1:%d\r\n",TIM_GetCapture1(TIM2));
		printf("ccr2:%d\r\n",TIM_GetCapture2(TIM2));
		printf("duty:%d\r\n",100*TIM_GetCapture2(TIM2)/TIM_GetCapture1(TIM2));
		
		OLED_ShowNum(40,16,(SystemCoreClock/(TIM2->PSC+1))/TIM_GetCapture1(TIM2),5,OLED_8X16);
		OLED_ShowNum(40,32,100*TIM_GetCapture2(TIM2)/TIM_GetCapture1(TIM2),3,OLED_8X16);
		OLED_Update();
	}
}
