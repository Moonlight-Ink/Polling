/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                     Micrium uC-Eval-STM32F107
*                                        Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : EHS
*                 DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>
#include <string.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
OS_MUTEX List;                         //ÉùÃ÷»¥³âÐÅºÅÁ¿
OS_MUTEX Usart;
OS_MUTEX Socket;

OS_SEM  SemOfPoll;

 Node *Head;

	uint8_t Device_Exist=0;
	uint8_t Find_Device=0;

/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/

static OS_TCB AppTaskStartTCB;    //ÈÎÎñ¿ØÖÆ¿é
static OS_TCB AppTaskUSART1CheckTCB;
static OS_TCB AppTaskTCPServerTCB;



/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];       //ÈÎÎñ¶ÑÕ»
static  CPU_STK  AppTaskUSART1CheckStk[ APP_TASK_USART1_CHECK_SIZE ];
static  CPU_STK  AppTaskTCPServerStk[ APP_TASK_TCP_SERVERT_SIZE ];



/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart  (void *p_arg);               //ÈÎÎñº¯ÊýÉùÃ÷
static  void  AppTaskUSART1Check (void *p_arg);
static  void  AppTaskTCPServer (void *p_arg);

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int  main (void)
{
    OS_ERR  err;

	
    OSInit(&err);                                                           //³õÊ¼»¯ uC/OS-III

	  /* ´´½¨ÆðÊ¼ÈÎÎñ */
    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                            //ÈÎÎñ¿ØÖÆ¿éµØÖ·
                 (CPU_CHAR   *)"App Task Start",                            //ÈÎÎñÃû³Æ
                 (OS_TASK_PTR ) AppTaskStart,                               //ÈÎÎñº¯Êý
                 (void       *) 0,                                          //´«µÝ¸øÈÎÎñº¯Êý£¨ÐÎ²Îp_arg£©µÄÊµ²Î
                 (OS_PRIO     ) APP_TASK_START_PRIO,                        //ÈÎÎñµÄÓÅÏÈ¼¶
                 (CPU_STK    *)&AppTaskStartStk[0],                         //ÈÎÎñ¶ÑÕ»µÄ»ùµØÖ·
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,               //ÈÎÎñ¶ÑÕ»¿Õ¼äÊ£ÏÂ1/10Ê±ÏÞÖÆÆäÔö³¤
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,                    //ÈÎÎñ¶ÑÕ»¿Õ¼ä£¨µ¥Î»£ºsizeof(CPU_STK)£©
                 (OS_MSG_QTY  ) 5u,                                         //ÈÎÎñ¿É½ÓÊÕµÄ×î´óÏûÏ¢Êý
                 (OS_TICK     ) 0u,                                         //ÈÎÎñµÄÊ±¼äÆ¬½ÚÅÄÊý£¨0±íÄ¬ÈÏÖµOSCfg_TickRate_Hz/10£©
                 (void       *) 0,                                          //ÈÎÎñÀ©Õ¹£¨0±í²»À©Õ¹£©
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //ÈÎÎñÑ¡Ïî
                 (OS_ERR     *)&err);                                       //·µ»Ø´íÎóÀàÐÍ

    OSStart(&err);                                                          //Æô¶¯¶àÈÎÎñ¹ÜÀí£¨½»ÓÉuC/OS-III¿ØÖÆ£©

}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;


    (void)p_arg;

    BSP_Init();                                                 //°å¼¶³õÊ¼»¯
    CPU_Init();                                                 //³õÊ¼»¯ CPU ×é¼þ£¨Ê±¼ä´Á¡¢¹ØÖÐ¶ÏÊ±¼ä²âÁ¿ºÍÖ÷»úÃû£©

    cpu_clk_freq = BSP_CPU_ClkFreq();                           //»ñÈ¡ CPU ÄÚºËÊ±ÖÓÆµÂÊ£¨SysTick ¹¤×÷Ê±ÖÓ£©
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        //¸ù¾ÝÓÃ»§Éè¶¨µÄÊ±ÖÓ½ÚÅÄÆµÂÊ¼ÆËã SysTick ¶¨Ê±Æ÷µÄ¼ÆÊýÖµ
    OS_CPU_SysTickInit(cnts);                                   //µ÷ÓÃ SysTick ³õÊ¼»¯º¯Êý£¬ÉèÖÃ¶¨Ê±Æ÷¼ÆÊýÖµºÍÆô¶¯¶¨Ê±Æ÷

    Mem_Init();                                                 //³õÊ¼»¯ÄÚ´æ¹ÜÀí×é¼þ£¨¶ÑÄÚ´æ³ØºÍÄÚ´æ³Ø±í£©

