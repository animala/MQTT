#ifndef __FY_USART3_H
#define __FY_USART3_H

#include "fy_includes.h"

void Usart3_Configuration(u32 BAUD);
void Usart3_SendByte(u8 val);
void Usart3_SendBuf(u8 *buf,u8 len);
void Usart3_SendString(u8 *str);
void _USART3_IRQHandler(void);

#endif



/*********************************************END OF FILE********************************************/

