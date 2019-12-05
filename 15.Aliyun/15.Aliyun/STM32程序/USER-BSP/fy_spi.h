#ifndef __FY_SPI_H
#define __FY_SPI_H

#include "fy_includes.h"

extern SPI_InitTypeDef  SPI_InitStructure;

typedef enum
{
    _NRF24L01_SPI_MODE=0,
    _LT8920_SPI_MODE,
    _FLASH_SPI_MODE,
    _SPI_MODE_MAX
}_typdef_spi_mode;

#define W25QXX_CS_RCC   RCC_APB2Periph_GPIOB
#define W25QXX_CS_PORT  GPIOB
#define W25QXX_CS_PIN   GPIO_Pin_12
#define W25QXX_CS_H()	GPIO_SetBits(W25QXX_CS_PORT,W25QXX_CS_PIN)
#define W25QXX_CS_L()	GPIO_ResetBits(W25QXX_CS_PORT,W25QXX_CS_PIN)

//#define NRF24L01_CSN_RCC    RCC_APB2Periph_GPIOB
//#define NRF24L01_CSN_PORT   GPIOB
//#define NRF24L01_CSN_PIN    GPIO_Pin_12
//#define NRF24L01_CSN_H()	GPIO_SetBits(NRF24L01_CSN_PORT,NRF24L01_CSN_PIN)
//#define NRF24L01_CSN_L()	GPIO_ResetBits(NRF24L01_CSN_PORT,NRF24L01_CSN_PIN)

//#define LT8920_CS_RCC    RCC_APB2Periph_GPIOB
//#define LT8920_CS_PORT   GPIOB
//#define LT8920_CS_PIN    GPIO_Pin_12
//#define LT8920_CS_H()	 GPIO_SetBits(LT8920_CS_PORT,LT8920_CS_PIN)
//#define LT8920_CS_L()	 GPIO_ResetBits(LT8920_CS_PORT,LT8920_CS_PIN)

void SPI2_Configuration(void);			 //初始化SPI口
void SPI2_SetSpeed(u8 SpeedSet); //设置SPI速度
u8 SPI2_ReadWriteByte(u8 TxData);//SPI总线读写一个字节
void SPI2_ChangeMode(u8 mode);

#endif

