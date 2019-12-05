#ifndef __FY_OLED_12864_H
#define __FY_OLED_12864_H

#include "fy_includes.h"

void Oled_Configuration(void);
void Oled_RefreshGram(void);
void Oled_Clear(void);
void Oled_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 draw);
void Oled_Reverse(u8 x1,u8 y1,u8 x2,u8 y2);
void Oled_ShowChar(u8 x,u8 y,u8 chr,u8 fw,u8 fh,u8 mode);
void Oled_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 fw,u8 fh);
void Oled_ShowString(u8 x,u8 y,char *str,u8 fw,u8 fh,u8 mode);
void Oled_ShowText(u8 x,u8 y,char *str,u8 fw,u8 fh,u8 mode);
void Oled_ShowPicture(u8 x,u8 y,const u8 *p,u8 pw,u8 ph);



#endif

/*********************************************END OF FILE**********************************************/

