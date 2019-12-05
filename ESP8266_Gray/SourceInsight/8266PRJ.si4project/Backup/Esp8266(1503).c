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
static void GetIP(void);

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
		
		if(FindStr((char*)_NetWork.rxbuf,"OK",500) == 0)  //设置不成功
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
		memset(_NetWork.rxbuf,0,_NetWork.rxlen); 	 //清空接收缓冲
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
	_net_u8 cnt=5;
	while(cnt--)						//5次重连
	{
		memset(_NetWork.rxbuf,0,_NetWork.rxlen);	 
		_NetWork.SendString("AT+CWMODE_CUR=1\r\n"); 			 //设置为STATION模式	
		if(FindStr((char*)_NetWork.rxbuf,"OK",200) != 0)
		{
			break;
		}					
	}
	if(cnt == 0) 
		return 0;

	cnt = 2;
	while(cnt--)   //两次重连
	{
		memset(_NetWork.txbuf,0,_NetWork.txlen);								//清空发送缓冲
		memset(_NetWork.rxbuf,0,_NetWork.rxlen);								//清空接收缓冲
		sprintf((char*)_NetWork.txbuf,"AT+CWJAP_CUR=\"%s\",\"%s\"\r\n",ssid,pswd);//连接目标AP
		_NetWork.SendString((char*)_NetWork.txbuf); 
		if(FindStr((char*)_NetWork.rxbuf,"OK",8000)!=0) 					 	//连接成功且分配到IP
		{
			return 1;
		}
	}
	return 0;
}


static _net_u8 ConnectServer(char* mode,char* ip,_net_u16 port)
{
	u8 cnt;   
    _NetWork.CloseTransmission();                   //多次连接需退出透传
    _NetWork.Net_delay_ms(500);

	//连接服务器
	cnt = 2;
	while(cnt--)
	{
		memset(_NetWork.rxbuf,0,_NetWork.rxlen);      
		memset(_NetWork.txbuf,0,_NetWork.txlen);   
		sprintf((char*)_NetWork.txbuf,"AT+CIPSTART=\"%s\",\"%s\",%d\r\n",mode,ip,port);
		_NetWork.SendString((char*)_NetWork.txbuf);
		if(FindStr((char*)_NetWork.rxbuf,"CONNECT",8000) !=0 )
		{
			break;
		}
	}
	if(cnt == 0) 
		return 0;
	
	//设置透传模式
	if(_NetWork.OpenTransmission()==0) return 0;
	
	//开启发送状态
	cnt=2;
	while(cnt--)
	{
		memset(_NetWork.rxbuf,0,_NetWork.rxlen); 
		_NetWork.SendString("AT+CIPSEND\r\n");				//开始处于透传发送状态
		if(FindStr((char*)_NetWork.rxbuf,">",200)!=0)
		{
			return 1;
		}
	}
	return 0;

}

static _net_u8 DisconnectServer(void)
{
	u8 cnt;
  _NetWork.CloseTransmission();	//退出透传
  _NetWork.Net_delay_ms(500);
	while(cnt--)
	{
		memset(_NetWork.rxbuf,0,_NetWork.rxlen);  
		_NetWork.SendString("AT+CIPCLOSE\r\n");//关闭链接

		if(FindStr((char*)_NetWork.rxbuf,"CLOSED",200)!=0)//操作成功,和服务器成功断开
		{
			break;
		}
	}
	if(cnt) return 1;
	return 0;
}

static void GetIP(void)
{
	u8 cnt;
	u8 *p;
    while(cnt--)
	{
		memset(_NetWork.rxbuf,0,_NetWork.rxlen);  
		_NetWork.SendString("AT+CIFSR\r\n");	//获取IP

		if(FindStr((char*)_NetWork.rxbuf,"IP,",200)!=0)//操作成功,和服务器成功断开
		{
			break;
		}
	}
	p = (u8 *)strstr((const char*)(_NetWork.rxbuf),",");
	NET_DEBUG("IP is %s\r\n",p);
}

static void SendBuf(_net_u8* buf,_net_u16 len)
{
	memset(_NetWork.rxbuf,0,_NetWork.rxlen);
	#ifdef _DEBUG_NET
	Usart1_SendBuf(buf,len);
	#endif	
	Net_SendBuf(buf,len);
}


static void SendString(char* str)
{
	memset(_NetWork.rxbuf,0,_NetWork.rxlen);
	#ifdef _DEBUG_NET
	Usart1_SendString(str);
	#endif	
	Net_SendString(str);
}

static _net_u8 OpenTransmission(void)
{
	u8 cnt=2;
	while(cnt--)
	{
		memset(_NetWork.rxbuf,0,_NetWork.rxlen);	
		_NetWork.SendString("AT+CIPMODE=1\r\n");  
		if(FindStr((char*)_NetWork.rxbuf,"OK",200)!=0)
		{	
			return 1;
		}			
	}
	return 0;

}

//退出透传
static void CloseTransmission(void)
{
	_NetWork.SendString("+++"); 
	_NetWork.Net_delay_ms(50);
	_NetWork.SendString("+++"); 
	_NetWork.Net_delay_ms(50);
}


