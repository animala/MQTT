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
static void Get_IP(void);

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
	Get_IP,
};

uint8_t FindStr(char* dest,char* src,uint16_t retry_nms)
{
	retry_nms/=10;					 //��ʱʱ��

	while(strstr(dest,src)==0 && retry_nms--)//�ȴ����ڽ�����ϻ�ʱ�˳�
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
	 
		_NetWork.CloseTransmission();				//�ر�͸��
		_NetWork.Net_delay_ms(500);
		_NetWork.SendString("AT+RST\r\n");			//����ģ��
		_NetWork.Net_delay_ms(500);
		if ( _NetWork.Check() == 0 )
		{
	    return 0;
		}
		memset(_NetWork.rxbuf,0,_NetWork.rxlen);
		_NetWork.SendString("ATE0\r\n");     									//�رջ���
		
		if(FindStr((char*)_NetWork.rxbuf,"OK",500) == 0)  //���ò��ɹ�
    {
        return 0;      
    }
    return 1;                         //���óɹ�
}

/**
 * ���ܣ��ָ���������
 * ������None
 * ����ֵ��None
 * ˵��:��ʱESP8266�е��û����ý�ȫ����ʧ�ظ��ɳ���״̬
 */
 
static void Restore(void)
{
	_NetWork.CloseTransmission();           //�˳�͸��
    _NetWork.Net_delay_ms(500);
	_NetWork.SendString("AT+RESTORE\r\n");	//�ָ����� 	
}

/**
 * ���ܣ����ESP8266�Ƿ�����
 * ������None
 * ����ֵ��ESP8266����״̬
 *        ��0 ESP8266����
 *        0 ESP8266������  
 */
static _net_u8 Check(void)
{
	_net_u8 check_cnt=5;
	while(check_cnt--)
	{
		memset(_NetWork.rxbuf,0,_NetWork.rxlen); 	 //��ս��ջ���
		_NetWork.SendString("AT\r\n");     		 //����AT����ָ��	
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
	while(cnt--)						//5������
	{
		memset(_NetWork.rxbuf,0,_NetWork.rxlen);	 
		_NetWork.SendString("AT+CWMODE_CUR=1\r\n"); 			 //����ΪSTATIONģʽ	
		if(FindStr((char*)_NetWork.rxbuf,"OK",200) != 0)
		{
			break;
		}					
	}
	if(cnt == 0) 
		return 0;

	cnt = 2;
	while(cnt--)   //��������
	{
		memset(_NetWork.txbuf,0,_NetWork.txlen);								//��շ��ͻ���
		memset(_NetWork.rxbuf,0,_NetWork.rxlen);								//��ս��ջ���
		sprintf((char*)_NetWork.txbuf,"AT+CWJAP_CUR=\"%s\",\"%s\"\r\n",ssid,pswd);//����Ŀ��AP
		_NetWork.SendString((char*)_NetWork.txbuf); 
		if(FindStr((char*)_NetWork.rxbuf,"OK",8000)!=0) 					 	//���ӳɹ��ҷ��䵽IP
		{
			return 1;
		}
	}
	return 0;
}


static _net_u8 ConnectServer(char* mode,char* ip,_net_u16 port)
{
	u8 cnt;   
    _NetWork.CloseTransmission();                   //����������˳�͸��
    _NetWork.Net_delay_ms(500);

	//���ӷ�����
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
	
	//����͸��ģʽ
	if(_NetWork.OpenTransmission()==0) return 0;
	
	//��������״̬
	cnt=2;
	while(cnt--)
	{
		memset(_NetWork.rxbuf,0,_NetWork.rxlen); 
		_NetWork.SendString("AT+CIPSEND\r\n");				//��ʼ����͸������״̬
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
  _NetWork.CloseTransmission();	//�˳�͸��
  _NetWork.Net_delay_ms(500);
	while(cnt--)
	{
		memset(_NetWork.rxbuf,0,_NetWork.rxlen);  
		_NetWork.SendString("AT+CIPCLOSE\r\n");//�ر�����

		if(FindStr((char*)_NetWork.rxbuf,"CLOSED",200)!=0)//�����ɹ�,�ͷ������ɹ��Ͽ�
		{
			break;
		}
	}
	if(cnt) return 1;
	return 0;
}

static void Get_IP(void)
{
	u8 cnt;
	u8 *p,*p1;
	u8 *ipbuf;
	u8 *buf;
    while(cnt--)
	{
		memset(_NetWork.rxbuf,0,_NetWork.rxlen);  
		_NetWork.SendString("AT+CIFSR\r\n");	//��ȡIP

		if(FindStr((char*)_NetWork.rxbuf,"IP,",200)!=0)//�����ɹ�,�ͷ������ɹ��Ͽ�
		{
			break;
		}
	}
	p = (u8 *)strstr((const char*)(_NetWork.rxbuf),"IP,");
	p1 = (u8*)strstr((const char*)(p+2),",");
	ipbuf = p+2;
	sprintf((char*)buf,"AP IP:%s ",ipbuf);
	NET_DEBUG("IP is %s\r\n",buf);
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

//�˳�͸��
static void CloseTransmission(void)
{
	_NetWork.SendString("+++"); 
	_NetWork.Net_delay_ms(50);
	_NetWork.SendString("+++"); 
	_NetWork.Net_delay_ms(50);
}


