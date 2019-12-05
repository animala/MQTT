#include "fy_oled12864.h"


/*******************************************以下为IIC底层部分*******************************************/
#define IIC_SCL_RCC		RCC_APB2Periph_GPIOB
#define IIC_SCL_PORT	GPIOB
#define IIC_SCL_PIN		GPIO_Pin_10

#define IIC_SDA_RCC		RCC_APB2Periph_GPIOB
#define IIC_SDA_PORT	GPIOB
#define IIC_SDA_PIN		GPIO_Pin_11

#define IIC_SDA_H()		GPIO_SetBits(IIC_SDA_PORT,IIC_SDA_PIN)
#define IIC_SDA_L()		GPIO_ResetBits(IIC_SDA_PORT,IIC_SDA_PIN)

#define IIC_SCL_H()	GPIO_SetBits(IIC_SCL_PORT,IIC_SCL_PIN)
#define IIC_SCL_L()	GPIO_ResetBits(IIC_SCL_PORT,IIC_SCL_PIN)

#define IIC_Delay()	{__NOP();__NOP();__NOP();}//Delay_us(2)  

static void IIC_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd( IIC_SCL_RCC,ENABLE);//1、配置时钟
    GPIO_InitStructure.GPIO_Pin = IIC_SCL_PIN;		//2、配置GPIO
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(IIC_SCL_PORT, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd( IIC_SDA_RCC,ENABLE);//1、配置时钟
    GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;		//2、配置GPIO
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure);
}

static void Oled_IIC_Start(void)               //start the IIC, SDA High-to-low when SCL is high
{
    IIC_SDA_H();
    IIC_Delay();

    IIC_SCL_H();
    IIC_Delay();

    IIC_SDA_L();
    IIC_Delay();

    IIC_SCL_L();
    IIC_Delay();
}


static void Oled_IIC_Stop(void)                //STOP the IIC, SDA Low-to-high when SCL is high
{
    IIC_SDA_L();
    IIC_Delay();

    IIC_SCL_H();
    IIC_Delay();

    IIC_SDA_H();
    IIC_Delay();
}

void Oled_IIC_Ack(void)              //Send ACK (LOW)
{
    IIC_SDA_L();
    IIC_Delay();

    IIC_SCL_H();
    IIC_Delay();

    IIC_SCL_L();
    IIC_Delay();
}

void Oled_IIC_SendByte(u8 dat)     //write a byte to IIC
{
    u8 i;
    i = 8;
    do
    {
        if(dat & 0x80)  IIC_SDA_H();
        else            IIC_SDA_L();
        dat <<= 1;

        IIC_Delay();

        IIC_SCL_H();
        IIC_Delay();

        IIC_SCL_L();
        IIC_Delay();
    }
    while(--i);
}
/*******************************************以上为IIC底层部分*******************************************/


#define OLED_SLAVE_ADD 0x78
#define OLED_CMD  0X00	//写命令
#define OLED_DATA 0X40	//写数据
//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127
//[1]0 1 2 3 ... 127
//[2]0 1 2 3 ... 127
//[3]0 1 2 3 ... 127
//[4]0 1 2 3 ... 127
//[5]0 1 2 3 ... 127
//[6]0 1 2 3 ... 127
//[7]0 1 2 3 ... 127
volatile static u8 gram[128][64];


//OLED写一个字节
//dat：写入的字节
//cmd：命令或者数据
void Oled_WriteByte(u8 dat,u8 cmd)
{
//	i2cWrite(OLED_SLAVE_ADD,cmd,dat);
    Oled_IIC_Start();
    Oled_IIC_SendByte(0x78);
    Oled_IIC_Ack();
    Oled_IIC_SendByte(cmd);
    Oled_IIC_Ack();
    Oled_IIC_SendByte(dat);
    Oled_IIC_Ack();
    Oled_IIC_Stop();
}

//画点
//x:0~127
//y:0~63
//t:1 填充 0,清空
void Oled_DrawPoint(u8 x,u8 y,u8 draw)
{
    u8 page,off,temp=0;
    if(x>127||y>63)return;//超出范围了.

    page=7-y/8;
    off=y%8;
    temp=1<<(7-off);
    if(draw)    gram[x][page] |= temp;
    else        gram[x][page] &=~temp;
}



