#include "fy_systick.h"

u32 System_ms=0;

u32 usTicks;
void Systick_Configuration(void)
{
	RCC_ClocksTypeDef RCC_ClocksStucture;
	RCC_GetClocksFreq(&RCC_ClocksStucture);	
	usTicks = RCC_ClocksStucture.SYSCLK_Frequency/1000000;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	SysTick_Config(RCC_ClocksStucture.SYSCLK_Frequency/1000);
}

u32 GetSystick_us(void)
{
	register u32 ms,cycle_cnt;
	do{
		ms = System_ms;
		cycle_cnt = SysTick->VAL;
	}while(ms != System_ms);
	return (System_ms*1000) + (usTicks * 1000 - cycle_cnt) / usTicks;
}

u32 GetSystick_ms(void)
{
	return System_ms;
}

void Delay_us(u32 nus)
{
	u32 now = GetSystick_us();
	while(GetSystick_us() - now < nus);
}

void Delay_ms(u32 nms)
{
	while(nms--)
		Delay_us(1000);
}



void SysTick_Handler(void)
{
	System_ms++;
}
