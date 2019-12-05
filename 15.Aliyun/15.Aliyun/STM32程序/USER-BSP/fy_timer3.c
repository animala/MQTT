#include "fy_timer3.h"

void Timer3_Pwm_Configuration(u16 prescaler,u16 period)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                     //���ϼ���ģʽ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;                         //����ʱ�ӷ�Ƶ���ӣ��÷�Ƶ����Ԥ��Ƶֵ��Ҫ����
 	TIM_TimeBaseStructure.TIM_Period = period;                                      //���ö�ʱ������
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler-1;                              //����Ԥ��Ƶֵ
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);                                 //��Ч����

    TIM_Cmd(TIM3, ENABLE);                                                          //����������

//    TIM_ITConfig(TIM3,IT_Source,NewState);                                        //ʹ�ܶ�ʱ�������ж�  

	TIM_OCInitTypeDef  TIM_OCInitStructure;						  //��������Ƚϳ�ʼ���ṹ��			 
 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;			  //����PWM1ģʽ
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //���ʹ��
	TIM_OCInitStructure.TIM_Pulse = 0;						  	  //���ò���ȽϼĴ�����ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	  //������Ч��ƽΪ�ߵ�ƽ
	TIM_OC1Init(TIM3, &TIM_OCInitStructure); 					  //��Ч��ʼ������

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);			  //ʹ������Ƚ�Ԥװ��

	TIM_ARRPreloadConfig(TIM3,ENABLE);
}

void Timer3_SetPeriod(u16 Autoreload)
{
	TIM3->ARR = Autoreload;
}

u16 Timer3_GetPeriod(void)
{
	return TIM3->ARR;
}

void Timer3_SetCompare1(u16 Compare1)
{
	TIM3->CCR1 = Compare1;
}

u16 Timer3_GetCompare1(void)
{
	return TIM3->CCR1;
}

/*********************************************END OF FILE********************************************/


