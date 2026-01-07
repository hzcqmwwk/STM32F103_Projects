#include "W25Q128.h"

//W25Q128驱动代码

void W25Q128_Init(void)
{
	SPI2_Init();
}

//W25Q128读取ID号,MID:工厂ID,DID:设备ID
void W25Q128_ReadID(uint8_t *MID, uint16_t *DID)
{
	SPI2_Start();
	SPI2_SwapByte(W25Q128_JEDEC_ID);			//交换发送读取ID的指令
	*MID = SPI2_SwapByte(W25Q128_DUMMY_BYTE);	//交换接收MID
	*DID = SPI2_SwapByte(W25Q128_DUMMY_BYTE);	//交换接收DID高8位
	*DID <<= 8;									//高8位移到高位
	*DID |= SPI2_SwapByte(W25Q128_DUMMY_BYTE);	//或上交换接收DID的低8位
	SPI2_Stop();
}

//W25Q128写使能
void W25Q128_WriteEnable(void)
{
	SPI2_Start();
	SPI2_SwapByte(W25Q128_WRITE_ENABLE);		//交换发送写使能的指令
	SPI2_Stop();
}

//W25Q128等待忙
void W25Q128_WaitBusy(void)
{
	uint32_t Timeout;
	SPI2_Start();
	SPI2_SwapByte(W25Q128_READ_STATUS_REGISTER_1);				//交换发送读状态寄存器1的指令
	Timeout = 100000;											//给定超时计数时间
	while ((SPI2_SwapByte(W25Q128_DUMMY_BYTE) & 0x01) == 0x01)	//循环等待忙标志位
	{
		Timeout --;												//等待时，计数值自减
		if (Timeout == 0)										//自减到0后，等待超时
		{
			//超时的错误处理代码，可以添加到此处
			break;												//跳出等待，不等了
		}
	}
	SPI2_Stop();
}

//W25Q128页编程,Address:页编程的起始地址，范围：0x000000~0x7FFFFF,写入的地址范围不能跨页
void W25Q128_PageProgram(uint32_t Address, uint8_t *DataArray, uint16_t Count)
{
	uint16_t i;
	
	W25Q128_WriteEnable();						//写使能
	
	SPI2_Start();
	SPI2_SwapByte(W25Q128_PAGE_PROGRAM);		//交换发送页编程的指令
	SPI2_SwapByte(Address >> 16);				//交换发送地址23~16位
	SPI2_SwapByte(Address >> 8);				//交换发送地址15~8位
	SPI2_SwapByte(Address);						//交换发送地址7~0位
	for (i = 0; i < Count; i ++)				//循环Count次
		SPI2_SwapByte(DataArray[i]);			//依次在起始地址后写入数据
	SPI2_Stop();
	
	W25Q128_WaitBusy();							//等待忙
}

//W25Q128扇区擦除（4KB）,Address:指定扇区的地址，范围：0x000000~0x7FFFFF
void W25Q128_SectorErase(uint32_t Address)
{
	W25Q128_WriteEnable();						//写使能
	
	SPI2_Start();
	SPI2_SwapByte(W25Q128_SECTOR_ERASE_4KB);	//交换发送扇区擦除的指令
	SPI2_SwapByte(Address >> 16);				//交换发送地址23~16位
	SPI2_SwapByte(Address >> 8);				//交换发送地址15~8位
	SPI2_SwapByte(Address);						//交换发送地址7~0位
	SPI2_Stop();
	
	W25Q128_WaitBusy();							//等待忙
}

//W25Q128读取数据,Address:读取数据的起始地址，范围：0x000000~0x7FFFFF
void W25Q128_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count)
{
	uint32_t i;
	SPI2_Start();
	SPI2_SwapByte(W25Q128_READ_DATA);						//交换发送读取数据的指令
	SPI2_SwapByte(Address >> 16);							//交换发送地址23~16位
	SPI2_SwapByte(Address >> 8);							//交换发送地址15~8位
	SPI2_SwapByte(Address);									//交换发送地址7~0位
	for (i = 0; i < Count; i ++)							//循环Count次
		DataArray[i] = SPI2_SwapByte(W25Q128_DUMMY_BYTE);	//依次在起始地址后读取数据
	SPI2_Stop();
}
