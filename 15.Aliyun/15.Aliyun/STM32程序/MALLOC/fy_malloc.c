#include "fy_malloc.h"

//�ڴ��(4�ֽڶ���)
__align(4) u8 membase[MEM_MAX_SIZE];			//SRAM�ڴ��
//�ڴ�����
u8 memmapbase[MEM_ALLOC_TABLE_SIZE];			//SRAM�ڴ��MAP
//�ڴ�������	   

u8  memrdy; 					//�ڴ�����Ƿ����

static void Init(void);
static void Set(void *s,u8 c,u32 count);
static void Copy(void *des,void *src,u32 n);
static u8 GetPerUsed(void);
static void Free(void *ptr);
static void *Alloc(u32 size);
static void *Realloc(void *ptr,u32 size);

_typdef_mem mem = 
{
	Init,		//�ڴ��ʼ��
	Set,		//�����ڴ�
	Copy,		//�����ڴ�  
	GetPerUsed,	//���ڴ�ʹ����
	Alloc,		//�ڴ����
	Realloc,	//���·����ڴ�
	Free		//�ڴ��ͷ�
};

//�ڴ�����ʼ��  
static void Init(void)  
{  
    Set(memmapbase, 0,MEM_ALLOC_TABLE_SIZE);//�ڴ�״̬����������  
	Set(membase, 0,MEM_MAX_SIZE);	//�ڴ��������������  
	memrdy=1;						//�ڴ�����ʼ��OK  
}  
//�����ڴ�
//*s:�ڴ��׵�ַ
//c :Ҫ���õ�ֵ
//count:��Ҫ���õ��ڴ��С(�ֽ�Ϊ��λ)
static void Set(void *s,u8 c,u32 count)  
{  
    u8 *xs = s;  
    while(count--)*xs++=c;  
}	
//�����ڴ�
//*des:Ŀ�ĵ�ַ
//*src:Դ��ַ
//n:��Ҫ���Ƶ��ڴ泤��(�ֽ�Ϊ��λ)
static void Copy(void *des,void *src,u32 n)  
{  
    u8 *xdes=des;
	u8 *xsrc=src; 
    while(n--)*xdes++=*xsrc++;  
}   
//��ȡ�ڴ�ʹ����
//����ֵ:ʹ����(0~100)
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
//�ڴ����(�ڲ�����)
//memx:�����ڴ��
//size:Ҫ������ڴ��С(�ֽ�)
//����ֵ:0XFFFFFFFF,�������;����,�ڴ�ƫ�Ƶ�ַ 
static u32 mem_malloc(u32 size)  
{  
    signed long offset=0;  
    u16 nmemb;	//��Ҫ���ڴ����  
	u16 cmemb=0;//�������ڴ����
    u32 i;  
    if(!memrdy)Init();	//δ��ʼ��,��ִ�г�ʼ�� 
    if(size==0)return 0XFFFFFFFF;				//����Ҫ����
    nmemb=size/MEM_BLOCK_SIZE;  					//��ȡ��Ҫ����������ڴ����
    if(size%MEM_BLOCK_SIZE)nmemb++;  
    for(offset=MEM_ALLOC_TABLE_SIZE-1;offset>=0;offset--)	//���������ڴ������  
    {     
		if(!memmapbase[offset])cmemb++;	//�������ڴ��������
		else cmemb=0;							//�����ڴ������
		if(cmemb==nmemb)						//�ҵ�������nmemb�����ڴ��
		{
            for(i=0;i<nmemb;i++)  				//��ע�ڴ��ǿ� 
            {  
                memmapbase[offset+i]=nmemb;  
            }  
            return (offset*MEM_BLOCK_SIZE);			//����ƫ�Ƶ�ַ  
		}
    }  
    return 0XFFFFFFFF;//δ�ҵ����Ϸ����������ڴ��  
}  
//�ͷ��ڴ�(�ڲ�����) 
//offset:�ڴ��ַƫ��
//����ֵ:0,�ͷųɹ�;1,�ͷ�ʧ��;  
static u8 mem_free(u32 offset)  
{  
    int i;  
    if(!memrdy)//δ��ʼ��,��ִ�г�ʼ��
	{
		Init();    
        return 1;//δ��ʼ��  
    }  
    if(offset<MEM_MAX_SIZE)//ƫ�����ڴ����. 
    {  
        int index=offset/MEM_BLOCK_SIZE;		//ƫ�������ڴ�����  
        int nmemb=memmapbase[index];	//�ڴ������
        for(i=0;i<nmemb;i++)  				//�ڴ������
        {  
            memmapbase[index+i]=0;  
        }
        return 0;  
    }else return 2;//ƫ�Ƴ�����.  
}  

//�ͷ��ڴ�(�ⲿ����) 
//ptr:�ڴ��׵�ַ 
static void Free(void *ptr)  
{  
	u32 offset;  
    if(ptr==NULL)return;//��ַΪ0.  
 	offset=(u32)ptr-(u32)membase;  
    mem_free(offset);	//�ͷ��ڴ�     
}  
//�����ڴ�(�ⲿ����)
//size:�ڴ��С(�ֽ�)
//����ֵ:���䵽���ڴ��׵�ַ.
static void* Alloc(u32 size)  
{  
    u32 offset;  									      
	offset=mem_malloc(size);  	   				   
    if(offset==0XFFFFFFFF)return NULL;  
    else return (void*)((u32)membase+offset);  
}  
//���·����ڴ�(�ⲿ����)
//*ptr:���ڴ��׵�ַ
//size:Ҫ������ڴ��С(�ֽ�)
//����ֵ:�·��䵽���ڴ��׵�ַ.
static void* Realloc(void *ptr,u32 size)  
{  
    u32 offset;  
    offset=mem_malloc(size);  
    if(offset==0XFFFFFFFF)return NULL;     
    else  
    {  									   
	    Copy((void*)((u32)membase+offset),ptr,size);	//�������ڴ����ݵ����ڴ�   
        Free(ptr);  											  	//�ͷž��ڴ�
        return (void*)((u32)membase+offset);  			//�������ڴ��׵�ַ
    }  
}


/*********************************************END OF FILE********************************************/

