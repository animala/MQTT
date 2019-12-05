#include "fy_timer2.h"

//�ж�����
static void TIM2_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4; //��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4; //�����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure); //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}

void Timer2_Configuration(u32 nus)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);//ʱ��ʹ��

    //�Զ���װ�ؼĴ������ڵ�ֵ(����ֵ)
    TIM_TimeBaseStructure.TIM_Period=nus-1;
    TIM_TimeBaseStructure.TIM_Prescaler= 71;//ʱ��Ԥ��Ƶ��Ϊ72
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;//���ϼ���
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//��������ж�

    TIM2_NVIC_Configuration();

    TIM_Cmd(TIM2, ENABLE);	//ʹ�ܶ�ʱ��2
}

/*********************************************END OF FILE********************************************/