#if OS_CFG_STAT_TASK_EN > 0u                                    //Èç¹ûÊ¹ÄÜ£¨Ä¬ÈÏÊ¹ÄÜ£©ÁËÍ³¼ÆÈÎÎñ
    OSStatTaskCPUUsageInit(&err);                               //¼ÆËãÃ»ÓÐÓ¦ÓÃÈÎÎñ£¨Ö»ÓÐ¿ÕÏÐÈÎÎñ£©ÔËÐÐÊ± CPU µÄ£¨×î´ó£©
#endif                                                          //ÈÝÁ¿£¨¾ö¶¨ OS_Stat_IdleCtrMax µÄÖµ£¬ÎªºóÃæ¼ÆËã CPU 
                                                                //Ê¹ÓÃÂÊÊ¹ÓÃ£©¡£
    CPU_IntDisMeasMaxCurReset();                                //¸´Î»£¨ÇåÁã£©µ±Ç°×î´ó¹ØÖÐ¶ÏÊ±¼ä

    
    /* ÅäÖÃÊ±¼äÆ¬ÂÖ×ªµ÷¶È */		
    OSSchedRoundRobinCfg((CPU_BOOLEAN   )DEF_ENABLED,          //Ê¹ÄÜÊ±¼äÆ¬ÂÖ×ªµ÷¶È
		                     (OS_TICK       )0,                    //°Ñ OSCfg_TickRate_Hz / 10 ÉèÎªÄ¬ÈÏÊ±¼äÆ¬Öµ
												 (OS_ERR       *)&err );               //·µ»Ø´íÎóÀàÐÍ

		/* ´´½¨»¥³âÐÅºÅÁ¿ mutex */
    OSMutexCreate ((OS_MUTEX  *)&List,           //Ö¸ÏòÐÅºÅÁ¿±äÁ¿µÄÖ¸Õë
                   (CPU_CHAR  *)"List", //ÐÅºÅÁ¿µÄÃû×Ö
                   (OS_ERR    *)&err);            //´íÎóÀàÐÍ
									 
				/* ´´½¨»¥³âÐÅºÅÁ¿ mutex */
    OSMutexCreate ((OS_MUTEX  *)&Usart,           //Ö¸ÏòÐÅºÅÁ¿±äÁ¿µÄÖ¸Õë
                   (CPU_CHAR  *)"Usart", //ÐÅºÅÁ¿µÄÃû×Ö
                   (OS_ERR    *)&err);            //´íÎóÀàÐÍ
									 
		/* ´´½¨»¥³âÐÅºÅÁ¿ mutex */
    OSMutexCreate ((OS_MUTEX  *)&Socket,           //Ö¸ÏòÐÅºÅÁ¿±äÁ¿µÄÖ¸Õë
                   (CPU_CHAR  *)"Socket", //ÐÅºÅÁ¿µÄÃû×Ö
                   (OS_ERR    *)&err);            //´íÎóÀàÐÍ									 
									 
	
  						 
     /*´´½¨ÂÖÑ¯²éÑ¯UsartÊý¾ÝÊÇ·ñ½ÓÊÕÍê±ÏÈÎÎñ*/		
    OSTaskCreate((OS_TCB     *)&AppTaskUSART1CheckTCB,                             //ÈÎÎñ¿ØÖÆ¿éµØÖ·
                 (CPU_CHAR   *)"App_Task_Check_Device",                             //ÈÎÎñÃû³Æ
                 (OS_TASK_PTR ) AppTaskUSART1Check,                                //ÈÎÎñº¯Êý
                 (void       *) 0,                                          //´«µÝ¸øÈÎÎñº¯Êý£¨ÐÎ²Îp_arg£©µÄÊµ²Î
                 (OS_PRIO     ) APP_TASK_USART1_CHECK_PRIO,                         //ÈÎÎñµÄÓÅÏÈ¼¶
                 (CPU_STK    *)&AppTaskUSART1CheckStk[0],                          //ÈÎÎñ¶ÑÕ»µÄ»ùµØÖ·
                 (CPU_STK_SIZE) APP_TASK_USART1_CHECK_SIZE / 10,                //ÈÎÎñ¶ÑÕ»¿Õ¼äÊ£ÏÂ1/10Ê±ÏÞÖÆÆäÔö³¤
                 (CPU_STK_SIZE) APP_TASK_USART1_CHECK_SIZE,                     //ÈÎÎñ¶ÑÕ»¿Õ¼ä£¨µ¥Î»£ºsizeof(CPU_STK)£©
                 (OS_MSG_QTY  ) 50u,                                        //ÈÎÎñ¿É½ÓÊÕµÄ×î´óÏûÏ¢Êý
                 (OS_TICK     ) 0u,                                         //ÈÎÎñµÄÊ±¼äÆ¬½ÚÅÄÊý£¨0±íÄ¬ÈÏÖµOSCfg_TickRate_Hz/10£©
                 (void       *) 0,                                          //ÈÎÎñÀ©Õ¹£¨0±í²»À©Õ¹£©
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //ÈÎÎñÑ¡Ïî
                 (OS_ERR     *)&err);                                       //·µ»Ø´íÎóÀàÐÍ


     /*´´½¨ÂÖÑ¯²éÑ¯UsartÊý¾ÝÊÇ·ñ½ÓÊÕÍê±ÏÈÎÎñ*/		
    OSTaskCreate((OS_TCB     *)&AppTaskTCPServerTCB,                             //ÈÎÎñ¿ØÖÆ¿éµØÖ·
                 (CPU_CHAR   *)"App_Task_TCP_Server",                             //ÈÎÎñÃû³Æ
                 (OS_TASK_PTR ) AppTaskTCPServer,                                //ÈÎÎñº¯Êý
                 (void       *) 0,                                          //´«µÝ¸øÈÎÎñº¯Êý£¨ÐÎ²Îp_arg£©µÄÊµ²Î
                 (OS_PRIO     ) APP_TASK_TCP_SERVER_PRIO,                         //ÈÎÎñµÄÓÅÏÈ¼¶
                 (CPU_STK    *)&AppTaskTCPServerStk[0],                          //ÈÎÎñ¶ÑÕ»µÄ»ùµØÖ·
                 (CPU_STK_SIZE) APP_TASK_TCP_SERVERT_SIZE / 10,                //ÈÎÎñ¶ÑÕ»¿Õ¼äÊ£ÏÂ1/10Ê±ÏÞÖÆÆäÔö³¤
                 (CPU_STK_SIZE) APP_TASK_TCP_SERVERT_SIZE,                     //ÈÎÎñ¶ÑÕ»¿Õ¼ä£¨µ¥Î»£ºsizeof(CPU_STK)£©
                 (OS_MSG_QTY  ) 50u,                                        //ÈÎÎñ¿É½ÓÊÕµÄ×î´óÏûÏ¢Êý
                 (OS_TICK     ) 0u,                                         //ÈÎÎñµÄÊ±¼äÆ¬½ÚÅÄÊý£¨0±íÄ¬ÈÏÖµOSCfg_TickRate_Hz/10£©
                 (void       *) 0,                                          //ÈÎÎñÀ©Õ¹£¨0±í²»À©Õ¹£©
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //ÈÎÎñÑ¡Ïî
                 (OS_ERR     *)&err);     


								 
														 
		OSTaskDel ( 0, & err );                     //É¾³ýÆðÊ¼ÈÎÎñ±¾Éí£¬¸ÃÈÎÎñ²»ÔÙÔËÐÐ
		
		
}

