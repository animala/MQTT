#include "fy_w25qxx.h"

u16 W25QXX_TYPE=W25Q64;	//Ĭ����W25Q64

#define FLASH_SIZE 64/8		//��ѡFLASH������С(��λΪ MBytes )
#define SECTOR_SIZE 4096    //�ֽ�

//4KbytesΪһ��Sector
//16������Ϊ1��Block
//W25Q64
//����Ϊ8M�ֽ�,����128��Block,2048��Sector

//��ȡоƬID
//����ֵ����:
//0XEF13,��ʾоƬ�ͺ�ΪW25Q80
//0XEF14,��ʾоƬ�ͺ�ΪW25Q16
//0XEF15,��ʾоƬ�ͺ�ΪW25Q32
//0XEF16,��ʾоƬ�ͺ�ΪW25Q64
//0XEF17,��ʾоƬ�ͺ�ΪW25Q128
static u16 W25QXX_ReadID(void)
{
    u16 Temp = 0;
    W25QXX_CS_L();
    W25QXX_SPI_ReadWriteByte(0x90);//���Ͷ�ȡID����
    W25QXX_SPI_ReadWriteByte(0x00);
    W25QXX_SPI_ReadWriteByte(0x00);
    W25QXX_SPI_ReadWriteByte(0x00);
    Temp|=W25QXX_SPI_ReadWriteByte(0xFF)<<8;
    Temp|=W25QXX_SPI_ReadWriteByte(0xFF);
    W25QXX_CS_H();
    return Temp;
}

//��ȡW25QXX��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
static u8 W25QXX_ReadSR(void)
{
    u8 byte=0;
    W25QXX_CS_L();                            //ʹ������
    W25QXX_SPI_ReadWriteByte(W25X_ReadStatusReg); //���Ͷ�ȡ״̬�Ĵ�������
    byte=W25QXX_SPI_ReadWriteByte(0Xff);          //��ȡһ���ֽ�
    W25QXX_CS_H();                            //ȡ��Ƭѡ
    return byte;
}
//дW25QXX״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
void W25QXX_WriteSR(u8 sr)
{
    W25QXX_CS_L();                            //ʹ������
    W25QXX_SPI_ReadWriteByte(W25X_WriteStatusReg);//����дȡ״̬�Ĵ�������
    W25QXX_SPI_ReadWriteByte(sr);               	//д��һ���ֽ�
    W25QXX_CS_H();                            //ȡ��Ƭѡ
}

//�ȴ�����
static void W25QXX_WaitBusy(void)
{
    while((W25QXX_ReadSR()&0x01)==0x01);  		// �ȴ�BUSYλ���
}

//W25QXXдʹ��
//��WEL��λ
static void W25QXX_WriteEnable(void)
{
    W25QXX_CS_L();                          	//ʹ������
    W25QXX_SPI_ReadWriteByte(W25X_WriteEnable); 	//����дʹ��
    W25QXX_CS_H();                           	//ȡ��Ƭѡ
}

//W25QXXд��ֹ
//��WEL����
void W25QXX_WriteDisable(void)
{
    W25QXX_CS_L();                            //ʹ������
    W25QXX_SPI_ReadWriteByte(W25X_WriteDisable);  //����д��ָֹ��
    W25QXX_CS_H();                            //ȡ��Ƭѡ
}

//�������ģʽ
static void W25QXX_PowerDown(void)
{
    W25QXX_CS_L();                           	 	//ʹ������
    W25QXX_SPI_ReadWriteByte(W25X_PowerDown);        //���͵�������
    W25QXX_CS_H();                            	//ȡ��Ƭѡ
    Delay_us(3);                               //�ȴ�TPD
}
//����
static void W25QXX_Wakeup(void)
{
    W25QXX_CS_L();                            	//ʹ������
    W25QXX_SPI_ReadWriteByte(W25X_ReleasePowerDown);	//  send W25X_PowerDown command 0xAB
    W25QXX_CS_H();                            	//ȡ��Ƭѡ
    Delay_us(3);                            	//�ȴ�TRES1
}


//��ȡSPI FLASH
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//buf:���ݴ洢��
//addr:��ʼ��ȡ�ĵ�ַ(24bit)
//len:Ҫ��ȡ���ֽ���(���65535)
void W25QXX_ReadBytes(u32 addr,u8 *buf,u32 len)
{
    u16 i;
    SPI2_ChangeMode(_FLASH_SPI_MODE);

    W25QXX_CS_L();                            		//ʹ������
    W25QXX_SPI_ReadWriteByte(W25X_ReadData);         //���Ͷ�ȡ����
    W25QXX_SPI_ReadWriteByte((u8)((addr)>>16));  	//����24bit��ַ
    W25QXX_SPI_ReadWriteByte((u8)((addr)>>8));
    W25QXX_SPI_ReadWriteByte((u8)addr);
    for(i=0; i<len; i++)
    {
        *(buf+i)=W25QXX_SPI_ReadWriteByte(0XFF);   	//ѭ������
    }
    W25QXX_CS_H();
}

//��ָ��ҳ�׵�ַ��ʼд������
//buf:���ݴ洢��
//addr:��ʼд��ĵ�ַ(24bit)
//len:Ҫд����ֽ��� ÿҳ���4K�ֽ�
static void W25QXX_WritePage(u32 addr,u8 *buf,u16 len)
{
    u16 i;

    W25QXX_WriteEnable();                  	//SET WEL
    W25QXX_CS_L();                           //ʹ������
    W25QXX_SPI_ReadWriteByte(W25X_PageProgram);      	//����дҳ����
    W25QXX_SPI_ReadWriteByte((u8)((addr)>>16)); 	//����24bit��ַ
    W25QXX_SPI_ReadWriteByte((u8)((addr)>>8));
    W25QXX_SPI_ReadWriteByte((u8)addr);
    for(i=0; i<len; i++) W25QXX_SPI_ReadWriteByte(*(buf+i)); //д��������
    W25QXX_CS_H();                            	//ȡ��Ƭѡ
    W25QXX_WaitBusy();					   		//�ȴ�д�����
}

