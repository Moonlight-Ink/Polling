#include "bsp_cjson.h"
#include "malloc.h"
#include "cJSON.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "tcp_demo.h"
#include "socket.h"
#include "w5500.h"
#include "bsp_list.h"
#include "bsp_crc8.h"
#include "bsp_usart1.h"
#include <includes.h>


uint32_t tst[8]={0xad,0xda,0x04,0x01,0x60,0x01,0x03,0x0c};

void TCP_Cmd_Cjson_Analyze(uint8_t *TCP_Cmd,uint16_t TCP_Cmd_Cnt)
{
	cJSON *root,*out;
  
	
	root=cJSON_Parse((char *)TCP_Cmd);
	if(!root)
  {
    printf("Error before: [%s]\n",cJSON_GetErrorPtr());	
	}
	else
	{
	  out=cJSON_GetObjectItem(root,"cmd");
		
    if(!strcmp(out->valuestring,"query"))
		{
			out=cJSON_GetObjectItem(root,"addr");
			Query_Node(out->valueint);
		}
		else if(!strcmp(out->valuestring,"write"))
		{
			  USART1_Send_Data(tst,8);
//			out=cJSON_GetObjectItem(root,"addr");
//			if(Find_Node(cJSON_GetObjectItem(root,"addr")->valueint))
//			{
//			  Cmd_Ctronl(cJSON_GetObjectItem(root,"addr")->valueint,
//			             cJSON_GetObjectItem(root,"relay")->valuestring);
//			}
		}
		else if(!strcmp(out->valuestring,"heartbeat"))
		{
			out=cJSON_GetObjectItem(root,"addr"); 
		  Creat_Cjson_Heartbeat(out->valueint);
		}
	}

	 cJSON_Delete(root);  
}


void Cmd_Ctronl(uint8_t Addr,char *Ctrl)
{
	OS_ERR      err;	
		CPU_SR_ALLOC();
	
	uint8_t i=0,Relay;
	uint8_t Cmd_Buffer[15]={0};
  uint8_t CRC_Buffer[10]={0};
	char temp[20]="";
  cJSON *item = NULL,*out1 = NULL;
  
	item = cJSON_Parse(Ctrl);
	
	if(!item)
	{
		printf("Error before: [%s]\n",cJSON_GetErrorPtr());	
	}
	else
	{
	  for(i=0;i<4;i++)
		{
		  sprintf(temp,"channel_%d",i);		  
			out1=cJSON_GetObjectItem(item,temp);
			if(out1)
			{
				if(!strcmp(out1->valuestring,"on"))
				{
					Relay = 0x02;
        }
				if(!strcmp(out1->valuestring,"off")) 
				{
					Relay = 0x01;
        }
        
				if(Relay==0x02 || Relay==0x01)
				{
					Cmd_Buffer[0]=0xad;
					CRC_Buffer[0]=Cmd_Buffer[0];
					
					Cmd_Buffer[1]=0xda;
					CRC_Buffer[1]=Cmd_Buffer[1];
					
					Cmd_Buffer[2]=0x04;
					CRC_Buffer[2]=Cmd_Buffer[2];  
					
					Cmd_Buffer[3]=Addr;
					CRC_Buffer[3]=Cmd_Buffer[3];
					
					Cmd_Buffer[4]=0x60;
					CRC_Buffer[4]=Cmd_Buffer[4];
					
					Cmd_Buffer[5]=i+1;
					CRC_Buffer[5]=Cmd_Buffer[5];
										
					Cmd_Buffer[6]=Relay;
					CRC_Buffer[6]=Cmd_Buffer[5];
					
					Cmd_Buffer[7]=CRC8_Check(7,CRC_Buffer);	

//		OS_CRITICAL_ENTER();    
          USART1_Send_Data1(Cmd_Buffer,8);
//	    OS_CRITICAL_EXIT();   				
//					OSTimeDlyHMSM ( 0, 0, 1,0, OS_OPT_TIME_DLY, &err);	
				}				
			}		  
		}
	}

}



void Creat_Cjson_Heartbeat(uint8_t Addr)
{
	 cJSON *root;
	 char *out;
	
	 if(Find_Node(Addr))
	 {
	   root=cJSON_CreateObject();
		 cJSON_AddStringToObject(root,"cmd","heartbeat");
		 cJSON_AddStringToObject(root,"model","perfe1");
		 cJSON_AddNumberToObject(root,"addr",Addr);
		 out=cJSON_Print(root);
	   cJSON_Delete(root);

		 if(getSn_SR(SOCK_TCPS) == SOCK_ESTABLISHED)
		 {
			 TCP_Send_Cnt = strlen(out);
    	 memcpy(TCP_Send_Buffer,(uint8_t*)out,TCP_Send_Cnt);
			 TCP_Send_Flag = 1; 	
		 }		 	 
		 	myfree(out);		 
   }
}



void Creat_Cjson_Report(uint8_t *IO_Channel,uint8_t *IO_State,uint8_t IO_Num,uint8_t *Relay_State,uint8_t Relay_Num,uint8_t Addr)
{
  cJSON *root;
	char *out;
	char String1[200]={0};
	char String2[200]={0};
	
	Generate_IO_String(IO_Channel,IO_State,IO_Num,String1);
  Generate_Relay_String(Relay_State,Relay_Num,String2);
	
	root=cJSON_CreateObject();
	cJSON_AddStringToObject(root,"cmd","report");
	cJSON_AddStringToObject(root,"model","perfe1");
	cJSON_AddNumberToObject(root,"addr",Addr);
//	cJSON_AddStringToObject(root,"type","io");
	cJSON_AddStringToObject(root,"io",String1);
	cJSON_AddStringToObject(root,"relay",String2);
  out=cJSON_Print(root);
	cJSON_Delete(root);
	
//  /*·¢ËÍº¯Êýrs485*/	
//	printf("%s\n",out);

	if(getSn_SR(SOCK_TCPS) == SOCK_ESTABLISHED)
	{
		TCP_Send_Cnt = strlen(out);
    memcpy(TCP_Send_Buffer,(uint8_t*)out,TCP_Send_Cnt);
		TCP_Send_Flag = 1; 	
	}	




	myfree(out);	
	
//	memset(String1,0,200);
//	memset(String2,0,200);	
	
}

