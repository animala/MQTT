#ifndef __FY_STMFLASH_H
#define __FY_STMFLASH_H
#include "fy_includes.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
//用户根据自己的需要设置
#define STM32_FLASH_SIZE 64 	 		//所选STM32的FLASH容量大小(单位为K)

#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //字节
#else
#define STM_SECTOR_SIZE	2048
#endif

#define STM32_FLASH_WREN 1              //使能FLASH写入(0，不是能;1，使能)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH起始地址
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址

void STMFLASH_Write(u32 addr,u16 *buf,u16 len);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(u32 addr,u16 *buf,u16 len);  //从指定地址开始读出指定长度的数据

#endif
/*********************************************END OF FILE********************************************/

















