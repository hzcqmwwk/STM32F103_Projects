#ifndef __USART3_H
#define __USART3_H
	
#include "stm32f10x.h"

#define USART3_REC_LEN  			200  	//定义最大接收字节数 200

extern uint8_t  USART3_RX_BUF[USART3_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern uint16_t USART3_RX_STA;         		   //接收状态标记

void uart3_init(uint32_t bound);
void uart3_SendByte(uint8_t Byte);
void uart3_SendArray(uint8_t *Array, uint16_t Length);
void uart3_SendString(char *String);

#endif