/*
*********************************************************************************************************
*                                 USART CHECK TASK
*********************************************************************************************************
*/
static  void  AppTaskUSART1Check( void * p_arg )
{
	OS_ERR      err;
	CPU_SR_ALLOC();
	
//	uint8_t i=0;
	uint8_t Addr=0x01;
	uint8_t Check_Temp[20]={0};
	
	(void)p_arg;

//uint8_t USART_Rx_Finsh = 0;
//uint8_t USART_Rx_Buffer[20] = {0};
//uint8_t USART_Rx_Count = 0;	
	
	
	
					 
	while (DEF_TRUE) 
		{      
			if(Addr>0x3f)
			{
			 Addr=0;
			}
      Device_Exist=Find_Node(Addr);
			
			Check_Temp[0]=0xad;
			Check_Temp[1]=0xda;
			Check_Temp[2]=0x02;
			Check_Temp[3]=Addr;
			Check_Temp[4]=0x20;
			Check_Temp[5]=0x03;
		 	
      USART1_Send_Data(Check_Temp,6);
		


			
//			for(i=0;i<6;i++)
//			{
//			 TCP_Send_Buffer[i]=(uint8_t)Check_Temp[i];			
//			}			
////			memcpy(TCP_Send_Buffer,(uint8_t *)Check_Temp,6);
//			TCP_Send_Cnt=6;
//      TCP_Send_Flag = 1;
		
			
		OS_CRITICAL_ENTER();                                       //½øÈëÁÙ½ç¶Î£¬±ÜÃâ´®¿Ú´òÓ¡±»´ò¶Ï
		
//		Find_Device=(*(Msg+3)==Addr?1:0);
//		
//		
//    if(Device_Exist)
//		{ 
//			if(Find_Device)        //Éè±¸´æÔÚÓÚÁ´±í²¢ÇÒ²éÕÒÓÐ»Ø¸´
//			{
//			//¶Ô±ÈÉè±¸µÄ×´Ì¬ÐÅÏ¢£¬¿´¿´ÊÇ·ñÓÐÐÂIOÌí¼Ó¡¢ÐÂ×´Ì¬¸üÐÂ			
//        Updata_Node(Msg);		
//			}
//			else                 //Éè±¸´æÔÚÓÚÁ´±í£¬µ«ÊÇ²éÕÒÃ»ÓÐ»Ø¸´
//			{
//			//¶ÔÉè±¸½øÐÐoffline²Ù×÷£¬É¾³ý½Úµã
//		    Delete_Node(Addr);		
//			}
//		  
//		}
//		else 
//		{
//			if(Find_Device)    //Éè±¸²»´æÔÚÓÚÁ´±í£¬µ«ÊÇ²éÕÒÓÐ»Ø¸´
//			{
//			//Õâ¸öÊÇÐÂµÄÉè±¸¼ÓÈë£¬ÐèÒª²åÈë½Úµã£¬¸üÐÂ×´Ì¬
//        Insert_Node(Msg); 	
//			}
//			else               //Éè±¸²»´æÔÚÓÚÁ´±í£¬²éÕÒÒ²Ã»ÓÐ»Ø¸´   
//			{
//			//Ã»ÓÐÕâ¸öÉè±¸£¬¼ÌÐø²éÕÒÏÂÒ»¸öÉè±¸
//			
//			}	
//		}

		macLED2_TOGGLE();	
		OS_CRITICAL_EXIT();                                        //ÍË³öÁÙ½ç¶Î							
	  }
}


