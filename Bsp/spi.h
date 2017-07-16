 /* 
  * @file    st7735.h
  * @author  DK
  * @version V1.0.0
  * @date    16-July-2017
  * @brief   This file contains main routines about 1.8" st7735 TFT.
  *
  */
  
#ifndef _SPI_H
#define _SPI_H

void SPI1Init(void);
int SPI1ReadWrite(uint8_t *rbuf, const uint8_t *tbuf, int cnt);
int SPI1ReadWrite16(uint16_t *rbuf, const uint16_t *tbuf, int cnt);
	
#endif
