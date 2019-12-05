#include "fy_malloc.h"

//内存池(4字节对齐)
__align(4) u8 membase[MEM_MAX_SIZE];			//SRAM内存池
//内存管理表
u8 memmapbase[MEM_ALLOC_TABLE_SIZE];			//SRAM内存池MAP
//内存管理参数	   

u8  memrdy; 					//内存管理是否就绪

static void Init(void);
static void Set(void *s,u8 c,u32 count);
static void Copy(void *des,void *src,u32 n);
static u8 GetPerUsed(void);
static void Free(void *ptr);
static void *Alloc(u32 size);
static void *Realloc(void *ptr,u32 size);

_typdef_mem mem = 
{
	Init,		//内存初始化
	Set,		//设置内存
	Copy,		//复制内存  
	GetPerUsed,	//得内存使用率
	Alloc,		//内存分配
	Realloc,	//重新分配内存
	Free		//内存释放
};

//内存管理初始化  
static void Init(void)  
{  
    Set(memmapbase, 0,MEM_ALLOC_TABLE_SIZE);//内存状态表数据清零  
	Set(membase, 0,MEM_MAX_SIZE);	//内存池所有数据清零  
	memrdy=1;						//内存管理初始化OK  
}  
//设置内存
//*s:内存首地址
//c :要设置的值
//count:需要设置的内存大小(字节为单位)
static void Set(void *s,u8 c,u32 count)  
{  
    u8 *xs = s;  
    while(count--)*xs++=c;  
}	
//复制内存
//*des:目的地址
//*src:源地址
//n:需要复制的内存长度(字节为单位)
static void Copy(void *des,void *src,u32 n)  
{  
    u8 *xdes=des;
	u8 *xsrc=src; 
    while(n--)*xdes++=*xsrc++;  
}   
//获取内存使用率
//返回值:使用率(0~100)
static u8 GetPerUsed(void)  
{  
    u32 used=0;  
    u32 i;  
    for(i=0;i<MEM_ALLOC_TABLE_SIZE;i++)  
    {  
        if(memmapbase[i])used++; 
    } 
    return (used*100)/(MEM_ALLOC_TABLE_SIZE);  
}  
//内存分配(内部调用)
//memx:所属内存块
//size:要分配的内存大小(字节)
//返回值:0XFFFFFFFF,代表错误;其他,内存偏移地址 
static u32 mem_malloc(u32 size)  
{  
    signed long offset=0;  
    u16 nmemb;	//需要的内存块数  
	u16 cmemb=0;//连续空内存块数
    u32 i;  
    if(!memrdy)Init();	//未初始化,先执行初始化 
    if(size==0)return 0XFFFFFFFF;				//不需要分配
    nmemb=size/MEM_BLOCK_SIZE;  					//获取需要分配的连续内存块数
    if(size%MEM_BLOCK_SIZE)nmemb++;  
    for(offset=MEM_ALLOC_TABLE_SIZE-1;offset>=0;offset--)	//搜索整个内存控制区  
    {     
		if(!memmapbase[offset])cmemb++;	//连续空内存块数增加
		else cmemb=0;							//连续内存块清零
		if(cmemb==nmemb)						//找到了连续nmemb个空内存块
		{
            for(i=0;i<nmemb;i++)  				//标注内存块非空 
            {  
                memmapbase[offset+i]=nmemb;  
            }  
            return (offset*MEM_BLOCK_SIZE);			//返回偏移地址  
		}
    }  
    return 0XFFFFFFFF;//未找到符合分配条件的内存块  
}  
//释放内存(内部调用) 
//offset:内存地址偏移
//返回值:0,释放成功;1,释放失败;  
static u8 mem_free(u32 offset)  
{  
    int i;  
    if(!memrdy)//未初始化,先执行初始化
	{
		Init();    
        return 1;//未初始化  
    }  
    if(offset<MEM_MAX_SIZE)//偏移在内存池内. 
    {  
        int index=offset/MEM_BLOCK_SIZE;		//偏移所在内存块号码  
        int nmemb=memmapbase[index];	//内存块数量
        for(i=0;i<nmemb;i++)  				//内存块清零
        {  
            memmapbase[index+i]=0;  
        }
        return 0;  
    }else return 2;//偏移超区了.  
}  

//释放内存(外部调用) 
//ptr:内存首地址 
static void Free(void *ptr)  
{  
	u32 offset;  
    if(ptr==NULL)return;//地址为0.  
 	offset=(u32)ptr-(u32)membase;  
    mem_free(offset);	//释放内存     
}  
//分配内存(外部调用)
//size:内存大小(字节)
//返回值:分配到的内存首地址.
static void* Alloc(u32 size)  
{  
    u32 offset;  									      
	offset=mem_malloc(size);  	   				   
    if(offset==0XFFFFFFFF)return NULL;  
    else return (void*)((u32)membase+offset);  
}  
//重新分配内存(外部调用)
//*ptr:旧内存首地址
//size:要分配的内存大小(字节)
//返回值:新分配到的内存首地址.
static void* Realloc(void *ptr,u32 size)  
{  
    u32 offset;  
    offset=mem_malloc(size);  
    if(offset==0XFFFFFFFF)return NULL;     
    else  
    {  									   
	    Copy((void*)((u32)membase+offset),ptr,size);	//拷贝旧内存内容到新内存   
        Free(ptr);  											  	//释放旧内存
        return (void*)((u32)membase+offset);  			//返回新内存首地址
    }  
}


/*********************************************END OF FILE********************************************/

