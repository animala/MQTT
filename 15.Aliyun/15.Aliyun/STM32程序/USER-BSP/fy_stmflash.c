#include "fy_stmflash.h"

//��ȡָ����ַ�İ���(16λ����)
//faddr:����ַ(�˵�ַ����Ϊ2�ı���!!)
//����ֵ:��Ӧ����.
u16 STMFLASH_ReadHalfWord(u32 addr)
{
    return *(vu16*)addr;
}

//��ָ����ַ��ʼ����ָ�����ȵ�����
//addr:��ʼ��ַ
//buf:����ָ��
//len:����(16λ)��
void STMFLASH_Read(u32 addr,u16 *buf,u16 len)
{
    u16 i;
    for(i=0; i<len; i++)
    {
        *(buf+i) = STMFLASH_ReadHalfWord(addr);//��ȡ2���ֽ�.
        addr+=2;//ƫ��2���ֽ�.
    }
}

//������д��
//addr:��ʼ��ַ
//buf:����ָ��
//len:����(16λ)��
void STMFLASH_Write_NoCheck(u32 addr,u16 *buf,u16 len)
{
    u16 i;
    for(i = 0; i<len; i++)
    {
        FLASH_ProgramHalfWord(addr,*(buf+i));
        addr+=2;
    }
}

//������д��
//addr:��ʼ��ַ
//buf:����ָ��
//len:д����ֽ���
void STMFLASH_Write(u32 addr,u16 *buf,u16 len)
{
    u32 secpos;	   //������ַ
    u16 secoff;	   //������ƫ�Ƶ�ַ(16λ�ּ���)
    u16 secremain; //������ʣ���ַ(16λ�ּ���)
    u16 i;
    u32 offaddr;   //ȥ��0X08000000��ĵ�ַ
    u16 *STMFLASH_BUF;
    if(	addr<STM32_FLASH_BASE || (addr+len>=(STM32_FLASH_BASE+STM_SECTOR_SIZE*STM32_FLASH_SIZE))) return;//�Ƿ���ַ

    /*�����ڴ���Ҫע�������ļ�����Ķ�̬�ռ� Heap_Size       EQU     0x00001000 �������ó�4K*/
    STMFLASH_BUF = (u16 *)malloc(STM_SECTOR_SIZE*sizeof(u8));//����һ��������С���ڴ�
    if(STMFLASH_BUF==NULL)	return;


    FLASH_Unlock();						//����

    offaddr=addr-STM32_FLASH_BASE;		//ʵ��ƫ�Ƶ�ַ.
    secpos=offaddr/STM_SECTOR_SIZE;		//������ַ  0~63 for STM32F103C8T6
    secoff=(offaddr%STM_SECTOR_SIZE)/2;	//�������ڵ�ƫ��(1���ֽ�Ϊ������λ.)
    secremain=STM_SECTOR_SIZE/2-secoff;	//����ʣ��ռ��С
    if(len<=secremain) secremain=len;	//�����ڸ�������Χ

    while(1)
    {
        STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//������������������
        for(i=0; i<secremain; i++) //У������
        {
            if(*(STMFLASH_BUF+secoff+i) != 0XFFFF) break;//��Ҫ����
        }
        if(i<secremain)//��Ҫ����
        {
            FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//�����������
            for(i=0; i<secremain; i++) //����
            {
                *(STMFLASH_BUF+secoff+i) = buf[i];
            }
            STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//д����������
        } else STMFLASH_Write_NoCheck(addr,buf,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������.
        if(len==secremain)break;//д�������
        else//д��δ����
        {
            secpos++;			//������ַ��1
            secoff=0;			//ƫ��λ��Ϊ0
            buf+=secremain;  	//ָ��ƫ��
            addr+=secremain*2;	//д��ַƫ��
            len-=secremain;		//�ֽ�(8λ)���ݼ�
            if(len>STM_SECTOR_SIZE/2) secremain=STM_SECTOR_SIZE/2;//��һ����������д����
            else secremain=len;//��һ����������д����
        }
    }
    FLASH_Lock();//����
    free(STMFLASH_BUF);
}

/*********************************************END OF FILE********************************************/




