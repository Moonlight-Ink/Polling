#ifndef __BSP_CJSON_H
#define __BSP_CJSON_H


#include "stm32f10x.h"




void Creat_Cjson_Offline(uint8_t Addr);
void Creat_Cjson_Report(uint8_t *IO_Channel,uint8_t *IO_State,uint8_t IO_Num,uint8_t *Relay_State,uint8_t Relay_Num,uint8_t Addr);
void Creat_Cjson_Join(uint8_t *IO_Channel,uint8_t *IO_State,uint8_t IO_Num,uint8_t *Relay_State,uint8_t Relay_Num,uint8_t Addr);
void Generate_IO_String(uint8_t *Channel,uint8_t *State,uint8_t Cnt,char *Str);
void Generate_Relay_String(uint8_t *State,uint8_t Cnt,char *Str);
void Create_Cjson_Discovery(uint8_t *IO_Channel,uint8_t *IO_State,uint8_t IO_Num,uint8_t Addr);
void TCP_Cmd_Cjson_Analyze(uint8_t *TCP_Cmd,uint16_t TCP_Cmd_Cnt);
void Creat_Cjson_Heartbeat(uint8_t Addr);
void Cmd_Ctronl(uint8_t Addr,char *Ctrl);




#endif

