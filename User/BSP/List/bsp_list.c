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


void Write_Check_Node_Relay_Status_Process(uint8_t Write_Process_Addr,uint8_t Write_Process_Num,uint8_t Write_Process_State)
{
   Node *Write_Cur=Head->Next;
	 Node *Write_Pre=Head;	
   
	uint8_t Cmd_Write_State = (Write_Process_State == 0x02?0x10:0x00);
	
	
	 while(Write_Cur)
	 {
		 if(Write_Cur->data.addr == Write_Process_Addr)
		 {
		   if(Write_Cur->data.Relay_State[Write_Process_Num] != Cmd_Write_State)
			 {
			   Write_Cur->data.Relay_State[Write_Process_Num] = Cmd_Write_State;		 
				 Updata_Cjson(Write_Process_Num,0x02,Write_Cur->data.Relay_State[Write_Process_Num],Write_Cur->data.addr);					 
			 }
		  Write_Pre = Write_Cur;
		  Write_Cur = Write_Cur->Next; 		 		 
		 }	 
	 }
}

void Write_Check_Node_Relay_Status(uint8_t Write_Addr,uint8_t Write_Num,uint8_t Write_State)
{
	OS_ERR      err;	
	
	OSMutexPend ((OS_MUTEX  *)&List,                  //申请互斥信号量 mutex
							 (OS_TICK    )0,                  

	//无期限等待
							 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //如果申请不到就堵塞任务
							 (CPU_TS    *)0,                       //不想获得时间戳
							 (OS_ERR    *)&err);                   //返回错误类型		
	
 Write_Check_Node_Relay_Status_Process(Write_Addr,Write_Num,Write_State);
	

	OSMutexPost ((OS_MUTEX  *)&List,                 //释放互斥信号量 mutex
							 (OS_OPT     )OS_OPT_POST_NONE,       //进行任务调度
							 (OS_ERR    *)&err); 				
	
}



void Query_Node_Process(uint8_t Query_Process_Addr)
{
	 uint8_t Query_Num = 0;
	 uint8_t Query_Cnt = 0;
	 uint8_t Query_IO_Channel[6] = {0};
	 uint8_t Query_IO_State[6] = {0};
	 uint8_t Query_Relay_State[4] = {0};
	 
	 Node *Que_Cur=Head->Next;
	 Node *Que_Pre=Head;	 
	 
	 while(Que_Cur)
	 {
		if(Que_Cur->data.addr == Query_Process_Addr)
		{		
			for(Query_Num = 0;Query_Num < 6;Query_Num++)
			{
				if(Que_Cur->data.IO_Enable[Query_Num])          //IO被使用了
				{
					Query_IO_Channel[Query_Cnt] = Query_Num;
					Query_IO_State[Query_Cnt] = Que_Cur->data.IO_Triggle[Query_Num];
					Query_Cnt++;
				}			
			}
			
			for(Query_Num = 0;Query_Num < Que_Cur->data.Type;Query_Num++)
			{
			 Query_Relay_State[Query_Num] = Que_Cur->data.Relay_State[Query_Num];			
			}	
			
	    Reort_Cjson(Query_IO_Channel,Query_IO_State,Query_Cnt,Query_Relay_State,Que_Cur->data.Type,Que_Cur->data.addr);		
			
		  break;
		}
		
		Que_Pre=Que_Cur;
		Que_Cur=Que_Cur->Next; 		 
	 }
}

void Query_Node(uint8_t Query_Addr)
{
	OS_ERR      err;	
	

	
	OSMutexPend ((OS_MUTEX  *)&List,                  //申请互斥信号量 mutex
							 (OS_TICK    )0,                  

	//无期限等待
							 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //如果申请不到就堵塞任务
							 (CPU_TS    *)0,                       //不想获得时间戳
							 (OS_ERR    *)&err);                   //返回错误类型		
	
  Query_Node_Process(Query_Addr);
	

	OSMutexPost ((OS_MUTEX  *)&List,                 //释放互斥信号量 mutex
							 (OS_OPT     )OS_OPT_POST_NONE,       //进行任务调度
							 (OS_ERR    *)&err); 				
	
}

