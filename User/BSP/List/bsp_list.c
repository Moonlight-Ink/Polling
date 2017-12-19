#include "bsp_list.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include "bsp_cjson.h"
#include "bsp_usart1.h"
#include <includes.h>

extern OS_MUTEX List; 

uint8_t test1=0,test2=0,test3=0;
uint8_t tat1[6]={0},tat2[6]={0},tat3[4]={0};
uint8_t tat4[6]={0},tat5[6]={0},tat6[4]={0};



uint8_t repeat=0;

struct NODE *NodeCreat(void)
{
  Head=(Node *)malloc(sizeof(Node));
	if(Head == NULL)
	{
	 printf("false1");
   return 0;
  }
  Head->data.addr=0;
	Head->data.Offline_Cnt=0;
	Head->data.Type=0;
	memset(Head->data.IO_Enable,0,sizeof(Head->data.IO_Enable));
	memset(Head->data.IO_Triggle,0,sizeof(Head->data.IO_Triggle));
	memset(Head->data.Relay_State,0,sizeof(Head->data.Relay_State));
	Head->Next=NULL;
// 或者
//	memset(Head,0,sizeof(Node));
	
	
//	Head->Next=NULL;
	return Head;
}


void Query_Node(uint8_t Addr)
{
	OS_ERR      err;	
	
	uint8_t i = 0,Cnt = 0;
	uint8_t IO_Channel[6] = {0};
	uint8_t IO_State[6] = {0};
	uint8_t Relay_State[4] = {0};
	Node *Que_Cur=Head->Next;
	Node *Que_Pre=Head;
	
	OSMutexPend ((OS_MUTEX  *)&List,                  //申请互斥信号量 mutex
							 (OS_TICK    )0,                       //无期限等待
							 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //如果申请不到就堵塞任务
							 (CPU_TS    *)0,                       //不想获得时间戳
							 (OS_ERR    *)&err);                   //返回错误类型		
	
	
	while(Que_Cur)
	{
    if(Que_Cur->data.addr ==Addr)
    {
			for(i=0;i<6;i++)
			{
				if(Que_Cur->data.IO_Enable[i] == 0x10)
				{
					IO_Channel[Cnt] = i;
					IO_State[Cnt] = Que_Cur->data.IO_Triggle[i];
					Cnt++;
				}
		  }
			
      for(i=0;i<Que_Cur->data.Type;i++)
			{
			  Relay_State[i] = Que_Cur->data.Relay_State[i];			
			}
			Creat_Cjson_Report(IO_Channel,IO_State,Cnt,Relay_State,Que_Cur->data.Type,Que_Cur->data.addr);					
			break;
    }
		Que_Pre=Que_Cur;
		Que_Cur=Que_Cur->Next; 
  }
	
	OSMutexPost ((OS_MUTEX  *)&List,                 //释放互斥信号量 mutex
							 (OS_OPT     )OS_OPT_POST_NONE,       //进行任务调度
							 (OS_ERR    *)&err); 				
	
}


void Insert_Node(uint32_t *Insert_Temp)
{
	OS_ERR      err;	
	
	uint8_t i=0,Cnt=0;
	
	uint8_t IO_Channel[6]={0};
	uint8_t IO_State[6]={0};
	uint8_t Relay_State[4]={0};
	
	Node *Insert_Cur=Head->Next;
	Node *Insert_Pre=Head;

	Node *Insert;
	Insert = (Node*)malloc (sizeof(Node));
	
	Insert->data=Array_to_structure(Insert_Temp);

	
	
	OSMutexPend ((OS_MUTEX  *)&List,                  //申请互斥信号量 mutex
							 (OS_TICK    )0,                       //无期限等待
							 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //如果申请不到就堵塞任务
							 (CPU_TS    *)0,                       //不想获得时间戳
							 (OS_ERR    *)&err);                   //返回错误类型			
	
	while(Insert_Cur)
	{
    Insert_Pre=Insert_Cur;
	  Insert_Cur=Insert_Cur->Next;
  }	
		Insert->Next=Insert_Pre->Next;
		Insert_Pre->Next=Insert;

	
	for(i=0;i<6;i++)
  {			
		if(Insert->data.IO_Enable[i])
		{
			IO_Channel[Cnt]=i;
			IO_State[Cnt]=Insert->data.IO_Triggle[i];
			Cnt++;			
		}
	} 

  for(i=0;i<Insert->data.Type;i++)
	{
		 Relay_State[i]=Insert->data.Relay_State[i];			
	}  
			
	Creat_Cjson_Join(IO_Channel,IO_State,Cnt,Relay_State,Insert->data.Type,Insert->data.addr);
	
	
	OSMutexPost ((OS_MUTEX  *)&List,                 //释放互斥信号量 mutex
							 (OS_OPT     )OS_OPT_POST_NONE,       //进行任务调度
							 (OS_ERR    *)&err); 					
//      Print_Node();		
}


