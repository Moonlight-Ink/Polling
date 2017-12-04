#ifndef __BSP_CJSON_H
#define __BSP_CJSON_H


#include "stm32f10x.h"






void Creat_Cjson_Offline(uint8_t Addr);
void Creat_Cjson_Report(char *IO_Channel,char *IO_State,uint8_t IO_Num,char *Relay_State,uint8_t Relay_Num);
void Creat_Cjson_Join(char *IO_Channel,char *IO_State,uint8_t IO_Num,char *Relay_State,uint8_t Relay_Num);











#endif

