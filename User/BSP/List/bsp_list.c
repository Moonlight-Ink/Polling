#include "bsp_list.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include "bsp_cjson.h"

uint8_t repeat=0;

struct NODE *NodeCreat(void)
{
  Head=(Node *)malloc(sizeof(Node));
	if(Head == NULL)
	{
	 printf("false1");
   return FALSE;
  }
  Head->data.addr=0;
	Head->data.Offline_Cnt=0;
	Head->data.Type=0;
	memset(Head->data.IO_Enable,0,sizeof(Head->data.IO_Enable));
	memset(Head->data.IO_Triggle,0,sizeof(Head->data.IO_Triggle));
	memset(Head->data.Relay_State,0,sizeof(Head->data.Relay_State));
	Head->Next=NULL;
// Лђеп
//	memset(Head,0,sizeof(Node));
	
	
//	Head->Next=NULL;
	return Head;
}

void Insert_Node(uint32_t *Insert_Temp)
{
	uint8_t i=0,Cnt=0;
	
	uint8_t IO_Channel[6]={0};
	uint8_t IO_State[6]={0};
	uint8_t Relay_State[4]={0};

	Node *Insert;
  Node *Insert_Cur=Head->Next;
	Node *Insert_Pre=Head;
	
	
	Insert->data=Array_to_structure(Insert_Temp);
	
	
	while(Insert_Cur)
	{
		if(Insert_Pre->data.addr > Insert->data.addr && Insert_Cur->data.addr < Insert->data.addr)
			break;
		
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
}


void Delete_Node(uint8_t address)
{
  Node *Cur=Head->Next;
	Node *Pre=Head;
	
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
}

uint8_t Find_Node(uint8_t address)
{
  Node *Cur=Head->Next;
	
	while(Cur)
	{
	  if(Cur->data.addr ==address)
		{
		 return TRUE;		
		}
		Cur = Cur->Next;
	}
	
	return FALSE;
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
  uint8_t i=0;
	uint8_t Cnt=0;
	Data Comp;
	
	uint8_t IO_Channel[6]={0};
	uint8_t IO_State[6]={0};
	uint8_t Relay_State[4]={0};
	
  Node *Cpa_Cur=Head->Next;
	Node *Cpa_Pre=Head;
  
	Comp=Array_to_structure(Temp);

	
	while(Cpa_Cur)
	{
    if(Cpa_Cur->data.addr==Comp.addr)
		{
      Cpa_Cur->data=Comp;
		
      for(i=0;i<6;i++)
      {			
				if(Cpa_Cur->data.IO_Enable[i])
				{
				 IO_Channel[Cnt]=i;
				 IO_State[Cnt]=Cpa_Cur->data.IO_Triggle[i];
				 Cnt++;			
				}
		  } 

      for(i=0;i<Cpa_Cur->data.Type;i++)
			{
			  Relay_State[i]=Cpa_Cur->data.Relay_State[i];			
			}
			
//			Creat_Cjson_Report(Comp);
			Creat_Cjson_Report(IO_Channel,IO_State,Cnt,Relay_State,Cpa_Cur->data.Type,Cpa_Cur->data.addr);			
			break;

    }	
	  Cpa_Pre=Cpa_Cur;
		Cpa_Cur=Cpa_Cur->Next;	
  }
}

void Print_Node(void)
{
//	Node *Pri_Cur=Head->Next;
//	
//	while(Pri_Cur)
//	{
//		printf(" addr:%d,type:%s,chanel:%d,data:%s\r\n",Pri_Cur->data.addr,Pri_Cur->data.type,Pri_Cur->data.channel,Pri_Cur->data.data);		
//		Pri_Cur=Pri_Cur->Next;		
// }
}

Data Array_to_structure(uint32_t *Trans_Temp)
{
	uint8_t i=0;
  Data Trans;
	
	Trans.addr=*(Trans_Temp+3);
	Trans.Type=*(Trans_Temp+5);
	Trans.Offline_Cnt=0;
	for(i=0;i<4;i++)
	{
		Trans.IO_Enable[i]=((*(Trans_Temp+7)>>i)&0x02)==0x02?0x10:0x00; 
	  Trans.IO_Triggle[i]=((*(Trans_Temp+7)>>i)&0x01)==0x01?0x10:0x00;
	}
	for(i=4;i<6;i++)
	{
		Trans.IO_Enable[i]=((*(Trans_Temp+6)>>i)&0x02)==0x02?0x10:0x00; 
	  Trans.IO_Triggle[i]=((*(Trans_Temp+6)>>i)&0x01)==0x01?0x10:0x00;	
	}
	for(i=0;i<4;i++)
	{
	 Trans.Relay_State[i]=((*(Trans_Temp+8)>>i)&0x02)==0x02?0x10:0x00;
	}
	
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









