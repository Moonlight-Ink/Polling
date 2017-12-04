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
OS_SEM  SemOfPoll;
OS_MEM  Mem;                    //ÉùÃ÷ÄÚ´æ¹ÜÀí¶ÔÏó

uint8_t Array [ 3 ] [ 100 ];   //ÉùÃ÷ÄÚ´æ·ÖÇø´óÐ¡
uint8_t Rx_Cnt=0;
uint8_t Rx_Temp[20]={0};
Node *Head;

/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/

static  OS_TCB   AppTaskStartTCB;    //ÈÎÎñ¿ØÖÆ¿é

       OS_TCB AppTaskCheckDeviceTCB;
static OS_TCB AppTaskListTCB;
//  OS_TCB   AppTaskUsartPendTCB;
//static  OS_TCB   AppTaskPollTCB;
//static  OS_TCB   AppTaskPollPendTCB;

//OS_TCB   AppTaskPostTCB;
//OS_TCB   AppTaskPendTCB;

/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];       //ÈÎÎñ¶ÑÕ»

static  CPU_STK  AppTaskCheckDeviceStk[ APP_TASK_CHECK_DEVICE_SIZE ];
static  CPU_STK  AppTaskListStk[ APP_TASK_LIST_SIZE ];
//static  CPU_STK  AppTaskUsartPendStk[ APP_TASK_USART_PEND_SIZE ];
//static  CPU_STK  AppTaskPollStk[ APP_TASK_POLL_SIZE ];
//static  CPU_STK  AppTaskPollPendStk[ APP_TASK_POLL_PEND_SIZE ];


//static  CPU_STK  AppTaskPostStk [ APP_TASK_POST_STK_SIZE ];
//static  CPU_STK  AppTaskPendStk [ APP_TASK_PEND_STK_SIZE ];


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart  (void *p_arg);               //ÈÎÎñº¯ÊýÉùÃ÷

static  void  AppTaskCheckDevice (void *p_arg);
static  void  AppTaskList(void *p_arg);
//static  void  AppTaskUsartPend (void *p_arg);
//static  void  AppTaskPoll (void *p_arg);
//static  void  AppTaskPollPend (void *p_arg);
//static  void  AppTaskPost   ( void * p_arg );
//static  void  AppTaskPend   ( void * p_arg );


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
									 
									 
		/* ´´½¨ÄÚ´æ¹ÜÀí¶ÔÏó mem */
		OSMemCreate ((OS_MEM      *)&Mem,             //Ö¸ÏòÄÚ´æ¹ÜÀí¶ÔÏó
								 (CPU_CHAR    *)"Mem For Test",   //ÃüÃûÄÚ´æ¹ÜÀí¶ÔÏó
								 (void        *)Array,          //ÄÚ´æ·ÖÇøµÄÊ×µØÖ·
								 (OS_MEM_QTY   )3,               //ÄÚ´æ·ÖÇøÖÐÄÚ´æ¿éÊýÄ¿
								 (OS_MEM_SIZE  )100,                //ÄÚ´æ¿éµÄ×Ö½ÚÊýÄ¿
								 (OS_ERR      *)&err);            //·µ»Ø´íÎóÀàÐÍ
		 