void Creat_Cjson_Offline(uint8_t Addr)
{
	cJSON *root;
	char *out;
	
	root=cJSON_CreateObject();
	cJSON_AddStringToObject(root,"cmd","offline");
	cJSON_AddStringToObject(root,"model","perfe1");
	cJSON_AddNumberToObject(root,"addr",Addr);
  out=cJSON_Print(root);
	cJSON_Delete(root);
	
	if(getSn_SR(SOCK_TCPS) == SOCK_ESTABLISHED)
	{
		TCP_Send_Cnt = strlen(out);
    memcpy(TCP_Send_Buffer,(uint8_t*)out,TCP_Send_Cnt);
		TCP_Send_Flag = 1; 	
	}	
	
	myfree(out);	

}	


void Creat_Cjson_Join(uint8_t *IO_Channel,uint8_t *IO_State,uint8_t IO_Num,uint8_t *Relay_State,uint8_t Relay_Num,uint8_t Addr)
{
  cJSON *root;
	char *out;
	char String1[200]={0};
	char String2[200]={0};
	
	Generate_IO_String(IO_Channel,IO_State,IO_Num,String1);
  Generate_Relay_String(Relay_State,Relay_Num,String2);
	
	root=cJSON_CreateObject();
	cJSON_AddStringToObject(root,"cmd","join");
	cJSON_AddStringToObject(root,"model","perfe1");
	cJSON_AddNumberToObject(root,"addr",Addr);
//	cJSON_AddStringToObject(root,"type","io");
	cJSON_AddStringToObject(root,"io",String1);
	cJSON_AddStringToObject(root,"relay",String2);
  out=cJSON_Print(root);
	cJSON_Delete(root);
	
	if(getSn_SR(SOCK_TCPS) == SOCK_ESTABLISHED)
	{
		TCP_Send_Cnt = strlen(out);
    memcpy(TCP_Send_Buffer,(uint8_t*)out,TCP_Send_Cnt);
		TCP_Send_Flag = 1; 	
	}	
	
	myfree(out);	
	
//	memset(String1,0,200);
//	memset(String2,0,200);  

}


void Create_Cjson_Discovery(uint8_t *IO_Channel,uint8_t *IO_State,uint8_t IO_Num,uint8_t Addr)
{
  cJSON *root;
	char *out;
	char String1[200]={0};
	
	Generate_IO_String(IO_Channel,IO_State,IO_Num,String1);
	
	root=cJSON_CreateObject();
	cJSON_AddStringToObject(root,"cmd","discovery");
	cJSON_AddStringToObject(root,"model","perfe1");
	cJSON_AddNumberToObject(root,"addr",Addr);
	cJSON_AddStringToObject(root,"io",String1);
  out=cJSON_Print(root);
	cJSON_Delete(root);
	
	if(getSn_SR(SOCK_TCPS) == SOCK_ESTABLISHED)
	{
		TCP_Send_Cnt = strlen(out);
    memcpy(TCP_Send_Buffer,(uint8_t*)out,TCP_Send_Cnt);
		TCP_Send_Flag = 1; 	
	}	
	
	myfree(out);
}




void Generate_IO_String(uint8_t *Channel,uint8_t *State,uint8_t Cnt,char *Str)
{
  uint8_t i=0;
	char *str1=NULL;
	char *str2=NULL;
	str1=(char*)malloc(sizeof(char) *10);
	str2=(char*)malloc(sizeof(char) *10);

	strcpy(Str,"{");
	
	for(i=0;i<Cnt;)
	{
		sprintf(str1,"channel_%d",*(Channel+i));
		
		if(*(State+i))
		{
			memcpy(str2,"press",8); 
		}		
		else
		{
		 memcpy(str2,"unpress",10); 
		}
		
			
	  strcat(Str,"\"");
	  strcat(Str,str1);
		strcat(Str,"\":\"");
		strcat(Str,str2);
	  strcat(Str,"\"");
		i++;
		if(i<Cnt)
		{
     strcat(Str,",");
    }
  }
    free(str1);
    free(str2);
		str1=NULL;
		str2=NULL;	
		strcat(Str,"}");
}

void Generate_Relay_String(uint8_t *State,uint8_t Cnt,char *Str)
{
  uint8_t i=0;
	char *str3=NULL;
	char *str4=NULL;
	str3=(char*)malloc(sizeof(char) *10);
	str4=(char*)malloc(sizeof(char) *10);

	strcpy(Str,"{");
	
	for(i=0;i<Cnt;)
	{
		sprintf(str3,"channel_%d",i);
		
		if(*(State+i))
		{
			memcpy(str4,"on",3); 
		}		
		else
		{
		 memcpy(str4,"off",4); 
		}
		
			
	  strcat(Str,"\"");
	  strcat(Str,str3);
		strcat(Str,"\":\"");
		strcat(Str,str4);
	  strcat(Str,"\"");
		i++;
		if(i<Cnt)
		{
     strcat(Str,",");
    }
  }
    free(str3);
    free(str4);
		str3=NULL;
		str4=NULL;	
		strcat(Str,"}");
}	



















