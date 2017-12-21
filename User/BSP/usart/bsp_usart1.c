/**
  ******************************************************************************
  * @file    bsp_usart1.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   usart应用bsp
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "bsp_usart1.h"
#include "bsp_crc8.h"
#include <includes.h>

extern OS_MUTEX Usart;

uint8_t USART_Rx_Finsh = 0;
uint8_t USART_Rx_Buffer[20] = {0};
uint8_t USART_Rx_Count = 0;

 /**
  * @brief  USART1 GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */
void USARTx_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* config USART1 clock */
	macUSART_APBxClock_FUN(macUSART_CLK, ENABLE);
	macUSART_GPIO_APBxClock_FUN(macUSART_GPIO_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	/* USART1 GPIO config */
	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin =  macUSART_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(macUSART_TX_PORT, &GPIO_InitStructure);    
	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = macUSART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(macUSART_RX_PORT, &GPIO_InitStructure);
	
		  /* 485收发控制管脚 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);	//进入接收模式
	
	
	
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = macUSART_BAUD_RATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(macUSARTx, &USART_InitStructure);
	


  
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* 配置中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = macUSART_IRQ ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	
		/* 使能串口1接收中断 */
	//NVIC_Configuration();
	USART_ITConfig(macUSARTx, USART_IT_RXNE, ENABLE);
	
	USART_Cmd(macUSARTx, ENABLE);
}

void Check_Device_Cmd_Buffer(uint8_t Check_Addr)
{
	uint8_t Check_Buffer[6];
	
  Check_Buffer[0] = 0xad;
	Check_Buffer[1] = 0xda;
	Check_Buffer[2] = 0x02;
	Check_Buffer[3] = Check_Addr;
	Check_Buffer[4] = 0x20;
	Check_Buffer[5] = 0x03;
		
  USART1_Send_Data(Check_Buffer,6);		
}

void TCP_Cmd_Control_Device(uint8_t Control_Addr,uint8_t Control_Relay_Num,uint8_t Control_Relay_State)
{
	uint8_t Write_Buffer[8] = {0};
	uint8_t Write_CRC_Buffer[7]= {0};
	
	Write_Buffer[0]=0xad;
	Write_CRC_Buffer[0]=Write_Buffer[0];
						
	Write_Buffer[1]=0xda;
	Write_CRC_Buffer[1]=Write_Buffer[1];
						
	Write_Buffer[2]=0x04;
	Write_CRC_Buffer[2]=Write_Buffer[2];  
						
	Write_Buffer[3]=Control_Addr;
	Write_CRC_Buffer[3]=Write_Buffer[3];
						
	Write_Buffer[4]=0x60;
	Write_CRC_Buffer[4]=Write_Buffer[4];
						
	Write_Buffer[5]=Control_Relay_Num+1;
	Write_CRC_Buffer[5]=Write_Buffer[5];
											
	Write_Buffer[6]=Control_Relay_State;
	Write_CRC_Buffer[6]=Write_Buffer[5];
						
	Write_Buffer[7]=CRC8_Check(7,Write_CRC_Buffer);	

	USART1_Send_Data(Write_Buffer,8); 				
}

void USART1_Send_Data(volatile u8 *buf,u8 len)
{
  uint8_t i=0;
	OS_ERR      err;	
	
	OSMutexPend ((OS_MUTEX  *)&Usart,                  //申请互斥信号量 mutex
							 (OS_TICK    )0,                       //无期限等待
							 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //如果申请不到就堵塞任务
							 (CPU_TS    *)0,                       //不想获得时间戳
							 (OS_ERR    *)&err);                   //返回错误类型			
	
  GPIO_SetBits(GPIOB,GPIO_Pin_5); //进入发送模式	
	
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);		
	for(i=0;i<len;i++)
	{
   USART_SendData(USART1,buf[i]);
		
	 while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	
  }
  GPIO_ResetBits(GPIOB,GPIO_Pin_5);	//进入接收模式	
	OSTimeDlyHMSM ( 0, 0, 0,20, OS_OPT_TIME_DLY, &err);
	OSMutexPost ((OS_MUTEX  *)&Usart,                 //释放互斥信号量 mutex
							 (OS_OPT     )OS_OPT_POST_NONE,       //进行任务调度
							 (OS_ERR    *)&err);                  //返回错误类型		
}


/// 重定向c库函数printf到USART1
int fputc(int ch, FILE *f)
{
	  GPIO_SetBits(GPIOB,GPIO_Pin_5); //进入发送模式	
		while (USART_GetFlagStatus(macUSARTx, USART_FLAG_TXE) == RESET);		
		/* 发送一个字节数据到USART1 */
		USART_SendData(macUSARTx, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(macUSARTx, USART_FLAG_TXE) == RESET);		
  GPIO_ResetBits(GPIOB,GPIO_Pin_5);	//进入接收模式			
		return (ch);
}

/// 重定向c库函数scanf到USART1
int fgetc(FILE *f)
{
		/* 等待串口1输入数据 */
		while (USART_GetFlagStatus(macUSARTx, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(macUSARTx);
}
/*********************************************END OF FILE**********************/
