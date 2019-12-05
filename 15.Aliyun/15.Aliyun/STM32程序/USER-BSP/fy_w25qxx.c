#include "fy_w25qxx.h"

u16 W25QXX_TYPE=W25Q64;	//默认是W25Q64

#define FLASH_SIZE 64/8		//所选FLASH容量大小(单位为 MBytes )
#define SECTOR_SIZE 4096    //字节

//4Kbytes为一个Sector
//16个扇区为1个Block
//W25Q64
//容量为8M字节,共有128个Block,2048个Sector

//读取芯片ID
//返回值如下:
//0XEF13,表示芯片型号为W25Q80
//0XEF14,表示芯片型号为W25Q16
//0XEF15,表示芯片型号为W25Q32
//0XEF16,表示芯片型号为W25Q64
//0XEF17,表示芯片型号为W25Q128
static u16 W25QXX_ReadID(void)
{
    u16 Temp = 0;
    W25QXX_CS_L();
    W25QXX_SPI_ReadWriteByte(0x90);//发送读取ID命令
    W25QXX_SPI_ReadWriteByte(0x00);
    W25QXX_SPI_ReadWriteByte(0x00);
    W25QXX_SPI_ReadWriteByte(0x00);
    Temp|=W25QXX_SPI_ReadWriteByte(0xFF)<<8;
    Temp|=W25QXX_SPI_ReadWriteByte(0xFF);
    W25QXX_CS_H();
    return Temp;
}

//读取W25QXX的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
static u8 W25QXX_ReadSR(void)
{
    u8 byte=0;
    W25QXX_CS_L();                            //使能器件
    W25QXX_SPI_ReadWriteByte(W25X_ReadStatusReg); //发送读取状态寄存器命令
    byte=W25QXX_SPI_ReadWriteByte(0Xff);          //读取一个字节
    W25QXX_CS_H();                            //取消片选
    return byte;
}
//写W25QXX状态寄存器
//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
void W25QXX_WriteSR(u8 sr)
{
    W25QXX_CS_L();                            //使能器件
    W25QXX_SPI_ReadWriteByte(W25X_WriteStatusReg);//发送写取状态寄存器命令
    W25QXX_SPI_ReadWriteByte(sr);               	//写入一个字节
    W25QXX_CS_H();                            //取消片选
}

//等待空闲
static void W25QXX_WaitBusy(void)
{
    while((W25QXX_ReadSR()&0x01)==0x01);  		// 等待BUSY位清空
}

//W25QXX写使能
//将WEL置位
static void W25QXX_WriteEnable(void)
{
    W25QXX_CS_L();                          	//使能器件
    W25QXX_SPI_ReadWriteByte(W25X_WriteEnable); 	//发送写使能
    W25QXX_CS_H();                           	//取消片选
}

//W25QXX写禁止
//将WEL清零
void W25QXX_WriteDisable(void)
{
    W25QXX_CS_L();                            //使能器件
    W25QXX_SPI_ReadWriteByte(W25X_WriteDisable);  //发送写禁止指令
    W25QXX_CS_H();                            //取消片选
}

//进入掉电模式
static void W25QXX_PowerDown(void)
{
    W25QXX_CS_L();                           	 	//使能器件
    W25QXX_SPI_ReadWriteByte(W25X_PowerDown);        //发送掉电命令
    W25QXX_CS_H();                            	//取消片选
    Delay_us(3);                               //等待TPD
}
//唤醒
static void W25QXX_Wakeup(void)
{
    W25QXX_CS_L();                            	//使能器件
    W25QXX_SPI_ReadWriteByte(W25X_ReleasePowerDown);	//  send W25X_PowerDown command 0xAB
    W25QXX_CS_H();                            	//取消片选
    Delay_us(3);                            	//等待TRES1
}


//读取SPI FLASH
//在指定地址开始读取指定长度的数据
//buf:数据存储区
//addr:开始读取的地址(24bit)
//len:要读取的字节数(最大65535)
void W25QXX_ReadBytes(u32 addr,u8 *buf,u32 len)
{
    u16 i;
    SPI2_ChangeMode(_FLASH_SPI_MODE);

    W25QXX_CS_L();                            		//使能器件
    W25QXX_SPI_ReadWriteByte(W25X_ReadData);         //发送读取命令
    W25QXX_SPI_ReadWriteByte((u8)((addr)>>16));  	//发送24bit地址
    W25QXX_SPI_ReadWriteByte((u8)((addr)>>8));
    W25QXX_SPI_ReadWriteByte((u8)addr);
    for(i=0; i<len; i++)
    {
        *(buf+i)=W25QXX_SPI_ReadWriteByte(0XFF);   	//循环读数
    }
    W25QXX_CS_H();
}

//在指定页首地址开始写入数据
//buf:数据存储区
//addr:开始写入的地址(24bit)
//len:要写入的字节数 每页最长是4K字节
static void W25QXX_WritePage(u32 addr,u8 *buf,u16 len)
{
    u16 i;

    W25QXX_WriteEnable();                  	//SET WEL
    W25QXX_CS_L();                           //使能器件
    W25QXX_SPI_ReadWriteByte(W25X_PageProgram);      	//发送写页命令
    W25QXX_SPI_ReadWriteByte((u8)((addr)>>16)); 	//发送24bit地址
    W25QXX_SPI_ReadWriteByte((u8)((addr)>>8));
    W25QXX_SPI_ReadWriteByte((u8)addr);
    for(i=0; i<len; i++) W25QXX_SPI_ReadWriteByte(*(buf+i)); //写整个扇区
    W25QXX_CS_H();                            	//取消片选
    W25QXX_WaitBusy();					   		//等待写入结束
}

