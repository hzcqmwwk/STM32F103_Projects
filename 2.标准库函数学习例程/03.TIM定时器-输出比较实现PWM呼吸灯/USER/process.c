#include "process.h"

void process(void)
{
	uint8_t i;
	while(1)
	{
		for(i=0;i<=100;i++)
		{
			TIM_SetCompare2(TIM3,i);
			delay_ms(10);
		}
		for(i=0;i<=100;i++)
		{
			TIM_SetCompare2(TIM3,100-i);
			delay_ms(10);
		}
	}
}
