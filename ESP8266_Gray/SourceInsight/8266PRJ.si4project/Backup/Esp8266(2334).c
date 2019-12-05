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
	NULL,
	Check,
	Init,
	Restore,
	ConnectAP,
	ConnectServer,
	DisconnectServer,
	OpenTransmission,
	CloseTransmission,
	SendString,
	SendBuf
};


/*   UART3 作为esp8266的通信串口*/ 
void USART3_IRQHandler(void)                	//串口1中断服务程序
{
	static uint8_t value;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断
	{	    
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);		 //完成一次数据接收，清除中断标志	
		value = USART_ReceiveData(USART3);								//读取接收到的数据
		NetPutData(&value, 1);					
	} 
}


int32_t NetPutData(uint8_t *buf, uint32_t len)
{
    int32_t count = 0;

    if(NULL == buf)
    {
        NET_DEBUG("ERR: PutData buf is empty \n");
        return -1;
    }
		
    count = rbWrite(_NetWork.Data, buf, len);
	  
    if(count != len)
    {
        NET_DEBUG("ERR: Failed to rbWrite \n");
        return -1;
    }
		
    return count;
}


static _net_u8 Init(_net_u8 *prx, _net_u16 rxlen, _net_u8 *ptx,_net_u16 txlen)
{
     _NetWork.Data->rbCapacity = rxlen;
     _NetWork.Data->rbBuff = prx;

	 (rbCreate(_NetWork.Data) == 0) ? NET_DEBUG("rbCreate Success \n") : NET_DEBUG("rbCreate Faild \n");
	 
	_NetWork.CloseTransmission();				//关闭透传
	_NetWork.Net_delay_ms(500);
	_NetWork.SendString("AT+RST\r\n");			//重启模块
	_NetWork.Net_delay_ms(500);
	if ( _NetWork.Check() == 0 )
	{
	    return 0;
	}
	_NetWork.SendString("ATE0\r\n");     		//关闭回显
	if(FindStr((char*)_NetWork.Data->rbBuff,"OK",500)==0)  //设置不成功
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
static u8 Check(void)
{
	u8 check_cnt=5;
	while(check_cnt--)
	{
		//memset(_net.rxbuf,0,_net.rxlen); 	 //清空接收缓冲
		_NetWork.SendString("AT\r\n");     		 //发送AT握手指令	
		if(FindStr((char*)_NetWork.Data->rbBuff,"OK",200) != 0)
		{
			return 1;
		}
	}
	return 0;
}



static u8 ConnectAP(char* ssid,char* pswd)
{
	return 0;

}


static u8 ConnectServer(char* mode,char* ip,u16 port)
{

	return 0;
}

static u8 DisconnectServer(void)
{
    return 0;
}


static void SendBuf(u8* buf,u16 len)
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

static u8 OpenTransmission(void)
{
    
	return 0;
}

//退出透传
static void CloseTransmission(void)
{
	//_net.SendString("+++"); Delay_ms(50);
	;//_net.SendString("+++"); Delay_ms(50);
}


