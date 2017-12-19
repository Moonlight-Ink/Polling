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

void Insert_Node(uint32_t *Insert_Temp);
uint8_t Find_Node(uint8_t address);
void Delete_Node(uint8_t address);

void Updata_Node(uint32_t *Temp);

Data Array_to_structure(uint32_t *Trans_Temp);


void Print_Node(void);

void Query_Node(uint8_t Addr);






















#endif /* __LIST_H */















