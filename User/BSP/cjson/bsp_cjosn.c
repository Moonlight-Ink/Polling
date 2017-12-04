#include "bsp_cjson.h"
#include "malloc.h"
#include "cJSON.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
	
  /*发送函数rs485*/	
	printf("%s\n",out);
	
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
	
  /*发送函数rs485*/		
	printf("%s\n",out);
	
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
	cJSON_AddStringToObject(root,"cmd","report");
	cJSON_AddStringToObject(root,"model","perfe1");
	cJSON_AddNumberToObject(root,"addr",Addr);
//	cJSON_AddStringToObject(root,"type","io");
	cJSON_AddStringToObject(root,"io",String1);
	cJSON_AddStringToObject(root,"relay",String2);
  out=cJSON_Print(root);
	cJSON_Delete(root);
	
  /*发送函数rs485*/		
	printf("%s\n",out);
	
	myfree(out);	
	
//	memset(String1,0,200);
//	memset(String2,0,200);  

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



















