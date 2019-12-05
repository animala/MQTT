#include "fy_ws2812b.h"



#if 0

#define WS2812B_PIN_H() GPIO_SetBits(GPIOB,GPIO_Pin_6)
#define WS2812B_PIN_L() GPIO_ResetBits(GPIOB,GPIO_Pin_6)


void Ws2812b_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB,ENABLE);//1、配置时钟
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;		//2、配置GPIO
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

static void Ws2812b_BitL(void)
{
    WS2812B_PIN_H();
    __nop();__nop();__nop();__nop();__nop();__nop();

    WS2812B_PIN_L();
    __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
    __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
    __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
    __nop();__nop();
}

static void Ws2812b_BitH(void)
{
    WS2812B_PIN_H();
	
    __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
    __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
    __nop();__nop();
    

    WS2812B_PIN_L();
    __nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
    __nop();__nop();

}

static void Ws2812b_WriteByte(uint8_t val)
{
    uint8_t i=0;
    for(i=0; i<8; i++)
    {
        if((val<<i)&0x01)
            Ws2812b_BitH();
        else
            Ws2812b_BitL();
    }
}

void Ws2812b_Write(u32 color)
{
    Ws2812b_WriteByte(BYTE1(color));//green
    Ws2812b_WriteByte(BYTE2(color));//red
    Ws2812b_WriteByte(BYTE0(color));//blue
}


#else
extern _typdef_rgb _rgb;

void Ws2812b_Configuration(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;		
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	PWM_DMA_Configuration((u32)_rgb.buf,sizeof(_rgb.buf));
	Timer4_Pwm_Configuration(1,90);
		
}

void Ws2812b_Write(u32 color)
{
    u8 i;
	
	DMA_Cmd(DMA1_Channel1, DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel1,sizeof(_rgb.buf));
    for(i=0;i<24;++i)
    {
        if((color<<i)&0x800000)     //高位先发，此时高位为1时
        {
            _rgb.buf[i+45] = 61;       //68%占空比           
        }
		else 
        {
            _rgb.buf[i+45] = 28;       //32%占空比
        }
    }
	DMA_Cmd(DMA1_Channel1, ENABLE);
}
#endif

/*********************************************END OF FILE********************************************/
