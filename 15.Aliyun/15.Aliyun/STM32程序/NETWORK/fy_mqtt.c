#include "fy_mqtt.h"

//#define _DEBUG_MQTT

typedef enum
{
	//���� 	    ֵ 			������������ 	����
	M_RESERVED1	=0	,	//	��ֹ	����
	M_CONNECT		,	//	�ͻ��˵������	�ͻ����������ӷ����
	M_CONNACK		,	//	����˵��ͻ���	���ӱ���ȷ��
	M_PUBLISH		,	//	������������	������Ϣ
	M_PUBACK		,	//	������������	QoS 1��Ϣ�����յ�ȷ��
	M_PUBREC		,	//	������������	�����յ�����֤������һ����
	M_PUBREL		,	//	������������	�����ͷţ���֤�����ڶ�����
	M_PUBCOMP		,	//	������������	QoS 2��Ϣ������ɣ���֤������������
	M_SUBSCRIBE		,	//	�ͻ��˵������	�ͻ��˶�������
	M_SUBACK		,	//	����˵��ͻ���	����������ȷ��
	M_UNSUBSCRIBE	,	//	�ͻ��˵������	�ͻ���ȡ����������
	M_UNSUBACK		,	//	����˵��ͻ���	ȡ�����ı���ȷ��
	M_PINGREQ		,	//	�ͻ��˵������	��������
	M_PINGRESP		,	//	����˵��ͻ���	������Ӧ
	M_DISCONNECT	,	//	�ͻ��˵������	�ͻ��˶Ͽ�����
	M_RESERVED2		,	//	��ֹ	����
}_typdef_mqtt_message;



//���ӳɹ���������Ӧ 20 02 00 00
//�ͻ��������Ͽ����� e0 00
const u8 parket_connetAck[] = {0x20,0x02,0x00,0x00};
const u8 parket_disconnet[] = {0xe0,0x00};
const u8 parket_heart[] = {0xc0,0x00};
const u8 parket_heart_reply[] = {0xc0,0x00};
const u8 parket_subAck[] = {0x90,0x03};

static void Mqtt_SendBuf(u8 *buf,u16 len);

static void Init(u8 *prx,u16 rxlen,u8 *ptx,u16 txlen);
static u8 Connect(char *ClientID,char *Username,char *Password);
static u8 SubscribeTopic(char *topic,u8 qos,u8 whether);
static u8 PublishData(char *topic, char *message, u8 qos);
static void SentHeart(void);
static void Disconnect(void);

_typdef_mqtt _mqtt = 
{
	0,0,
	0,0,
	Init,
	Connect,
	SubscribeTopic,
	PublishData,
	SentHeart,
	Disconnect,
};

static void Init(u8 *prx,u16 rxlen,u8 *ptx,u16 txlen)
{
	_mqtt.rxbuf = prx;_mqtt.rxlen = rxlen;
	_mqtt.txbuf = ptx;_mqtt.txlen = txlen;
	
	memset(_mqtt.rxbuf,0,_mqtt.rxlen);
	memset(_mqtt.txbuf,0,_mqtt.txlen);
	
	//�������������Ͽ�
	_mqtt.Disconnect();Delay_ms(100);
	_mqtt.Disconnect();Delay_ms(100);
}


