#include "fy_tlink.h"
#include "fy_network.h"

u8 Tlink_Connect(char* mode,char* ip,u16 port,char *serialNum)
{
	if(_net.ConnectServer(mode,ip,port))
	{
		printf(" Connect Tlink Success! \r\n ");
		_net.SendString(serialNum);
	}
	else
	{
		printf(" Connect Tlink Error! \r\n ");
		return 0;
	}
	return 1;
}

void Tlink_UploadData(float temp,u16 light,u16 vcc,u8 led_sta)
{
	memset(_net.txbuf,0,_net.txlen);
	
    sprintf((char*)_net.txbuf,"M>%.1f,%d,%d,%d\r\n",temp,light,vcc,led_sta);

	_net.SendString((char*)_net.txbuf);
}

