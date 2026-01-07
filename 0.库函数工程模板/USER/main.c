#include "process.h"

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart_init(115200);	    //初始化串口
	delay_init();           //延时函数初始化
	led_init();             //初始化led
	key_init();             //初始化key
	oled_init();            //初始化oled
	
	OLED_Clear();
    OLED_ShowString(0,0,"Start",OLED_8X16);
    OLED_Update();
	
	process();
	
	printf("程序意外退出,请重启!");
    OLED_Clear();
    OLED_ShowString(0,32,"Unexpected exit",OLED_8X16);
    OLED_Update();
}
