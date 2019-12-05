#include "fy_timer2.h"

//中断配置
static void TIM2_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4; //先占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4; //从优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
    NVIC_Init(&NVIC_InitStructure); //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

void Timer2_Configuration(u32 nus)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);//时钟使能

    //自动重装载寄存器周期的值(计数值)
    TIM_TimeBaseStructure.TIM_Period=nus-1;
    TIM_TimeBaseStructure.TIM_Prescaler= 71;//时钟预分频数为72
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;//向上计数
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//允许更新中断

    TIM2_NVIC_Configuration();

    TIM_Cmd(TIM2, ENABLE);	//使能定时器2
}

/*********************************************END OF FILE********************************************/