void Insert_Report(Node *Insert_Report)
{
	 uint8_t Insert_Num = 0;
	 uint8_t Cnt = 0;
	 uint8_t Insert_IO_Channel[6] = {0};
	 uint8_t Insert_IO_State[6] = {0};
	 uint8_t Insert_Relay_State[4] = {0};
	
  for(Insert_Num = 0;Insert_Num < 6;Insert_Num++)
	{
	  if(Insert_Report->data.IO_Enable[Insert_Num])          //IO被使用了
		{
		  Insert_IO_Channel[Cnt] = Insert_Num;
			Insert_IO_State[Cnt] = Insert_Report->data.IO_Triggle[Insert_Num];
			Cnt++;
		}
	
	}
	for(Insert_Num = 0;Insert_Num < Insert_Report->data.Type;Insert_Num++)
	{
	 Insert_Relay_State[Insert_Num] = Insert_Report->data.Relay_State[Insert_Num];
	
	}

	Reort_Cjson(Insert_IO_Channel,Insert_IO_State,Cnt,Insert_Relay_State,Insert_Report->data.Type,Insert_Report->data.addr);	
}


void Insert_Node_Process(uint8_t *Insert_Process_Temp)
{
	Node *Insert_Cur=Head->Next;
	Node *Insert_Pre=Head;

	Node *Insert;
	
	Insert = (Node*)malloc (sizeof(Node));
	
	Insert->data=Array_to_structure(Insert_Process_Temp);
  Insert->data.Offline_Cnt = 0;
	
  while(Insert_Cur)
	{
    Insert_Pre=Insert_Cur;
	  Insert_Cur=Insert_Cur->Next;
  }	
	Insert->Next=Insert_Pre->Next;
  Insert_Pre->Next=Insert;


	Join_Cjson(Insert->data.addr);
	
	Insert_Report(Insert);
}


