#ifndef __FY_RTC_H
#define __FY_RTC_H

#include "fy_includes.h"
//时间结构体
typedef struct
{
    vu8 hour;
    vu8 min;
    vu8 sec;
    //公历日月年周
    vu16 w_year;
    vu8  w_month;
    vu8  w_date;
    vu8  week;
} _typedef_calendar;

extern _typedef_calendar _calendar;	//日历结构体


extern const u8 mon_table[12];	//平年的月份日期表
u8 Rtc_Configuration(void);        	//初始化RTC,返回0,失败;1,成功;
u8 Is_Leap_Year(u16 year);
u8 Rtc_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);
u8 Rtc_GetTime(void);
u8 Rtc_Alarm_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);
void _RTC_IRQHandler(void);
#endif

/*********************************************END OF FILE********************************************/

