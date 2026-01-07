#ifndef __KEY_H
#define __KEY_H

#include "sys.h"

#define KEY0		GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4)  //读取按键KEY0
#define KEY1		GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3)  //读取按键KEY1
#define WK_UP       GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)  //读取按键WK_UP

#define KEY0_PRES 	  1   //KEY0按下
#define KEY1_PRES	  2	  //KEY1按下
#define WKUP_PRES     3   //WK_UP按下

void key_init(void);
uint8_t key_scan(uint8_t mode);

#endif
