#include "fy_includes.h"

// �ض���c�⺯��printf��USARTx
int fputc(int ch, FILE *f)
{
    /* ����һ���ֽ����ݵ�USART1 */
    USART_SendData(PRINTF_USART, (uint8_t) ch);
    /* �ȴ�������� */
    while (USART_GetFlagStatus(PRINTF_USART, USART_FLAG_TC) == RESET);

    return (ch);
}
// �ض���c�⺯��scanf��USARTx
int fgetc(FILE *f)
{
    /* �ȴ�����1�������� */
    while (USART_GetFlagStatus(PRINTF_USART, USART_FLAG_RXNE) == RESET);
    return (int)USART_ReceiveData(PRINTF_USART);
}




/**
 * ���ܣ������ַ������Ƿ������һ���ַ���
 * ������
 *       dest��������Ŀ���ַ���
 *       src������������
 *       retry_ms:��ѯ��ʱʱ��
 * ����ֵ�����ҽ������0Ϊ���ҳɹ�,0Ϊʧ��
 * strstr(char* s,char* l);�����ַ�"l"��"s"
 * �ĵ�һ�γ��ֵĵط��������س��ֵ�λ�õ�����
 * �������ַ�
 */
u8 FindStr(char* dest,char* src,u16 retry_nms)
{
    retry_nms/=10;                   //��ʱʱ��

    while(strstr(dest,src)==0 && retry_nms--)//�ȴ����ڽ�����ϻ�ʱ�˳�
    {		
			Delay_ms(10);
    }
	
   if(retry_nms)     return 1;                       

   return 0; 
}

/******************************************************************************
�������ƣ���ȡоƬΨһID��
STM32ÿ��ϵ�ж�����Ψһ��һ��оƬ���кţ�96λbit��

STM32F10X �ĵ�ַ�� 0x1FFFF7E8

STM32F20X �ĵ�ַ�� 0x1FFF7A10

STM32F30X �ĵ�ַ�� 0x1FFFF7AC

STM32F40X �ĵ�ַ�� 0x1FFF7A10

STM32L1XX �ĵ�ַ�� 0x1FF80050
******************************************************************************/

void STMF10X_GetChipID(u32 *idBuf)
{
    *(idBuf+0) = *(__IO u32*)(0x1FFFF7E8);    //��ƷΨһ��ݱ�ʶ�Ĵ�����96λ��
    *(idBuf+1) = *(__IO u32*)(0x1FFFF7EC);
    *(idBuf+2) = *(__IO u32*)(0x1FFFF7F0);
    //ID���ַ�� 0x1FFFF7E8   0x1FFFF7EC  0x1FFFF7F0 ��ֻ��Ҫ��ȡ�����ַ�е����ݾͿ����ˡ�
}
void WFI_SET(void)
{
    __ASM volatile("wfi");
}
//�ر������ж�
void INTX_DISABLE(void)
{
    __ASM volatile("cpsid i");
}
//���������ж�
void INTX_ENABLE(void)
{
    __ASM volatile("cpsie i");
}
//����ջ����ַ
//addr:ջ����ַ
__asm void MSR_MSP(u32 addr)
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

/*********************************************END OF FILE********************************************/

