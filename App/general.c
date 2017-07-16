#define _GENERAL_C

#include <stm32f4xx.h>

static __IO uint32_t delayTime;

void Delay(uint32_t nTime)
{
	uint8_t flagSysTickCnfg = 0;
    
    if(!flagSysTickCnfg) // if systick is not config
    {
        if(SysTick_Config(SystemCoreClock/1000/3))while (1); 
        flagSysTickCnfg++;
    }
    delayTime = nTime;
    while(delayTime > 0);
}

void SysTick_Handler(void)
{
    if(delayTime > 0)delayTime--;
}
