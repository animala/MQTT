#ifndef __FY_STMFLASH_H
#define __FY_STMFLASH_H
#include "fy_includes.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
//�û������Լ�����Ҫ����
#define STM32_FLASH_SIZE 64 	 		//��ѡSTM32��FLASH������С(��λΪK)

#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //�ֽ�
#else
#define STM_SECTOR_SIZE	2048
#endif

#define STM32_FLASH_WREN 1              //ʹ��FLASHд��(0��������;1��ʹ��)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ

void STMFLASH_Write(u32 addr,u16 *buf,u16 len);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 addr,u16 *buf,u16 len);  //��ָ����ַ��ʼ����ָ�����ȵ�����

#endif
/*********************************************END OF FILE********************************************/

















