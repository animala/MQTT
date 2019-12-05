#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h"  
#include "usart3.h"
#include "dataPointTools.h"
#include "Esp8266.h"
//ALIENTEK̽����STM32F407������ ��չʵ��5
//ATK-RM04 WIFIģ�����ʵ��   -�⺯���汾 
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com
//������������ӿƼ����޹�˾    
//���ߣ�����ԭ�� @ALIENTEK 


u8 txbuf[250];
u8 rxbuf[250];

int main(void)
{        
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  //��ʼ����ʱ����
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200
	usart3_init(115200);  //��ʼ������3������Ϊ115200
	LED_Init();					//��ʼ��LED  
 	LCD_Init();					//LCD��ʼ��  
 	KEY_Init();					//������ʼ��  
	if(_NetWork.Init(rxbuf,sizeof(rxbuf),txbuf,sizeof(txbuf))!=0){

	printf("Net Init OK!\r\n");

	}



	while(1)
	{
		//u3_printf("%s\r\n",cmd);

		delay_ms(2000);
		
	}
}


/*   UART3 ��Ϊesp8266��ͨ�Ŵ���*/ 
void USART3_IRQHandler(void)                	//����1�жϷ������
{
	static uint8_t value;
	static uint8_t rxlen = 0;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�
	{	    
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);		 //���һ�����ݽ��գ�����жϱ�־	
		value = USART_ReceiveData(USART3);								//��ȡ���յ�������
		if(rxlen>=255) rxlen=0;
		rxbuf[value++] = value;
		rxlen%=sizeof(rxbuf);
	} 
}










