#ifndef __FY_MALLOC_H
#define __FY_MALLOC_H

#include "stm32f10x.h"

#ifndef NULL
#define NULL 0
#endif

//内存参数设定.
#define MEM_BLOCK_SIZE			32  	  						//内存块大小为32字节
#define MEM_MAX_SIZE			8*1024  						//最大管理内存 42K
#define MEM_ALLOC_TABLE_SIZE	MEM_MAX_SIZE/MEM_BLOCK_SIZE 	//内存表大小


typedef struct
{
	void  (*Init)(void);						//内存初始化
	void  (*Set)(void *des,u8 c,u32 count);		//设置内存
	void  (*Copy)(void *des,void *src,u32 n);	//复制内存  
	u8    (*GetPerUsed)(void);			//得内存使用率
	void* (*Alloc)(u32 len);			//内存分配
	void* (*Realloc)(void *p,u32 len);	//重新分配内存
	void  (*Free)(void *p);				//内存释放
}_typdef_mem;


#endif

/*********************************************END OF FILE********************************************/

