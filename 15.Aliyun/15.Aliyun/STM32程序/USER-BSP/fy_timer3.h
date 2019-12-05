#ifndef __FY_TIMER3_H
#define __FY_TIMER3_H

#include "fy_includes.h"

void Timer3_Pwm_Configuration(u16 prescaler,u16 period);
void Timer3_SetPeriod(u16 Autoreload);
u16  Timer3_GetPeriod(void);
void Timer3_SetCompare1(u16 Compare1);
u16  Timer3_GetCompare1(void);

#endif

/*********************************************END OF FILE********************************************/