//static  void  AppTaskList(void *p_arg)
//{
//	OS_ERR      err;
//  uint32_t *List_Msg=0;
//	
//  OS_MSG_SIZE List_Msg_Size;
//	
//	CPU_SR_ALLOC();
//	
//	(void)p_arg;

//					 
//	while (DEF_TRUE)
//	{
///* ×èÈûÈÎÎñ£¬µÈ´ýÈÎÎñÏûÏ¢ */
//		 List_Msg = OSTaskQPend ((OS_TICK        )0,                    //ÎÞÆÚÏÞµÈ´ý
//											       (OS_OPT         )OS_OPT_PEND_BLOCKING, //Ã»ÓÐÏûÏ¢¾Í×èÈûÈÎÎñ
//											       (OS_MSG_SIZE   *)&List_Msg_Size,            //·µ»ØÏûÏ¢³¤¶È
//											       (CPU_TS        *)0,                    //·µ»ØÏûÏ¢±»·¢²¼µÄÊ±¼ä´Á
//											       (OS_ERR        *)&err);                //·µ»Ø´íÎóÀ

//		OS_CRITICAL_ENTER();      		
//	  OSMutexPend ((OS_MUTEX  *)&List,                  //ÉêÇë»¥³âÐÅºÅÁ¿ mutex
//								 (OS_TICK    )0,                       //ÎÞÆÚÏÞµÈ´ý
//								 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //Èç¹ûÉêÇë²»µ½¾Í¶ÂÈûÈÎÎñ
//								 (CPU_TS    *)0,                       //²»Ïë»ñµÃÊ±¼ä´Á
//								 (OS_ERR    *)&err);                   //·µ»Ø´íÎóÀàÐÍ		
//	
//	  USART1_Send_Data(List_Msg,(u16)List_Msg_Size);
//		macLED1_TOGGLE();	
//		OSMutexPost ((OS_MUTEX  *)&List,                 //ÊÍ·Å»¥³âÐÅºÅÁ¿ mutex
//								 (OS_OPT     )OS_OPT_POST_NONE,       //½øÐÐÈÎÎñµ÷¶È
//								 (OS_ERR    *)&err);                  //·µ»Ø´íÎóÀàÐÍ	

