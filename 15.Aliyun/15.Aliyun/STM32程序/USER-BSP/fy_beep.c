#include "fy_beep.h"

//vol：默认音量
//tone:默认频率
void Beep_Configuration(u8 vol,u16 tone)
{
    GPIO_InitTypeDef GPIO_InitStructure;                                        //定义GPIO初始化结构体变量

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); //使能GPIOB和复用功能时钟

    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);                        //将TIM3部分重映射到PB4

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                             //设置为复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;                                    //设置引脚4
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                           //工作速度50MHz    
    GPIO_Init(GPIOB, &GPIO_InitStructure);                                      //初始化设置生效   

	Timer3_Pwm_Configuration(36,1000);	//1MHz频率
	
    Beep_SetVol(vol);                                                          //设置蜂鸣器默认音量
    Beep_SetTone(tone);                                                        //设置蜂鸣器默认音调
}

/**
 * 功能：设置蜂鸣器的音量
 * 参数：
 *          vol:音量大小 0-99 0就是静音 99就是最大音量
 * 返回值：None
 */
void Beep_SetVol(u8 vol)
{
	Timer3_SetCompare1(Timer3_GetPeriod()*vol/100);
}

/**
 * 功能：设置蜂鸣器的音调
 * 参数：
 *          tone:音调大小单位Hz，建议100-10000
 *               另外我们封装了七音阶音调，方便大家使用
 * 注意：   这里是以定时器时钟频率1us进行计算的
 * 返回值：None
 */
void Beep_SetTone(u16 tone)
{
	Timer3_SetPeriod(1000000/tone);
}

/**
 * 功能：根据乐谱弹奏音乐
 * 参数：
 *         vol:音量大小 0-99 0就是静音 99就是最大音量
 *         musicscore:乐谱指针
 * 注意：   这里是以定时器时钟频率10us进行计算的
 * 返回值：None
 */
void Beep_PlayMusic(u8 vol,u16* musicscore)
{
    u16 i = 0;
    
    Beep_SetVol(vol);
    while(*(musicscore+i))      //判断乐谱结束标识
    {
        Beep_SetTone(*(musicscore+i));
		Beep_SetVol(vol);
        Delay_ms(*(musicscore+i+1));

        i += 2;
    }
	Beep_SetVol(0);
}

/*********************************************END OF FILE********************************************/
