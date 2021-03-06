#include "fy_timer4.h"

void Timer4_Pwm_Configuration(u16 prescaler,u16 period)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;


	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                     //向上计数模式
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;                         //设置时钟分频因子，该分频不是预分频值，要区分
 	TIM_TimeBaseStructure.TIM_Period = period-1;                                      //设置定时器周期
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler-1;                                 //设置预分频值
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);                                 //生效更改

    TIM_Cmd(TIM4, ENABLE);                                                          //开启计数器

//    TIM_ITConfig(TIM4,IT_Source,NewState);                                        //使能定时器更新中断  

	TIM_OCInitTypeDef  TIM_OCInitStructure;						  //定义输出比较初始化结构体			 
 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;			  //设置PWM1模式
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //输出使能
	TIM_OCInitStructure.TIM_Pulse = 0;						  	  //设置捕获比较寄存器的值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	  //设置有效电平为高电平

	TIM_OC1Init(TIM4, &TIM_OCInitStructure); 					  //生效初始化设置

	TIM_OC1PreloadConfig(TIM4, ENABLE);			  			      //使能输出比较预装载功能

	TIM_DMACmd(TIM4,TIM_DMA_CC1,ENABLE);					      //使能捕获比较DMA请求


}

/*********************************************END OF FILE********************************************/


