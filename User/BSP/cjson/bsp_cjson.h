#ifndef __BSP_CJSON_H
#define __BSP_CJSON_H


#include "stm32f10x.h"

extern uint8_t Query_Address;


void Creat_Cjson_Offline(uint8_t Addr);

void Reort_Cjson(uint8_t *Reort_IO_Channel,uint8_t *Reprot_IO_State,uint8_t Reort_IO_Num,uint8_t *Report_Relay_State,uint8_t Reprt_Relay_Num,uint8_t Repor_Addr);

void Join_Cjson(uint8_t Join_Addr);

void Discovery_Cjson(uint8_t Disc_Channel_Num,uint8_t Disc_Channel_Status,uint8_t Disc_Addr);

void Leave_Cjson(uint8_t Leave_Channel_Num,uint8_t Leave_Channel_Status,uint8_t Leave_Addr);

void Updata_Cjson(uint8_t Updata_Channel_Num,uint8_t Updata_Type,uint8_t Updata_Channel_Status,uint8_t Updata_Addr);

void TCP_Cmd_Cjson_Analyze(uint8_t *TCP_Cmd,uint16_t TCP_Cmd_Cnt);

void Creat_Cjson_Heartbeat(uint8_t Addr);

void Cmd_Write_Device(uint8_t Write_Addr,char *Write_String);

void Generate_Single_IO_String(uint8_t IO_Channel_Num,uint8_t IO_State,char *IO_Str);

void Generate_Single_Relay_String(uint8_t Relay_Channel_Num,uint8_t Relay_State,char *Relay_Str);

void Generate_Multiple_IO_String(uint8_t *Channel,uint8_t *State,uint8_t Cnt,char *Str);

void Generate_Multiple_Relay_String(uint8_t *State,uint8_t Cnt,char *Str);



#endif

