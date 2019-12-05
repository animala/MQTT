#include "fy_dma.h"

void Adc_DMA_Configuration(u32 Buff_Addr,u16 Buff_Len)
{
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);						//使能DMA时钟

    DMA_DeInit(DMA1_Channel1);												//DMA1_Channel1寄存器缺省

    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(ADC1->DR));	 		//ADC地址
    DMA_InitStructure.DMA_MemoryBaseAddr = Buff_Addr;						//内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; 						//外设作为数据传输的来源
    DMA_InitStructure.DMA_BufferSize = Buff_Len;							// DMA 通道的 DMA 缓存的大小，单位为数据单位
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		//外设地址固定
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  				//内存地址自增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//外设数据宽度半字
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;							//循环传输
    DMA_InitStructure.DMA_Priority = DMA_Priority_High; 					//高优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;							//DMA 通道 x 没有设置为内存到内存传输
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    DMA_Cmd(DMA1_Channel1, ENABLE);											//Enable DMA channel1

    ADC_DMACmd(ADC1,ENABLE);
}


void PWM_DMA_Configuration(u32 Buff_Addr,u16 Buff_Len)
{
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_DeInit(DMA1_Channel1);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&TIM4->CCR1);                       //外设地址  
	DMA_InitStructure.DMA_MemoryBaseAddr = Buff_Addr;                           //存储器地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                            //数据方向为存储器到外设
	DMA_InitStructure.DMA_BufferSize = Buff_Len;                                //单次发送数据个数
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;              //外设地址不自增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                       //内存地址自增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;   //设置外设数据位宽为半字(16位)
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;       //设置存储器数据位宽为半字(16位)
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                                 //正常在工作模式，另外一个是循环模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;                           //设置最高优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                  //失能存储器到存储器传输
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);                                        //初始化配置生效

    DMA_Cmd(DMA1_Channel1, ENABLE);

}

/*********************************************END OF FILE********************************************/

