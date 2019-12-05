#ifndef __FY_W25QXX_H
#define __FY_W25QXX_H

#include "fy_includes.h"

//W25Xϵ��/Qϵ��оƬ�б�
//W25Q80  ID  0XEF13
//W25Q16  ID  0XEF14
//W25Q32  ID  0XEF15
//W25Q64  ID  0XEF16
//W25Q128 ID  0XEF17
#define W25Q80 	0XEF13
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17

#define W25QXX_SPI_ReadWriteByte(a)	SPI2_ReadWriteByte(a)

////////////////////////////////////////////////////////////////////////////

//ָ���
#define W25X_WriteEnable		0x06
#define W25X_WriteDisable		0x04
#define W25X_ReadStatusReg		0x05
#define W25X_WriteStatusReg		0x01
#define W25X_ReadData			0x03
#define W25X_FastReadData		0x0B
#define W25X_FastReadDual		0x3B
#define W25X_PageProgram		0x02
#define W25X_BlockErase			0xD8
#define W25X_SectorErase		0x20
#define W25X_ChipErase			0xC7
#define W25X_PowerDown			0xB9
#define W25X_ReleasePowerDown	0xAB
#define W25X_DeviceID			0xAB
#define W25X_ManufactDeviceID	0x90
#define W25X_JedecDeviceID		0x9F

void W25QXX_Configuration(void);

void W25QXX_ReadBytes(u32 addr,u8 *buf,u32 len);   	//��ȡflash
void W25QXX_WriteBytes(u32 addr,u8 *buf,u32 len);	//д��flash

void W25QXX_EraseChip(void);    	  				//��Ƭ����
void W25QXX_EraseSector(u16 addr);					//��������

extern u16 W25QXX_TYPE;	//Ĭ����W25Q64

#endif

/*********************************************END OF FILE********************************************/
