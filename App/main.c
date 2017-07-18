#include <stm32f4xx.h>
#include <stm32f4xx_conf.h>
#include "..\Bsp\st7735.h"
#include "general.h"

int main()
{
    ST7735Init();
    ST7735BackLight(1); // lightup BackLight LED
    FillScreen(ST7735_COLOR_RED);
    while(1);
}
