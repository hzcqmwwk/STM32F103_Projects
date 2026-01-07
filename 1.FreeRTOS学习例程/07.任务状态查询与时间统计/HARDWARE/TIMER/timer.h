#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

//定时器 驱动代码	   
void TIM3_Int_Init(u16 arr,u16 psc);
void ConfigureTimeForRunTimeStats(void);

#endif