//		/* ´´½¨¶àÖµÐÅºÅÁ¿ SemOfKey */
//    OSSemCreate((OS_SEM      *)&SemOfPoll,    //Ö¸ÏòÐÅºÅÁ¿±äÁ¿µÄÖ¸Õë
//               (CPU_CHAR    *)"SemOfPoll",    //ÐÅºÅÁ¿µÄÃû×Ö
//               (OS_SEM_CTR   )0,             //ÐÅºÅÁ¿ÕâÀïÊÇÖ¸Ê¾ÊÂ¼þ·¢Éú£¬ËùÒÔ¸³ÖµÎª0£¬±íÊ¾ÊÂ¼þ»¹Ã»ÓÐ·¢Éú
//               (OS_ERR      *)&err);         //´íÎóÀàÐÍ								 
								 
								 
//			/* ´´½¨ AppTaskPost ÈÎÎñ */
//    OSTaskCreate((OS_TCB     *)&AppTaskPostTCB,                             //ÈÎÎñ¿ØÖÆ¿éµØÖ·
//                 (CPU_CHAR   *)"App Task Post",                             //ÈÎÎñÃû³Æ
//                 (OS_TASK_PTR ) AppTaskPost,                                //ÈÎÎñº¯Êý
//                 (void       *) 0,                                          //´«µÝ¸øÈÎÎñº¯Êý£¨ÐÎ²Îp_arg£©µÄÊµ²Î
//                 (OS_PRIO     ) APP_TASK_POST_PRIO,                         //ÈÎÎñµÄÓÅÏÈ¼¶
//                 (CPU_STK    *)&AppTaskPostStk[0],                          //ÈÎÎñ¶ÑÕ»µÄ»ùµØÖ·
//                 (CPU_STK_SIZE) APP_TASK_POST_STK_SIZE / 10,                //ÈÎÎñ¶ÑÕ»¿Õ¼äÊ£ÏÂ1/10Ê±ÏÞÖÆÆäÔö³¤
//                 (CPU_STK_SIZE) APP_TASK_POST_STK_SIZE,                     //ÈÎÎñ¶ÑÕ»¿Õ¼ä£¨µ¥Î»£ºsizeof(CPU_STK)£©
//                 (OS_MSG_QTY  ) 5u,                                         //ÈÎÎñ¿É½ÓÊÕµÄ×î´óÏûÏ¢Êý
//                 (OS_TICK     ) 0u,                                         //ÈÎÎñµÄÊ±¼äÆ¬½ÚÅÄÊý£¨0±íÄ¬ÈÏÖµOSCfg_TickRate_Hz/10£©
//                 (void       *) 0,                                          //ÈÎÎñÀ©Õ¹£¨0±í²»À©Õ¹£©
//                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //ÈÎÎñÑ¡Ïî
//                 (OS_ERR     *)&err);                                       //·µ»Ø´íÎóÀàÐÍ

  						 
     /*´´½¨ÂÖÑ¯²éÑ¯UsartÊý¾ÝÊÇ·ñ½ÓÊÕÍê±ÏÈÎÎñ*/		
    OSTaskCreate((OS_TCB     *)&AppTaskCheckDeviceTCB,                             //ÈÎÎñ¿ØÖÆ¿éµØÖ·
                 (CPU_CHAR   *)"App_Task_Check_Device",                             //ÈÎÎñÃû³Æ
                 (OS_TASK_PTR ) AppTaskCheckDevice,                                //ÈÎÎñº¯Êý
                 (void       *) 0,                                          //´«µÝ¸øÈÎÎñº¯Êý£¨ÐÎ²Îp_arg£©µÄÊµ²Î
                 (OS_PRIO     ) APP_TASK_CHECK_DEVICE_PRIO,                         //ÈÎÎñµÄÓÅÏÈ¼¶
                 (CPU_STK    *)&AppTaskCheckDeviceStk[0],                          //ÈÎÎñ¶ÑÕ»µÄ»ùµØÖ·
                 (CPU_STK_SIZE) APP_TASK_CHECK_DEVICE_SIZE / 10,                //ÈÎÎñ¶ÑÕ»¿Õ¼äÊ£ÏÂ1/10Ê±ÏÞÖÆÆäÔö³¤
                 (CPU_STK_SIZE) APP_TASK_CHECK_DEVICE_SIZE,                     //ÈÎÎñ¶ÑÕ»¿Õ¼ä£¨µ¥Î»£ºsizeof(CPU_STK)£©
                 (OS_MSG_QTY  ) 50u,                                        //ÈÎÎñ¿É½ÓÊÕµÄ×î´óÏûÏ¢Êý
                 (OS_TICK     ) 0u,                                         //ÈÎÎñµÄÊ±¼äÆ¬½ÚÅÄÊý£¨0±íÄ¬ÈÏÖµOSCfg_TickRate_Hz/10£©
                 (void       *) 0,                                          //ÈÎÎñÀ©Õ¹£¨0±í²»À©Õ¹£©
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //ÈÎÎñÑ¡Ïî
                 (OS_ERR     *)&err);                                       //·µ»Ø´íÎóÀàÐÍ


		/* ´´½¨ AppTaskPend ÈÎÎñ */
    OSTaskCreate((OS_TCB     *)&AppTaskListTCB,                             //ÈÎÎñ¿ØÖÆ¿éµØÖ·
                 (CPU_CHAR   *)"App Task Pend",                             //ÈÎÎñÃû³Æ
                 (OS_TASK_PTR ) AppTaskList,                                //ÈÎÎñº¯Êý
                 (void       *) 0,                                          //´«µÝ¸øÈÎÎñº¯Êý£¨ÐÎ²Îp_arg£©µÄÊµ²Î
                 (OS_PRIO     ) APP_TASK_LIST_PRIO,                         //ÈÎÎñµÄÓÅÏÈ¼¶
                 (CPU_STK    *)&AppTaskListStk[0],                          //ÈÎÎñ¶ÑÕ»µÄ»ùµØÖ·
                 (CPU_STK_SIZE) APP_TASK_LIST_SIZE / 10,                //ÈÎÎñ¶ÑÕ»¿Õ¼äÊ£ÏÂ1/10Ê±ÏÞÖÆÆäÔö³¤
                 (CPU_STK_SIZE) APP_TASK_LIST_SIZE,                     //ÈÎÎñ¶ÑÕ»¿Õ¼ä£¨µ¥Î»£ºsizeof(CPU_STK)£©
                 (OS_MSG_QTY  ) 50u,                                        //ÈÎÎñ¿É½ÓÊÕµÄ×î´óÏûÏ¢Êý
                 (OS_TICK     ) 0u,                                         //ÈÎÎñµÄÊ±¼äÆ¬½ÚÅÄÊý£¨0±íÄ¬ÈÏÖµOSCfg_TickRate_Hz/10£©
                 (void       *) 0,                                          //ÈÎÎñÀ©Õ¹£¨0±í²»À©Õ¹£©
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //ÈÎÎñÑ¡Ïî
                 (OS_ERR     *)&err);                                       //·µ»Ø´íÎóÀàÐÍ
 

								 
