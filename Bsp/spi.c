#include <stm32f4xx.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_spi.h>
#include "spi.h"

void SPIInit()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //Enable RCC GPIOA
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);// Enable RCC SPI1
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource4,GPIO_AF_SPI1);// Set Pin Alternate Function(SPI)
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1);
	
	GPIO_InitTypeDef GPIOStructure; // Configure GPIO Pins
	
	GPIO_StructInit(&GPIOStructure);
	GPIOStructure.GPIO_Pin = GPIO_Pin_4; // PA4:NSS
	GPIOStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOA, &GPIOStructure);
	
	GPIO_StructInit(&GPIOStructure);//Config Pins assosiate to SPI
	GPIOStructure.GPIO_Pin = GPIO_Pin_5| GPIO_Pin_6|GPIO_Pin_7;
	GPIOStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIOStructure.GPIO_OType = GPIO_OType_PP;
	GPIOStructure.GPIO_Speed = GPIO_Fast_Speed;
	GPIO_Init(GPIOA, &GPIOStructure);
	
	SPI_InitTypeDef SPIStructure;//Configure SPI
	SPIStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPIStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPIStructure.SPI_Mode = SPI_Mode_Master;
	SPIStructure.SPI_DataSize = SPI_DataSize_8b;
	SPIStructure.SPI_CPOL = SPI_CPOL_Low;
	SPIStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPIStructure.SPI_NSS = SPI_NSS_Soft;
	SPIStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
	SPIStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPIStructure);
}

int SPI1ReadWrite(uint8_t *rbuf, const uint8_t *tbuf, int cnt)
{
	int i;
	
	SPI_DataSizeConfig(SPI1, SPI_DataSize_8b);
	for(i = 0; i < cnt; i++){
		if(tbuf){
				SPI_I2S_SendData(SPI1, *tbuf++);
		}else{
				SPI_I2S_SendData(SPI1, 0xFF);
		}
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
		if(rbuf){
			*rbuf++ = SPI_I2S_ReceiveData(SPI1);
		}else{
			SPI_I2S_ReceiveData(SPI1);
		}
	}
	return i;
}

int SPI1ReadWrite16(uint8_t *rbuf, const uint8_t *tbuf, int cnt)
{
	int i;
	
	SPI_DataSizeConfig(SPI1, SPI_DataSize_16b);
	for(i = 0; i < cnt; i++){
		if(tbuf){
				SPI_I2S_SendData(SPI1, *tbuf++);
		}else{
				SPI_I2S_SendData(SPI1, 0xFF);
		}
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)== RESET);
		if(rbuf){
			*rbuf++ = SPI_I2S_ReceiveData(SPI1);
		}else{
			SPI_I2S_ReceiveData(SPI1);
		}
	}
	return i;
}