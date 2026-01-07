#ifndef __I2C1_H
#define __I2C1_H

#include "stm32f10x.h"            // Device header
#include "delay.h"

#define I2C1_PORT		GPIOB
#define I2C1_SCL_PIN	GPIO_Pin_6
#define I2C1_SDA_PIN	GPIO_Pin_7

void I2C1_Init(void);
void I2C1_W_SCL(uint8_t BitValue);
void I2C1_W_SDA(uint8_t BitValue);
uint8_t I2C1_R_SDA(void);
void I2C1_Start(void);
void I2C1_Stop(void);
void I2C1_SendByte(uint8_t Byte);
uint8_t I2C1_ReceiveByte(void);
void I2C1_SendAck(uint8_t AckBit);
uint8_t I2C1_ReceiveAck(void);

#endif
