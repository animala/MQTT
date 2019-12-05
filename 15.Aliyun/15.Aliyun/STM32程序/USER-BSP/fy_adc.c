#include "fy_adc.h"

void Adc_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);		//��ADC1ʱ��
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); 							//����ADCʱ�ӣ�ΪPCLK2��6��Ƶ����12MHz

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    ADC_DeInit(ADC1);											//���� ADCx ��ȫ���Ĵ�������Ϊȱʡֵ

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;			//����ADCģʽ
    ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 	 			//ɨ��ģʽ��ɨ��ģʽ���ڶ�ͨ���ɼ�
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;			//��������ת��ģʽ������ͣ�ؽ���ADCת��
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//��ʹ���ⲿ����ת��
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 		//�ɼ������Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = 3;	 	 			//Ҫת����ͨ����Ŀ 1--16
    ADC_Init(ADC1, &ADC_InitStructure);


    //ADCת����                      ͨ����    ����˳��   ����ʱ��12.5+239 = 68

    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 	1, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 	2, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 	3, ADC_SampleTime_55Cycles5);


    ADC_Cmd(ADC1, ENABLE);//Enable ADC1


    ADC_ResetCalibration(ADC1);					//��λУ׼�Ĵ���

    while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ�У׼�Ĵ�����λ���


    ADC_StartCalibration(ADC1);					//ADCУ׼

    while(ADC_GetCalibrationStatus(ADC1));		//�ȴ�У׼���

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//����û�в����ⲿ����������ʹ���������ADCת��

}

/*********************************************END OF FILE********************************************/

