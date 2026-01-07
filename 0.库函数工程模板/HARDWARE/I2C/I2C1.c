//软件实现4针脚I2C1通信
#include "I2C1.h"

void I2C1_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	    //开启GPIOB的时钟
	
	//PB6:I2C1_SCL_PIN   PB7:I2C1_SDA_PIN
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;            //开漏输出
	GPIO_InitStructure.GPIO_Pin = I2C1_SCL_PIN | I2C1_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C1_PORT, &GPIO_InitStructure);					
	
	I2C1_W_SCL(1);
	I2C1_W_SDA(1);												//引脚初始化后默认为高电平（释放总线状态）
}

//I2C1写SCL引脚电平
void I2C1_W_SCL(uint8_t BitValue)
{
	GPIO_WriteBit(I2C1_PORT, I2C1_SCL_PIN, (BitAction)BitValue);
	delay_us(10);												//延时10us，防止时序频率超过要求
}

//I2C1写SDA引脚电平
void I2C1_W_SDA(uint8_t BitValue)
{
	GPIO_WriteBit(I2C1_PORT, I2C1_SDA_PIN, (BitAction)BitValue);
	delay_us(10);
}

//I2C1读SDA引脚电平
uint8_t I2C1_R_SDA(void)
{
	uint8_t BitValue;
	BitValue = GPIO_ReadInputDataBit(I2C1_PORT, I2C1_SDA_PIN);
	delay_us(10);
	return BitValue;
}

//I2C1起始
void I2C1_Start(void)
{
	I2C1_W_SDA(1);								//释放SDA，确保SDA为高电平
	I2C1_W_SCL(1);								//释放SCL，确保SCL为高电平
	I2C1_W_SDA(0);								//在SCL高电平期间，拉低SDA，产生起始信号
	I2C1_W_SCL(0);								//起始后把SCL也拉低，即为了占用总线，也为了方便总线时序的拼接
}

//I2C1结束
void I2C1_Stop(void)
{
	I2C1_W_SDA(0);								//拉低SDA，确保SDA为低电平
	I2C1_W_SCL(1);								//释放SCL，使SCL呈现高电平
	I2C1_W_SDA(1);								//在SCL高电平期间，释放SDA，产生终止信号
}

//I2C1发送一个字节
void I2C1_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i ++)					//循环8次，主机依次发送数据的每一位
	{
		//两个!可以对数据进行两次逻辑取反，作用是把非0值统一转换为1，即：!!(0) = 0，!!(非0) = 1
		I2C1_W_SDA(!!(Byte & (0x80 >> i)));		//使用掩码的方式取出Byte的指定一位数据并写入到SDA线
		I2C1_W_SCL(1);							//释放SCL，从机在SCL高电平期间读取SDA
		I2C1_W_SCL(0);							//拉低SCL，主机开始发送下一位数据
	}
}

//I2C1接收一个字节
uint8_t I2C1_ReceiveByte(void)
{
	uint8_t i, Byte = 0x00;						//定义接收的数据，并赋初值0x00，此处必须赋初值0x00，后面会用到
	I2C1_W_SDA(1);								//接收前，主机先确保释放SDA，避免干扰从机的数据发送
	for (i = 0; i < 8; i ++)					//循环8次，主机依次接收数据的每一位
	{
		I2C1_W_SCL(1);							//释放SCL，主机机在SCL高电平期间读取SDA
		if (I2C1_R_SDA())
			Byte |= (0x80 >> i);				//读取SDA数据，并存储到Byte变量
		I2C1_W_SCL(0);							//拉低SCL，从机在SCL低电平期间写入SDA
	}
	return Byte;
}

//I2C1发送应答位，0表示应答，1表示非应答
void I2C1_SendAck(uint8_t AckBit)
{
	I2C1_W_SDA(AckBit);							//主机把应答位数据放到SDA线
	I2C1_W_SCL(1);								//释放SCL，从机在SCL高电平期间，读取应答位
	I2C1_W_SCL(0);								//拉低SCL，开始下一个时序模块
}

//I2C1接收应答位，0表示应答，1表示非应答
uint8_t I2C1_ReceiveAck(void)
{
	uint8_t AckBit;								//定义应答位变量
	I2C1_W_SDA(1);								//接收前，主机先确保释放SDA，避免干扰从机的数据发送
	I2C1_W_SCL(1);								//释放SCL，主机机在SCL高电平期间读取SDA
	AckBit = I2C1_R_SDA();						//将应答位存储到变量里
	I2C1_W_SCL(0);								//拉低SCL，开始下一个时序模块
	return AckBit;								//返回定义应答位变量
}