//		
//		
//		
//		OSMemPut ((OS_MEM  *)&Mem,                                 //Ö¸ÏòÄÚ´æ¹ÜÀí¶ÔÏó
//							(void    *)List_Msg,                                 //ÄÚ´æ¿éµÄÊ×µØÖ·
//							(OS_ERR  *)&err);		                             //·µ»Ø´íÎóÀàÐÍ	
//		
//    OS_CRITICAL_EXIT();                                        //ÍË³öÁÙ½ç¶Î							
//		
//	}
//}

static  void  AppTaskTCPServer (void *p_arg)
{
	
	OS_ERR      err;
	(void)p_arg;
		
	Load_Net_Parameters();		//×°ÔØÍøÂç²ÎÊý	
	W5500_Hardware_Reset();		//Ó²¼þ¸´Î»W5500	
	W5500_Initialization();		//W5500³õÊ¼ÅäÖÃ
	
	
	while (DEF_TRUE)
	{
		W5500_Socket_Set();            //W5500¶Ë¿Ú³õÊ¼»¯ÅäÖÃ		
		
		if(W5500_Interrupt)            //Èç¹ûW5500_Interrupt±êÖ¾Î»ÖÃÎ»£¬±íÊ¾ÖÐ¶Ï·¢Éú£¬´¦ÀíW5500ÖÐ¶Ï		
		{
			W5500_Interrupt_Process(); //W5500ÖÐ¶Ï´¦Àí³ÌÐò
		}
		if((S0_Data & S_RECEIVE) == S_RECEIVE)//Èç¹ûSocket0½ÓÊÕµ½Êý¾Ý
		{
			S0_Data&=~S_RECEIVE;              //Çå³þ½ÓÊÕÊý¾Ý±êÖ¾Î»
			Process_Socket_Data(0);           //W5500´¦Àí½ÓÊÕÊý¾ÝµÄº¯Êý		
		}
//		else if(time >= 500)                  //¶¨Ê±·¢ËÍ×Ö·û´®
//		{
//			time=0;                           //Çå³ýtime                     
//			if(S0_State == (S_INIT|S_CONN))   //Èç¹û¶Ë¿Ú³õÊ¼»¯³É¹¦£¬ÇÒÁ¬½Ó³É¹¦
//			{
//				S0_Data&=~S_TRANSMITOK;	                                             //Çå³ý·¢ËÍ±êÖ¾	
//				Write_SOCK_Data_Buffer(0, TxPtr, strlen((const char*)TxPtr));  //¶Ë¿Ú0·¢ËÍÊý¾Ý
//			}			                      
//		}
		OSTimeDlyHMSM ( 0, 0, 0,10, OS_OPT_TIME_DLY, &err);	
	}


}	