void Insert_Node(uint8_t *Insert_Temp)
{
	OS_ERR      err;	
		
	OSMutexPend ((OS_MUTEX  *)&List,                  //申请互斥信号量 mutex
							 (OS_TICK    )0,                       //无期限等待
							 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //如果申请不到就堵塞任务
							 (CPU_TS    *)0,                       //不想获得时间戳
							 (OS_ERR    *)&err);                   //返回错误类型			
	
  Insert_Node_Process(Insert_Temp);
	
	
	OSMutexPost ((OS_MUTEX  *)&List,                 //释放互斥信号量 mutex
							 (OS_OPT     )OS_OPT_POST_NONE,       //进行任务调度
							 (OS_ERR    *)&err); 					
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
			if(Cur->data.Offline_Cnt>5)
			{
				Pre->Next=Cur->Next;
        free(Cur);
        Cur=Pre->Next;
				Creat_Cjson_Offline(address);
			}
			else
			{
			  Cur->data.Offline_Cnt++;
				test1 =  Cur->data.Offline_Cnt;
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


uint8_t Find_Node_Process(uint8_t Find_Node_Addr)
{
  Node *Find_Cur=Head->Next;
	
	while(Find_Cur)
	{
	  if(Find_Cur->data.addr == Find_Node_Addr)
		{
		  return 1;		
		}
		Find_Cur = Find_Cur->Next;
	}
	return 0;
}

uint8_t Find_Node(uint8_t Find_Addr)
{
	OS_ERR      err;	

	 OSMutexPend ((OS_MUTEX  *)&List,                  //申请互斥信号量 mutex
								(OS_TICK    )0,                       //无期限等待
								(OS_OPT     )OS_OPT_PEND_BLOCKING,    //如果申请不到就堵塞任务
								(CPU_TS    *)0,                       //不想获得时间戳
								(OS_ERR    *)&err);                   //返回错误类型		

	 if(Find_Node_Process(Find_Addr))
	 {
		 OSMutexPost ((OS_MUTEX  *)&List,                 //释放互斥信号量 mutex
								  (OS_OPT     )OS_OPT_POST_NONE,       //进行任务调度
								  (OS_ERR    *)&err); 				 
		 return 1;
	 }
	 else
	 {
		 OSMutexPost ((OS_MUTEX  *)&List,                 //释放互斥信号量 mutex
								  (OS_OPT     )OS_OPT_POST_NONE,       //进行任务调度
								  (OS_ERR    *)&err); 					 
		 return 0;
	 }
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


void Check_Node_Statues_Process(uint8_t *Check_Node_Process_Temp)
{
	uint8_t Check_Num ;
	Data Check_Node = Array_to_structure(Check_Node_Process_Temp);
	
  Node *Check_Cur=Head->Next;
	Node *Check_Pre=Head; 

  while(Check_Cur)
	{
    if(Check_Cur->data.addr == Check_Node.addr)
		{			
			Check_Cur->data.Offline_Cnt=0;
			Creat_Cjson_Heartbeat(0);
			for(Check_Num =0;Check_Num<6;Check_Num++)
			{						
				if(Check_Cur->data.IO_Enable[Check_Num] != Check_Node.IO_Enable[Check_Num]) //"哪个IO是否使用"的状态不一致
				{					
							if(Check_Cur->data.IO_Enable[Check_Num]) //以前这个IO是使用，那么现在不用了,准备"IO离线上报"--------------------------------------------------------------------leave
							{                                          
								Check_Cur->data.IO_Enable[Check_Num] = Check_Node.IO_Enable[Check_Num];
								Check_Cur->data.IO_Triggle[Check_Num] = Check_Node.IO_Triggle[Check_Num];
								
								Leave_Cjson(Check_Num,Check_Cur->data.IO_Triggle[Check_Num],Check_Cur->data.addr);//哪个channel,该channel的状态															
							}
							
							
							else   //以前这个IO不使用，那么现在启用了,准备"发现新IO上报"---------------------------discovery
							{
								Check_Cur->data.IO_Enable[Check_Num] = Check_Node.IO_Enable[Check_Num];
								Check_Cur->data.IO_Triggle[Check_Num] = Check_Node.IO_Triggle[Check_Num];
								
								Discovery_Cjson(Check_Num,Check_Cur->data.IO_Triggle[Check_Num],Check_Cur->data.addr);//哪个channel,该channel的状态								
							}
						
				}
				
				else                      //以前和现在的IO是否使能状态一致     
       	{
				   if(Check_Cur->data.IO_Enable[Check_Num])     //这个IO正在使用中。。。。
					 {
					    if(Check_Cur->data.IO_Triggle[Check_Num] != Check_Node.IO_Triggle[Check_Num]) //这个IO的状态前后不一致，则启动IO状态变化上报----updata,0x01代表是IO
							{
							  Check_Cur->data.IO_Triggle[Check_Num] = Check_Node.IO_Triggle[Check_Num];
							
								Updata_Cjson(Check_Num,0x01,Check_Cur->data.IO_Triggle[Check_Num],Check_Cur->data.addr);
							}
					 }
				}							
			}		

			for(Check_Num =0;Check_Num<Check_Cur->data.Type;Check_Num++)
			{	
				 if(Check_Cur->data.Relay_State[Check_Num] != Check_Node.Relay_State[Check_Num])     //继电器以前和现在的状态不一致，那么启动Relay状态变化上报-----updata,0x02代表继电器
				 { 
					 Check_Cur->data.Relay_State[Check_Num] = Check_Node.Relay_State[Check_Num];
				 
					 Updata_Cjson(Check_Num,0x02,Check_Cur->data.Relay_State[Check_Num],Check_Cur->data.addr);				 
				 }		
			}				
			
			break;

    }	
	  Check_Pre=Check_Cur;
		Check_Cur=Check_Cur->Next;	
  }
}

void Check_Node_Status(uint8_t *Check_Node_Temp)
{
	OS_ERR      err;	
	
	OSMutexPend ((OS_MUTEX  *)&List,                  //申请互斥信号量 mutex
							 (OS_TICK    )0,                       //无期限等待
							 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //如果申请不到就堵塞任务
							 (CPU_TS    *)0,                       //不想获得时间戳
							 (OS_ERR    *)&err);                   //返回错误类型		
	
  Check_Node_Statues_Process(Check_Node_Temp);
	
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

Data Array_to_structure(uint8_t *Trans_Temp)
{
	uint8_t i=0;
  Data Trans;
	
	Trans.addr=*(Trans_Temp+3);
	Trans.Type=*(Trans_Temp+5);
	
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









