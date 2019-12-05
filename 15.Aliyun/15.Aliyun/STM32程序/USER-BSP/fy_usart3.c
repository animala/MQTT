#include "fy_usart3.h"

//����3���ж�����
static void Usart3_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;	   //�����ȼ�2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);
}

//����3����
void Usart3_Configuration(u32 BAUD)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    //��һ������ʼ��ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 , ENABLE);
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB,ENABLE);

    //�ڶ�������ʼ��GPIO
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //����������ʼ�����ڲ���
    USART_InitStructure.USART_BaudRate = BAUD;//���ڲ�����
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(USART3, &USART_InitStructure); //��ʼ������2

    //���Ĳ�����������ж�
//	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�������ڽ����ж�

    //���岽�������ж����ȼ�
    Usart3_NVIC_Configuration();//���ȼ�����

    //��������ʹ�ܴ���
    USART_Cmd(USART3, ENABLE);            //ʹ�ܴ���3
    USART_ClearFlag(USART3,USART_FLAG_TC);
}

//����3����һ���ֽ�
void Usart3_SendByte(u8 val)
{
    USART_SendData(USART3, val);
    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);	//�ȴ��������
}
//����3����һ�����ݰ�
void Usart3_SendBuf(u8 *buf,u8 len)
{
    while(len--)	Usart3_SendByte(*buf++);
}
//����3����һ���ַ���
void Usart3_SendString(u8 *str)
{
    while(*str!='\0')	Usart3_SendByte(*str++);
}
/*********************************************END OF FILE********************************************/
