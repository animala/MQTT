#include "fy_led.h"

void Led_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB,ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;		
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	Led_Off();
}

void Led_On(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);
}

void Led_Off(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_5);
}

void Led_Tog(void)
{
	GPIOB->ODR ^= GPIO_Pin_5;
}
/*********************************************END OF FILE********************************************/