void Delete_Node(uint8_t address)
{
	OS_ERR      err;	
	
  Node *Cur=Head->Next;
	Node *Pre=Head;


	OSMutexPend ((OS_MUTEX  *)&List,                  //申请互斥信号量 mutex
							 (OS_TICK    )0,                       //无期限等待
							 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //如果申请不到就堵塞任务
							 (CPU_TS    *)0,                       //不想获得时间戳
							 (OS_ERR    *)&err);                   //返回错误类型		

	
	while(Cur)
	{
    if(Cur->data.addr ==address)
    {
			if(Cur->data.Offline_Cnt>3)
			{
				Pre->Next=Cur->Next;
        free(Cur);
        Cur=Pre->Next;
				Creat_Cjson_Offline(address);
			}
			else
			{
			  Cur->data.Offline_Cnt++;
			}
			break;
    }
		else
		{
			Pre=Cur;
			Cur=Cur->Next;
		}
 }

	OSMutexPost ((OS_MUTEX  *)&List,                 //释放互斥信号量 mutex
							 (OS_OPT     )OS_OPT_POST_NONE,       //进行任务调度
							 (OS_ERR    *)&err); 				

 
}

uint8_t Find_Node(uint8_t address)
{
	OS_ERR      err;	
	
  Node *Cur=Head->Next;

	OSMutexPend ((OS_MUTEX  *)&List,                  //申请互斥信号量 mutex
							 (OS_TICK    )0,                       //无期限等待
							 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //如果申请不到就堵塞任务
							 (CPU_TS    *)0,                       //不想获得时间戳
							 (OS_ERR    *)&err);                   //返回错误类型		

	
	while(Cur)
	{
	  if(Cur->data.addr ==address)
		{
			
	    OSMutexPost ((OS_MUTEX  *)&List,                 //释放互斥信号量 mutex
							     (OS_OPT     )OS_OPT_POST_NONE,       //进行任务调度
							     (OS_ERR    *)&err); 							
		 return 1;		
		}
		Cur = Cur->Next;
	}


	OSMutexPost ((OS_MUTEX  *)&List,                 //释放互斥信号量 mutex
							 (OS_OPT     )OS_OPT_POST_NONE,       //进行任务调度
							 (OS_ERR    *)&err); 				
	
	return 0;
}



//void Delete_Node(uint8_t address)
//{
//  Node *Cur=Head->Next;
//	Node *Pre=Head;
//	
//	while(Cur)
//	{
//    if(Cur->data.addr ==address)
//    {
//			Pre->Next=Cur->Next;
//      free(Cur);
//      Cur=Pre;			
//    }
//			Pre=Cur;
//			Cur=Cur->Next;

// }
// 	Print_Node();
// }

