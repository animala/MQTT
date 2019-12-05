#include "fy_usart1.h"

//����1�ж�����
//static void Usart1_NVIC_Configuration(void)
//{
//    NVIC_InitTypeDef NVIC_InitStructure;
//    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //��ռ���ȼ�
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //�����ȼ�
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //ʹ���ⲿ�ж�ͨ��
//    NVIC_Init(&NVIC_InitStructure); //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
//}

//����1���ú���
void Usart1_Configuration(u32 BAUD)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    //��һ������ʼ��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    //�ڶ�������ʼ��GPIO
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10

    //����������ʼ�����ڲ���
    USART_InitStructure.USART_BaudRate = BAUD;//���ڲ�����
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(USART1, &USART_InitStructure); //��ʼ������1

    //���Ĳ�����������ж�
//    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
//	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//�������ڿ����ж�

    //���岽�������ж����ȼ�
//    Usart1_NVIC_Configuration();

    //��������ʹ�ܴ���
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1
    USART_ClearFlag(USART1, USART_FLAG_TC);       //��շ�����ɱ�־
}

//����1����һ���ֽ�
void Usart1_SendByte(u8 val)
{
    USART_SendData(USART1, val);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	//�ȴ��������
}
//����1����һ�����ݰ�
void Usart1_SendBuf(u8 *buf,u8 len)
{
    while(len--)	Usart1_SendByte(*buf++);
}

//����1����һ���ַ���
void Usart1_SendString(char *str)
{
    while(*str)	Usart1_SendByte(*str++);
}


/*********************************************END OF FILE********************************************/
