#include "fy_usart2.h"

//����1�ж�����
static void Usart2_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //�����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //ʹ���ⲿ�ж�ͨ��
    NVIC_Init(&NVIC_InitStructure); //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}

//����1���ú���
void Usart2_Configuration(u32 BAUD)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/*����USART2��GPIOʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/*GPIO����*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	  
	/* UART2���� */
	USART_InitStructure.USART_BaudRate = BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART2, &USART_InitStructure); 
	USART_Cmd(USART2, ENABLE);
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
  USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);	
	
	Usart2_NVIC_Configuration();
}

//����1����һ���ֽ�
void Usart2_SendByte(u8 val)
{
    USART_SendData(USART2, val);
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	//�ȴ��������
}
//����1����һ�����ݰ�
void Usart2_SendBuf(u8 *buf,u8 len)
{
    while(len--)	Usart2_SendByte(*buf++);
}

//����1����һ���ַ���
void Usart2_SendString(char *str)
{
    while(*str)	Usart2_SendByte(*str++);
}


/*********************************************END OF FILE********************************************/
