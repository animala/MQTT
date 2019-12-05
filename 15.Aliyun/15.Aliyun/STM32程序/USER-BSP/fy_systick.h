#ifndef __FY_SYSTICK_H
#define __FY_SYSTICK_H

#include "fy_includes.h"


void Systick_Configuration(void);
void Delay_us(u32 nus);
void Delay_ms(u32 nms);
u32 GetSystick_ms(void);

#endif

