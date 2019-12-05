#include "Uart_Protocol.h"
#include "ringbuffer.h"
#include "usart.h"
#include "usart3.h"
#include "delay.h"
rb_t pRb;                                               ///< Ring buffer structure variable

Command_data CMD_Rb;       //控制结构体

static uint8_t rbBuf[RB_MAX_LEN];                       ///< Ring buffer data cache buffer
u8 FindStr(char* dest,char* src,u16 retry_nms);
uint8_t aRxBuffer;
static int32_t gizPutData(uint8_t *buf, uint32_t len);
int32_t uartWrite(uint8_t *buf, uint32_t len);
/** Protocol global variables **/
gizwitsProtocol_t gizwitsProtocol;
 


//void USART1_IRQHandler(void)                	//串口1中断服务程序
//{
//	 	static uint8_t value;
//	  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
//		{	    
//			USART_ClearITPendingBit(USART1, USART_IT_RXNE);		 //完成一次数据接收，清除中断标志	
//			value = USART_ReceiveData(USART1);								//读取接收到的数据
//			gizPutData(&value, 1);					
//    } 
//}


/*   UART3 作为esp8266的通信串口*/ 
void USART3_IRQHandler(void)                	//串口1中断服务程序
{
	 	static uint8_t value;
	  if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断
		{	    
			USART_ClearITPendingBit(USART3, USART_IT_RXNE);		 //完成一次数据接收，清除中断标志	
			value = USART_ReceiveData(USART3);								//读取接收到的数据
			gizPutData(&value, 1);					
    } 
}




//--------------------------------------------------------------
//				计算校验
//			pData:  数据数组
//			len :  计算长度
//      index: 从哪个字节开始累加
//			checkMode: 校验模式
//--------------------------------------------------------------
u8 calcCRC(u8 *pData, u16 len, u8 index, u8 checkMode)
{
	u8 sum = 0;
	u16 i;
	
	for(i = index; i < len ; i++)
	{
		 switch(checkMode)
		{
			case 1:      //异或
				sum ^= pData[i];
				break;
				
			case 2:			//累加
			default:
				sum += pData[i];
				break;
		}
	}

	return sum;
}

/**********读取缓冲里面的数据**********/

void read_data_fromRingbuff(void)
{
  
	 u8 tmpData = 0;
   u8 dataLen = 0;
	 u8 i=0;
	 uint8_t data[15];
	 dataLen = rbCanRead(&pRb);
//	 printf("data can read is %d\r\n",dataLen);
	
	if(dataLen > 0)
	{
		for(; i < dataLen; i++)
		{
			rbRead(&pRb, &data[i], 1);
			USART_SendData(USART1,data[i]);
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC ) == RESET);   //等待发送完成 
		}
		
	}
	
	
/*	if(dataLen > 0)					//读取ESP8266的返回值，通过串口1发送
	{
			rbRead(&pRb, &tmpData, 10);		
			USART_SendData(USART1,tmpData);
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC ) == RESET);   //等待发送完成 
	}*/
}


void Extrat_frame(void)
{
	u8 i = 0;
	uint8_t data[15];
	u8 dataLen = 0;
	u16 CRC_rel = 0;
	dataLen = rbCanRead(&pRb);
	GIZWITS_LOG("Data can read is %d! \n" ,dataLen);
	if(dataLen >= 10)
	{
		for(; i < dataLen; i++)
		{
			rbRead(&pRb, &data[i], 1);
		}
		
	}

	if((data[0] == 0xff) && (data[10] == 0x55))   //判断包头包尾
	{
		 CRC_rel = calcCRC(data,9,1,2);    // 累加8个字节，从第一个字节开始，累加的校验方式
		 GIZWITS_LOG("CRC is %d! \n" ,CRC_rel);
		 if(CRC_rel > 0)  // == data[9])   //判断校验和
		 {
				CMD_Rb.Car_dir =  data[1];
				CMD_Rb.Car_speed =data[2];
			  CMD_Rb.Year   =   data[3];	
				CMD_Rb.Month = 		data[4];
			  CMD_Rb.Day = 			data[5];
			  CMD_Rb.Hour = 		data[6];
			  CMD_Rb.Minute = 	data[7];
			  CMD_Rb.Second = 	data[8];	
				CMD_Rb.Finish_flag = 1;
		 }
		 else
		 {
				GIZWITS_LOG("Data CRC wrong! \n");
		 }
	}	
	
	else
	{
		//GIZWITS_LOG("Frame not correct! \n");
	}		
}