void Updata_Node(uint32_t *Temp)
{
	OS_ERR      err;	
	
  uint8_t i=0;
	uint8_t Cnt=0;
	Data Comp;
	
	
	uint8_t IO_Channel[6]={0};
	uint8_t IO_State[6]={0};
	uint8_t Change_IO_Cnt=0;
	uint8_t Relay_State[4]={0};
	uint8_t Change_Relay_Cnt=0;
	
	uint8_t Discovery_Channel[6]={0};
	uint8_t Discovery_State[6]={0};
	uint8_t Discovery_Cnt=0;
	
	uint8_t Offline_Channel[6]={0};
	uint8_t Offline_Cnt=0;
	
  Node *Cpa_Cur=Head->Next;
	Node *Cpa_Pre=Head;
  
	Comp=Array_to_structure(Temp);

//	memcpy(tat1,Comp.IO_Enable,6);
//	memcpy(tat2,Comp.IO_Triggle,6);
//	memcpy(tat3,Comp.Relay_State,4);	
	
	OSMutexPend ((OS_MUTEX  *)&List,                  //申请互斥信号量 mutex
							 (OS_TICK    )0,                       //无期限等待
							 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //如果申请不到就堵塞任务
							 (CPU_TS    *)0,                       //不想获得时间戳
							 (OS_ERR    *)&err);                   //返回错误类型			
	
	while(Cpa_Cur)
	{
    if(Cpa_Cur->data.addr==Comp.addr)
		{
			
//	memcpy(tat4,Cpa_Cur->data.IO_Enable,6);
//	memcpy(tat5,Cpa_Cur->data.IO_Triggle,6);
//	memcpy(tat6,Cpa_Cur->data.Relay_State,4);			
			
			Cpa_Cur->data.Offline_Cnt=0;
			
			for(i=0;i<6;i++)
			{
		  	if(Cpa_Cur->data.IO_Enable[i]==0x10) //若链表io控制端是“使能”状态
				{	
					 if(Comp.IO_Enable[i]==0x10)        //新的io控制端也是"使能状态"，且io状态与链表一致，则不需要更新数据，否则更新链表中io状态，并上报
					 {
						 if(Cpa_Cur->data.IO_Triggle[i] != Comp.IO_Triggle[i])
						 {
							 Cpa_Cur->data.IO_Triggle[i]=Comp.IO_Triggle[i];								
							 Change_IO_Cnt++;							   
						 }					 
					 }
					 else                                       //若新的io是"失能"状态，则更新链表的io控制状态为"失能"状态，并更新链表io状态,并上报offline
					 {
						 Cpa_Cur->data.IO_Enable[i]=Comp.IO_Enable[i];
						 Cpa_Cur->data.IO_Triggle[i]=Comp.IO_Triggle[i];
					   Offline_Cnt++;
					 }		
				}	
        else                                    //若链表中io控制端是"失能"状态
				{
					if(Comp.IO_Enable[i]==0x10)        //新的io控制端是"使能"状态，则更新链表的io控制端状态为"使能"，更新链表io状态，并上报discovery
					{
					   Cpa_Cur->data.IO_Enable[i]=Comp.IO_Enable[i];
						 Cpa_Cur->data.IO_Triggle[i]=Comp.IO_Triggle[i];
						 Discovery_Channel[Discovery_Cnt]=i;
						 Discovery_State[Discovery_Cnt]=Cpa_Cur->data.IO_Triggle[i];
					   Discovery_Cnt++;	
					}
				
				}
				
				if(Cpa_Cur->data.IO_Enable[i])
				{
					 IO_Channel[Cnt]=i;
					 IO_State[Cnt]=Cpa_Cur->data.IO_Triggle[i];
					 Cnt++;			
				}					
			}		

			for(i=0;i<Cpa_Cur->data.Type;i++)
			{
			  if(Cpa_Cur->data.Relay_State[i] != Comp.Relay_State[i])  //链表中继电器状态和新的继电器状态不一致，则更新继电器状态，并上报
				{
				  Cpa_Cur->data.Relay_State[i] = Comp.Relay_State[i];
					Change_Relay_Cnt++;				
				}
        Relay_State[i]=   Cpa_Cur->data.Relay_State[i];	
			}

			
			if(Discovery_Cnt)
			{		
        Create_Cjson_Discovery(Discovery_Channel,Discovery_State,Discovery_Cnt,Cpa_Cur->data.addr);				
			}
		  if(Change_IO_Cnt || Offline_Cnt || Discovery_Cnt)
			{				
			  Creat_Cjson_Report(IO_Channel,IO_State,Cnt,Relay_State,Cpa_Cur->data.Type,Cpa_Cur->data.addr);					
			}
			break;

    }	
	  Cpa_Pre=Cpa_Cur;
		Cpa_Cur=Cpa_Cur->Next;	
  }
	
	
	OSMutexPost ((OS_MUTEX  *)&List,                 //释放互斥信号量 mutex
							 (OS_OPT     )OS_OPT_POST_NONE,       //进行任务调度
							 (OS_ERR    *)&err); 					
	
	
}