//	/* ´´½¨ AppTaskUsart ÈÎÎñ */
//    OSTaskCreate((OS_TCB     *)&AppTaskUsartPendTCB,                            //ÈÎÎñ¿ØÖÆ¿éµØÖ·
//                 (CPU_CHAR   *)"App Task Usart Pend",                            //ÈÎÎñÃû³Æ
//                 (OS_TASK_PTR ) AppTaskUsartPend,                               //ÈÎÎñº¯Êý
//                 (void       *) 0,                                          //´«µÝ¸øÈÎÎñº¯Êý£¨ÐÎ²Îp_arg£©µÄÊµ²Î
//                 (OS_PRIO     ) APP_TASK_USART_PEND_PRIO,                        //ÈÎÎñµÄÓÅÏÈ¼¶
//                 (CPU_STK    *)&AppTaskUsartPendStk[0],                         //ÈÎÎñ¶ÑÕ»µÄ»ùµØÖ·
//                 (CPU_STK_SIZE) APP_TASK_USART_PEND_SIZE / 10,               //ÈÎÎñ¶ÑÕ»¿Õ¼äÊ£ÏÂ1/10Ê±ÏÞÖÆÆäÔö³¤
//                 (CPU_STK_SIZE) APP_TASK_USART_PEND_SIZE,                    //ÈÎÎñ¶ÑÕ»¿Õ¼ä£¨µ¥Î»£ºsizeof(CPU_STK)£©
//                 (OS_MSG_QTY  ) 50u,                                        //ÈÎÎñ¿É½ÓÊÕµÄ×î´óÏûÏ¢Êý
//                 (OS_TICK     ) 0u,                                         //ÈÎÎñµÄÊ±¼äÆ¬½ÚÅÄÊý£¨0±íÄ¬ÈÏÖµOSCfg_TickRate_Hz/10£©
//                 (void       *) 0,                                          //ÈÎÎñÀ©Õ¹£¨0±í²»À©Õ¹£©
//                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //ÈÎÎñÑ¡Ïî
//                 (OS_ERR     *)&err);       								 
								 					 