//读点
//x:0~127
//y:0~63
u8 Oled_ReadPoint(u8 x,u8 y)
{
    u8 page,off,temp=0x00;
    if(x>127||y>63) return 0;//超出范围了.

    page=7-y/8;
    off=y%8;
    temp=1<<(7-off);
    if(temp&gram[x][page]) return 1;
    return 0;
}
//反白一个点
//x:0~127
//y:0~63
void Oled_ReversePoint(u8 x,u8 y)
{
    Oled_DrawPoint(x,y,Oled_ReadPoint(x,y)?0:1);
}
//更新显存、页地址模式
//在GDDRAM访问后(读/写), 列地址指针将自动增加1。
//如果列地址指针到达列终止地址, 列地址指针将复位到列起始地址, 但页地址指针不会改变。
void Oled_RefreshGram(void)
{
    u8 i,n;
    for(i=0; i<8; i++)
    {
        Oled_WriteByte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
        Oled_WriteByte (0x00,OLED_CMD);      //设置显示位置—列低地址
        Oled_WriteByte (0x10,OLED_CMD);      //设置显示位置—列高地址
        for(n=0; n<128; n++)
            Oled_WriteByte(gram[n][i],OLED_DATA);
    }
}
//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63
//dot:0,清空;1,填充
void Oled_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 draw)
{
    u8 x,y;

    for(x=x1; x<=x2; x++)
        for(y=y1; y<=y2; y++)
            Oled_DrawPoint(x,y,draw);
//	Oled_RefreshGram();//更新显示
}
void Oled_Reverse(u8 x1,u8 y1,u8 x2,u8 y2)
{
    u8 x,y;

    for(x=x1; x<=x2; x++)
        for(y=y1; y<=y2; y++)
            Oled_ReversePoint(x,y);
//	Oled_RefreshGram();//更新显示
}
//清屏
void Oled_Clear(void)
{
    memset((u8 *)gram,0,sizeof(gram));
//	u8 i,n;
//	for(i=0;i<8;i++)
//		for(n=0;n<128;n++)
//			gram[n][i]=0X00;
//    Oled_RefreshGram();//更新显示
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//fw:字宽
//fh:字高
//mode:0,反白显示;1,正常显示
void Oled_ShowChar(u8 x,u8 y,u8 chr,u8 fw,u8 fh,u8 mode)
{
    u8 temp,t,tbit;
    u8 y0=y;
    u8 *p;
    u8 csize=(fh/8+((fh%8)?1:0))*fw;//得到自由分辨字符所占的字节数
    chr=chr-' ';//得到偏移后的值

    if(fw==6&&fh==8)        p = (u8 *)asc2_0608[chr];	//调用0608ascii字体
    else if(fw==6&&fh==12)  p = (u8 *)asc2_0612[chr];	//调用0612ascii字体
    else if(fw==8&&fh==16)  p = (u8 *)asc2_0816[chr];	//调用0612ascii字体
    else if(fw==12&&fh==24) p = (u8 *)asc2_1224[chr];   //调用1224ascii字体
    else return;	//没有的字库

    for(t=0; t<csize; t++)
    {
        temp = p[t];
        for(tbit=0; tbit<8; tbit++)
        {
            if(temp&0x80) Oled_DrawPoint(x,y,mode);
            else Oled_DrawPoint(x,y,!mode);
            temp<<=1;
            y++;
            if((y-y0)==fh)//第一列更新完了切换到下一页
            {
                y=y0;
                x++;
                break;
            }
        }
    }
}
//m^n函数
u32 mypow(u8 m,u8 n)
{
    u32 result=1;
    while(n--)result*=m;
    return result;
}
//显示数字
//x,y :起点坐标
//len :数字的位数
//fw:字宽
//fh:字高
//num:数值(0~4294967295);
void Oled_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 fw,u8 fh)
{
    u8 t,temp;
    u8 enshow=0;
    for(t=0; t<len; t++)
    {
        temp=(num/mypow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                Oled_ShowChar(x+(fw)*t,y,' ',fw,fh,1);
                continue;
            }
            else
                enshow=1;
        }
        Oled_ShowChar(x+(fw)*t,y,temp+'0',fw,fh,1);
    }
}
//在指定位置开始显示一个字符串
//支持自动换行
//(x,y):起始坐标
//width,height:区域
//str  :字符串
//fw:字宽
//fh:字高
//mode:0,非叠加方式;1,叠加方式
void Oled_ShowString(u8 x,u8 y,char *str,u8 fw,u8 fh,u8 mode)
{
    while((*str<='~')&&(*str>=' '))//判断是不是非法字符!
    {
        if(x>(128-(fw))) {
            x=0;
            y+=fh;
        }
        if(y>(64-fh)) {
            y=x=0;   //Oled_Clear();
        }
        Oled_ShowChar(x,y,*str,fw,fh,mode);
        x+=fw;
        str++;
    }
}
//显示一个指定大小的汉字
//x,y :汉字的坐标
//font:汉字GBK码
//fw:字宽
//fh:字高
//mode:0,正常显示,1,叠加显示
static void Oled_ShowFont(u8 x,u8 y,char *font,u8 fw,u8 fh,u8 mode)
{
    const char* font_table;
    uint16_t fontSeq;
    uint8_t temp,t,t1;
    uint16_t y0=y;
    uint8_t csize=(fh/8+((fh%8)?1:0))*fw;//得到自由分辨字符所占的字节数

    if(fw==12 && fh==12)
        font_table = font1212_table;
    else if(fw==24 && fh==24)
        font_table = font2424_table;
    else return;/*没有的字库*/
    for(fontSeq=0; fontSeq<strlen(font_table)/2; fontSeq++)/*计算font_table对应字库的数组下标*/
    {
        if(font_table[2*fontSeq]==font[0] && font_table[2*fontSeq+1]==font[1])
            break;
    }
    if(fontSeq >= strlen(font_table)/2) return;/*font_table中没有font该字*/

    for(t=0; t<csize; t++)
    {
        if(fw==12 && fh==12)
            temp = font_1212[fontSeq][t];/*调用font_1212字库*/
        else if(fw==24 && fh==24)
            temp = font_2424[fontSeq][t];/*调用font_2424字库*/
        for(t1=0; t1<8; t1++)
        {
            if(temp&0x80) Oled_DrawPoint(x,y,mode);
            else Oled_DrawPoint(x,y,!mode);
            temp<<=1;
            y++;
            if((y-y0)==fh)
            {
                y=y0;
                x++;
                break;
            }
        }
    }
}