/**
* @brief Serial port write operation, send data to WiFi module
*
* @param buf      : buf address
* @param len      : buf length
*
* @return : Return effective data length;-1，return failure
*/
int32_t uartWrite(uint8_t *buf, uint32_t len)
{
		uint8_t crc = 0x55;
    uint32_t i = 0;
	
    if(NULL == buf)
    {
        return -1;
    }

    for(i=0; i<len; i++)
    {   
			  USART_SendData(USART1,buf[i]);
        
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC )==RESET);;//Loop until the end of transmission

        if(i >=2 && buf[i] == 0xFF)
        {
						USART_SendData(USART1,crc);       //数据发送完成加上0x55
						while(USART_GetFlagStatus(USART1,USART_FLAG_TC )==RESET);;//Loop until the end of transmission;//Loop until the end of transmission
        }
    }
		
		return len;
}


/**
* @brief gizwits Protocol initialization interface

* Protocol-related timer, serial port initialization

* Datapoint initialization

* @param none
* @return none
*/
void gizwitsInit(void)
{    
    pRb.rbCapacity = RB_MAX_LEN;
    pRb.rbBuff = rbBuf;
    if(0 == rbCreate(&pRb))
	{
		GIZWITS_LOG("rbCreate Success \n");
	}
	else
	{
		GIZWITS_LOG("rbCreate Faild \n");
	}
    memset((uint8_t *)&gizwitsProtocol, 0, sizeof(gizwitsProtocol_t));
}


/**
* @brief Send general protocol message data
* 
* @param [in] head              : Protocol header address
*
* @return : Return effective data length;-1，return failure
*/
static int32_t gizProtocolCommonAck(protocolHead_t *head)
{
    int32_t ret = 0;
    protocolCommon_t ack;

    if(NULL == head)
    {
        GIZWITS_LOG("ERR: gizProtocolCommonAck data is empty \n");
        return -1;
    }
    memcpy((uint8_t *)&ack, (uint8_t *)head, sizeof(protocolHead_t));
    ack.head.cmd = ack.head.cmd+1;
    ack.head.len = exchangeBytes(sizeof(protocolCommon_t)-4);
    ack.sum = gizProtocolSum((uint8_t *)&ack, sizeof(protocolCommon_t));

    ret = uartWrite((uint8_t *)&ack, sizeof(protocolCommon_t));
    if(ret < 0)
    {
        GIZWITS_LOG("ERR: uart write error %d \n", ret);
    }
	
    return ret;
}


/**
* @brief 把串口接收到的数据写入环形buff中
* @param [in] buf        : buf adress
* @param [in] len        : byte length
* @return   正确 : 返回写入数据的长度
            错误 : -1
*/
int32_t gizPutData(uint8_t *buf, uint32_t len)
{
    int32_t count = 0;

    if(NULL == buf)
    {
        GIZWITS_LOG("ERR: gizPutData buf is empty \n");
        return -1;
    }
		
    count = rbWrite(&pRb, buf, len);
	  
    if(count != len)
    {
        GIZWITS_LOG("ERR: Failed to rbWrite \n");
        return -1;
    }
		
    return count;
}






