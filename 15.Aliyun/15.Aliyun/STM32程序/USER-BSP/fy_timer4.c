#include "fy_timer4.h"

void Timer4_Pwm_Configuration(u16 prescaler,u16 period)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;


	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                     //���ϼ���ģʽ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;                         //����ʱ�ӷ�Ƶ���ӣ��÷�Ƶ����Ԥ��Ƶֵ��Ҫ����
 	TIM_TimeBaseStructure.TIM_Period = period-1;                                      //���ö�ʱ������
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler-1;                                 //����Ԥ��Ƶֵ
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);                                 //��Ч����

    TIM_Cmd(TIM4, ENABLE);                                                          //����������

//    TIM_ITConfig(TIM4,IT_Source,NewState);                                        //ʹ�ܶ�ʱ�������ж�  

	TIM_OCInitTypeDef  TIM_OCInitStructure;						  //��������Ƚϳ�ʼ���ṹ��			 
 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;			  //����PWM1ģʽ
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //���ʹ��
	TIM_OCInitStructure.TIM_Pulse = 0;						  	  //���ò���ȽϼĴ�����ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	  //������Ч��ƽΪ�ߵ�ƽ

	TIM_OC1Init(TIM4, &TIM_OCInitStructure); 					  //��Ч��ʼ������

	TIM_OC1PreloadConfig(TIM4, ENABLE);			  			      //ʹ������Ƚ�Ԥװ�ع���

	TIM_DMACmd(TIM4,TIM_DMA_CC1,ENABLE);					      //ʹ�ܲ���Ƚ�DMA����


}

/*********************************************END OF FILE********************************************/


