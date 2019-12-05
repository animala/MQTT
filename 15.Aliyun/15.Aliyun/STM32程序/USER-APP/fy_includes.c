#include "fy_includes.h"

// 重定向c库函数printf到USARTx
int fputc(int ch, FILE *f)
{
    /* 发送一个字节数据到USART1 */
    USART_SendData(PRINTF_USART, (uint8_t) ch);
    /* 等待发送完毕 */
    while (USART_GetFlagStatus(PRINTF_USART, USART_FLAG_TC) == RESET);

    return (ch);
}
// 重定向c库函数scanf到USARTx
int fgetc(FILE *f)
{
    /* 等待串口1输入数据 */
    while (USART_GetFlagStatus(PRINTF_USART, USART_FLAG_RXNE) == RESET);
    return (int)USART_ReceiveData(PRINTF_USART);
}




/**
 * 功能：查找字符串中是否包含另一个字符串
 * 参数：
 *       dest：待查找目标字符串
 *       src：待查找内容
 *       retry_ms:查询超时时间
 * 返回值：查找结果，非0为查找成功,0为失败
 * strstr(char* s,char* l);查找字符"l"在"s"
 * 的第一次出现的地方，并返回出现的位置到结束
 * 的所有字符
 */
u8 FindStr(char* dest,char* src,u16 retry_nms)
{
    retry_nms/=10;                   //超时时间

    while(strstr(dest,src)==0 && retry_nms--)//等待串口接收完毕或超时退出
    {		
			Delay_ms(10);
    }
	
   if(retry_nms)     return 1;                       

   return 0; 
}

/******************************************************************************
函数名称：读取芯片唯一ID码
STM32每个系列都会有唯一的一个芯片序列号（96位bit）

STM32F10X 的地址是 0x1FFFF7E8

STM32F20X 的地址是 0x1FFF7A10

STM32F30X 的地址是 0x1FFFF7AC

STM32F40X 的地址是 0x1FFF7A10

STM32L1XX 的地址是 0x1FF80050
******************************************************************************/

void STMF10X_GetChipID(u32 *idBuf)
{
    *(idBuf+0) = *(__IO u32*)(0x1FFFF7E8);    //产品唯一身份标识寄存器（96位）
    *(idBuf+1) = *(__IO u32*)(0x1FFFF7EC);
    *(idBuf+2) = *(__IO u32*)(0x1FFFF7F0);
    //ID码地址： 0x1FFFF7E8   0x1FFFF7EC  0x1FFFF7F0 ，只需要读取这个地址中的数据就可以了。
}
void WFI_SET(void)
{
    __ASM volatile("wfi");
}
//关闭所有中断
void INTX_DISABLE(void)
{
    __ASM volatile("cpsid i");
}
//开启所有中断
void INTX_ENABLE(void)
{
    __ASM volatile("cpsie i");
}
//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr)
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

/*********************************************END OF FILE********************************************/

