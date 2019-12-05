#include "fy_includes.h"
#include "fy_tlink.h"
/*
晶振使用的是16M 其他频率在system_stm32f10x.c中修改
使用printf需要在fy_includes.h修改串口重定向为#define PRINTF_USART USART1 
*/

_typdef_adc _adc;
u16 adc_light;
u16 adc_ntc;
u16 battery;
float temperature;
u8 led_sta;
u8 sta;
void Adc_GetValue(void);

u8 txbuf[256];
u8 rxbuf[256];

char mqtt_message[200];


void Mqtt_Progress(u8 *buf,u16 len);

void UsartTrance(void)
{
	while(1)
	{
		Led_Tog();
		Delay_ms(500);
	}
}

int main(void)
{
	u8 cnt_2s=0;
	u16 cnt_5s=0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Systick_Configuration();
    Led_Configuration();
//	UsartTrance();
	Key_Configuration();
	Adc_Configuration();
	Adc_DMA_Configuration((u32)&_adc.buf,ADC_FILTER*ADC_CH_MAX);
	Usart1_Configuration(115200);
	Usart2_Configuration(115200);
	Oled_Configuration();
	Usart1_SendString("  usart1 is ok!\r\n");
//	Usart2_SendString("  usart2 is ok!\r\n");
	Delay_ms(200);
	
//	UsartTrance();
	sta=0;
	
	//检查ESP8266
    if(_net.Init(rxbuf,sizeof(rxbuf),txbuf,sizeof(txbuf))!=0){
		Oled_ShowString(0,0*16,"Net Init OK!",8,16,1);
//		printf("Net Init OK!\r\n");
		sta++;
	}
	else{
		Oled_ShowString(0,0*16,"Net Init Error!",8,16,1);
//		printf("Net Init Error!\r\n");
		sta=0;
	}
	Oled_RefreshGram();
	
	if(sta==1){
		//连接热点
		if(_net.ConnectAP("Tenda_2B2B20","18768803909")!=0){
			Oled_ShowString(0,1*16,"Conncet AP OK!",8,16,1);
//			printf("Conncet AP OK!\r\n");
			sta++;
		}
		else {
			Oled_ShowString(0,1*16,"Conncet AP Error!",8,16,1);
//			printf("Conncet AP Error!\r\n");
			sta=0;
		}
		Oled_RefreshGram();
	}
	
	if(sta==2){	
		//连接TCP
		if(_net.ConnectServer("TCP","a1ugBNniFGU.iot-as-mqtt.cn-shanghai.aliyuncs.com",1883)!=0) {
			Oled_ShowString(0,2*16,"Conet Server OK!",8,16,1);
//			printf("Conncet Server OK!\r\n");
			sta++;
		}
		else{
			Oled_ShowString(0,2*16,"Server Error!",8,16,1);
//			printf("Conncet Server Error!\r\n");	
			sta=0;
		}
		Oled_RefreshGram();
	}
	
	if(sta==3){
		//登录MQTT
		_mqtt.Init(rxbuf,sizeof(rxbuf),txbuf,sizeof(txbuf));		
		if(_mqtt.Connect(
			"123456|securemode=3,signmethod=hmacsha1,timestamp=789|",//ClientID
			"FY-STM32&a1ugBNniFGU",//Username
			"b447b9f26938d8eba6b2a4878066aae91839600c"//Password
			) != 0){
			Oled_ShowString(0,3*16,"Enter MQTT OK!",8,16,1);
//			printf("Enter MQTT OK!\r\n");
			sta++;
		}
		else{		
			Oled_ShowString(0,3*16,"Enter MQTT Error",8,16,1);
//			printf("Enter MQTT Error!\r\n");
			sta=0;
		}	
		Oled_RefreshGram();
	}	
	
	if(sta==4){
		Oled_Clear();
		//订阅主题
		if(_mqtt.SubscribeTopic("/sys/a1ugBNniFGU/FY-STM32/thing/service/property/set",0,1) != 0){
			Oled_ShowString(0,0*16,"Subscribe OK!",8,16,1);
//			printf("SubscribeTopic OK!\r\n");
		}
		else{			
			Oled_ShowString(0,0*16,"Subscribe Error!",8,16,1);
//			printf("SubscribeTopic Error!\r\n");
		}
		Oled_RefreshGram();		
	}
	Delay_ms(1000);
	Oled_Clear();			
	Oled_ShowString(32,0*16,"Mars Tech",8,16,1);
	Oled_ShowString(0,1*16," VCC: 0000 mV",8,16,1);
	Oled_ShowString(0,2*16,"Temp: 00.0 C",8,16,1);
	Oled_ShowString(0,3*16," LUX: 0000  ",8,16,1);
	Oled_RefreshGram();
	sta = 0;
    while(1)
    {
		if(++cnt_2s>=200){
			cnt_2s=0;
			Adc_GetValue();
			temperature = Ntc_GetTemp(adc_ntc)*0.1f;
			Oled_ShowNum(8*6,1*16,battery,4,8,16);			
			Oled_ShowNum(8*6,2*16,(u8)temperature,2,8,16);
			Oled_ShowNum(8*6+3*8,2*16,(u16)(temperature*10)%10,1,8,16);			
			Oled_ShowNum(8*6,3*16,adc_light,4,8,16);
			
			Oled_RefreshGram();
		}
		if(++cnt_5s>=500){//
			cnt_5s=0;
			sprintf(mqtt_message,
			"{\"method\":\"thing.service.property.set\",\"id\":\"630262306\",\"params\":{\
				\"CurrentTemperature\":%.1f,\
				\"hue\":%d,\
				\"mlux\":%d\
			},\"version\":\"1.0.0\"}",
			temperature,
			rand()%0x00ffffff,
			adc_light
			);
			
			_mqtt.PublishData("/sys/a1ugBNniFGU/FY-STM32/thing/event/property/post",mqtt_message,0);			
		}
		if(_mqtt.rxlen){
			Mqtt_Progress(_mqtt.rxbuf,_mqtt.rxlen);		
			memset(_mqtt.rxbuf,0,_mqtt.rxlen);
			_mqtt.rxlen = 0;
		}			
		Delay_ms(10); 
    }
}

