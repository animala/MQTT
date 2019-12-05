#include "fy_adc.h"

void Adc_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);		//打开ADC1时钟
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); 							//配置ADC时钟，为PCLK2的6分频，即12MHz

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    ADC_DeInit(ADC1);											//外设 ADCx 的全部寄存器重设为缺省值

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;			//独立ADC模式
    ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 	 			//扫描模式，扫描模式用于多通道采集
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;			//开启连续转换模式，即不停地进行ADC转换
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//不使用外部触发转换
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 		//采集数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 3;	 	 			//要转换的通道数目 1--16
    ADC_Init(ADC1, &ADC_InitStructure);


    //ADC转换器                      通道号    采样顺序   采样时间12.5+239 = 68

    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 	1, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 	2, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 	3, ADC_SampleTime_55Cycles5);


    ADC_Cmd(ADC1, ENABLE);//Enable ADC1


    ADC_ResetCalibration(ADC1);					//复位校准寄存器

    while(ADC_GetResetCalibrationStatus(ADC1));	//等待校准寄存器复位完成


    ADC_StartCalibration(ADC1);					//ADC校准

    while(ADC_GetCalibrationStatus(ADC1));		//等待校准完成

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//由于没有采用外部触发，所以使用软件触发ADC转换

}

/*********************************************END OF FILE********************************************/