//	/* ´´½¨ AppTaskUsart ÈÎÎñ */
//    OSTaskCreate((OS_TCB     *)&AppTaskPollTCB,                            //ÈÎÎñ¿ØÖÆ¿éµØÖ·
//                 (CPU_CHAR   *)"App Task Poll",                            //ÈÎÎñÃû³Æ
//                 (OS_TASK_PTR ) AppTaskPoll,                               //ÈÎÎñº¯Êý
//                 (void       *) 0,                                          //´«µÝ¸øÈÎÎñº¯Êý£¨ÐÎ²Îp_arg£©µÄÊµ²Î
//                 (OS_PRIO     ) APP_TASK_POLL_PRIO,                        //ÈÎÎñµÄÓÅÏÈ¼¶
//                 (CPU_STK    *)&AppTaskPollStk[0],                         //ÈÎÎñ¶ÑÕ»µÄ»ùµØÖ·
//                 (CPU_STK_SIZE) APP_TASK_POLL_SIZE / 10,               //ÈÎÎñ¶ÑÕ»¿Õ¼äÊ£ÏÂ1/10Ê±ÏÞÖÆÆäÔö³¤
//                 (CPU_STK_SIZE) APP_TASK_POLL_SIZE,                    //ÈÎÎñ¶ÑÕ»¿Õ¼ä£¨µ¥Î»£ºsizeof(CPU_STK)£©
//                 (OS_MSG_QTY  ) 50u,                                        //ÈÎÎñ¿É½ÓÊÕµÄ×î´óÏûÏ¢Êý
//                 (OS_TICK     ) 0u,                                         //ÈÎÎñµÄÊ±¼äÆ¬½ÚÅÄÊý£¨0±íÄ¬ÈÏÖµOSCfg_TickRate_Hz/10£©
//                 (void       *) 0,                                          //ÈÎÎñÀ©Õ¹£¨0±í²»À©Õ¹£©
//                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //ÈÎÎñÑ¡Ïî
//                 (OS_ERR     *)&err);     

//	/* ´´½¨ AppTaskUsart ÈÎÎñ */
//    OSTaskCreate((OS_TCB     *)&AppTaskPollPendTCB,                            //ÈÎÎñ¿ØÖÆ¿éµØÖ·
//                 (CPU_CHAR   *)"App Task Poll Pend",                            //ÈÎÎñÃû³Æ
//                 (OS_TASK_PTR ) AppTaskPollPend,                               //ÈÎÎñº¯Êý
//                 (void       *) 0,                                          //´«µÝ¸øÈÎÎñº¯Êý£¨ÐÎ²Îp_arg£©µÄÊµ²Î
//                 (OS_PRIO     ) APP_TASK_POLL_PEND_PRIO,                        //ÈÎÎñµÄÓÅÏÈ¼¶
//                 (CPU_STK    *)&AppTaskPollPendStk[0],                         //ÈÎÎñ¶ÑÕ»µÄ»ùµØÖ·
//                 (CPU_STK_SIZE) APP_TASK_POLL_PEND_SIZE / 10,               //ÈÎÎñ¶ÑÕ»¿Õ¼äÊ£ÏÂ1/10Ê±ÏÞÖÆÆäÔö³¤
//                 (CPU_STK_SIZE) APP_TASK_POLL_PEND_SIZE,                    //ÈÎÎñ¶ÑÕ»¿Õ¼ä£¨µ¥Î»£ºsizeof(CPU_STK)£©
//                 (OS_MSG_QTY  ) 50u,                                        //ÈÎÎñ¿É½ÓÊÕµÄ×î´óÏûÏ¢Êý
//                 (OS_TICK     ) 0u,                                         //ÈÎÎñµÄÊ±¼äÆ¬½ÚÅÄÊý£¨0±íÄ¬ÈÏÖµOSCfg_TickRate_Hz/10£©
//                 (void       *) 0,                                          //ÈÎÎñÀ©Õ¹£¨0±í²»À©Õ¹£©
//                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //ÈÎÎñÑ¡Ïî
//                 (OS_ERR     *)&err);     								 
								 
								 
								 
								 
		OSTaskDel ( 0, & err );                     //É¾³ýÆðÊ¼ÈÎÎñ±¾Éí£¬¸ÃÈÎÎñ²»ÔÙÔËÐÐ
		
		
}


/*
*********************************************************************************************************
*                                          POST TASK
*********************************************************************************************************
*/
//static  void  AppTaskPost ( void * p_arg )
//{
//	OS_ERR      err;


//	(void)p_arg;

