#ifndef __SPI2_H
#define __SPI2_H

#include "stm32f10x.h"            // Device header
#include "usart.h"

#define SPI2_PORT		GPIOB
#define SPI2_CS_PIN		GPIO_Pin_12
#define SPI2_SCK_PIN	GPIO_Pin_13
#define SPI2_MISO_PIN	GPIO_Pin_14
#define SPI2_MOSI_PIN	GPIO_Pin_15

void SPI2_Init(void);
void SPI2_W_CS(uint8_t BitValue);
void SPI2_W_SCK(uint8_t BitValue);
void SPI2_W_MOSI(uint8_t BitValue);
uint8_t SPI2_R_MISO(void);
void SPI2_Start(void);
void SPI2_Stop(void);
uint8_t SPI2_SwapByte(uint8_t ByteSend);

#endif
