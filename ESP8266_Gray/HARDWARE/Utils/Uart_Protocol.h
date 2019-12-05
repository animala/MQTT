#ifndef _UART_PROTOCOL_H
#define _UART_PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"


/* Corresponding to the protocol "4.10 WiFi module control device" in the flag " attr_flags" */ 
typedef struct {
  uint8_t flagred:1;
  uint8_t flaggreen:1;
  uint8_t flagblue:1;
} attrFlags_t;


#define MAX_PACKAGE_LEN    (sizeof(devStatus_t)+sizeof(attrFlags_t)+20)                 ///< Data buffer maximum length
#define RB_MAX_LEN          100    ///< Maximum length of ring buffer




typedef struct {
  uint8_t Car_dir;
  uint8_t Car_speed;
  uint8_t Year;
	uint8_t Month;
	uint8_t Day;
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
	uint8_t Finish_flag;
} Command_data;

extern Command_data CMD_Rb;







/** Corresponding protocol "4.9 Device MCU to the WiFi module to actively report the current state" in the device status "dev_status(11B)" */ 

typedef struct {
  uint8_t valuered;
  uint8_t valuegreen;
  uint8_t valueblue;
} devStatus_t; 

/** Protocol standard header structure */
typedef struct
{
    uint8_t                 head[2];                ///< The head is 0xFFFF
    uint16_t                len;                    ///< From cmd to the end of the entire packet occupied by the number of bytes
    uint8_t                 cmd;                    ///< command
    uint8_t                 sn;                     ///< 
    uint8_t                 flags[2];               ///< flag,default is 0
} protocolHead_t;


/** Protocol common data frame(4.2、4.4、4.6、4.9、4.10) protocol structure */
typedef struct
{
    protocolHead_t          head;                   ///< Protocol standard header structure
    uint8_t                 sum;                    ///< checksum
} protocolCommon_t;



/** Protocol main and very important struct */
typedef struct
{
    uint8_t issuedFlag;                             ///< P0 action type
    uint8_t protocolBuf[MAX_PACKAGE_LEN];           ///< Protocol data handle buffer
    uint8_t transparentBuff[MAX_PACKAGE_LEN];       ///< Transparent data storage area
    uint32_t transparentLen;                        ///< Transmission data length
    
    uint32_t sn;                                    ///< Message SN
    uint32_t timerMsCount;                          ///< Timer Count 
//    protocolWaitAck_t waitAck;                      ///< Protocol wait ACK data structure
    
//    eventInfo_t issuedProcessEvent;                 ///< Control events
 //   eventInfo_t wifiStatusEvent;                    ///< WIFI Status events
 //   eventInfo_t NTPEvent;                           ///< NTP events
  //  eventInfo_t moduleInfoEvent;                    ///< Module Info events

//	  gizwitsReport_t reportData;                     ///< The protocol reports data for standard product
 //   dataPoint_t gizCurrentDataPoint;                ///< Current device datapoints status
 //   dataPoint_t gizLastDataPoint;                   ///< Last device datapoints status
 //   moduleStatusInfo_t wifiStatusData;              ///< WIFI signal intensity
//    protocolTime_t TimeNTP;                         ///< Network time information
#if MODULE_TYPE
    gprsInfo_t   gprsInfoNews;
#else  
//    moduleInfo_t  wifiModuleNews;                   ///< WIFI module Info
#endif
	
        
}gizwitsProtocol_t; 

#pragma pack()

void gizwitsInit(void);
int32_t gizwitsHandle(void);
void read_data_fromRingbuff(void);
void Extrat_frame(void);
#endif


