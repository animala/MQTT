#include "Esp8266.h"
#include "ringbuffer.h"
#include "usart.h"
#include "usart3.h"
#include "delay.h"


#define NET_DEBUG printf

static _net_u8 Check(void);
static _net_u8 Init(_net_u8 *prx, _net_u16 rxlen, _net_u8 *ptx,_net_u16 txlen);
static void Restore(void);
static _net_u8 ConnectAP(char* ssid,char* pswd);
static _net_u8 ConnectServer(char* mode,char* ip,_net_u16 port);
static _net_u8 DisconnectServer(void);
static _net_u8 OpenTransmission(void);
static void CloseTransmission(void);
static void SendString(char* str);
static void SendBuf(_net_u8* buf,_net_u16 len);
static int32_t NetPutData(uint8_t *buf, uint32_t len);


_8266_Net _NetWork=
{
	0,
	0,
	0,
	0,
	Check,
	Init,
	Restore,
	ConnectAP,
	ConnectServer,
	DisconnectServer,
	OpenTransmission,
	CloseTransmission,
	SendString,
	SendBuf,
	delay_ms,
};

uint8_t FindStr(char* dest,char* src,uint16_t retry_nms)
{
	retry_nms/=10;					 //超时时间

	while(strstr(dest,src)==0 && retry_nms--)//等待串口接收完毕或超时退出
	{		
			delay_ms(10);
	}
	
   if(retry_nms)	 return 1;						 

   return 0; 
}








static _net_u8 Init(_net_u8 *prx, _net_u16 rxlen, _net_u8 *ptx,_net_u16 txlen)
{
		_NetWork.rxbuf = prx;
		_NetWork.rxlen = rxlen;
		_NetWork.txbuf = ptx;
	  _NetWork.txlen = txlen;
		memset(_NetWork.rxbuf,0,_NetWork.rxlen);
		memset(_NetWork.txbuf,0,_NetWork.txlen);
	 
		_NetWork.CloseTransmission();				//关闭透传
		_NetWork.Net_delay_ms(500);
		_NetWork.SendString("AT+RST\r\n");			//重启模块
		_NetWork.Net_delay_ms(500);
		if ( _NetWork.Check() == 0 )
		{
	    return 0;
		}
		memset(_NetWork.rxbuf,0,_NetWork.rxlen);
		_NetWork.SendString("ATE0\r\n");     									//关闭回显
		
		if(FindStr((char*)_NetWork.rxbuf,"OK",500)==0)  //设置不成功
    {
        return 0;      
    }
    return 1;                         //设置成功
}

/**
 * 功能：恢复出厂设置
 * 参数：None
 * 返回值：None
 * 说明:此时ESP8266中的用户设置将全部丢失回复成出厂状态
 */
 
static void Restore(void)
{
	_NetWork.CloseTransmission();           //退出透传
    _NetWork.Net_delay_ms(500);
	_NetWork.SendString("AT+RESTORE\r\n");	//恢复出厂 	
}

/**
 * 功能：检查ESP8266是否正常
 * 参数：None
 * 返回值：ESP8266返回状态
 *        非0 ESP8266正常
 *        0 ESP8266有问题  
 */
static _net_u8 Check(void)
{
	_net_u8 check_cnt=5;
	while(check_cnt--)
	{
		//memset(_net.rxbuf,0,_net.rxlen); 	 //清空接收缓冲
		_NetWork.SendString("AT\r\n");     		 //发送AT握手指令	
		if(FindStr((char*)_NetWork.rxbuf,"OK",200) != 0)
		{
			return 1;
		}
	}
	return 0;
}



static _net_u8 ConnectAP(char* ssid,char* pswd)
{
return 0;

}


static _net_u8 ConnectServer(char* mode,char* ip,_net_u16 port)
{

return 0;
}

static _net_u8 DisconnectServer(void)
{
	return 0;
}


static void SendBuf(_net_u8* buf,_net_u16 len)
{
	//memset(_net.rxbuf,0,_net.rxlen);
	#ifdef _DEBUG_NET
	Usart1_SendBuf(buf,len);
	#endif	
	Net_SendBuf(buf,len);
}


static void SendString(char* str)
{
		//memset(_net.rxbuf,0,_net.rxlen);
	#ifdef _DEBUG_NET
	Usart1_SendString(str);
	#endif	
	Net_SendString(str);
}

static _net_u8 OpenTransmission(void)
{
	
return 0;
}

//退出透传
static void CloseTransmission(void)
{
//_net.SendString("+++"); Delay_ms(50);
;//_net.SendString("+++"); Delay_ms(50);
}


