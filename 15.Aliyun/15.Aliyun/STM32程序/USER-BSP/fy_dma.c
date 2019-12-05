#include "fy_dma.h"

void Adc_DMA_Configuration(u32 Buff_Addr,u16 Buff_Len)
{
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);						//ʹ��DMAʱ��

    DMA_DeInit(DMA1_Channel1);												//DMA1_Channel1�Ĵ���ȱʡ

    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(ADC1->DR));	 		//ADC��ַ
    DMA_InitStructure.DMA_MemoryBaseAddr = Buff_Addr;						//�ڴ��ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; 						//������Ϊ���ݴ������Դ
    DMA_InitStructure.DMA_BufferSize = Buff_Len;							// DMA ͨ���� DMA ����Ĵ�С����λΪ���ݵ�λ
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//�����ַ�̶�
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  				//�ڴ��ַ����
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//�������ݿ�Ȱ���
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;							//ѭ������
    DMA_InitStructure.DMA_Priority = DMA_Priority_High; 					//�����ȼ�
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;							//DMA ͨ�� x û������Ϊ�ڴ浽�ڴ洫��
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    DMA_Cmd(DMA1_Channel1, ENABLE);											//Enable DMA channel1

    ADC_DMACmd(ADC1,ENABLE);
}


void PWM_DMA_Configuration(u32 Buff_Addr,u16 Buff_Len)
{
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_DeInit(DMA1_Channel1);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&TIM4->CCR1);                       //�����ַ  
	DMA_InitStructure.DMA_MemoryBaseAddr = Buff_Addr;                           //�洢����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                            //���ݷ���Ϊ�洢��������
	DMA_InitStructure.DMA_BufferSize = Buff_Len;                                //���η������ݸ���
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;              //�����ַ������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                       //�ڴ��ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;   //������������λ��Ϊ����(16λ)
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;       //���ô洢������λ��Ϊ����(16λ)
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                                 //�����ڹ���ģʽ������һ����ѭ��ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;                           //����������ȼ� 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                  //ʧ�ܴ洢�����洢������
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);                                        //��ʼ��������Ч

    DMA_Cmd(DMA1_Channel1, ENABLE);

}

/*********************************************END OF FILE********************************************/

