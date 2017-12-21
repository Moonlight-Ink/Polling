#include "bsp_cjson.h"
#include "malloc.h"
#include "cJSON.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "w5500.h"
#include "bsp_list.h"

#include "bsp_usart1.h"
#include <includes.h>


uint8_t tst[8]={0xad,0xda,0x04,0x01,0x60,0x01,0x03,0x0c};

uint8_t Cmd_Control_Addr = 0;
uint8_t Cmd_Control_Response = 0;
uint8_t Query_Address = 0;

void TCP_Cmd_Cjson_Analyze(uint8_t *TCP_Cmd,uint16_t TCP_Cmd_Cnt)
{
	cJSON *root = NULL,*out = NULL;
  
	
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
      Check_Device_Cmd_Buffer(out->valueint);	
			Query_Node(out->valueint);
		}
		else if(!strcmp(out->valuestring,"write"))
		{
			out=cJSON_GetObjectItem(root,"addr");
			if(Find_Node(cJSON_GetObjectItem(root,"addr")->valueint))
			{
//			  Cmd_Control_Addr = cJSON_GetObjectItem(root,"addr")->valueint;
			  Cmd_Write_Device(cJSON_GetObjectItem(root,"addr")->valueint,
			                   cJSON_GetObjectItem(root,"relay")->valuestring);			  
			}
		}
		else if(!strcmp(out->valuestring,"heartbeat"))
		{
			out=cJSON_GetObjectItem(root,"addr"); 
		  Creat_Cjson_Heartbeat(out->valueint);
		}
	}

	 cJSON_Delete(root);  
}


void Cmd_Write_Device(uint8_t Write_Addr,char *Write_String)
{
	uint8_t Relay_Num = 0;
	uint8_t Relay_State = 0;
	char Write_temp[20]="";
  cJSON *Write_Item = NULL,*Write_Out1 = NULL;
  
	Write_Item = cJSON_Parse(Write_String);
	
	if(!Write_Item)
	{
		printf("Error before: [%s]\n",cJSON_GetErrorPtr());	
	}
	else
	{
	  for(Relay_Num=0;Relay_Num<4;Relay_Num++)
		{
		  sprintf(Write_temp,"channel_%d",Relay_Num);		  
			Write_Out1=cJSON_GetObjectItem(Write_Item,Write_temp);
			if(!strcmp(Write_Out1->string,Write_temp))
			{
//				if(Control_Out1)
//				{
					if(!strcmp(Write_Out1->valuestring,"on"))
					{
						Relay_State = 0x02;
					}
					else if(!strcmp(Write_Out1->valuestring,"off")) 
					{
						Relay_State = 0x01;
					}
										
					if(Relay_State ==0x02 || Relay_State ==0x01)
					{
            TCP_Cmd_Control_Device(Write_Addr,Relay_Num,Relay_State);
            Write_Check_Node_Relay_Status_Process(Write_Addr,Relay_Num,Relay_State);						
					}				
//				}
		  }		  
		}
	}
	 cJSON_Delete(Write_Item);  
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

//     if(!Tx_Buffer_Size)
//		 {
		  Tx_Buffer_Size = strlen(out);
			memcpy(Tx_Buffer,(uint8_t*)out,Tx_Buffer_Size);
			Process_Socket_Send_Data(0);
//		 }
		 
//		 if(getSn_SR(SOCK_TCPS) == SOCK_ESTABLISHED)
//		 {
//			 TCP_Send_Cnt = strlen(out);
//    	 memcpy(TCP_Send_Buffer,(uint8_t*)out,TCP_Send_Cnt);
//			 TCP_Send_Flag = 1; 	
//		 }		 	 
		 	myfree(out);		 
   }
}


