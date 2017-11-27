/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTI
  
  AL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include <includes.h>
#include <string.h>
#include "bsp_usart1.h"





extern  OS_MEM    Mem;
uint32_t *Mem_blk;
u8 Re_Temp[20]={0};	
u8 Re_Cnt=0;		
extern  OS_TCB   AppTaskUsartPendTCB;


/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/


/**
  * @brief  USART 中断服务函数
  * @param  无
  * @retval 无
  */

void USART1_IRQHandler(void)
{
	OS_ERR err;
	
  uint8_t  ch;
	uint8_t i=0;
	
	OSIntEnter(); 	                                     //进入中断
		
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET )
	{
		ch = USART_ReceiveData(USART1);     //获取接收到的数据
		Re_Temp[Re_Cnt++]=ch;
		
		if(Re_Temp[0]==0xad)
		{
			if(Re_Cnt == (Re_Temp[2]+4))
			{
				Mem_blk = OSMemGet((OS_MEM *)&Mem,
													 (OS_ERR *)&err);
				
				for(i=0;i<Re_Cnt;i++)
				{
				*(Mem_blk+i)=*(Re_Temp+i);				
				}
				
				/* 发布任务消息到任务 AppTaskUsart */
			  OSTaskQPost ((OS_TCB      *)&AppTaskUsartPendTCB,      //目标任务的控制块
									   (void        *)Mem_blk,             //消息内容的首地址
										 (OS_MSG_SIZE  )Re_Cnt,                     //消息长度
										 (OS_OPT       )OS_OPT_POST_FIFO,      //发布到任务消息队列的入口端
										 (OS_ERR      *)&err);                 //返回错误类型
										 
				memset(Re_Temp,0,20);
				Re_Cnt=0;			
			}
			else if(Re_Cnt > (Re_Temp[2]+4))
			{
			 memset(Re_Temp,0,20);
			 Re_Cnt=0;								
			}		 		
		}
 		else if(Re_Temp[0]!=0xad)
		{
		 memset(Re_Temp,0,20);
		 Re_Cnt=0;	
		}				 
	}
	
	OSIntExit();	                                       //退出中断	
}




/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
