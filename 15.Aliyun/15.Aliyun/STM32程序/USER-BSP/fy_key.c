#include "fy_key.h"

#define KEYUP_READ()	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)
#define KEYDOWN_READ()	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)

void Key_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA,ENABLE);
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;		
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;		
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

}

//ֻ��ȡ���ΰ�����ƽ״̬����״̬���н�һ������
static u8 Key_ReadPin(void)
{
    if(KEYUP_READ())  
		return KEYUP_PRES;
	if(!KEYDOWN_READ())
		return KEYDOWN_PRES;
	return KEY_NONE;
}

//���ز�ͬ�����ĵ�����˫�����������޼�����״̬
//�˺���10ms����һ��
//״̬����ʽ
u8 Key_Scan(void)
{
	static u8 state = 0; //������ʼ��״̬
	static u8 KEY_LAST=0,KEY_NOW=0; //��¼���ε�ƽ��״̬
	u8 KEY_VALUE=0;

	KEY_NOW = Key_ReadPin();//������ֵ
   
	switch(state)
	{
		case 0:
		{
			if(KEY_NOW != KEY_LAST)	state = 1; //�а�������
		}break;
		case 1: 
		{
			if(KEY_NOW == KEY_LAST)	state = 2; //����֮�󰴼���Ч
			else state = 0; //��Ϊ��
		}break; 
		case 2: //����֮��
		{
			if(KEY_NOW == KEY_LAST) //���ǰ��µ�״̬ 
			{
			  	state = 3;
			}
			else//�ɿ��ˣ��̰�
			{
				state = 0; 
				
				KEY_VALUE = KEY_LAST|KEY_SHORT;  //���ؼ�ֵ�̰�	
			}
		}break;
		
		case 3: //�жϳ����̰�
		{
			static u8 cnt = 0;
			if(KEY_NOW == KEY_LAST) 
			{
				if(cnt++ > 180) //1800ms
				{
					cnt = 0; 
					state = 4;
					KEY_VALUE = KEY_LAST|KEY_LONG; //���ؼ�ֵ����
				}			  
			}
			else
			{
                cnt = 0; 
				state = 0;
				KEY_VALUE = KEY_LAST|KEY_SHORT; //���ؼ�ֵ�̰�			
			}
		}break;
		case 4://�������ּ��
		{
			if(KEY_NOW != KEY_LAST) 
				state = 0;
		}break;
	}//switch
	
	KEY_LAST = KEY_NOW; //����
	
	return KEY_VALUE;
}

//���ز�ͬ�����ĵ�����˫�����������޼�����״̬
//�˺���10ms����һ��
u8 Key_Read(void)
{
	static u8 sta = 0;
	static u8 cnt = 0;
	static u8 KEY_LAST = KEY_NONE;
	u8 KEY_RETURN = KEY_NONE;
	
	u8 KEY_VAL;
	
	KEY_VAL = Key_Scan();
	
	switch(sta)
	{
		case 0:
			if(KEY_VAL & KEY_SHORT)
			{
				KEY_LAST = KEY_VAL; 	//�����ֵ
				cnt = 0;  //��һ�ε����������أ����¸�״̬�ж��Ƿ�����˫��
				sta = 1;
			}
			else 
				KEY_RETURN = KEY_VAL;  //�����޼�������������ԭ�¼�
			break;
			
		case 1:
			if((KEY_VAL & KEY_SHORT) && (KEY_VAL == KEY_LAST))  //��һ�ε�����ʱ����С��500ms
			{
				KEY_LAST = KEY_VAL; 	//�����ֵ				
				sta = 2;
				cnt = 0;//���¼�ʱ
			}
			else if(++cnt >= 50)	 //��һ�ε����󳬹�500ms ��Ϊ����
			{											
				KEY_RETURN = KEY_LAST;  //500ms��û���ٴγ��ֵ����¼������ص����¼�
				sta = 0;  //���س�ʼ״̬							
			}
			break;
		case 2:
			if((KEY_VAL & KEY_SHORT) && (KEY_VAL == KEY_LAST))  //��һ�ε�����ʱ����С��500ms
			{
				KEY_RETURN = (KEY_VAL & ~KEY_SHORT) | KEY_THREE;  //���������¼����ص���ʼ״̬
				sta = 0;
			}
			else if(++cnt >= 50) ////�ڶ��ε����󳬹�500ms ��Ϊ˫��
			{												
				KEY_RETURN = (KEY_LAST & ~KEY_SHORT) | KEY_DOUBLE;
				sta = 0;  //���س�ʼ״̬							
			}
			break;
	}//switch
	
	return KEY_RETURN;
}

/*********************************************END OF FILE**********************************************/