void Reort_Cjson(uint8_t *Reort_IO_Channel,uint8_t *Reprot_IO_State,uint8_t Reort_IO_Num,uint8_t *Report_Relay_State,uint8_t Reprt_Relay_Num,uint8_t Repor_Addr)
{
  cJSON *Report_Root;
	char *Report_Out;
	char String1[200]={0};
	char String2[200]={0};
	
	Generate_Multiple_IO_String(Reort_IO_Channel,Reprot_IO_State,Reort_IO_Num,String1);
  Generate_Multiple_Relay_String(Report_Relay_State,Reprt_Relay_Num,String2);
	
	Report_Root = cJSON_CreateObject();
	cJSON_AddStringToObject(Report_Root,"cmd","report");
	cJSON_AddStringToObject(Report_Root,"model","perfe1");
	cJSON_AddNumberToObject(Report_Root,"addr",Repor_Addr);
	cJSON_AddStringToObject(Report_Root,"io",String1);
	cJSON_AddStringToObject(Report_Root,"relay",String2);
  Report_Out = cJSON_Print(Report_Root);
	cJSON_Delete(Report_Root);
	
	Tx_Buffer_Size = strlen(Report_Out);
	memcpy(Tx_Buffer,(uint8_t*)Report_Out,Tx_Buffer_Size);
	Process_Socket_Send_Data(0);

	myfree(Report_Out);		
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

//	if(!Tx_Buffer_Size)
//	{
		Tx_Buffer_Size = strlen(out);
	  memcpy(Tx_Buffer,(uint8_t*)out,Tx_Buffer_Size);
	  Process_Socket_Send_Data(0);
//	}

	
//	if(getSn_SR(SOCK_TCPS) == SOCK_ESTABLISHED)
//	{
//		TCP_Send_Cnt = strlen(out);
//    memcpy(TCP_Send_Buffer,(uint8_t*)out,TCP_Send_Cnt);
//		TCP_Send_Flag = 1; 	
//	}	
	
	myfree(out);	

}	

//void Join_Cjson(Insert->data.addr);
void Join_Cjson(uint8_t Join_Addr)
{
  cJSON *Join_Root;
	
	char *Join_Out;
	
	Join_Root=cJSON_CreateObject();
	cJSON_AddStringToObject(Join_Root,"cmd","join");
	cJSON_AddStringToObject(Join_Root,"model","perfe1");
	cJSON_AddNumberToObject(Join_Root,"addr",Join_Addr);
  Join_Out = cJSON_Print(Join_Root);
	
	cJSON_Delete(Join_Root);
	
	Tx_Buffer_Size = strlen(Join_Out);
	memcpy(Tx_Buffer,(uint8_t*)Join_Out,Tx_Buffer_Size);
	Process_Socket_Send_Data(0);
	
	myfree(Join_Out);		
}

void Discovery_Cjson(uint8_t Disc_Channel_Num,uint8_t Disc_Channel_Status,uint8_t Disc_Addr)
{
  cJSON *Disc_Root;
	char *Disc_Out;
	char Disc_String[200] = {0};
	
	Generate_Single_IO_String(Disc_Channel_Num,Disc_Channel_Status,Disc_String);
	
	Disc_Root=cJSON_CreateObject();
	cJSON_AddStringToObject(Disc_Root,"cmd","discovery");
	cJSON_AddStringToObject(Disc_Root,"model","perfe1");
	cJSON_AddNumberToObject(Disc_Root,"addr",Disc_Addr);
	cJSON_AddStringToObject(Disc_Root,"io",Disc_String);
  Disc_Out=cJSON_Print(Disc_Root);
	
	cJSON_Delete(Disc_Root);

	Tx_Buffer_Size = strlen(Disc_Out);
	memcpy(Tx_Buffer,(uint8_t*)Disc_Out,Tx_Buffer_Size);
	Process_Socket_Send_Data(0);
	
	myfree(Disc_Out);
}


void Leave_Cjson(uint8_t Leave_Channel_Num,uint8_t Leave_Channel_Status,uint8_t Leave_Addr)  //ÄÄ¸öchannel,¸ÃchannelµÄ×´Ì¬		
{
  cJSON *Leave_Root;
	char *Leave_Out;
	char Leave_String[200] = {0};
	
  Generate_Single_IO_String(Leave_Channel_Num,Leave_Channel_Status,Leave_String);
	
	Leave_Root=cJSON_CreateObject();
	cJSON_AddStringToObject(Leave_Root,"cmd","leave");
	cJSON_AddStringToObject(Leave_Root,"model","perfe1");
	cJSON_AddNumberToObject(Leave_Root,"addr",Leave_Addr);
	cJSON_AddStringToObject(Leave_Root,"io",Leave_String);
  Leave_Out=cJSON_Print(Leave_Root);
	
	cJSON_Delete(Leave_Root);

	Tx_Buffer_Size = strlen(Leave_Out);
	memcpy(Tx_Buffer,(uint8_t*)Leave_Out,Tx_Buffer_Size);
	Process_Socket_Send_Data(0);
	
	myfree(Leave_Out);	
}


