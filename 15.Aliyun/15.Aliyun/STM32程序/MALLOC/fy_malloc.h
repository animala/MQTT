#ifndef __FY_MALLOC_H
#define __FY_MALLOC_H

#include "stm32f10x.h"

#ifndef NULL
#define NULL 0
#endif

//�ڴ�����趨.
#define MEM_BLOCK_SIZE			32  	  						//�ڴ���СΪ32�ֽ�
#define MEM_MAX_SIZE			8*1024  						//�������ڴ� 42K
#define MEM_ALLOC_TABLE_SIZE	MEM_MAX_SIZE/MEM_BLOCK_SIZE 	//�ڴ���С


typedef struct
{
	void  (*Init)(void);						//�ڴ��ʼ��
	void  (*Set)(void *des,u8 c,u32 count);		//�����ڴ�
	void  (*Copy)(void *des,void *src,u32 n);	//�����ڴ�  
	u8    (*GetPerUsed)(void);			//���ڴ�ʹ����
	void* (*Alloc)(u32 len);			//�ڴ����
	void* (*Realloc)(void *p,u32 len);	//���·����ڴ�
	void  (*Free)(void *p);				//�ڴ��ͷ�
}_typdef_mem;


#endif

/*********************************************END OF FILE********************************************/