/**
* @brief 环形串口 包头数据初始化
*
* @param [out] head         : Protocol header pointer
*
* @return 0， success; other， failure    
*/
/*static int8_t gizProtocolHeadInit(protocolHead_t *head)
{
    if(NULL == head)
    {
      //  GIZWITS_LOG("ERR: gizProtocolHeadInit head is empty \n");
        return -1;
    }

    memset((uint8_t *)head, 0, sizeof(protocolHead_t));
    head->head[0] = 0xFF;
    head->head[1] = 0xFF;

    return 0;
}*/


/**
* @brief The protocol sends data back , P0 ACK
*
* @param [in] head                  : Protocol head pointer
* @param [in] data                  : Payload data 
* @param [in] len                   : Payload data length
* @param [in] proFlag               : DID flag ,1 for Virtual sub device did ,0 for single product or gateway 
*
* @return : 0,Ack success;
*           -1，Input Param Illegal
*           -2，Serial send faild
*/
static int32_t gizProtocolIssuedDataAck(protocolHead_t *head, uint8_t *gizdata, uint32_t len, uint8_t proFlag)
{
    int32_t ret = 0;
    uint8_t tx_buf[RB_MAX_LEN];
    uint32_t offset = 0;
    uint8_t sDidLen = 0;
    uint16_t data_len = 0;
	  uint8_t *pTxBuf = tx_buf;
    if(NULL == gizdata)
    {
        GIZWITS_LOG("[ERR]  data Is Null \n");
        return -1;
    }
    

    if(0x1 == proFlag)
    {
        sDidLen = *((uint8_t *)head + sizeof(protocolHead_t));
        data_len = 5 + 1 + sDidLen + len;   
    }
    else
    {
        data_len = 5 + len;
    }
   // GIZWITS_LOG("len = ?%d , sDidLen = %d ,data_len = %d\n", len,sDidLen,data_len);
    *pTxBuf ++= 0xFF;
    *pTxBuf ++= 0xFF;
    *pTxBuf ++= (uint8_t)(data_len>>8);
    *pTxBuf ++= (uint8_t)(data_len);
    *pTxBuf ++= head->cmd + 1;
    *pTxBuf ++= head->sn;
    *pTxBuf ++= 0x00;
    *pTxBuf ++= proFlag;
    offset = 8;
    if(0x1 == proFlag)
    {
        *pTxBuf ++= sDidLen;
        offset += 1;
        memcpy(&tx_buf[offset],(uint8_t *)head+sizeof(protocolHead_t)+1,sDidLen);
        offset += sDidLen;
        pTxBuf += sDidLen;

    }
    if(0 != len)
    {
        memcpy(&tx_buf[offset],gizdata,len);
    }
    tx_buf[data_len + 4 - 1 ] = gizProtocolSum( tx_buf , (data_len+4));

   // ret = uartWrite(tx_buf, data_len+4);
    if(ret < 0)
    {
      //  GIZWITS_LOG("uart write error %d \n", ret);
        return -2;
    }

    return 0;
}

