#include "fy_beep.h"

//vol��Ĭ������
//tone:Ĭ��Ƶ��
void Beep_Configuration(u8 vol,u16 tone)
{
    GPIO_InitTypeDef GPIO_InitStructure;                                        //����GPIO��ʼ���ṹ�����

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); //ʹ��GPIOB�͸��ù���ʱ��

    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);                        //��TIM3������ӳ�䵽PB4

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                             //����Ϊ�����������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;                                    //��������4
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                           //�����ٶ�50MHz    
    GPIO_Init(GPIOB, &GPIO_InitStructure);                                      //��ʼ��������Ч   

	Timer3_Pwm_Configuration(36,1000);	//1MHzƵ��
	
    Beep_SetVol(vol);                                                          //���÷�����Ĭ������
    Beep_SetTone(tone);                                                        //���÷�����Ĭ������
}

/**
 * ���ܣ����÷�����������
 * ������
 *          vol:������С 0-99 0���Ǿ��� 99�����������
 * ����ֵ��None
 */
void Beep_SetVol(u8 vol)
{
	Timer3_SetCompare1(Timer3_GetPeriod()*vol/100);
}

/**
 * ���ܣ����÷�����������
 * ������
 *          tone:������С��λHz������100-10000
 *               �������Ƿ�װ��������������������ʹ��
 * ע�⣺   �������Զ�ʱ��ʱ��Ƶ��1us���м����
 * ����ֵ��None
 */
void Beep_SetTone(u16 tone)
{
	Timer3_SetPeriod(1000000/tone);
}

/**
 * ���ܣ��������׵�������
 * ������
 *         vol:������С 0-99 0���Ǿ��� 99�����������
 *         musicscore:����ָ��
 * ע�⣺   �������Զ�ʱ��ʱ��Ƶ��10us���м����
 * ����ֵ��None
 */
void Beep_PlayMusic(u8 vol,u16* musicscore)
{
    u16 i = 0;
    
    Beep_SetVol(vol);
    while(*(musicscore+i))      //�ж����׽�����ʶ
    {
        Beep_SetTone(*(musicscore+i));
		Beep_SetVol(vol);
        Delay_ms(*(musicscore+i+1));

        i += 2;
    }
	Beep_SetVol(0);
}

/*********************************************END OF FILE********************************************/
