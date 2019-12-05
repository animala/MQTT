#ifndef __FY_KEY_H
#define __FY_KEY_H

#include "fy_includes.h"

#define KEYUP_PRES 		0x01
#define KEYDOWN_PRES 	0x02

#define KEY_SHORT	0x10
#define KEY_DOUBLE	0x20
#define KEY_THREE	0x40
#define KEY_LONG   	0x80

#define KEY_NONE    0x00

void Key_Configuration(void);
u8 Key_Scan(void);
u8 Key_Read(void);

#endif



/*********************************************END OF FILE**********************************************/

