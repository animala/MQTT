#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h"  
#include "usart3.h"
#include "dataPointTools.h"
#include "Esp8266.h"
//ALIENTEK探索者STM32F407开发板 扩展实验5
//ATK-RM04 WIFI模块测试实验   -库函数版本 
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com
//广州市星翼电子科技有限公司    
//作者：正点原子 @ALIENTEK 


u8 txbuf[250];
u8 rxbuf[250];

int main(void)
{        
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	usart3_init(115200);  //初始化串口3波特率为115200
	LED_Init();					//初始化LED  
 	LCD_Init();					//LCD初始化  
 	KEY_Init();					//按键初始化  
	if(_NetWork.Init(rxbuf,sizeof(rxbuf),txbuf,sizeof(txbuf))!=0)
		printf("Net Init OK!\r\n");
	else
		printf("Net Init Error!\r\n");

	if(_NetWork.ConnectAP("ChinaUnicon","22334455") != 0)
	{
		printf("Net has connect to ChinaUicon!\r\n");
		_NetWork.GetIP();
	}
	else
	{
		printf("Net Connect Error !\r\n");
	}
	while(1)
	{
		//u3_printf("%s\r\n",cmd);

		delay_ms(2000);
		
	}
}


/*   UART3 作为esp8266的通信串口*/ 
void USART3_IRQHandler(void)                	//串口1中断服务程序
{
	static uint8_t value;
	static uint8_t rxlen = 0;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断
	{	    
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);		 //完成一次数据接收，清除中断标志	
		value = USART_ReceiveData(USART3);								//读取接收到的数据
		if(rxlen>=255) rxlen=0;
		rxbuf[value++] = value;
		rxlen%=sizeof(rxbuf);
	} 
}










