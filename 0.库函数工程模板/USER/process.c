#include "process.h"

void process(void)
{
	while(1)
	{
		LED0 = 0;
		LED1 = 1;
		delay_ms(1000);
		LED0 = 1;
		LED1 = 0;
		delay_ms(1000);
	}
}
