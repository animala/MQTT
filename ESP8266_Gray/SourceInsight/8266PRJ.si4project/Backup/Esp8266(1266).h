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

#define Net_SendString(str) Usart2_SendString(str)
#define Net_SendBuf(buf,len) Usart2_SendBuf(buf,len)

typedef struct
{
	u8 *rxbuf;u16 rxlen;
	u8 *txbuf;u16 txlen;
	
	u8 (*Check)(void);
	u8 (*Init)(u8 *prx,u16 rxlen,u8 *ptx,u16 txlen);
	void (*Restore)(void);
	u8 (*ConnectAP)(char *ssid,char *pswd);
	u8 (*ConnectServer)(char* mode,char *ip,u16 port);
	u8 (*DisconnectServer)(void);
	u8 (*OpenTransmission)(void);
	void (*CloseTransmission)(void);		
	void (*SendString)(char *str);
	void (*SendBuf)(u8 *buf,u16 len);
}_typdef_net;

extern _typdef_net _net;


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
