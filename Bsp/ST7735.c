#include <stm32f4xx.h>
#include <stm32f4xx_gpio.h>
#include "st7735.h"
#include "spi.h"
#include "..\App\general.h"

#define MADCTLGRAPHICS 0x6
#define MADCTLBMP 0x2
#define ST7735_width 128
#define ST7735_height 160

#define LOW 0
#define HIGH 1
#define LCD_C LOW
#define LCD_D HIGH
#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_MADCTL 0x36
#define ST7735_RAMWR 0x2C
#define ST7735_RAMRD 0x2E
#define ST7735_COLMOD 0x3A
#define MADVAL(x) (((x) << 5) | 8)

#define LCD_PORT GPIOC
#define GPIO_PIN_DC GPIO_Pin_2
#define GPIO_PIN_SCE GPIO_Pin_0
#define GPIO_PIN_RST GPIO_Pin_1
#define LCD_PORT_BKL GPIOA
#define GPIO_PIN_BKL GPIO_Pin_1

struct ST7735_cmdBuf {
	uint8_t command; // ST7735 command byte
	uint8_t delay; // ms delay after
	uint8_t len; // length of parameter data
	uint8_t data [16]; // parameter data
};

static const struct ST7735_cmdBuf initializers[] = {
	// SWRESET Software reset
	{ 0x01 , 150, 0, 0},
	// SLPOUT Leave sleep mode
	{ 0x11 , 150, 0, 0},
	// FRMCTR1 , FRMCTR2 Frame Rate configuration -- Normal mode , idle
	// frame rate = fosc / (1 x 2 + 40) * (LINE + 2C + 2D)
	{ 0xB1 , 0, 3, { 0x01 , 0x2C , 0x2D }},
	{ 0xB2 , 0, 3, { 0x01 , 0x2C , 0x2D }},
	// FRMCTR3 Frame Rate configureation -- partial mode
	{ 0xB3 , 0, 6, { 0x01 , 0x2C , 0x2D , 0x01 , 0x2C , 0x2D }},
	// INVCTR Display inversion (no inversion)
	{ 0xB4 , 0, 1, { 0x07 }},
	// PWCTR1 Power control -4.6V, Auto mode
	{ 0xC0 , 0, 3, { 0xA2 , 0x02 , 0x84}},
	// PWCTR2 Power control VGH25 2.4C, VGSEL -10, VGH = 3 * AVDD
	{ 0xC1 , 0, 1, { 0xC5}},
	// PWCTR3 Power control , opamp current smal , boost frequency
	{ 0xC2 , 0, 2, { 0x0A , 0x00 }},
	// PWCTR4 Power control , BLK/2, opamp current small and medium low
	{ 0xC3 , 0, 2, { 0x8A , 0x2A}},
	// PWRCTR5 , VMCTR1 Power control
	{ 0xC4 , 0, 2, { 0x8A , 0xEE}},
	{ 0xC5 , 0, 1, { 0x0E }},
	// INVOFF Don't invert display
	{ 0x20 , 0, 0, 0},
	// Memory access directions. row address/col address , bottom to top refesh (10.1.27)
	{ ST7735_MADCTL , 0, 1, {MADVAL(MADCTLGRAPHICS)}},
	// Color mode 16 bit (10.1.30
	{ ST7735_COLMOD , 0, 1, {0x05}},
	// Column address set 0..127
	{ ST7735_CASET , 0, 4, {0x00 , 0x00 , 0x00 , 0x7F }},
	// Row address set 0..159
	{ ST7735_RASET , 0, 4, {0x00 , 0x00 , 0x00 , 0x9F }},
	// GMCTRP1 Gamma correction
	{ 0xE0 , 0, 16, {0x02 , 0x1C , 0x07 , 0x12 , 0x37 , 0x32 , 0x29 , 0x2D ,
	0x29 , 0x25 , 0x2B , 0x39 , 0x00 , 0x01 , 0x03 , 0x10 }},
	// GMCTRP2 Gamma Polarity corrction
	{ 0xE1 , 0, 16, {0x03 , 0x1d , 0x07 , 0x06 , 0x2E , 0x2C , 0x29 , 0x2D ,
	0x2E , 0x2E , 0x37 , 0x3F , 0x00 , 0x00 , 0x02 , 0x10 }},
	// DISPON Display on
	{ 0x29 , 100, 0, 0},
	// NORON Normal on
	{ 0x13 , 10, 0, 0},
	// End
	{ 0, 0, 0, 0}
};

static uint8_t madctlcurrent = MADVAL(MADCTLGRAPHICS);

