#include <stm32f4xx.h>
#include <stm32f4xx_gpio.h>
#include "st7735.h"
#include "spi.h"

#define MADCTLGRAPHICS 0x6
#define MADCTLBMP 0x2
#define ST7735_width 128
#define ST7735_height 160

#define LCD_PORT GPIOC
#define GPIO_PIN_DC GPIO_PIN_2
#define GPIO_PIN_SCE GPIO_PIN_0
#define SPILCD 

static void LcdWrite(char dc , const char *data , int nbytes)
{
	GPIO_WriteBit(LCD_PORT ,GPIO_PIN_DC , dc); // dc 1 = data , 0 = control
	GPIO_ResetBits(LCD_PORT ,GPIO_PIN_SCE);
	SPIReadWrite(SPILCD, 0, data , nbytes);
	GPIO_SetBits(LCD_PORT ,GPIO_PIN_SCE);
}

static void LcdWrite16(char dc , const uint16_t *data , int cnt)
{
GPIO_WriteBit(LCD_PORT ,GPIO_PIN_DC , dc); // dc 1 = data , 0 =control
GPIO_ResetBits(LCD_PORT ,GPIO_PIN_SCE);
spiReadWrite16(SPILCD, 0, data , cnt);
GPIO_SetBits(LCD_PORT, GPIO_PIN_SCE);
}

static void ST7735_writeCmd(uint8_t c)
{
	LcdWrite(LCD_C , &c, 1);
}