//�޼���дSPI FLASH
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ����
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//buf:���ݴ洢��
//addr:��ʼд��ĵ�ַ(24bit)
//len:Ҫд����ֽ���(���65535)
//CHECK OK
static void W25QXX_Write_NoCheck(u32 addr,u8* buf,u16 len)
{
    u16 pageremain;
    pageremain=256-addr%256; //��ҳʣ����ֽ���
    if(len<=pageremain)pageremain=len;//������256���ֽ�
    while(1)
    {
        W25QXX_WritePage(addr,buf,pageremain);
        if(len==pageremain)break;//д�������
        else //len>pageremain
        {
            buf+=pageremain;
            addr+=pageremain;

            len-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
            if(len>256)pageremain=256; //һ�ο���д��256���ֽ�
            else pageremain=len; 	  //����256���ֽ���
        }
    };
}

//дSPI FLASH
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//buf:���ݴ洢��
//addr:��ʼд��ĵ�ַ(24bit)
//len:Ҫд����ֽ���(���65535)
void W25QXX_WriteBytes(u32 addr,u8* buf,u32 len)
{
    u32 secpos;
    u16 secoff;
    u16 secremain;
    u16 i;
    u8 *W25QXX_BUFFER;

    secpos=addr/SECTOR_SIZE;//������ַ
    secoff=addr%SECTOR_SIZE;//�������ڵ�ƫ��
    secremain=4096-secoff;//����ʣ��ռ��С

    if( addr>=(FLASH_SIZE*1024*1024) ||	//����β��ַ
            addr+len>=(FLASH_SIZE*1024*1024)	//ʣ��ռ䲻��
      )return;//�Ƿ���ַ
    SPI2_ChangeMode(_FLASH_SPI_MODE);
    W25QXX_Wakeup();//����
   
	
    W25QXX_BUFFER = (u8 *)malloc(SECTOR_SIZE*sizeof(u8));//����һ��������С���ڴ�
    if(W25QXX_BUFFER==NULL)	return;

    if(len<=secremain)secremain=len;//������4096���ֽ�
    while(1)
    {
        W25QXX_ReadBytes(secpos*SECTOR_SIZE,W25QXX_BUFFER,SECTOR_SIZE);//������������������
        for(i=0; i<secremain; i++) //У������
        {
            if(W25QXX_BUFFER[secoff+i]!=0XFF)break;//��Ҫ����
        }
        if(i<secremain)//��Ҫ����
        {
            W25QXX_EraseSector(secpos);		//�����������
            for(i=0; i<secremain; i++)	   		//����
            {
                W25QXX_BUFFER[i+secoff]=buf[i];
            }
            W25QXX_Write_NoCheck(secpos*SECTOR_SIZE,W25QXX_BUFFER,SECTOR_SIZE);//д����������

        } else W25QXX_Write_NoCheck(addr,buf,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������.
        if(len==secremain)break;//д�������
        else//д��δ����
        {
            secpos++;//������ַ��1
            secoff=0;//ƫ��λ��Ϊ0

            buf+=secremain;  				//ָ��ƫ��
            addr+=secremain;				//д��ַƫ��
            len-=secremain;			//�ֽ����ݼ�
            if(len>SECTOR_SIZE)secremain=SECTOR_SIZE;//��һ����������д����
            else secremain=len;		//��һ����������д����
        }
    };
    free(W25QXX_BUFFER);
    W25QXX_PowerDown();//��������
    free(W25QXX_BUFFER);
}
//��������оƬ
//�ȴ�ʱ�䳬��...
void W25QXX_Erase_Chip(void)
{
    W25QXX_WriteEnable();                 	 	//SET WEL
    W25QXX_WaitBusy();
    W25QXX_CS_L();                            	//ʹ������
    W25QXX_SPI_ReadWriteByte(W25X_ChipErase);        	//����Ƭ��������
    W25QXX_CS_H();                            	//ȡ��Ƭѡ
    W25QXX_WaitBusy();   				   		//�ȴ�оƬ��������
}

//����һ������
//addr:������ַ ����ʵ����������
//����һ��ɽ��������ʱ��:150ms
void W25QXX_EraseSector(u16 addr)
{
    //����falsh�������,������
    //printf("sector num:%x\r\n",addr);
    addr*=4096;
    W25QXX_WriteEnable();                  	//SET WEL
    W25QXX_WaitBusy();
    W25QXX_CS_L();                            	//ʹ������
    W25QXX_SPI_ReadWriteByte(W25X_SectorErase);      	//������������ָ��
    W25QXX_SPI_ReadWriteByte((u8)((addr)>>16));  	//����24bit��ַ
    W25QXX_SPI_ReadWriteByte((u8)((addr)>>8));
    W25QXX_SPI_ReadWriteByte((u8)addr);
    W25QXX_CS_H();                            	//ȡ��Ƭѡ
    W25QXX_WaitBusy();   				   		//�ȴ��������
}

//��ʼ��SPI FLASH��IO��
void W25QXX_Configuration(void)
{
    W25QXX_CS_H();				//SPI FLASH��ѡ��
    SPI2_ChangeMode(_FLASH_SPI_MODE);
    W25QXX_TYPE=W25QXX_ReadID();//��ȡFLASH ID.
}

/*********************************************END OF FILE********************************************/