//					 
//	while (DEF_TRUE) {                                             //ÈÎÎñÌå
//		
//		if( Key_ReadStatus ( macKEY1_GPIO_PORT, macKEY1_GPIO_PIN, 1 ) == 1 | Key_ReadStatus ( macKEY2_GPIO_PORT, macKEY2_GPIO_PIN, 1 ) == 0  )
//		{
//		  OSTimeDlyHMSM ( 0, 0, 0, 20, OS_OPT_TIME_DLY, & err );
//		  if( Key_ReadStatus ( macKEY1_GPIO_PORT, macKEY1_GPIO_PIN, 1 ) == 1 | Key_ReadStatus ( macKEY2_GPIO_PORT, macKEY2_GPIO_PIN, 1 ) == 0  )
//			{
//			 if( Key_ReadStatus ( macKEY1_GPIO_PORT, macKEY1_GPIO_PIN, 1 ) == 1 && Key_ReadStatus ( macKEY2_GPIO_PORT, macKEY2_GPIO_PIN, 1 ) == 0  )
//			 {
//			   flag=0x03;		 
//			 }
//			 else if(Key_ReadStatus ( macKEY1_GPIO_PORT, macKEY1_GPIO_PIN, 1 ) == 1)
//			 {
//			   flag=0x01;
//			 }
//			 else
//			 {
//			   flag=0x02;
//			 }
//			}
//		}
//		
//		if(flag)
//		{
//		  OSTaskQPost ((OS_TCB      *)&AppTaskPendTCB,                 //Ä¿±êÈÎÎñµÄ¿ØÖÆ¿é
//							  	 (void        *)&flag,             //ÏûÏ¢ÄÚÈÝ
//								   (OS_MSG_SIZE  )sizeof(flag),  //ÏûÏ¢³¤¶È
//								   (OS_OPT       )OS_OPT_POST_FIFO,                //·¢²¼µ½ÈÎÎñÏûÏ¢¶ÓÁÐµÄÈë¿Ú¶Ë
//								   (OS_ERR      *)&err); 

//		}

//		OSTimeDlyHMSM ( 0, 0, 0,100, OS_OPT_TIME_DLY, & err ); 
//		flag=0;
//	}
//	
//}


///*
//*********************************************************************************************************
//*                                          PEND TASK
//*********************************************************************************************************
//*/
//static  void  AppTaskPend ( void * p_arg )
//{
//	OS_ERR         err;
//	OS_MSG_SIZE    msg_size;
//	
//	char * pMsg;

//	
//	(void)p_arg;
//	
//	while (DEF_TRUE) {                                           //ÈÎÎñÌå
//		/* ×èÈûÈÎÎñ£¬µÈ´ýÈÎÎñÏûÏ¢ */
//		pMsg = OSTaskQPend ((OS_TICK        )0,                    //ÎÞÆÚÏÞµÈ´ý
//											  (OS_OPT         )OS_OPT_PEND_BLOCKING, //Ã»ÓÐÏûÏ¢¾Í×èÈûÈÎÎñ
//											  (OS_MSG_SIZE   *)&msg_size,            //·µ»ØÏûÏ¢³¤¶È
//											  (CPU_TS        *)0,                  //·µ»ØÏûÏ¢±»·¢²¼µÄÊ±¼ä´Á
//											  (OS_ERR        *)&err);                //·µ»Ø´íÎóÀàÐÍ

//		if(*pMsg==0x01)
//		{
//			macLED1_TOGGLE();
//		}
//		else if(*pMsg==0x02)
//		  macLED2_TOGGLE();
//		else if(*pMsg==0x03)
//		  macLED3_TOGGLE();
//			
//		
//	}
//	
//}

