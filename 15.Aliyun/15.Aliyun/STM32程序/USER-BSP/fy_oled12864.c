#include "fy_oled12864.h"


/*******************************************����ΪIIC�ײ㲿��*******************************************/
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

    RCC_APB2PeriphClockCmd( IIC_SCL_RCC,ENABLE);//1������ʱ��
    GPIO_InitStructure.GPIO_Pin = IIC_SCL_PIN;		//2������GPIO
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(IIC_SCL_PORT, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd( IIC_SDA_RCC,ENABLE);//1������ʱ��
    GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;		//2������GPIO
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
/*******************************************����ΪIIC�ײ㲿��*******************************************/


#define OLED_SLAVE_ADD 0x78
#define OLED_CMD  0X00	//д����
#define OLED_DATA 0X40	//д����
//OLED���Դ�
//��Ÿ�ʽ����.
//[0]0 1 2 3 ... 127
//[1]0 1 2 3 ... 127
//[2]0 1 2 3 ... 127
//[3]0 1 2 3 ... 127
//[4]0 1 2 3 ... 127
//[5]0 1 2 3 ... 127
//[6]0 1 2 3 ... 127
//[7]0 1 2 3 ... 127
volatile static u8 gram[128][64];


//OLEDдһ���ֽ�
//dat��д����ֽ�
//cmd�������������
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

//����
//x:0~127
//y:0~63
//t:1 ��� 0,���
void Oled_DrawPoint(u8 x,u8 y,u8 draw)
{
    u8 page,off,temp=0;
    if(x>127||y>63)return;//������Χ��.

    page=7-y/8;
    off=y%8;
    temp=1<<(7-off);
    if(draw)    gram[x][page] |= temp;
    else        gram[x][page] &=~temp;
}



//����
//x:0~127
//y:0~63
u8 Oled_ReadPoint(u8 x,u8 y)
{
    u8 page,off,temp=0x00;
    if(x>127||y>63) return 0;//������Χ��.

    page=7-y/8;
    off=y%8;
    temp=1<<(7-off);
    if(temp&gram[x][page]) return 1;
    return 0;
}
//����һ����
//x:0~127
//y:0~63
void Oled_ReversePoint(u8 x,u8 y)
{
    Oled_DrawPoint(x,y,Oled_ReadPoint(x,y)?0:1);
}
//�����Դ桢ҳ��ַģʽ
//��GDDRAM���ʺ�(��/д), �е�ַָ�뽫�Զ�����1��
//����е�ַָ�뵽������ֹ��ַ, �е�ַָ�뽫��λ������ʼ��ַ, ��ҳ��ַָ�벻��ı䡣
void Oled_RefreshGram(void)
{
    u8 i,n;
    for(i=0; i<8; i++)
    {
        Oled_WriteByte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
        Oled_WriteByte (0x00,OLED_CMD);      //������ʾλ�á��е͵�ַ
        Oled_WriteByte (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ
        for(n=0; n<128; n++)
            Oled_WriteByte(gram[n][i],OLED_DATA);
    }
}
//x1,y1,x2,y2 �������ĶԽ�����
//ȷ��x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63
//dot:0,���;1,���
void Oled_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 draw)
{
    u8 x,y;

    for(x=x1; x<=x2; x++)
        for(y=y1; y<=y2; y++)
            Oled_DrawPoint(x,y,draw);
//	Oled_RefreshGram();//������ʾ
}
void Oled_Reverse(u8 x1,u8 y1,u8 x2,u8 y2)
{
    u8 x,y;

    for(x=x1; x<=x2; x++)
        for(y=y1; y<=y2; y++)
            Oled_ReversePoint(x,y);
//	Oled_RefreshGram();//������ʾ
}
//����
void Oled_Clear(void)
{
    memset((u8 *)gram,0,sizeof(gram));
//	u8 i,n;
//	for(i=0;i<8;i++)
//		for(n=0;n<128;n++)
//			gram[n][i]=0X00;
//    Oled_RefreshGram();//������ʾ
}

//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//fw:�ֿ�
//fh:�ָ�
//mode:0,������ʾ;1,������ʾ
void Oled_ShowChar(u8 x,u8 y,u8 chr,u8 fw,u8 fh,u8 mode)
{
    u8 temp,t,tbit;
    u8 y0=y;
    u8 *p;
    u8 csize=(fh/8+((fh%8)?1:0))*fw;//�õ����ɷֱ��ַ���ռ���ֽ���
    chr=chr-' ';//�õ�ƫ�ƺ��ֵ

    if(fw==6&&fh==8)        p = (u8 *)asc2_0608[chr];	//����0608ascii����
    else if(fw==6&&fh==12)  p = (u8 *)asc2_0612[chr];	//����0612ascii����
    else if(fw==8&&fh==16)  p = (u8 *)asc2_0816[chr];	//����0612ascii����
    else if(fw==12&&fh==24) p = (u8 *)asc2_1224[chr];   //����1224ascii����
    else return;	//û�е��ֿ�

    for(t=0; t<csize; t++)
    {
        temp = p[t];
        for(tbit=0; tbit<8; tbit++)
        {
            if(temp&0x80) Oled_DrawPoint(x,y,mode);
            else Oled_DrawPoint(x,y,!mode);
            temp<<=1;
            y++;
            if((y-y0)==fh)//��һ�и��������л�����һҳ
            {
                y=y0;
                x++;
                break;
            }
        }
    }
}
//m^n����
u32 mypow(u8 m,u8 n)
{
    u32 result=1;
    while(n--)result*=m;
    return result;
}
//��ʾ����
//x,y :�������
//len :���ֵ�λ��
//fw:�ֿ�
//fh:�ָ�
//num:��ֵ(0~4294967295);
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
//��ָ��λ�ÿ�ʼ��ʾһ���ַ���
//֧���Զ�����
//(x,y):��ʼ����
//width,height:����
//str  :�ַ���
//fw:�ֿ�
//fh:�ָ�
//mode:0,�ǵ��ӷ�ʽ;1,���ӷ�ʽ
void Oled_ShowString(u8 x,u8 y,char *str,u8 fw,u8 fh,u8 mode)
{
    while((*str<='~')&&(*str>=' '))//�ж��ǲ��ǷǷ��ַ�!
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
//��ʾһ��ָ����С�ĺ���
//x,y :���ֵ�����
//font:����GBK��
//fw:�ֿ�
//fh:�ָ�
//mode:0,������ʾ,1,������ʾ
static void Oled_ShowFont(u8 x,u8 y,char *font,u8 fw,u8 fh,u8 mode)
{
    const char* font_table;
    uint16_t fontSeq;
    uint8_t temp,t,t1;
    uint16_t y0=y;
    uint8_t csize=(fh/8+((fh%8)?1:0))*fw;//�õ����ɷֱ��ַ���ռ���ֽ���

    if(fw==12 && fh==12)
        font_table = font1212_table;
    else if(fw==24 && fh==24)
        font_table = font2424_table;
    else return;/*û�е��ֿ�*/
    for(fontSeq=0; fontSeq<strlen(font_table)/2; fontSeq++)/*����font_table��Ӧ�ֿ�������±�*/
    {
        if(font_table[2*fontSeq]==font[0] && font_table[2*fontSeq+1]==font[1])
            break;
    }
    if(fontSeq >= strlen(font_table)/2) return;/*font_table��û��font����*/

    for(t=0; t<csize; t++)
    {
        if(fw==12 && fh==12)
            temp = font_1212[fontSeq][t];/*����font_1212�ֿ�*/
        else if(fw==24 && fh==24)
            temp = font_2424[fontSeq][t];/*����font_2424�ֿ�*/
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
        Oled_ShowFont(x,y,str,fw,fh,mode); //��ʾ�������,������ʾ
        str+=2;
        x+=fh;//��һ������ƫ��
    }
}

//��ʾͼƬ
//x,y:�������
//pw:ͼƬ����λ���أ�
//ph:ͼƬ�ߣ���λ���أ�
//p:ͼƬ��ʼ��ַ
void Oled_ShowPicture(u8 x,u8 y,const u8 *p,u8 pw,u8 ph)
{
    u8 temp,i,col,row;
    u8 y0=y;
    u8 width=pw;
    if(x+pw>128)width=128-pw;//ʵ����ʾ���
    u8 high=ph;
    if(y+ph>64)high=64-ph;//ʵ����ʾ�߶�
    u8 exp_col_bytes=(ph/8+((ph%8)?1:0));//��ʾһ�е��ֽ���
    u8 act_col_bytes=(high/8+((high%8)?1:0));//ʵ����ʾһ�е��ֽ���

    for(row=0; row<width; row++) //��++
    {
        for(col=0; col<act_col_bytes; col++) //��ʾһ��
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

//��ʼ��SSD1306
void Oled_Configuration(void)
{
    IIC_Configuration();
    Delay_ms(100);

    Oled_WriteByte(0xAE,OLED_CMD); //�ر���ʾ
    Oled_WriteByte(0xD5,OLED_CMD); //����ʱ�ӷ�Ƶ����,��Ƶ��
    Oled_WriteByte(0x80,OLED_CMD); //[3:0],��Ƶ����;[7:4],��Ƶ��
    Oled_WriteByte(0xA8,OLED_CMD); //��������·��
    Oled_WriteByte(0X3F,OLED_CMD); //Ĭ��0X3F(1/64)
    Oled_WriteByte(0xD3,OLED_CMD); //������ʾƫ��
    Oled_WriteByte(0X00,OLED_CMD); //Ĭ��Ϊ0

    Oled_WriteByte(0x40,OLED_CMD); //������ʾ��ʼ�� [5:0],����.

    Oled_WriteByte(0x8D,OLED_CMD); //��ɱ�����
    Oled_WriteByte(0x14,OLED_CMD); //bit2������/�ر�
    Oled_WriteByte(0x20,OLED_CMD); //�����ڴ��ַģʽ
    Oled_WriteByte(0x02,OLED_CMD); //[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
    Oled_WriteByte(0xA1,OLED_CMD); //���ض�������,bit0:0,0->0;1,0->127;
    Oled_WriteByte(0xC0,OLED_CMD); //����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��
    Oled_WriteByte(0xDA,OLED_CMD); //����COMӲ����������
    Oled_WriteByte(0x12,OLED_CMD); //[5:4]����

    Oled_WriteByte(0x81,OLED_CMD); //�Աȶ�����
    Oled_WriteByte(0xEF,OLED_CMD); //1~255;Ĭ��0X7F (��������,Խ��Խ��)
    Oled_WriteByte(0xD9,OLED_CMD); //����Ԥ�������
    Oled_WriteByte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
    Oled_WriteByte(0xDB,OLED_CMD); //����VCOMH ��ѹ����
    Oled_WriteByte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

    Oled_WriteByte(0xA4,OLED_CMD); //ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
    Oled_WriteByte(0xA6,OLED_CMD); //������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ
    Oled_WriteByte(0xAF,OLED_CMD); //������ʾ
    Oled_Clear();
	Oled_RefreshGram();
}


/*********************************************END OF FILE**********************************************/

