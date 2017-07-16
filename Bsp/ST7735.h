 /* @file    st7735.h
  * @author  DK
  * @version V1.0.0
  * @date    16-July-2017
  * @brief   This file contains main routines about 1.8" st7735 TFT.
  */
  
#ifndef _ST7735_H
#define _ST7735_H

void ST7735Init();
void ST7735BackLight(uint8_t on);
void FillScreen(uint16_t color);
void ST7735SetAddrWindow(uint16_t x0 , uint16_t y0 ,uint16_t x1 , uint16_t y1 , uint8_t madctl);
void ST7735PushColor(uint16_t *color , int cnt);


#endif