/*
*********************************************************************************************************
*                                 USART CHECK TASK
*********************************************************************************************************
*/
static  void  AppTaskCheckDevice( void * p_arg )
{
	OS_ERR      err;
  OS_MSG_SIZE Msg_size;
	CPU_SR_ALLOC();
	
	uint8_t Addr=0;
	uint32_t Check_Temp[20]={0};
	uint32_t *Msg=0;
	uint8_t Device_Exist=0;
	uint8_t Find_Device=0;
	
	(void)p_arg;

					 
	while (DEF_TRUE) 
		{      
			if(Addr>0x3f)
			{
			 Addr=0;
			}
			OSMutexPend ((OS_MUTEX  *)&List,                  //ÉêÇë»¥³âÐÅºÅÁ¿ mutex
									 (OS_TICK    )0,                       //ÎÞÆÚÏÞµÈ´ý
									 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //Èç¹ûÉêÇë²»µ½¾Í¶ÂÈûÈÎÎñ
									 (CPU_TS    *)0,                       //²»Ïë»ñµÃÊ±¼ä´Á
									 (OS_ERR    *)&err);                   //·µ»Ø´íÎóÀàÐÍ	

      Device_Exist=Find_Node(Addr);
			
		  OSMutexPost ((OS_MUTEX  *)&List,                 //ÊÍ·Å»¥³âÐÅºÅÁ¿ mutex
								   (OS_OPT     )OS_OPT_POST_NONE,       //½øÐÐÈÎÎñµ÷¶È
								   (OS_ERR    *)&err);   	
			
			Check_Temp[0]=0xad;
			Check_Temp[1]=0xda;
			Check_Temp[2]=0x02;
			Check_Temp[3]=Addr;
			Check_Temp[4]=0x20;
			Check_Temp[5]=0xaa;
		 	
//      USART1_Send_Data(Check_Temp,6);


			
		/* ×èÈûÈÎÎñ£¬µÈ´ýÈÎÎñÏûÏ¢ */
		 Msg = OSTaskQPend ((OS_TICK        )2000,                    //ÎÞÆÚÏÞµÈ´ý
											  (OS_OPT         )OS_OPT_PEND_BLOCKING, //Ã»ÓÐÏûÏ¢¾Í×èÈûÈÎÎñ
											  (OS_MSG_SIZE   *)&Msg_size,            //·µ»ØÏûÏ¢³¤¶È
											  (CPU_TS        *)0,                    //·µ»ØÏûÏ¢±»·¢²¼µÄÊ±¼ä´Á
											  (OS_ERR        *)&err);                //·µ»Ø´íÎóÀàÐÍ
			
		OS_CRITICAL_ENTER();                                       //½øÈëÁÙ½ç¶Î£¬±ÜÃâ´®¿Ú´òÓ¡±»´ò¶Ï
		
		Find_Device=(*(Msg+3)==Addr?1:0);
		
		
    if(Device_Exist)
		{ 
			if(Find_Device)        //Éè±¸´æÔÚÓÚÁ´±í²¢ÇÒ²éÕÒÓÐ»Ø¸´
			{
			//¶Ô±ÈÉè±¸µÄ×´Ì¬ÐÅÏ¢£¬¿´¿´ÊÇ·ñÓÐÐÂIOÌí¼Ó¡¢ÐÂ×´Ì¬¸üÐÂ
				
			  OSMutexPend ((OS_MUTEX  *)&List,                  //ÉêÇë»¥³âÐÅºÅÁ¿ mutex
									   (OS_TICK    )0,                       //ÎÞÆÚÏÞµÈ´ý
									   (OS_OPT     )OS_OPT_PEND_BLOCKING,    //Èç¹ûÉêÇë²»µ½¾Í¶ÂÈûÈÎÎñ
									   (CPU_TS    *)0,                       //²»Ïë»ñµÃÊ±¼ä´Á
									   (OS_ERR    *)&err);                   //·µ»Ø´íÎóÀàÐÍ		
				
        Updata_Node(Msg);

		    OSMutexPost ((OS_MUTEX  *)&List,                 //ÊÍ·Å»¥³âÐÅºÅÁ¿ mutex
								     (OS_OPT     )OS_OPT_POST_NONE,       //½øÐÐÈÎÎñµ÷¶È
								     (OS_ERR    *)&err); 				
			
			}
			else                 //Éè±¸´æÔÚÓÚÁ´±í£¬µ«ÊÇ²éÕÒÃ»ÓÐ»Ø¸´
			{
			//¶ÔÉè±¸½øÐÐoffline²Ù×÷£¬É¾³ý½Úµã
				
        OSMutexPend ((OS_MUTEX  *)&List,                  //ÉêÇë»¥³âÐÅºÅÁ¿ mutex
									   (OS_TICK    )0,                       //ÎÞÆÚÏÞµÈ´ý
									   (OS_OPT     )OS_OPT_PEND_BLOCKING,    //Èç¹ûÉêÇë²»µ½¾Í¶ÂÈûÈÎÎñ
									   (CPU_TS    *)0,                       //²»Ïë»ñµÃÊ±¼ä´Á
									   (OS_ERR    *)&err);                   //·µ»Ø´íÎóÀàÐÍ		

		    Delete_Node(*(Msg+3));		

		    OSMutexPost ((OS_MUTEX  *)&List,                 //ÊÍ·Å»¥³âÐÅºÅÁ¿ mutex
								     (OS_OPT     )OS_OPT_POST_NONE,       //½øÐÐÈÎÎñµ÷¶È
								     (OS_ERR    *)&err); 	
			}
		  
		}
		else 
		{
			if(Find_Device)    //Éè±¸²»´æÔÚÓÚÁ´±í£¬µ«ÊÇ²éÕÒÓÐ»Ø¸´
			{
			//Õâ¸öÊÇÐÂµÄÉè±¸¼ÓÈë£¬ÐèÒª²åÈë½Úµã£¬¸üÐÂ×´Ì¬
			  OSMutexPend ((OS_MUTEX  *)&List,                  //ÉêÇë»¥³âÐÅºÅÁ¿ mutex
									   (OS_TICK    )0,                       //ÎÞÆÚÏÞµÈ´ý
									   (OS_OPT     )OS_OPT_PEND_BLOCKING,    //Èç¹ûÉêÇë²»µ½¾Í¶ÂÈûÈÎÎñ
									   (CPU_TS    *)0,                       //²»Ïë»ñµÃÊ±¼ä´Á
									   (OS_ERR    *)&err);                   //·µ»Ø´íÎóÀàÐÍ		

        Insert_Node(Msg); 
				
		    OSMutexPost ((OS_MUTEX  *)&List,                 //ÊÍ·Å»¥³âÐÅºÅÁ¿ mutex
								     (OS_OPT     )OS_OPT_POST_NONE,       //½øÐÐÈÎÎñµ÷¶È
								     (OS_ERR    *)&err); 				
				
				
			}
			else               //Éè±¸²»´æÔÚÓÚÁ´±í£¬²éÕÒÒ²Ã»ÓÐ»Ø¸´   
			{
			//Ã»ÓÐÕâ¸öÉè±¸£¬¼ÌÐø²éÕÒÏÂÒ»¸öÉè±¸
			
			}	
		}


//		tt[0]=*Msg>>24;
//		tt[1]=*Msg>>16;
//		tt[2]=*Msg>>8;
//		tt[3]=*Msg;
//		USART1_Send_Data1(tt,4);
//		  USART1_Send_Data(Msg,4);	
//		if(*(Msg+3)==Addr++)
//		{
//								/* ·¢²¼ÈÎÎñÏûÏ¢µ½ÈÎÎñ AppTaskUsart */
//		  OSTaskQPost ((OS_TCB      *)&AppTaskListTCB,      //Ä¿±êÈÎÎñµÄ¿ØÖÆ¿é
//									 (void        *)Msg,             //ÏûÏ¢ÄÚÈÝµÄÊ×µØÖ·
//									 (OS_MSG_SIZE  )Msg_size,                     //ÏûÏ¢³¤¶È
//									 (OS_OPT       )OS_OPT_POST_FIFO,      //·¢²¼µ½ÈÎÎñÏûÏ¢¶ÓÁÐµÄÈë¿Ú¶Ë
//									 (OS_ERR      *)&err);                 //·µ»Ø´íÎóÀàÐÍ
//				 
//			
//			
//		  USART1_Send_Data(Msg,(u16)Msg_size);	  			
//		  macLED1_TOGGLE();	
//		}
//		else
//		{

		/* ÍË»¹ÄÚ´æ¿é */
		OSMemPut ((OS_MEM  *)&Mem,                                 //Ö¸ÏòÄÚ´æ¹ÜÀí¶ÔÏó
							(void    *)Msg,                                 //ÄÚ´æ¿éµÄÊ×µØÖ·
							(OS_ERR  *)&err);		                             //·µ»Ø´íÎóÀàÐÍ			
//		}
/*//»òÕß×ª»»³Éu8ÐÍÊý×é
//		for(i=0;i<Msg_size;i++)
//		{
//			lc[i]=*(Msg+i);
//		}	
//		USART1_Send_Data1(lc,Msg_size);		*/			

		macLED2_TOGGLE();	
//		/* ÍË»¹ÄÚ´æ¿é */
//		OSMemPut ((OS_MEM  *)&Mem,                                 //Ö¸ÏòÄÚ´æ¹ÜÀí¶ÔÏó
//							(void    *)Msg,                                 //ÄÚ´æ¿éµÄÊ×µØÖ·
//							(OS_ERR  *)&err);		                             //·µ»Ø´íÎóÀàÐÍ	
//							
		OS_CRITICAL_EXIT();                                        //ÍË³öÁÙ½ç¶Î							
	  }
}