static void LcdWrite(char dc , const uint8_t *data , int nbytes)
{
    BitAction dcAct;
    
    dc == 0 ? (dcAct = Bit_RESET): (dcAct = Bit_SET);
	GPIO_WriteBit(LCD_PORT ,GPIO_PIN_DC , dcAct); // dc 1 = data , 0 = control
	GPIO_ResetBits(LCD_PORT ,GPIO_PIN_SCE);
	SPI1ReadWrite(0, data , nbytes);
	GPIO_SetBits(LCD_PORT ,GPIO_PIN_SCE);
}

static void LcdWrite16(char dc , const uint16_t *data , int cnt)
{
    BitAction dcAct;
    
    dc == 0 ? (dcAct = Bit_RESET): (dcAct = Bit_SET);
	GPIO_WriteBit(LCD_PORT ,GPIO_PIN_DC , dcAct); // dc 1 = data , 0 =control
	GPIO_ResetBits(LCD_PORT ,GPIO_PIN_SCE);
	SPI1ReadWrite16( 0, data , cnt);
	GPIO_SetBits(LCD_PORT, GPIO_PIN_SCE);
}

static void ST7735_writeCmd(uint8_t c)
{
	LcdWrite(LCD_C , &c, 1);
}

void ST7735SetAddrWindow(uint16_t x0 , uint16_t y0 ,uint16_t x1 , uint16_t y1 , uint8_t madctl)
{
	madctl = MADVAL(madctl);
	if (madctl != madctlcurrent){
		ST7735_writeCmd(ST7735_MADCTL);
		LcdWrite(LCD_D , &madctl , 1);
		madctlcurrent = madctl;
	}
	ST7735_writeCmd(ST7735_CASET);
	LcdWrite16(LCD_D , &x0 , 1);
	LcdWrite16(LCD_D , &x1 , 1);
	ST7735_writeCmd(ST7735_RASET);
	LcdWrite16(LCD_D , &y0 , 1);
	LcdWrite16(LCD_D , &y1 , 1);
	ST7735_writeCmd(ST7735_RAMWR);
}

void ST7735PushColor(uint16_t *color , int cnt)
{
	LcdWrite16(LCD_D , color , cnt);
}

void ST7735BackLight(uint8_t on)
{
	if (on)
		GPIO_WriteBit(LCD_PORT_BKL ,GPIO_PIN_BKL, Bit_RESET); // LOW
	else
		GPIO_WriteBit(LCD_PORT_BKL ,GPIO_PIN_BKL , Bit_SET); // HIGH
}

void FillScreen(uint16_t color)
{
	uint8_t x,y;
	ST7735SetAddrWindow (0, 0, ST7735_width -1, ST7735_height -1,!MADCTLGRAPHICS);
	for (x=0; x < ST7735_width; x++) {
		for (y=0; y < ST7735_height; y++) {
			ST7735PushColor (&color ,1);
		}
	}
}

void ST7735Init(void)
{
	GPIO_InitTypeDef GPIOInitStructure;
	const struct ST7735_cmdBuf *cmd;
	// set up pins
	GPIO_StructInit(&GPIOInitStructure);
	GPIOInitStructure.GPIO_Pin = GPIO_PIN_SCE | GPIO_PIN_RST | GPIO_PIN_DC;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIOInitStructure.GPIO_Speed = GPIO_Low_Speed;
    GPIOInitStructure.GPIO_OType = GPIO_OType_PP;
    GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC,&GPIOInitStructure);
	
	GPIO_StructInit(&GPIOInitStructure);
	GPIOInitStructure.GPIO_Pin = GPIO_PIN_BKL;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIOInitStructure.GPIO_Speed = GPIO_Low_Speed;    
    GPIOInitStructure.GPIO_OType = GPIO_OType_PP;
    GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIOInitStructure);
	
    SPI1Init();
	// set cs , reset low
	GPIO_WriteBit(LCD_PORT ,GPIO_PIN_SCE , Bit_SET); //HIGH
	GPIO_WriteBit(LCD_PORT ,GPIO_PIN_RST , Bit_SET);
	Delay (10);
	GPIO_WriteBit(LCD_PORT ,GPIO_PIN_RST , Bit_RESET);//LOW
	Delay (10);
	GPIO_WriteBit(LCD_PORT ,GPIO_PIN_RST , Bit_SET);
	Delay (10);
    
	// Send initialization commands to ST7735
	for (cmd = initializers; cmd ->command; cmd ++)
	{
		LcdWrite(LCD_C , &(cmd ->command), 1);
		if (cmd ->len)
			LcdWrite(LCD_D , cmd ->data , cmd ->len);
		if (cmd ->delay)
			Delay(cmd ->delay);
	}
}
