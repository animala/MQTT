#ifndef __ESP8266___
#define __ESP8266___
/******************************************************************************

                  Copyright @1998 - 2019

 ******************************************************************************
  File Name     : Esp8266
  Version       : Initial Draft
  Author        : Gray
  Created       : Esp8266/12/5
  Last Modified :
  Description   : Esp8266.c header file
  Function List :
  History       :
  1.Date        : Esp8266/12/5
  Author        : Gray
  Modification: Created file

******************************************************************************/
#include "common.h"
#include "ringbuffer.h"
#include "sys.h"
/*----------------------------------------------*
 * external variables                           *
 *----------------------------------------------*/

/*----------------------------------------------*
 * external routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * internal routine prototypes                  *
 *----------------------------------------------*/

/*----------------------------------------------*
 * project-wide global variables                *
 *----------------------------------------------*/

/*----------------------------------------------*
 * module-wide global variables                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * constants                                    *
 *----------------------------------------------*/

/*----------------------------------------------*
 * Macros                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * routines' implementations                    *
 *----------------------------------------------*/
 
/*连接AP宏定义*/
#define SSID "Tenda_2B2B20"
#define PWD  "18768803909"

/*连接服务器宏定义*/
#define TCP "TCP"
#define UDP "UDP"
#define IP  "122.114.122.174"
#define PORT 40915

#define Net_SendString(str) Usart3_SendString(str)
#define Net_SendBuf(buf,len) Usart3_SendBuf(buf,len)


typedef struct
{
	_net_u8 *rxbuf;
	_net_u16 rxlen;
	_net_u8 *txbuf;
	_net_u16 txlen;
	rb_t *Data;
	_net_u8 (*Check)(void);
	_net_u8 (*Init)(_net_u8 *prx,_net_u16 rxlen,_net_u8 *ptx,_net_u16 txlen);
	void (*Restore)(void);
	_net_u8 (*ConnectAP)(char *ssid,char *pswd);
	_net_u8 (*ConnectServer)(char* mode,char *ip,_net_u16 port);
	_net_u8 (*DisconnectServer)(void);
	_net_u8 (*OpenTransmission)(void);
	void (*CloseTransmission)(void);		
	void (*SendString)(char *str);
	void (*SendBuf)(_net_u8 *buf,_net_u16 len);
	void (*Net_delay_ms)(_net_u16 x);
}_8266_Net;

extern _8266_Net _NetWork;


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __ESP8266___ */