static  void  AppTaskList(void *p_arg)
{
	OS_ERR      err;
  uint32_t *List_Msg=0;
	
  OS_MSG_SIZE List_Msg_Size;
	
	CPU_SR_ALLOC();
	
	(void)p_arg;

					 
	while (DEF_TRUE)
	{
/* ×èÈûÈÎÎñ£¬µÈ´ýÈÎÎñÏûÏ¢ */
		 List_Msg = OSTaskQPend ((OS_TICK        )0,                    //ÎÞÆÚÏÞµÈ´ý
											       (OS_OPT         )OS_OPT_PEND_BLOCKING, //Ã»ÓÐÏûÏ¢¾Í×èÈûÈÎÎñ
											       (OS_MSG_SIZE   *)&List_Msg_Size,            //·µ»ØÏûÏ¢³¤¶È
											       (CPU_TS        *)0,                    //·µ»ØÏûÏ¢±»·¢²¼µÄÊ±¼ä´Á
											       (OS_ERR        *)&err);                //·µ»Ø´íÎóÀ

		OS_CRITICAL_ENTER();      		
	  OSMutexPend ((OS_MUTEX  *)&List,                  //ÉêÇë»¥³âÐÅºÅÁ¿ mutex
								 (OS_TICK    )0,                       //ÎÞÆÚÏÞµÈ´ý
								 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //Èç¹ûÉêÇë²»µ½¾Í¶ÂÈûÈÎÎñ
								 (CPU_TS    *)0,                       //²»Ïë»ñµÃÊ±¼ä´Á
								 (OS_ERR    *)&err);                   //·µ»Ø´íÎóÀàÐÍ		
	
	  USART1_Send_Data(List_Msg,(u16)List_Msg_Size);
		macLED1_TOGGLE();	
		OSMutexPost ((OS_MUTEX  *)&List,                 //ÊÍ·Å»¥³âÐÅºÅÁ¿ mutex
								 (OS_OPT     )OS_OPT_POST_NONE,       //½øÐÐÈÎÎñµ÷¶È
								 (OS_ERR    *)&err);                  //·µ»Ø´íÎóÀàÐÍ	

		
		
		
		OSMemPut ((OS_MEM  *)&Mem,                                 //Ö¸ÏòÄÚ´æ¹ÜÀí¶ÔÏó
							(void    *)List_Msg,                                 //ÄÚ´æ¿éµÄÊ×µØÖ·
							(OS_ERR  *)&err);		                             //·µ»Ø´íÎóÀàÐÍ	
		
    OS_CRITICAL_EXIT();                                        //ÍË³öÁÙ½ç¶Î							
		
	}


}

