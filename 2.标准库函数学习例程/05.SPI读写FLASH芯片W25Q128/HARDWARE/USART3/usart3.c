//注意，未验证正确性！！！
#include "usart3.h"

uint8_t  USART3_RX_BUF[USART3_REC_LEN];    //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
uint16_t USART3_RX_STA = 0;         	   //接收状态标记


void uart3_init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	    //使能USART3，GPIOB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  
	//USART3_TX   GPIOB10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;				//复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
   
	//USART3_RX	  GPIOB11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		        //上拉输入
	GPIO_Init(GPIOB, &GPIO_InitStructure); 

	//Usart3 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;    //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		    //子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			    //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);
  
	//USART3 初始化设置
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);              //开启串口接受中断
	USART_Cmd(USART3, ENABLE);                                  //使能串口3
}

//串口3中断服务程序
void USART3_IRQHandler(void)                	
{
	uint8_t Res;

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾即\r\n)
	{
		Res =USART_ReceiveData(USART3);	//读取接收到的数据
		
		if((USART3_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART3_RX_STA&0x4000)//接收到了0x0d
			{
				if(Res!=0x0a)
					USART3_RX_STA=0;//接收错误,重新开始
				else 
					USART3_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(Res==0x0d)
					USART3_RX_STA|=0x4000;
				else
				{
					USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ;
					USART3_RX_STA++;
					if(USART3_RX_STA>(USART3_REC_LEN-1))
						USART3_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}
	} 
}

void uart3_SendByte(uint8_t Byte)
{
	USART_SendData(USART3,Byte);
	while(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == RESET);
}

void uart3_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for(i=0; i<Length; i++)
		uart3_SendByte(Array[i]);
}

void uart3_SendString(char *String)
{
	uint8_t i;
	for(i=0; String[i] != '\0'; i++)
		uart3_SendByte(String[i]);
}
