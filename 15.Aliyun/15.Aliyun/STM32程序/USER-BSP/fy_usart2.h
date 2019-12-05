#ifndef __FY_USART2_H
#define __FY_USART2_H

#include "fy_includes.h"

void Usart2_Configuration(u32 BAUD);
void Usart2_SendByte(u8 val);
void Usart2_SendBuf(u8 *buf,u8 len);
void Usart2_SendString(char *str);

#endif



/*********************************************END OF FILE********************************************/