//static  void  AppTaskUsartPend ( void * p_arg )
//{
//  
//	OS_ERR         err;
//	OS_MSG_SIZE    Msg_size;
//	CPU_SR_ALLOC();
//	
//	uint8_t i=0;
//	uint32_t *Msg;
//  uint8_t lc[20]={0};
//	
//	(void)p_arg;

//					 
//	while (DEF_TRUE) {                                           //ÈÎÎñÌå
//		/* ×èÈûÈÎÎñ£¬µÈ´ýÈÎÎñÏûÏ¢ */
//		 Msg = OSTaskQPend ((OS_TICK        )0,                    //ÎÞÆÚÏÞµÈ´ý
//											  (OS_OPT         )OS_OPT_PEND_BLOCKING, //Ã»ÓÐÏûÏ¢¾Í×èÈûÈÎÎñ
//											  (OS_MSG_SIZE   *)&Msg_size,            //·µ»ØÏûÏ¢³¤¶È
//											  (CPU_TS        *)0,                    //·µ»ØÏûÏ¢±»·¢²¼µÄÊ±¼ä´Á
//											  (OS_ERR        *)&err);                //·µ»Ø´íÎóÀàÐÍ

//		OS_CRITICAL_ENTER();                                       //½øÈëÁÙ½ç¶Î£¬±ÜÃâ´®¿Ú´òÓ¡±»´ò¶Ï
//		

//		USART1_Send_Data(Msg,(u16)Msg_size);
//		
///*//»òÕß×ª»»³Éu8ÐÍÊý×é
////		for(i=0;i<Msg_size;i++)
////		{
////			lc[i]=*(Msg+i);
////		}	
////		USART1_Send_Data1(lc,Msg_size);		*/
//		
//		OS_CRITICAL_EXIT();                                        //ÍË³öÁÙ½ç¶Î
//		
//		/* ÍË»¹ÄÚ´æ¿é */
//		OSMemPut ((OS_MEM  *)&Mem,                                 //Ö¸ÏòÄÚ´æ¹ÜÀí¶ÔÏó
//							(void    *)Msg,                                 //ÄÚ´æ¿éµÄÊ×µØÖ·
//							(OS_ERR  *)&err);		                             //·µ»Ø´íÎóÀàÐÍ
//		
//	}
//	
//}










