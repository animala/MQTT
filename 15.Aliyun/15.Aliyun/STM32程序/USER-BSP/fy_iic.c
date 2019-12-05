#include "fy_iic.h"

#define SCL_H         GPIOB->BSRR = GPIO_Pin_10
#define SCL_L         GPIOB->BRR  = GPIO_Pin_10

#define SDA_H         GPIOB->BSRR = GPIO_Pin_11
#define SDA_L         GPIOB->BRR  = GPIO_Pin_11

#define SCL_read      (GPIOB->IDR & GPIO_Pin_10)
#define SDA_read      (GPIOB->IDR & GPIO_Pin_11)
#define I2C_PINS      (GPIO_Pin_10 | GPIO_Pin_11)
#define I2C_PORT      GPIOB

static void I2C_delay(void)
{
    volatile int i = 7;
    while (i)
        i--;
}


/**
 * 功能：初始化模拟IIC引脚
 * 参数：None
 * 返回值：None
 */

void I2C_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;             //定义GPIO初始化结构体

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);     //使能SDA时钟
	
	GPIO_InitStructure.GPIO_Pin = I2C_PINS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;

    GPIO_Init(I2C_PORT, &GPIO_InitStructure);    //设置生效    

}
#ifndef false
	#define false 0
	#define true 0
#endif


static void I2C_Start(void)
{
    SDA_H;
    SCL_H;
    I2C_delay();
    SDA_L;
    I2C_delay();
    SCL_L;
}

static void I2C_Stop(void)
{
    SCL_L;
    SDA_L;
    I2C_delay();
    SCL_H;
    I2C_delay();
    SDA_H;
    I2C_delay();
}

static void I2C_Ack(void)
{
    SCL_L;
    I2C_delay();
    SDA_L;
    I2C_delay();
    SCL_H;
    I2C_delay();
    SCL_L;
    I2C_delay();
}

static void I2C_NoAck(void)
{
    SCL_L;
    I2C_delay();
    SDA_H;
    I2C_delay();
    SCL_H;
    I2C_delay();
    SCL_L;
    I2C_delay();
}

static u8 I2C_WaitAck(void)
{
	u8 t=0;
    SDA_H;I2C_delay();
    SCL_H;I2C_delay();
    
    while(SDA_read) {
        SCL_L;
        return false;
    }
    SCL_L;
    return true;
}

static void I2C_SendByte(uint8_t byte)
{
    uint8_t i = 8;
    while (i--) {
        SCL_L;
        I2C_delay();
        if (byte & 0x80)
            SDA_H;
        else
            SDA_L;
        byte <<= 1;
        I2C_delay();
        SCL_H;
        I2C_delay();
    }
    SCL_L;
}

static uint8_t I2C_ReceiveByte(void)
{
    uint8_t i = 8;
    uint8_t byte = 0;

    SDA_H;
    while (i--) {
        byte <<= 1;
        SCL_L;
        I2C_delay();
        SCL_H;
        I2C_delay();
        if (SDA_read) {
            byte |= 0x01;
        }
    }
    SCL_L;
    return byte;
}


u8 i2cWriteBuffer(uint8_t addr, uint8_t reg, uint8_t len, uint8_t * data)
{
    int i;
    if (!I2C_Start())
        return false;
    I2C_SendByte(addr << 1 | I2C_Direction_Transmitter);
    if (!I2C_WaitAck()) {
        I2C_Stop();
        return false;
    }
    I2C_SendByte(reg);
    I2C_WaitAck();
    for (i = 0; i < len; i++) {
        I2C_SendByte(data[i]);
        if (!I2C_WaitAck()) {
            I2C_Stop();
            return false;
        }
    }
    I2C_Stop();
    return true;
}

u8 i2cWrite(uint8_t addr, uint8_t reg, uint8_t data)
{
    if (!I2C_Start())
        return false;
    I2C_SendByte(addr << 1 | I2C_Direction_Transmitter);
    if (!I2C_WaitAck()) {
        I2C_Stop();
        return false;
    }
    I2C_SendByte(reg);
    I2C_WaitAck();
    I2C_SendByte(data);
    I2C_WaitAck();
    I2C_Stop();
    return true;
}

u8 i2cRead(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    if (!I2C_Start())
        return false;
    I2C_SendByte(addr << 1 | I2C_Direction_Transmitter);
    if (!I2C_WaitAck()) {
        I2C_Stop();
        return false;
    }
    I2C_SendByte(reg);
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(addr << 1 | I2C_Direction_Receiver);
    I2C_WaitAck();
    while (len) {
        *buf = I2C_ReceiveByte();
        if (len == 1)
            I2C_NoAck();
        else
            I2C_Ack();
        buf++;
        len--;
    }
    I2C_Stop();
    return true;
}

