#include "stm32f10x.h"            // Device header
#include "delay.h"
#include "key.h"
#include "sys.h"

void key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	// 使能 PORTA,PORTE 时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE,ENABLE);

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_3;     //GPIOE.3、4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;             //上拉输入，没有按下时，接高电平
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;                //初始化 WK_UP-->GPIOA.0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;             //下拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

//按键处理函数，返回按键值，注意此函数有响应优先级,KEY0>KEY1>KEY_UP!!
//mode:0,不支持连续按;1,支持连续按;
uint8_t key_scan(uint8_t mode)
{
    static uint8_t key_up = 1;    // 按键按松开标志

    if(mode)	key_up = 1;       // mode为1，key_up设置为1，支持连按
    if(key_up && (KEY0 == 0 || KEY1 == 0 || WK_UP == 1))  // 按键松开标志为1, 且有任意一个按键按下了
    {
        delay_ms(10);             // 去抖动
        key_up = 0;
        if(KEY0 == 0)	return KEY0_PRES;
        else if(KEY1 == 0)	return KEY1_PRES;
        else if(WK_UP == 1)	return WKUP_PRES;
    }
    else if(KEY0 == 1 && KEY1 == 1 && WK_UP == 0) // 没有任何按键按下, 标记按键松开
        key_up = 1;

    return 0;                     // 无按键按下
}

//uint8_t Key_GetNum(void)
//{
//	uint8_t KeyNum = 0;
//	if (KEY0 == 0)//读取指定端口
//	{
//		delay_ms(20);
//		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 1);
//		delay_ms(20);
//		KeyNum = 1;
//	}
//	if (KEY1 == 0)
//	{
//		delay_ms(20);
//		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 1);
//		delay_ms(20);
//		KeyNum = 2;
//	}
//	return KeyNum;
//}
