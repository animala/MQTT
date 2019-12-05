#ifndef __FY_RTC_H
#define __FY_RTC_H

#include "fy_includes.h"
//ʱ��ṹ��
typedef struct
{
    vu8 hour;
    vu8 min;
    vu8 sec;
    //������������
    vu16 w_year;
    vu8  w_month;
    vu8  w_date;
    vu8  week;
} _typedef_calendar;

extern _typedef_calendar _calendar;	//�����ṹ��


extern const u8 mon_table[12];	//ƽ����·����ڱ�
u8 Rtc_Configuration(void);        	//��ʼ��RTC,����0,ʧ��;1,�ɹ�;
u8 Is_Leap_Year(u16 year);
u8 Rtc_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);
u8 Rtc_GetTime(void);
u8 Rtc_Alarm_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);
void _RTC_IRQHandler(void);
#endif

/*********************************************END OF FILE********************************************/

