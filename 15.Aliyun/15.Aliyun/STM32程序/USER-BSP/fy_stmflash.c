#include "fy_stmflash.h"

//读取指定地址的半字(16位数据)
//faddr:读地址(此地址必须为2的倍数!!)
//返回值:对应数据.
u16 STMFLASH_ReadHalfWord(u32 addr)
{
    return *(vu16*)addr;
}

//从指定地址开始读出指定长度的数据
//addr:起始地址
//buf:数据指针
//len:半字(16位)数
void STMFLASH_Read(u32 addr,u16 *buf,u16 len)
{
    u16 i;
    for(i=0; i<len; i++)
    {
        *(buf+i) = STMFLASH_ReadHalfWord(addr);//读取2个字节.
        addr+=2;//偏移2个字节.
    }
}

//不检查的写入
//addr:起始地址
//buf:数据指针
//len:半字(16位)数
void STMFLASH_Write_NoCheck(u32 addr,u16 *buf,u16 len)
{
    u16 i;
    for(i = 0; i<len; i++)
    {
        FLASH_ProgramHalfWord(addr,*(buf+i));
        addr+=2;
    }
}

//不检查的写入
//addr:起始地址
//buf:数据指针
//len:写入的字节数
void STMFLASH_Write(u32 addr,u16 *buf,u16 len)
{
    u32 secpos;	   //扇区地址
    u16 secoff;	   //扇区内偏移地址(16位字计算)
    u16 secremain; //扇区内剩余地址(16位字计算)
    u16 i;
    u32 offaddr;   //去掉0X08000000后的地址
    u16 *STMFLASH_BUF;
    if(	addr<STM32_FLASH_BASE || (addr+len>=(STM32_FLASH_BASE+STM_SECTOR_SIZE*STM32_FLASH_SIZE))) return;//非法地址

    /*申请内存需要注意启动文件里面的动态空间 Heap_Size       EQU     0x00001000 这里设置成4K*/
    STMFLASH_BUF = (u16 *)malloc(STM_SECTOR_SIZE*sizeof(u8));//申请一个扇区大小的内存
    if(STMFLASH_BUF==NULL)	return;


    FLASH_Unlock();						//解锁

    offaddr=addr-STM32_FLASH_BASE;		//实际偏移地址.
    secpos=offaddr/STM_SECTOR_SIZE;		//扇区地址  0~63 for STM32F103C8T6
    secoff=(offaddr%STM_SECTOR_SIZE)/2;	//在扇区内的偏移(1个字节为基本单位.)
    secremain=STM_SECTOR_SIZE/2-secoff;	//扇区剩余空间大小
    if(len<=secremain) secremain=len;	//不大于该扇区范围

    while(1)
    {
        STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//读出整个扇区的内容
        for(i=0; i<secremain; i++) //校验数据
        {
            if(*(STMFLASH_BUF+secoff+i) != 0XFFFF) break;//需要擦除
        }
        if(i<secremain)//需要擦除
        {
            FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//擦除这个扇区
            for(i=0; i<secremain; i++) //复制
            {
                *(STMFLASH_BUF+secoff+i) = buf[i];
            }
            STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//写入整个扇区
        } else STMFLASH_Write_NoCheck(addr,buf,secremain);//写已经擦除了的,直接写入扇区剩余区间.
        if(len==secremain)break;//写入结束了
        else//写入未结束
        {
            secpos++;			//扇区地址增1
            secoff=0;			//偏移位置为0
            buf+=secremain;  	//指针偏移
            addr+=secremain*2;	//写地址偏移
            len-=secremain;		//字节(8位)数递减
            if(len>STM_SECTOR_SIZE/2) secremain=STM_SECTOR_SIZE/2;//下一个扇区还是写不完
            else secremain=len;//下一个扇区可以写完了
        }
    }
    FLASH_Lock();//上锁
    free(STMFLASH_BUF);
}

/*********************************************END OF FILE********************************************/