void Updata_Cjson(uint8_t Updata_Channel_Num,uint8_t Updata_Type,uint8_t Updata_Channel_Status,uint8_t Updata_Addr)
{
  cJSON *Updata_Root;
	char *Updata_Out;
	char Updata_String[200] = {0};
  char Updata_Type_String[7] = {0};
	
	
	if(Updata_Type == 0x01)
	{	
		memcpy(Updata_Type_String,"io",4);
    Generate_Single_IO_String(Updata_Channel_Num,Updata_Channel_Status,Updata_String);		
	}
	else if(Updata_Type == 0x02)
	{
		memcpy(Updata_Type_String,"relay",7);
    Generate_Single_Relay_String(Updata_Channel_Num,Updata_Channel_Status,Updata_String);				
	}
	
	Updata_Root=cJSON_CreateObject();
	cJSON_AddStringToObject(Updata_Root,"cmd","updata");
	cJSON_AddStringToObject(Updata_Root,"model","perfe1");
	cJSON_AddNumberToObject(Updata_Root,"addr",Updata_Addr);
	cJSON_AddStringToObject(Updata_Root,"type",Updata_Type_String);	
	cJSON_AddStringToObject(Updata_Root,Updata_Type_String,Updata_String);		
  Updata_Out=cJSON_Print(Updata_Root);
	
	cJSON_Delete(Updata_Root);

	Tx_Buffer_Size = strlen(Updata_Out);
	memcpy(Tx_Buffer,(uint8_t*)Updata_Out,Tx_Buffer_Size);
	Process_Socket_Send_Data(0);
	
	myfree(Updata_Out);	
}

void Generate_Single_IO_String(uint8_t IO_Channel_Num,uint8_t IO_State,char *IO_Str)
{
	char *str1=NULL;
	char *str2=NULL;
	str1=(char*)malloc(sizeof(char) *10);
	str2=(char*)malloc(sizeof(char) *10);

	sprintf(str1,"channel_%d",IO_Channel_Num);
		
	if(IO_State)
	{
		memcpy(str2,"press",8); 
	}		
	else
	{
		memcpy(str2,"unpress",10); 
	}
	strcpy(IO_Str,"{");				
	strcat(IO_Str,"\"");
	strcat(IO_Str,str1);
	strcat(IO_Str,"\":\"");
	strcat(IO_Str,str2);
	strcat(IO_Str,"\"");
		
  free(str1);
  free(str2);
	str1=NULL;
	str2=NULL;	
	strcat(IO_Str,"}");
}

void Generate_Single_Relay_String(uint8_t Relay_Channel_Num,uint8_t Relay_State,char *Relay_Str)
{
	char *str3=NULL;
	char *str4=NULL;
	str3=(char*)malloc(sizeof(char) *10);
	str4=(char*)malloc(sizeof(char) *10);

	sprintf(str3,"channel_%d",Relay_Channel_Num);
		
	if(Relay_State)
	{
			memcpy(str4,"on",3); 
	}		
	else
	{
		 memcpy(str4,"off",4); 
	}
		
		strcpy(Relay_Str,"{");		
	  strcat(Relay_Str,"\"");
	  strcat(Relay_Str,str3);
		strcat(Relay_Str,"\":\"");
		strcat(Relay_Str,str4);
	  strcat(Relay_Str,"\"");

    free(str3);
    free(str4);
		str3=NULL;
		str4=NULL;	
		strcat(Relay_Str,"}");
}	

//Multiple

void Generate_Multiple_IO_String(uint8_t *Channel,uint8_t *State,uint8_t Cnt,char *Str)
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

void Generate_Multiple_Relay_String(uint8_t *State,uint8_t Cnt,char *Str)
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