/**
* @brief  从环形缓冲数据中获取一次数据包
*
* @param [in]  rb                  : Input data address
* @param [out] data                : Output data address
* @param [out] len                 : Output data length
*
* @return : 0,Return correct ;-1，Return failure;-2，Data check failure
*/
static int8_t gizProtocolGetOnePacket(rb_t *rb, uint8_t *gizdata, uint16_t *len)
{
    int32_t ret = 0;
    uint8_t sum = 0;
    int32_t i = 0;
    uint8_t tmpData;
    uint8_t tmpLen = 0;
    uint16_t tmpCount = 0;
    static uint8_t protocolFlag = 0;
    static uint16_t protocolCount = 0;
    static uint8_t lastData = 0;
    static uint8_t debugCount = 0;
    uint8_t *protocolBuff = gizdata;
    protocolHead_t *head = NULL;

    if((NULL == rb) || (NULL == gizdata) ||(NULL == len))
    {
        GIZWITS_LOG("gizProtocolGetOnePacket Error , Illegal Param\n");
        return -1;
    }

    tmpLen = rbCanRead(rb);
    if(0 == tmpLen)
    {
        return -1;
    }

    for(i=0; i<tmpLen; i++)
    {
        ret = rbRead(rb, &tmpData, 1);
        if(0 != ret)
        {
            if((0xFF == lastData) && (0xFF == tmpData))
            {
                if(0 == protocolFlag)
                {
                    protocolBuff[0] = 0xFF;
                    protocolBuff[1] = 0xFF;
                    protocolCount = 2;
                    protocolFlag = 1;
                }
                else
                {
                    if((protocolCount > 4) && (protocolCount != tmpCount))
                    {
                        protocolBuff[0] = 0xFF;
                        protocolBuff[1] = 0xFF;
                        protocolCount = 2;
                    }
                }
            }
            else if((0xFF == lastData) && (0x55 == tmpData))
            {
            }
            else
            {
                if(1 == protocolFlag)
                {
                    protocolBuff[protocolCount] = tmpData;
                    protocolCount++;

                    if(protocolCount > 4)
                    {
                        head = (protocolHead_t *)protocolBuff;
                        tmpCount = exchangeBytes(head->len)+4;
                        if(protocolCount == tmpCount)
                        {
                            break;
                        }
                    }
                }
            }

            lastData = tmpData;
            debugCount++;
        }
    }

    if((protocolCount > 4) && (protocolCount == tmpCount))
    {
        sum = gizProtocolSum(protocolBuff, protocolCount);

        if(protocolBuff[protocolCount-1] == sum)
        {
            memcpy(gizdata, protocolBuff, tmpCount);
            *len = tmpCount;
            protocolFlag = 0;

            protocolCount = 0;
            debugCount = 0;
            lastData = 0;

            return 0;
        }
        else
        {
            return -2;
        }
    }

    return 1;
}


