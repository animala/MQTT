#ifndef __FY_TLINK_H
#define __FY_TLINK_H

#include "fy_includes.h"


#define TLINK_IP "tcp.tlink.io"
#define TLINK_PORT 8647
#define APP_ID "ZDX985V5Q6Z74H34"

void Tlink_UploadData(float temp,u16 light,u16 vcc,u8 led_sta);

u8 Tlink_Connect(char* mode,char* ip,u16 port,char *serialNum);
	
#endif