void Print_Node(void)
{
	OS_ERR      err;	
	
	uint8_t aa[2]={0};
	Node *Pri_Cur=Head->Next;

	OSMutexPend ((OS_MUTEX  *)&List,                  //申请互斥信号量 mutex
							 (OS_TICK    )0,                       //无期限等待
							 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //如果申请不到就堵塞任务
							 (CPU_TS    *)0,                       //不想获得时间戳
							 (OS_ERR    *)&err);                   //返回错误类型		

	
	
	while(Pri_Cur)
	{
		aa[0]=Pri_Cur->data.addr;
		USART1_Send_Data(aa,1);
//		printf("addr:%s\r\n",Pri_Cur->data.addr);		
		Pri_Cur=Pri_Cur->Next;		
 }
	
 
	OSMutexPost ((OS_MUTEX  *)&List,                 //释放互斥信号量 mutex
							 (OS_OPT     )OS_OPT_POST_NONE,       //进行任务调度
							 (OS_ERR    *)&err); 				 
 
 
}

Data Array_to_structure(uint32_t *Trans_Temp)
{
	uint8_t i=0;
  Data Trans;
	
	Trans.addr=*(Trans_Temp+3);
	Trans.Type=*(Trans_Temp+5);
	Trans.Offline_Cnt=0;
	
//	test1=*(Trans_Temp+6);
//	test2=*(Trans_Temp+7);
//	test3=*(Trans_Temp+8);
	
	for(i=0;i<4;i++)
	{
		Trans.IO_Enable[i]=((*(Trans_Temp+7)>>(2*i))&0x02)==0x02?0x10:0x00; 
	  Trans.IO_Triggle[i]=((*(Trans_Temp+7)>>(2*i))&0x01)==0x01?0x10:0x00;
	}
	for(i=0;i<2;i++)
	{	
		Trans.IO_Enable[4+i]=((*(Trans_Temp+6)>>(2*i))&0x02)==0x02?0x10:0x00; 
	  Trans.IO_Triggle[4+i]=((*(Trans_Temp+6)>>(2*i))&0x01)==0x01?0x10:0x00;	
	}
	for(i=0;i<4;i++)
	{
	 Trans.Relay_State[i]=((*(Trans_Temp+8)>>(2*i))&0x02)==0x02?0x10:0x00;
	}
	
//	memcpy(tat1,Trans.IO_Enable,6);
//	memcpy(tat2,Trans.IO_Triggle,6);
//	memcpy(tat3,Trans.Relay_State,4);
	
	return Trans;
}



//uint8_t Insert_data(void)
//{	

//	
//  Data *Insert;
//	
//	Insert=(Data *)malloc(sizeof(Node));

//	if(Insert==NULL)
//	{
//   printf("false0");	
//	 return FALSE;
//  }

//	Insert->addr=0x01;
//	strcpy(Insert->type,"IO");
//	Insert->channel=0x01;
//  strcpy(Insert->data,"unpress");
//	Insert_Node(Insert);
//	
//	Insert->addr=0x02;
//	strcpy(Insert->type,"Relay");
//	Insert->channel=0x01;
//	strcpy(Insert->data,"open");
//	Insert_Node(Insert);

//	

//	Insert->addr=0x02;
//	strcpy(Insert->type,"IO");
//	Insert->channel=0x01;
//	strcpy(Insert->data,"press");
//	Insert_Node(Insert);

////	Insert=(Data *)malloc(sizeof(Node));
////	Insert->addr=0x02;
////	strcpy(Insert->type,"Relay");
////	Insert->channel=0x03;
////	 strcpy(Insert->data,"close");
////	Insert_Node(Insert);

//	Insert=(Data *)malloc(sizeof(Node));
//	Insert->addr=0x02;
//	strcpy(Insert->type,"Relay");
//	Insert->channel=0x04;
//	strcpy(Insert->data,"close");
//	Insert_Node(Insert);

//	Insert=(Data *)malloc(sizeof(Node));
//	Insert->addr=0x01;
//	strcpy(Insert->type,"Relay");
//	Insert->channel=0x06;
//	strcpy(Insert->data,"open");
//	Insert_Node(Insert);


//	Insert->addr=0x04;
//	strcpy(Insert->type,"IO");
//	Insert->channel=0x01;
//	strcpy(Insert->data,"unpress");
//	Insert_Node(Insert);
// 
//	Print_Node();
//	return TRUE;
//}









