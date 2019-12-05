#include "fy_usart3.h"

//串口3终中断配置
static void Usart3_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;	   //子优先级2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);
}

//串口3配置
void Usart3_Configuration(u32 BAUD)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    //第一步：初始化时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 , ENABLE);
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB,ENABLE);

    //第二步：初始化GPIO
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //第三步：初始化串口参数
    USART_InitStructure.USART_BaudRate = BAUD;//串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART3, &USART_InitStructure); //初始化串口2

    //第四步：开启相关中断
//	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断

    //第五步：配置中断优先级
    Usart3_NVIC_Configuration();//优先级配置

    //第六步：使能串口
    USART_Cmd(USART3, ENABLE);            //使能串口3
    USART_ClearFlag(USART3,USART_FLAG_TC);
}

//串口3发送一个字节
void Usart3_SendByte(u8 val)
{
    USART_SendData(USART3, val);
    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);	//等待发送完成
}
//串口3发送一个数据包
void Usart3_SendBuf(u8 *buf,u8 len)
{
    while(len--)	Usart3_SendByte(*buf++);
}
//串口3发送一个字符串
void Usart3_SendString(u8 *str)
{
    while(*str!='\0')	Usart3_SendByte(*str++);
}
/*********************************************END OF FILE********************************************/