void Adc_GetValue(void)
{
	u32 sum[ADC_CH_MAX];	
	memset(sum,0,sizeof(sum));
	for(u8 ch=0; ch<ADC_CH_MAX; ch++)
	{
		for(u8 filter=0; filter<ADC_FILTER; filter++)
		{
			sum[ch] += _adc.buf[filter][ch];//计算累加和
		}		
	}
	adc_light = sum[ADC_LIGHT]/ADC_FILTER;
	adc_ntc = sum[ADC_NTC]/ADC_FILTER;
	battery = sum[ADC_VCC]/ADC_FILTER*3300/4095*11; 	//mV
}

u8 temp;
void USART2_IRQHandler(void)
{
    static u8 rxlen = 0;

    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //判断接收数据寄存器是否有数据
    {
			temp = USART2->DR;
//		USART1->DR = temp;//这里用作串口1打印WIFI模块发送的数据
			if(rxlen>=255) rxlen=0;
        rxbuf[rxlen++] = temp;
        rxlen%=sizeof(rxbuf);
    }

    if(USART_GetITStatus(USART2, USART_IT_IDLE))
    {
			temp = USART2->DR;
			temp = USART2->SR;
			_mqtt.rxlen = rxlen;
//		Mqtt_Progress(rxbuf,rxlen);//主循环做异步处理
			rxlen=0;
    }
}
//void *StrStr(void *dest,void *src);

u8 *p;
void Mqtt_Progress(u8 *buf,u16 len)
{ 
	char *keyStr = "hue";
	u8 keyStrLen = strlen(keyStr)-1;
	u8 i,j;
	for(i=0;i<len-keyStrLen;i++)
	{
		for(j=0;j<keyStrLen;j++)
		{
			if(buf[i+j] != keyStr[j])	break;
		}	
		if(j==keyStrLen) break;
	}
	if(i<=len-keyStrLen)
    {
		u16 temp = 0;
		p = &buf[i];
				
		while(*p != ':') p++;
		p++;
		while(*p != '}')
		{
			temp = temp *10 + (*p - '0');
			p++;
		}
		printf("receive message, hue = %d \r\n",temp);
//		Usart1_SendBuf(buf,len); 
    }
}


