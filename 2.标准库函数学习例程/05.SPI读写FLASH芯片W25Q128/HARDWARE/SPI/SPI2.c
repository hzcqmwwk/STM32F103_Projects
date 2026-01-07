//软件实现SPI2通信
#include "SPI2.h"

void SPI2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	    //开启GPIOB的时钟
	
	//CS、SCK、MOSI三个配置为推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = SPI2_CS_PIN|SPI2_SCK_PIN|SPI2_MOSI_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI2_PORT, &GPIO_InitStructure);
	//MISO配置为上拉输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = SPI2_MISO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI2_PORT, &GPIO_InitStructure);

	SPI2_W_CS(1);
	SPI2_W_SCK(0);
}

//SPI2写CS引脚电平
void SPI2_W_CS(uint8_t BitValue)
{
	GPIO_WriteBit(SPI2_PORT,SPI2_CS_PIN,(BitAction)BitValue);
}

//SPI2写SCK引脚电平
void SPI2_W_SCK(uint8_t BitValue)
{
	GPIO_WriteBit(SPI2_PORT,SPI2_SCK_PIN,(BitAction)BitValue);
}

//SPI2写MOSI引脚电平
void SPI2_W_MOSI(uint8_t BitValue)
{
	GPIO_WriteBit(SPI2_PORT,SPI2_MOSI_PIN,(BitAction)BitValue);
}

//SPI2读MISO引脚电平
uint8_t SPI2_R_MISO(void)
{
	return GPIO_ReadInputDataBit(SPI2_PORT,SPI2_MISO_PIN);
}

//SPI2起始
void SPI2_Start(void)
{
	SPI2_W_CS(0);
}

//SPI2终止
void SPI2_Stop(void)
{
	SPI2_W_CS(1);
}

//SPI2采用模式0交换传输一个字节
uint8_t SPI2_SwapByte(uint8_t ByteSend)
{
	uint8_t i,ByteReceive=0x00;
	
	for(i=0; i<8; i++)
	{
		//两个!可以对数据进行两次逻辑取反，作用是把非0值统一转换为1，即：!!(0) = 0，!!(非0) = 1
		SPI2_W_MOSI(!!(ByteSend & (0x80>>i)));		//使用掩码的方式取出ByteSend的指定一位数据并写入到MOSI线
		SPI2_W_SCK(1);								//拉高SCK，上升沿移出数据
		if(SPI2_R_MISO())
			ByteReceive |= (0x80>>i);				//读取MISO数据，并存储到ByteReceive变量
		SPI2_W_SCK(0);								//拉低SCK，下降沿移入数据
		printf("ByteReceive=%d\r\n",ByteReceive);
	}
	return ByteReceive;
}

//采用循环移位的方法，坏处是更改了ByteSend
//uint8_t SPI2_SwapByte(uint8_t ByteSend)
//{
//	uint8_t i;
//	
//	for(i=0; i<8; i++)
//	{
//		SPI2_W_MOSI(ByteSend & 0x80);
//		ByteSend <<= 1;
//		SPI2_W_SCK(1);								//拉高SCK，上升沿移出数据
//		if(SPI2_R_MISO())
//			ByteSend |= 0x01;						//读取MISO数据
//		SPI2_W_SCK(0);								//拉低SCK，下降沿移入数据
//	}
//	return ByteSend;
//}