//���ӷ������Ĵ������
static u8 Connect(char *ClientID,char *Username,char *Password)
{
    int ClientIDLen = strlen(ClientID);
    int UsernameLen = strlen(Username);
    int PasswordLen = strlen(Password);
    int DataLen;
	_mqtt.txlen=0;
	//�ɱ䱨ͷ+Payload  ÿ���ֶΰ��������ֽڵĳ��ȱ�ʶ
    DataLen = 10 + (ClientIDLen+2) + (UsernameLen+2) + (PasswordLen+2);
	
	//�̶���ͷ
	//���Ʊ�������
    _mqtt.txbuf[_mqtt.txlen++] = 0x10;		//MQTT Message Type CONNECT
	//ʣ�೤��(�������̶�ͷ��)
	do
	{
		u8 encodedByte = DataLen % 128;
		DataLen = DataLen / 128;
		// if there are more data to encode, set the top bit of this byte
		if ( DataLen > 0 )
			encodedByte = encodedByte | 128;
		_mqtt.txbuf[_mqtt.txlen++] = encodedByte;
	}while ( DataLen > 0 );
    	
	//�ɱ䱨ͷ
	//Э����
    _mqtt.txbuf[_mqtt.txlen++] = 0;        		// Protocol Name Length MSB    
    _mqtt.txbuf[_mqtt.txlen++] = 4;        		// Protocol Name Length LSB    
    _mqtt.txbuf[_mqtt.txlen++] = 'M';        	// ASCII Code for M    
    _mqtt.txbuf[_mqtt.txlen++] = 'Q';        	// ASCII Code for Q    
    _mqtt.txbuf[_mqtt.txlen++] = 'T';        	// ASCII Code for T    
    _mqtt.txbuf[_mqtt.txlen++] = 'T';        	// ASCII Code for T    
	//Э�鼶��
    _mqtt.txbuf[_mqtt.txlen++] = 4;        		// MQTT Protocol version = 4    
	//���ӱ�־
    _mqtt.txbuf[_mqtt.txlen++] = 0xc2;        	// conn flags 
    _mqtt.txbuf[_mqtt.txlen++] = 0;        		// Keep-alive Time Length MSB    
    _mqtt.txbuf[_mqtt.txlen++] = 60;        	// Keep-alive Time Length LSB  60S������  
	
    _mqtt.txbuf[_mqtt.txlen++] = BYTE1(ClientIDLen);// Client ID length MSB    
    _mqtt.txbuf[_mqtt.txlen++] = BYTE0(ClientIDLen);// Client ID length LSB  	
	memcpy(&_mqtt.txbuf[_mqtt.txlen],ClientID,ClientIDLen);
    _mqtt.txlen += ClientIDLen;
    
    if(UsernameLen > 0)
    {   
        _mqtt.txbuf[_mqtt.txlen++] = BYTE1(UsernameLen);		//username length MSB    
        _mqtt.txbuf[_mqtt.txlen++] = BYTE0(UsernameLen);    	//username length LSB    
		memcpy(&_mqtt.txbuf[_mqtt.txlen],Username,UsernameLen);
        _mqtt.txlen += UsernameLen;
    }
    
    if(PasswordLen > 0)
    {    
        _mqtt.txbuf[_mqtt.txlen++] = BYTE1(PasswordLen);		//password length MSB    
        _mqtt.txbuf[_mqtt.txlen++] = BYTE0(PasswordLen);    	//password length LSB  
		memcpy(&_mqtt.txbuf[_mqtt.txlen],Password,PasswordLen);
        _mqtt.txlen += PasswordLen; 
    }    
	
	u8 cnt=2;
	u8 wait;
	while(cnt--)
	{
		memset(_mqtt.rxbuf,0,_mqtt.rxlen);
		Mqtt_SendBuf(_mqtt.txbuf,_mqtt.txlen);
		wait=30;//�ȴ�3sʱ��
		while(wait--)
		{
			//CONNECT
			if(_mqtt.rxbuf[0]==parket_connetAck[0] && _mqtt.rxbuf[1]==parket_connetAck[1]) //���ӳɹ�			   
			{
				return 1;//���ӳɹ�
			}
			Delay_ms(100);			
		}
	}
	return 0;
}


