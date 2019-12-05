#ifndef __FY_INCLUDES_H
#define __FY_INCLUDES_H


#include "stm32f10x.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdarg.h"


#define BYTE0(dwTemp)       (*( char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))

#define PRINTF_USART USART1


//����Ϊ��ຯ��
void WFI_SET(void);		//ִ��WFIָ��
void INTX_DISABLE(void);//�ر������ж�
void INTX_ENABLE(void);	//���������ж�
void MSR_MSP(u32 addr);	//���ö�ջ��ַ
u8 FindStr(char* dest,char* src,u16 retry_nms);
void STMF10X_GetChipID(u32 *idBuf);//��ȡоƬ��UUID

#include "fy_systick.h"
#include "fy_led.h"
#include "fy_key.h"
#include "fy_usart1.h"
#include "fy_adc.h"
#include "fy_dma.h"
#include "fy_timer3.h"
#include "fy_timer4.h"
#include "fy_ws2812b.h"
#include "fy_beep.h"
#include "fy_ntc.h"
#include "fy_oled12864.h"
#include "fy_oledfont.h"
#include "fy_spi.h"
#include "fy_w25qxx.h"
#include "fy_malloc.h"
#include "fy_usart2.h"
#include "fy_network.h"
#include "fy_mqtt.h"

#endif

