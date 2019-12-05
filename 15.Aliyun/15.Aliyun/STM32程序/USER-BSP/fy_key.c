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

//只读取初次按键电平状态，在状态机中进一步处理
static u8 Key_ReadPin(void)
{
    if(KEYUP_READ())  
		return KEYUP_PRES;
	if(!KEYDOWN_READ())
		return KEYDOWN_PRES;
	return KEY_NONE;
}

//返回不同按键的单击、双击、长按、无键四种状态
//此函数10ms调用一次
//状态机方式
u8 Key_Scan(void)
{
	static u8 state = 0; //按键初始化状态
	static u8 KEY_LAST=0,KEY_NOW=0; //记录两次电平的状态
	u8 KEY_VALUE=0;

	KEY_NOW = Key_ReadPin();//读按键值
   
	switch(state)
	{
		case 0:
		{
			if(KEY_NOW != KEY_LAST)	state = 1; //有按键按下
		}break;
		case 1: 
		{
			if(KEY_NOW == KEY_LAST)	state = 2; //消斗之后按键有效
			else state = 0; //认为误触
		}break; 
		case 2: //消斗之后
		{
			if(KEY_NOW == KEY_LAST) //还是按下的状态 
			{
			  	state = 3;
			}
			else//松开了，短按
			{
				state = 0; 
				
				KEY_VALUE = KEY_LAST|KEY_SHORT;  //返回键值短按	
			}
		}break;
		
		case 3: //判断长按短按
		{
			static u8 cnt = 0;
			if(KEY_NOW == KEY_LAST) 
			{
				if(cnt++ > 180) //1800ms
				{
					cnt = 0; 
					state = 4;
					KEY_VALUE = KEY_LAST|KEY_LONG; //返回键值长按
				}			  
			}
			else
			{
                cnt = 0; 
				state = 0;
				KEY_VALUE = KEY_LAST|KEY_SHORT; //返回键值短按			
			}
		}break;
		case 4://长按松手检测
		{
			if(KEY_NOW != KEY_LAST) 
				state = 0;
		}break;
	}//switch
	
	KEY_LAST = KEY_NOW; //更新
	
	return KEY_VALUE;
}

//返回不同按键的单击、双击、长按、无键四种状态
//此函数10ms调用一次
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
				KEY_LAST = KEY_VAL; 	//保存键值
				cnt = 0;  //第一次单击，不返回，到下个状态判断是否会出现双击
				sta = 1;
			}
			else 
				KEY_RETURN = KEY_VAL;  //对于无键、长按，返回原事件
			break;
			
		case 1:
			if((KEY_VAL & KEY_SHORT) && (KEY_VAL == KEY_LAST))  //又一次单击，时间间隔小于500ms
			{
				KEY_LAST = KEY_VAL; 	//保存键值				
				sta = 2;
				cnt = 0;//重新计时
			}
			else if(++cnt >= 50)	 //第一次单击后超过500ms 视为单击
			{											
				KEY_RETURN = KEY_LAST;  //500ms内没有再次出现单击事件，返回单击事件
				sta = 0;  //返回初始状态							
			}
			break;
		case 2:
			if((KEY_VAL & KEY_SHORT) && (KEY_VAL == KEY_LAST))  //再一次单击，时间间隔小于500ms
			{
				KEY_RETURN = (KEY_VAL & ~KEY_SHORT) | KEY_THREE;  //返回三击事件，回到初始状态
				sta = 0;
			}
			else if(++cnt >= 50) ////第二次单击后超过500ms 视为双击
			{												
				KEY_RETURN = (KEY_LAST & ~KEY_SHORT) | KEY_DOUBLE;
				sta = 0;  //返回初始状态							
			}
			break;
	}//switch
	
	return KEY_RETURN;
}

/*********************************************END OF FILE**********************************************/
