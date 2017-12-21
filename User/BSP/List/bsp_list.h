#ifndef __LIST_H
#define __LIST_H

#include "stm32f10x.h"

//#define FALSE      0
//#define TRUE       1

typedef struct Data
{
  uint8_t addr;
	uint8_t Offline_Cnt;
	uint8_t Type;
	uint8_t IO_Enable[6];
	uint8_t IO_Triggle[6];
	uint8_t Relay_State[4];
}Data;


typedef struct NODE {    
   struct  Data data;
   struct  NODE *Next;
}Node;


extern Node *Head;

struct NODE *NodeCreat(void);

void Insert_Node(uint8_t *Insert_Temp);
void Insert_Node_Process(uint8_t *Insert_Process_Temp);
void Insert_Report(Node *Insert_Report);

uint8_t Find_Node(uint8_t Find_Addr);
uint8_t Find_Node_Process(uint8_t Find_Node_Addr);


void Delete_Node(uint8_t address);

void Check_Node_Status(uint8_t *Check_Node_Temp);
void Check_Node_Statues_Process(uint8_t *Check_Node_Process_Temp);

Data Array_to_structure(uint8_t *Trans_Temp);


void Print_Node(void);

void Query_Node(uint8_t Query_Addr);
void Query_Node_Process(uint8_t Query_Process_Addr);


void Write_Check_Node_Relay_Status(uint8_t Write_Addr,uint8_t Write_Num,uint8_t Write_State);
void Write_Check_Node_Relay_Status_Process(uint8_t Write_Process_Addr,uint8_t Write_Process_Num,uint8_t Write_Process_State);















#endif /* __LIST_H */