//MQTT����/ȡ���������ݴ������
//topic       ���� 
//qos         ��Ϣ�ȼ� 
//whether     ����/ȡ�����������
static u8 SubscribeTopic(char *topic,u8 qos,u8 whether)
{    
	_mqtt.txlen=0;
    int topiclen = strlen(topic);
	
	int DataLen = 2 + (topiclen+2) + (whether?1:0);//�ɱ䱨ͷ�ĳ��ȣ�2�ֽڣ�������Ч�غɵĳ���
	//�̶���ͷ
	//���Ʊ�������
    if(whether) _mqtt.txbuf[_mqtt.txlen++] = 0x82; //��Ϣ���ͺͱ�־����
    else	_mqtt.txbuf[_mqtt.txlen++] = 0xA2;    //ȡ������

	//ʣ�೤��
	do
	{
		u8 encodedByte = DataLen % 128;
		DataLen = DataLen / 128;
		// if there are more data to encode, set the top bit of this byte
		if ( DataLen > 0 )
			encodedByte = encodedByte | 128;
		_mqtt.txbuf[_mqtt.txlen++] = encodedByte;
	}while ( DataLen > 0 );	
	
	//�ɱ䱨ͷ
    _mqtt.txbuf[_mqtt.txlen++] = 0;				//��Ϣ��ʶ�� MSB
    _mqtt.txbuf[_mqtt.txlen++] = 0x01;           //��Ϣ��ʶ�� LSB
	//��Ч�غ�
    _mqtt.txbuf[_mqtt.txlen++] = BYTE1(topiclen);//���ⳤ�� MSB
    _mqtt.txbuf[_mqtt.txlen++] = BYTE0(topiclen);//���ⳤ�� LSB   
	memcpy(&_mqtt.txbuf[_mqtt.txlen],topic,topiclen);
    _mqtt.txlen += topiclen;
    
    if(whether)
    {
        _mqtt.txbuf[_mqtt.txlen++] = qos;//QoS����
    }
	
	u8 cnt=2;
	u8 wait;
	while(cnt--)
	{
		memset(_mqtt.rxbuf,0,_mqtt.rxlen);
		Mqtt_SendBuf(_mqtt.txbuf,_mqtt.txlen);
		wait=30;//�ȴ�3sʱ��
		while(wait--)
		{
			if(_mqtt.rxbuf[0]==parket_subAck[0] && _mqtt.rxbuf[1]==parket_subAck[1]) //���ĳɹ�			   
			{
				return 1;//���ĳɹ�
			}
			Delay_ms(100);			
		}
	}
	if(cnt) return 1;	//���ĳɹ�
	return 0;
}

//MQTT�������ݴ������
//topic   ���� 
//message ��Ϣ
//qos     ��Ϣ�ȼ� 
static u8 PublishData(char *topic, char *message, u8 qos)
{  
    int topicLength = strlen(topic);    
    int messageLength = strlen(message);     
    static u16 id=0;
	int DataLen;
	_mqtt.txlen=0;
	//��Ч�غɵĳ����������㣺�ù̶���ͷ�е�ʣ�೤���ֶε�ֵ��ȥ�ɱ䱨ͷ�ĳ���
	//QOSΪ0ʱû�б�ʶ��
	//���ݳ���             ������   ���ı�ʶ��   ��Ч�غ�
    if(qos)	DataLen = (2+topicLength) + 2 + messageLength;       
    else	DataLen = (2+topicLength) + messageLength;   

    //�̶���ͷ
	//���Ʊ�������
    _mqtt.txbuf[_mqtt.txlen++] = 0x30;    // MQTT Message Type PUBLISH  

	//ʣ�೤��
	do
	{
		u8 encodedByte = DataLen % 128;
		DataLen = DataLen / 128;
		// if there are more data to encode, set the top bit of this byte
		if ( DataLen > 0 )
			encodedByte = encodedByte | 128;
		_mqtt.txbuf[_mqtt.txlen++] = encodedByte;
	}while ( DataLen > 0 );	
	
    _mqtt.txbuf[_mqtt.txlen++] = BYTE1(topicLength);//���ⳤ��MSB
    _mqtt.txbuf[_mqtt.txlen++] = BYTE0(topicLength);//���ⳤ��LSB 
	memcpy(&_mqtt.txbuf[_mqtt.txlen],topic,topicLength);//��������
    _mqtt.txlen += topicLength;
        
	//���ı�ʶ��
    if(qos)
    {
        _mqtt.txbuf[_mqtt.txlen++] = BYTE1(id);
        _mqtt.txbuf[_mqtt.txlen++] = BYTE0(id);
        id++;
    }
	memcpy(&_mqtt.txbuf[_mqtt.txlen],message,messageLength);
    _mqtt.txlen += messageLength;
        
	Mqtt_SendBuf(_mqtt.txbuf,_mqtt.txlen);
    return _mqtt.txlen;
}

static void SentHeart(void)
{
	Mqtt_SendBuf((u8 *)parket_heart,sizeof(parket_heart));
}

static void Disconnect(void)
{
	Mqtt_SendBuf((u8 *)parket_disconnet,sizeof(parket_disconnet));
}

static void Mqtt_SendBuf(u8 *buf,u16 len)
{
	#ifdef _DEBUG_MQTT
	Usart1_SendBuf(buf,len);
	#endif
	Usart2_SendBuf(buf,len);
}	

/*********************************************END OF FILE********************************************/