//无检验写SPI FLASH
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//buf:数据存储区
//addr:开始写入的地址(24bit)
//len:要写入的字节数(最大65535)
//CHECK OK
static void W25QXX_Write_NoCheck(u32 addr,u8* buf,u16 len)
{
    u16 pageremain;
    pageremain=256-addr%256; //单页剩余的字节数
    if(len<=pageremain)pageremain=len;//不大于256个字节
    while(1)
    {
        W25QXX_WritePage(addr,buf,pageremain);
        if(len==pageremain)break;//写入结束了
        else //len>pageremain
        {
            buf+=pageremain;
            addr+=pageremain;

            len-=pageremain;			  //减去已经写入了的字节数
            if(len>256)pageremain=256; //一次可以写入256个字节
            else pageremain=len; 	  //不够256个字节了
        }
    };
}

//写SPI FLASH
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//buf:数据存储区
//addr:开始写入的地址(24bit)
//len:要写入的字节数(最大65535)
void W25QXX_WriteBytes(u32 addr,u8* buf,u32 len)
{
    u32 secpos;
    u16 secoff;
    u16 secremain;
    u16 i;
    u8 *W25QXX_BUFFER;

    secpos=addr/SECTOR_SIZE;//扇区地址
    secoff=addr%SECTOR_SIZE;//在扇区内的偏移
    secremain=4096-secoff;//扇区剩余空间大小

    if( addr>=(FLASH_SIZE*1024*1024) ||	//大于尾地址
            addr+len>=(FLASH_SIZE*1024*1024)	//剩余空间不足
      )return;//非法地址
    SPI2_ChangeMode(_FLASH_SPI_MODE);
    W25QXX_Wakeup();//唤醒
   
	
    W25QXX_BUFFER = (u8 *)malloc(SECTOR_SIZE*sizeof(u8));//申请一个扇区大小的内存
    if(W25QXX_BUFFER==NULL)	return;

    if(len<=secremain)secremain=len;//不大于4096个字节
    while(1)
    {
        W25QXX_ReadBytes(secpos*SECTOR_SIZE,W25QXX_BUFFER,SECTOR_SIZE);//读出整个扇区的内容
        for(i=0; i<secremain; i++) //校验数据
        {
            if(W25QXX_BUFFER[secoff+i]!=0XFF)break;//需要擦除
        }
        if(i<secremain)//需要擦除
        {
            W25QXX_EraseSector(secpos);		//擦除这个扇区
            for(i=0; i<secremain; i++)	   		//复制
            {
                W25QXX_BUFFER[i+secoff]=buf[i];
            }
            W25QXX_Write_NoCheck(secpos*SECTOR_SIZE,W25QXX_BUFFER,SECTOR_SIZE);//写入整个扇区

        } else W25QXX_Write_NoCheck(addr,buf,secremain);//写已经擦除了的,直接写入扇区剩余区间.
        if(len==secremain)break;//写入结束了
        else//写入未结束
        {
            secpos++;//扇区地址增1
            secoff=0;//偏移位置为0

            buf+=secremain;  				//指针偏移
            addr+=secremain;				//写地址偏移
            len-=secremain;			//字节数递减
            if(len>SECTOR_SIZE)secremain=SECTOR_SIZE;//下一个扇区还是写不完
            else secremain=len;		//下一个扇区可以写完了
        }
    };
    free(W25QXX_BUFFER);
    W25QXX_PowerDown();//进入休眠
    free(W25QXX_BUFFER);
}
//擦除整个芯片
//等待时间超长...
void W25QXX_Erase_Chip(void)
{
    W25QXX_WriteEnable();                 	 	//SET WEL
    W25QXX_WaitBusy();
    W25QXX_CS_L();                            	//使能器件
    W25QXX_SPI_ReadWriteByte(W25X_ChipErase);        	//发送片擦除命令
    W25QXX_CS_H();                            	//取消片选
    W25QXX_WaitBusy();   				   		//等待芯片擦除结束
}

//擦除一个扇区
//addr:扇区地址 根据实际容量设置
//擦除一个山区的最少时间:150ms
void W25QXX_EraseSector(u16 addr)
{
    //监视falsh擦除情况,测试用
    //printf("sector num:%x\r\n",addr);
    addr*=4096;
    W25QXX_WriteEnable();                  	//SET WEL
    W25QXX_WaitBusy();
    W25QXX_CS_L();                            	//使能器件
    W25QXX_SPI_ReadWriteByte(W25X_SectorErase);      	//发送扇区擦除指令
    W25QXX_SPI_ReadWriteByte((u8)((addr)>>16));  	//发送24bit地址
    W25QXX_SPI_ReadWriteByte((u8)((addr)>>8));
    W25QXX_SPI_ReadWriteByte((u8)addr);
    W25QXX_CS_H();                            	//取消片选
    W25QXX_WaitBusy();   				   		//等待擦除完成
}

//初始化SPI FLASH的IO口
void W25QXX_Configuration(void)
{
    W25QXX_CS_H();				//SPI FLASH不选中
    SPI2_ChangeMode(_FLASH_SPI_MODE);
    W25QXX_TYPE=W25QXX_ReadID();//读取FLASH ID.
}

/*********************************************END OF FILE********************************************/