int32_t gizwitsHandle()
{
    int8_t ret = 0;
#ifdef PROTOCOL_DEBUG
    uint16_t i = 0;
#endif
    uint8_t ackData[RB_MAX_LEN];
    uint16_t protocolLen = 0;
    uint32_t ackLen = 0;
    protocolHead_t *recvHead = NULL;
 //   char *didPtr = NULL;
 //   uint16_t offset = 0;


  /*  if(NULL == currentData)
    {
        GIZWITS_LOG("GizwitsHandle Error , Illegal Param\n");
        return -1;
    }*/

    /*resend strategy*/
   // gizProtocolAckHandle();
    ret = gizProtocolGetOnePacket(&pRb, gizwitsProtocol.protocolBuf, &protocolLen);

    if(0 == ret)
    {
        GIZWITS_LOG("Get One Packet!\n");
        
#ifdef PROTOCOL_DEBUG
       // GIZWITS_LOG("WiFi2MCU[%4d:%4d]: ", gizGetTimerCount(), protocolLen);
        for(i=0; i<protocolLen;i++)
        {
            GIZWITS_LOG("%02x ", gizwitsProtocol.protocolBuf[i]);
        }
        GIZWITS_LOG("\n");
#endif

        recvHead = (protocolHead_t *)gizwitsProtocol.protocolBuf;
        switch (recvHead->cmd)
        {
            case 0:
               // gizProtocolGetDeviceInfo(recvHead);
                break;
            case 1:
                GIZWITS_LOG("flag %x %x \n", recvHead->flags[0], recvHead->flags[1]);
                //offset = 1;
               
              //  if(0 == gizProtocolIssuedProcess(didPtr, gizwitsProtocol.protocolBuf+sizeof(protocolHead_t)+offset, protocolLen-(sizeof(protocolHead_t)+offset+1), ackData, &ackLen))
             //   {
                    gizProtocolIssuedDataAck(recvHead, ackData, ackLen,recvHead->flags[1]);
              //      GIZWITS_LOG("AckData : \n");
             //   }
                break;
            case 2:
                gizProtocolCommonAck(recvHead);
                break;
            case 3:
                gizProtocolCommonAck(recvHead);
               // gizProtocolModuleStatus((protocolWifiStatus_t *)recvHead);
                break;
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
              //  gizProtocolWaitAckCheck(recvHead);
                break;
            case 9:
                gizProtocolCommonAck(recvHead);
                GIZWITS_LOG("report:MCU reboot!\n");
                
               // gizProtocolReboot();
                break;
            case 10:
                break;
            case 11:
                //gizProtocolWaitAckCheck(recvHead);
                GIZWITS_LOG("Ack PRODUCTION_MODE success \n");
                break;           
            case 12:
                //gizProtocolWaitAckCheck(recvHead);
                //gizProtocolNTP(recvHead);
                GIZWITS_LOG("Ack GET_UTT success \n");
                break; 
            case 13:
                //gizProtocolWaitAckCheck(recvHead);
               // gizProtocolModuleInfoHandle(recvHead);
                GIZWITS_LOG("Ack GET_Module success \n");
            break;
 
            default:
               // gizProtocolErrorCmd(recvHead,0x02);
                GIZWITS_LOG("ERR: cmd code error!\n");
                break;
        }
    }
    else if(-2 == ret)
    {
        //Check failed, report exception
        recvHead = (protocolHead_t *)gizwitsProtocol.protocolBuf;
       // gizProtocolErrorCmd(recvHead,0x02);
        GIZWITS_LOG("ERR: check sum error!\n");
        return -2;
    }
    
  /*  switch(gizwitsProtocol.issuedFlag)
    {
        case ACTION_CONTROL_TYPE:
            gizwitsProtocol.issuedFlag = STATELESS_TYPE;
            gizwitsEventProcess(&gizwitsProtocol.issuedProcessEvent, (uint8_t *)&gizwitsProtocol.gizCurrentDataPoint, sizeof(dataPoint_t));
            memset((uint8_t *)&gizwitsProtocol.issuedProcessEvent,0x0,sizeof(gizwitsProtocol.issuedProcessEvent));  
            break;
        case WIFI_STATUS_TYPE:
            gizwitsProtocol.issuedFlag = STATELESS_TYPE;
            gizwitsEventProcess(&gizwitsProtocol.wifiStatusEvent, (uint8_t *)&gizwitsProtocol.wifiStatusData, sizeof(moduleStatusInfo_t));
            memset((uint8_t *)&gizwitsProtocol.wifiStatusEvent,0x0,sizeof(gizwitsProtocol.wifiStatusEvent));
            break;
        case ACTION_W2D_TRANSPARENT_TYPE:
            gizwitsProtocol.issuedFlag = STATELESS_TYPE;
            gizwitsEventProcess(&gizwitsProtocol.issuedProcessEvent, (uint8_t *)gizwitsProtocol.transparentBuff, gizwitsProtocol.transparentLen);
            break;
        case GET_NTP_TYPE:
            gizwitsProtocol.issuedFlag = STATELESS_TYPE;
            gizwitsEventProcess(&gizwitsProtocol.NTPEvent, (uint8_t *)&gizwitsProtocol.TimeNTP, sizeof(protocolTime_t));
            memset((uint8_t *)&gizwitsProtocol.NTPEvent,0x0,sizeof(gizwitsProtocol.NTPEvent));
            break;
        case GET_MODULEINFO_TYPE:
            gizwitsProtocol.issuedFlag = STATELESS_TYPE;
            gizwitsEventProcess(&gizwitsProtocol.moduleInfoEvent, (uint8_t *)&gizwitsProtocol.wifiModuleNews, sizeof(moduleInfo_t));
            memset((uint8_t *)&gizwitsProtocol.moduleInfoEvent,0x0,sizeof(moduleInfo_t));
            break;
        default:
            break;      
    }*/

   // gizDevReportPolicy(currentData);

    return 0;
}







