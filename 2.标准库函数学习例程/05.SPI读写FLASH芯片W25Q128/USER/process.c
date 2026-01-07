#include "process.h"

void process(void)
{
	uint8_t MID = 0;
	uint16_t DID = 0;
	uint8_t ArrayWrite[] = {0xAA,0xBB,0xCC,0xDD};
	uint8_t ArrayRead[4];
	
	W25Q128_SectorErase(0x000000);
	W25Q128_PageProgram(0x000000,ArrayWrite,4);
	
	while(1)
	{
		OLED_Clear();
		
		OLED_ShowString(0,0,"Start",OLED_8X16);
		OLED_ShowString(0,16,"MID:   DID:",OLED_8X16);
		OLED_ShowString(0,32,"W:",OLED_8X16);
		OLED_ShowString(0,48,"R:",OLED_8X16);
		
		W25Q128_ReadID(&MID,&DID);
		OLED_ShowHexNum(32,16,MID,2,OLED_8X16);
		OLED_ShowHexNum(88,16,DID,4,OLED_8X16);
		
		OLED_ShowHexNum(16,32,ArrayWrite[0],2,OLED_8X16);
		OLED_ShowHexNum(40,32,ArrayWrite[1],2,OLED_8X16);
		OLED_ShowHexNum(64,32,ArrayWrite[2],2,OLED_8X16);
		OLED_ShowHexNum(88,32,ArrayWrite[3],2,OLED_8X16);
		
		W25Q128_ReadData(0x000000,ArrayRead,4);
		OLED_ShowHexNum(16,48,ArrayRead[0],2,OLED_8X16);
		OLED_ShowHexNum(40,48,ArrayRead[1],2,OLED_8X16);
		OLED_ShowHexNum(64,48,ArrayRead[2],2,OLED_8X16);
		OLED_ShowHexNum(88,48,ArrayRead[3],2,OLED_8X16);
		
		OLED_Update();
	}
}