void Oled_ShowText(u8 x,u8 y,char *str,u8 fw,u8 fh,u8 mode)
{
    while(*str!=0)
    {
        Oled_ShowFont(x,y,str,fw,fh,mode); //显示这个汉字,空心显示
        str+=2;
        x+=fh;//下一个汉字偏移
    }
}

//显示图片
//x,y:起点坐标
//pw:图片宽（单位像素）
//ph:图片高（单位像素）
//p:图片起始地址
void Oled_ShowPicture(u8 x,u8 y,const u8 *p,u8 pw,u8 ph)
{
    u8 temp,i,col,row;
    u8 y0=y;
    u8 width=pw;
    if(x+pw>128)width=128-pw;//实际显示宽度
    u8 high=ph;
    if(y+ph>64)high=64-ph;//实际显示高度
    u8 exp_col_bytes=(ph/8+((ph%8)?1:0));//显示一列的字节数
    u8 act_col_bytes=(high/8+((high%8)?1:0));//实际显示一列的字节数

    for(row=0; row<width; row++) //列++
    {
        for(col=0; col<act_col_bytes; col++) //显示一列
        {
            temp = p[col+row*exp_col_bytes];
            for(i=0; i<8; i++)
            {
                if(temp&0x80)Oled_DrawPoint(x,y,1);
                else Oled_DrawPoint(x,y,0);
                temp<<=1;
                y++;
                if((y-y0)==high)
                {
                    y=y0;
                    x++;
                    break;
                }
            }
        }
    }
}

//初始化SSD1306
void Oled_Configuration(void)
{
    IIC_Configuration();
    Delay_ms(100);

    Oled_WriteByte(0xAE,OLED_CMD); //关闭显示
    Oled_WriteByte(0xD5,OLED_CMD); //设置时钟分频因子,震荡频率
    Oled_WriteByte(0x80,OLED_CMD); //[3:0],分频因子;[7:4],震荡频率
    Oled_WriteByte(0xA8,OLED_CMD); //设置驱动路数
    Oled_WriteByte(0X3F,OLED_CMD); //默认0X3F(1/64)
    Oled_WriteByte(0xD3,OLED_CMD); //设置显示偏移
    Oled_WriteByte(0X00,OLED_CMD); //默认为0

    Oled_WriteByte(0x40,OLED_CMD); //设置显示开始行 [5:0],行数.

    Oled_WriteByte(0x8D,OLED_CMD); //电荷泵设置
    Oled_WriteByte(0x14,OLED_CMD); //bit2，开启/关闭
    Oled_WriteByte(0x20,OLED_CMD); //设置内存地址模式
    Oled_WriteByte(0x02,OLED_CMD); //[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
    Oled_WriteByte(0xA1,OLED_CMD); //段重定义设置,bit0:0,0->0;1,0->127;
    Oled_WriteByte(0xC0,OLED_CMD); //设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
    Oled_WriteByte(0xDA,OLED_CMD); //设置COM硬件引脚配置
    Oled_WriteByte(0x12,OLED_CMD); //[5:4]配置

    Oled_WriteByte(0x81,OLED_CMD); //对比度设置
    Oled_WriteByte(0xEF,OLED_CMD); //1~255;默认0X7F (亮度设置,越大越亮)
    Oled_WriteByte(0xD9,OLED_CMD); //设置预充电周期
    Oled_WriteByte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
    Oled_WriteByte(0xDB,OLED_CMD); //设置VCOMH 电压倍率
    Oled_WriteByte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

    Oled_WriteByte(0xA4,OLED_CMD); //全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
    Oled_WriteByte(0xA6,OLED_CMD); //设置显示方式;bit0:1,反相显示;0,正常显示
    Oled_WriteByte(0xAF,OLED_CMD); //开启显示
    Oled_Clear();
	Oled_RefreshGram();
}


/*********************************************END OF FILE**********************************************/

