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
OS_MUTEX List;                         //ÉùÃ÷»¥³âĞÅºÅÁ¿
OS_SEM  SemOfPoll;
OS_MEM  Mem;                    //ÉùÃ÷ÄÚ´æ¹ÜÀí¶ÔÏó

uint8_t Array [ 3 ] [ 100 ];   //ÉùÃ÷ÄÚ´æ·ÖÇø´óĞ¡
uint8_t Rx_Cnt=0;
uint8_t Rx_Temp[20]={0};
Node *Head;

	uint8_t Device_Exist=0;
	uint8_t Find_Device=0;

/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/

static  OS_TCB   AppTaskStartTCB;    //ÈÎÎñ¿ØÖÆ¿é

       OS_TCB AppTaskCheckDeviceTCB;
static OS_TCB AppTaskListTCB;
static OS_TCB AppTaskTCPSERVERTCB;



/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];       //ÈÎÎñ¶ÑÕ»

static  CPU_STK  AppTaskCheckDeviceStk[ APP_TASK_CHECK_DEVICE_SIZE ];
static  CPU_STK  AppTaskListStk[ APP_TASK_LIST_SIZE ];
static  CPU_STK  AppTaskTCPServerStk[ APP_TASK_TCP_SERVERT_SIZE ];



/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart  (void *p_arg);               //ÈÎÎñº¯ÊıÉùÃ÷

static  void  AppTaskCheckDevice (void *p_arg);
static  void  AppTaskList(void *p_arg);
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

	  /* ´´½¨ÆğÊ¼ÈÎÎñ */
    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                            //ÈÎÎñ¿ØÖÆ¿éµØÖ·
                 (CPU_CHAR   *)"App Task Start",                            //ÈÎÎñÃû³Æ
                 (OS_TASK_PTR ) AppTaskStart,                               //ÈÎÎñº¯Êı
                 (void       *) 0,                                          //´«µİ¸øÈÎÎñº¯Êı£¨ĞÎ²Îp_arg£©µÄÊµ²Î
                 (OS_PRIO     ) APP_TASK_START_PRIO,                        //ÈÎÎñµÄÓÅÏÈ¼¶
                 (CPU_STK    *)&AppTaskStartStk[0],                         //ÈÎÎñ¶ÑÕ»µÄ»ùµØÖ·
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,               //ÈÎÎñ¶ÑÕ»¿Õ¼äÊ£ÏÂ1/10Ê±ÏŞÖÆÆäÔö³¤
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,                    //ÈÎÎñ¶ÑÕ»¿Õ¼ä£¨µ¥Î»£ºsizeof(CPU_STK)£©
                 (OS_MSG_QTY  ) 5u,                                         //ÈÎÎñ¿É½ÓÊÕµÄ×î´óÏûÏ¢Êı
                 (OS_TICK     ) 0u,                                         //ÈÎÎñµÄÊ±¼äÆ¬½ÚÅÄÊı£¨0±íÄ¬ÈÏÖµOSCfg_TickRate_Hz/10£©
                 (void       *) 0,                                          //ÈÎÎñÀ©Õ¹£¨0±í²»À©Õ¹£©
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //ÈÎÎñÑ¡Ïî
                 (OS_ERR     *)&err);                                       //·µ»Ø´íÎóÀàĞÍ

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
    CPU_Init();                                                 //³õÊ¼»¯ CPU ×é¼ş£¨Ê±¼ä´Á¡¢¹ØÖĞ¶ÏÊ±¼ä²âÁ¿ºÍÖ÷»úÃû£©

    cpu_clk_freq = BSP_CPU_ClkFreq();                           //»ñÈ¡ CPU ÄÚºËÊ±ÖÓÆµÂÊ£¨SysTick ¹¤×÷Ê±ÖÓ£©
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        //¸ù¾İÓÃ»§Éè¶¨µÄÊ±ÖÓ½ÚÅÄÆµÂÊ¼ÆËã SysTick ¶¨Ê±Æ÷µÄ¼ÆÊıÖµ
    OS_CPU_SysTickInit(cnts);                                   //µ÷ÓÃ SysTick ³õÊ¼»¯º¯Êı£¬ÉèÖÃ¶¨Ê±Æ÷¼ÆÊıÖµºÍÆô¶¯¶¨Ê±Æ÷

    Mem_Init();                                                 //³õÊ¼»¯ÄÚ´æ¹ÜÀí×é¼ş£¨¶ÑÄÚ´æ³ØºÍÄÚ´æ³Ø±í£©

#if OS_CFG_STAT_TASK_EN > 0u                                    //Èç¹ûÊ¹ÄÜ£¨Ä¬ÈÏÊ¹ÄÜ£©ÁËÍ³¼ÆÈÎÎñ
    OSStatTaskCPUUsageInit(&err);                               //¼ÆËãÃ»ÓĞÓ¦ÓÃÈÎÎñ£¨Ö»ÓĞ¿ÕÏĞÈÎÎñ£©ÔËĞĞÊ± CPU µÄ£¨×î´ó£©
#endif                                                          //ÈİÁ¿£¨¾ö¶¨ OS_Stat_IdleCtrMax µÄÖµ£¬ÎªºóÃæ¼ÆËã CPU 
                                                                //Ê¹ÓÃÂÊÊ¹ÓÃ£©¡£
    CPU_IntDisMeasMaxCurReset();                                //¸´Î»£¨ÇåÁã£©µ±Ç°×î´ó¹ØÖĞ¶ÏÊ±¼ä

    
    /* ÅäÖÃÊ±¼äÆ¬ÂÖ×ªµ÷¶È */		
    OSSchedRoundRobinCfg((CPU_BOOLEAN   )DEF_ENABLED,          //Ê¹ÄÜÊ±¼äÆ¬ÂÖ×ªµ÷¶È
		                     (OS_TICK       )0,                    //°Ñ OSCfg_TickRate_Hz / 10 ÉèÎªÄ¬ÈÏÊ±¼äÆ¬Öµ
												 (OS_ERR       *)&err );               //·µ»Ø´íÎóÀàĞÍ

		/* ´´½¨»¥³âĞÅºÅÁ¿ mutex */
    OSMutexCreate ((OS_MUTEX  *)&List,           //Ö¸ÏòĞÅºÅÁ¿±äÁ¿µÄÖ¸Õë
                   (CPU_CHAR  *)"List", //ĞÅºÅÁ¿µÄÃû×Ö
                   (OS_ERR    *)&err);            //´íÎóÀàĞÍ
									 
									 
		/* ´´½¨ÄÚ´æ¹ÜÀí¶ÔÏó mem */
		OSMemCreate ((OS_MEM      *)&Mem,             //Ö¸ÏòÄÚ´æ¹ÜÀí¶ÔÏó
								 (CPU_CHAR    *)"Mem For Test",   //ÃüÃûÄÚ´æ¹ÜÀí¶ÔÏó
								 (void        *)Array,          //ÄÚ´æ·ÖÇøµÄÊ×µØÖ·
								 (OS_MEM_QTY   )3,               //ÄÚ´æ·ÖÇøÖĞÄÚ´æ¿éÊıÄ¿
								 (OS_MEM_SIZE  )100,                //ÄÚ´æ¿éµÄ×Ö½ÚÊıÄ¿
								 (OS_ERR      *)&err);            //·µ»Ø´íÎóÀàĞÍ
	
  						 
     /*´´½¨ÂÖÑ¯²éÑ¯UsartÊı¾İÊÇ·ñ½ÓÊÕÍê±ÏÈÎÎñ*/		
    OSTaskCreate((OS_TCB     *)&AppTaskCheckDeviceTCB,                             //ÈÎÎñ¿ØÖÆ¿éµØÖ·
                 (CPU_CHAR   *)"App_Task_Check_Device",                             //ÈÎÎñÃû³Æ
                 (OS_TASK_PTR ) AppTaskCheckDevice,                                //ÈÎÎñº¯Êı
                 (void       *) 0,                                          //´«µİ¸øÈÎÎñº¯Êı£¨ĞÎ²Îp_arg£©µÄÊµ²Î
                 (OS_PRIO     ) APP_TASK_CHECK_DEVICE_PRIO,                         //ÈÎÎñµÄÓÅÏÈ¼¶
                 (CPU_STK    *)&AppTaskCheckDeviceStk[0],                          //ÈÎÎñ¶ÑÕ»µÄ»ùµØÖ·
                 (CPU_STK_SIZE) APP_TASK_CHECK_DEVICE_SIZE / 10,                //ÈÎÎñ¶ÑÕ»¿Õ¼äÊ£ÏÂ1/10Ê±ÏŞÖÆÆäÔö³¤
                 (CPU_STK_SIZE) APP_TASK_CHECK_DEVICE_SIZE,                     //ÈÎÎñ¶ÑÕ»¿Õ¼ä£¨µ¥Î»£ºsizeof(CPU_STK)£©
                 (OS_MSG_QTY  ) 50u,                                        //ÈÎÎñ¿É½ÓÊÕµÄ×î´óÏûÏ¢Êı
                 (OS_TICK     ) 0u,                                         //ÈÎÎñµÄÊ±¼äÆ¬½ÚÅÄÊı£¨0±íÄ¬ÈÏÖµOSCfg_TickRate_Hz/10£©
                 (void       *) 0,                                          //ÈÎÎñÀ©Õ¹£¨0±í²»À©Õ¹£©
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //ÈÎÎñÑ¡Ïî
                 (OS_ERR     *)&err);                                       //·µ»Ø´íÎóÀàĞÍ


		/* ´´½¨ AppTaskPend ÈÎÎñ */
    OSTaskCreate((OS_TCB     *)&AppTaskListTCB,                             //ÈÎÎñ¿ØÖÆ¿éµØÖ·
                 (CPU_CHAR   *)"App Task Pend",                             //ÈÎÎñÃû³Æ
                 (OS_TASK_PTR ) AppTaskList,                                //ÈÎÎñº¯Êı
                 (void       *) 0,                                          //´«µİ¸øÈÎÎñº¯Êı£¨ĞÎ²Îp_arg£©µÄÊµ²Î
                 (OS_PRIO     ) APP_TASK_LIST_PRIO,                         //ÈÎÎñµÄÓÅÏÈ¼¶
                 (CPU_STK    *)&AppTaskListStk[0],                          //ÈÎÎñ¶ÑÕ»µÄ»ùµØÖ·
                 (CPU_STK_SIZE) APP_TASK_LIST_SIZE / 10,                //ÈÎÎñ¶ÑÕ»¿Õ¼äÊ£ÏÂ1/10Ê±ÏŞÖÆÆäÔö³¤
                 (CPU_STK_SIZE) APP_TASK_LIST_SIZE,                     //ÈÎÎñ¶ÑÕ»¿Õ¼ä£¨µ¥Î»£ºsizeof(CPU_STK)£©
                 (OS_MSG_QTY  ) 50u,                                        //ÈÎÎñ¿É½ÓÊÕµÄ×î´óÏûÏ¢Êı
                 (OS_TICK     ) 0u,                                         //ÈÎÎñµÄÊ±¼äÆ¬½ÚÅÄÊı£¨0±íÄ¬ÈÏÖµOSCfg_TickRate_Hz/10£©
                 (void       *) 0,                                          //ÈÎÎñÀ©Õ¹£¨0±í²»À©Õ¹£©
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //ÈÎÎñÑ¡Ïî
                 (OS_ERR     *)&err);                                       //·µ»Ø´íÎóÀàĞÍ

     /*´´½¨ÂÖÑ¯²éÑ¯UsartÊı¾İÊÇ·ñ½ÓÊÕÍê±ÏÈÎÎñ*/		
    OSTaskCreate((OS_TCB     *)&AppTaskTCPSERVERTCB,                             //ÈÎÎñ¿ØÖÆ¿éµØÖ·
                 (CPU_CHAR   *)"App_Task_TCP_Server",                             //ÈÎÎñÃû³Æ
                 (OS_TASK_PTR ) AppTaskTCPServer,                                //ÈÎÎñº¯Êı
                 (void       *) 0,                                          //´«µİ¸øÈÎÎñº¯Êı£¨ĞÎ²Îp_arg£©µÄÊµ²Î
                 (OS_PRIO     ) APP_TASK_TCP_SERVER_PRIO,                         //ÈÎÎñµÄÓÅÏÈ¼¶
                 (CPU_STK    *)&AppTaskTCPServerStk[0],                          //ÈÎÎñ¶ÑÕ»µÄ»ùµØÖ·
                 (CPU_STK_SIZE) APP_TASK_TCP_SERVERT_SIZE / 10,                //ÈÎÎñ¶ÑÕ»¿Õ¼äÊ£ÏÂ1/10Ê±ÏŞÖÆÆäÔö³¤
                 (CPU_STK_SIZE) APP_TASK_TCP_SERVERT_SIZE,                     //ÈÎÎñ¶ÑÕ»¿Õ¼ä£¨µ¥Î»£ºsizeof(CPU_STK)£©
                 (OS_MSG_QTY  ) 50u,                                        //ÈÎÎñ¿É½ÓÊÕµÄ×î´óÏûÏ¢Êı
                 (OS_TICK     ) 0u,                                         //ÈÎÎñµÄÊ±¼äÆ¬½ÚÅÄÊı£¨0±íÄ¬ÈÏÖµOSCfg_TickRate_Hz/10£©
                 (void       *) 0,                                          //ÈÎÎñÀ©Õ¹£¨0±í²»À©Õ¹£©
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //ÈÎÎñÑ¡Ïî
                 (OS_ERR     *)&err);     


								 
														 
		OSTaskDel ( 0, & err );                     //É¾³ıÆğÊ¼ÈÎÎñ±¾Éí£¬¸ÃÈÎÎñ²»ÔÙÔËĞĞ
		
		
}

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
	
	uint8_t Addr=0x01;
	uint32_t Check_Temp[20]={0};
	uint32_t *Msg=0;
//	uint8_t Device_Exist=0;
//	uint8_t Find_Device=0;
	
	(void)p_arg;

					 
	while (DEF_TRUE) 
		{      
			if(Addr>0x3f)
			{
			 Addr=0;
			}
			OSMutexPend ((OS_MUTEX  *)&List,                  //ÉêÇë»¥³âĞÅºÅÁ¿ mutex
									 (OS_TICK    )0,                       //ÎŞÆÚÏŞµÈ´ı
									 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //Èç¹ûÉêÇë²»µ½¾Í¶ÂÈûÈÎÎñ
									 (CPU_TS    *)0,                       //²»Ïë»ñµÃÊ±¼ä´Á
									 (OS_ERR    *)&err);                   //·µ»Ø´íÎóÀàĞÍ	

      Device_Exist=Find_Node(Addr);
			
		  OSMutexPost ((OS_MUTEX  *)&List,                 //ÊÍ·Å»¥³âĞÅºÅÁ¿ mutex
								   (OS_OPT     )OS_OPT_POST_NONE,       //½øĞĞÈÎÎñµ÷¶È
								   (OS_ERR    *)&err);   	
			
			Check_Temp[0]=0xad;
			Check_Temp[1]=0xda;
			Check_Temp[2]=0x02;
			Check_Temp[3]=Addr;
			Check_Temp[4]=0x20;
			Check_Temp[5]=0x03;
		 	
      USART1_Send_Data(Check_Temp,6);


			
		/* ×èÈûÈÎÎñ£¬µÈ´ıÈÎÎñÏûÏ¢ */
		 Msg = OSTaskQPend ((OS_TICK        )5000,                    //ÎŞÆÚÏŞµÈ´ı
											  (OS_OPT         )OS_OPT_PEND_BLOCKING, //Ã»ÓĞÏûÏ¢¾Í×èÈûÈÎÎñ
											  (OS_MSG_SIZE   *)&Msg_size,            //·µ»ØÏûÏ¢³¤¶È
											  (CPU_TS        *)0,                    //·µ»ØÏûÏ¢±»·¢²¼µÄÊ±¼ä´Á
											  (OS_ERR        *)&err);                //·µ»Ø´íÎóÀàĞÍ
			
		OS_CRITICAL_ENTER();                                       //½øÈëÁÙ½ç¶Î£¬±ÜÃâ´®¿Ú´òÓ¡±»´ò¶Ï
		
		Find_Device=(*(Msg+3)==Addr?1:0);
		
		
    if(Device_Exist)
		{ 
			if(Find_Device)        //Éè±¸´æÔÚÓÚÁ´±í²¢ÇÒ²éÕÒÓĞ»Ø¸´
			{
			//¶Ô±ÈÉè±¸µÄ×´Ì¬ĞÅÏ¢£¬¿´¿´ÊÇ·ñÓĞĞÂIOÌí¼Ó¡¢ĞÂ×´Ì¬¸üĞÂ
				
			  OSMutexPend ((OS_MUTEX  *)&List,                  //ÉêÇë»¥³âĞÅºÅÁ¿ mutex
									   (OS_TICK    )0,                       //ÎŞÆÚÏŞµÈ´ı
									   (OS_OPT     )OS_OPT_PEND_BLOCKING,    //Èç¹ûÉêÇë²»µ½¾Í¶ÂÈûÈÎÎñ
									   (CPU_TS    *)0,                       //²»Ïë»ñµÃÊ±¼ä´Á
									   (OS_ERR    *)&err);                   //·µ»Ø´íÎóÀàĞÍ		
				
        Updata_Node(Msg);

		    OSMutexPost ((OS_MUTEX  *)&List,                 //ÊÍ·Å»¥³âĞÅºÅÁ¿ mutex
								     (OS_OPT     )OS_OPT_POST_NONE,       //½øĞĞÈÎÎñµ÷¶È
								     (OS_ERR    *)&err); 				
			
			}
			else                 //Éè±¸´æÔÚÓÚÁ´±í£¬µ«ÊÇ²éÕÒÃ»ÓĞ»Ø¸´
			{
			//¶ÔÉè±¸½øĞĞoffline²Ù×÷£¬É¾³ı½Úµã
				
        OSMutexPend ((OS_MUTEX  *)&List,                  //ÉêÇë»¥³âĞÅºÅÁ¿ mutex
									   (OS_TICK    )0,                       //ÎŞÆÚÏŞµÈ´ı
									   (OS_OPT     )OS_OPT_PEND_BLOCKING,    //Èç¹ûÉêÇë²»µ½¾Í¶ÂÈûÈÎÎñ
									   (CPU_TS    *)0,                       //²»Ïë»ñµÃÊ±¼ä´Á
									   (OS_ERR    *)&err);                   //·µ»Ø´íÎóÀàĞÍ		

		    Delete_Node(Addr);		

		    OSMutexPost ((OS_MUTEX  *)&List,                 //ÊÍ·Å»¥³âĞÅºÅÁ¿ mutex
								     (OS_OPT     )OS_OPT_POST_NONE,       //½øĞĞÈÎÎñµ÷¶È
								     (OS_ERR    *)&err); 	
			}
		  
		}
		else 
		{
			if(Find_Device)    //Éè±¸²»´æÔÚÓÚÁ´±í£¬µ«ÊÇ²éÕÒÓĞ»Ø¸´
			{
			//Õâ¸öÊÇĞÂµÄÉè±¸¼ÓÈë£¬ĞèÒª²åÈë½Úµã£¬¸üĞÂ×´Ì¬
			  OSMutexPend ((OS_MUTEX  *)&List,                  //ÉêÇë»¥³âĞÅºÅÁ¿ mutex
									   (OS_TICK    )0,                       //ÎŞÆÚÏŞµÈ´ı
									   (OS_OPT     )OS_OPT_PEND_BLOCKING,    //Èç¹ûÉêÇë²»µ½¾Í¶ÂÈûÈÎÎñ
									   (CPU_TS    *)0,                       //²»Ïë»ñµÃÊ±¼ä´Á
									   (OS_ERR    *)&err);                   //·µ»Ø´íÎóÀàĞÍ		

        Insert_Node(Msg); 
				
		    OSMutexPost ((OS_MUTEX  *)&List,                 //ÊÍ·Å»¥³âĞÅºÅÁ¿ mutex
								     (OS_OPT     )OS_OPT_POST_NONE,       //½øĞĞÈÎÎñµ÷¶È
								     (OS_ERR    *)&err); 				
				
				
			}
			else               //Éè±¸²»´æÔÚÓÚÁ´±í£¬²éÕÒÒ²Ã»ÓĞ»Ø¸´   
			{
			//Ã»ÓĞÕâ¸öÉè±¸£¬¼ÌĞø²éÕÒÏÂÒ»¸öÉè±¸
			
			}	
		}


		/* ÍË»¹ÄÚ´æ¿é */
		OSMemPut ((OS_MEM  *)&Mem,                                 //Ö¸ÏòÄÚ´æ¹ÜÀí¶ÔÏó
							(void    *)Msg,                                 //ÄÚ´æ¿éµÄÊ×µØÖ·
							(OS_ERR  *)&err);		                             //·µ»Ø´íÎóÀàĞÍ				

		macLED2_TOGGLE();	
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
/* ×èÈûÈÎÎñ£¬µÈ´ıÈÎÎñÏûÏ¢ */
		 List_Msg = OSTaskQPend ((OS_TICK        )0,                    //ÎŞÆÚÏŞµÈ´ı
											       (OS_OPT         )OS_OPT_PEND_BLOCKING, //Ã»ÓĞÏûÏ¢¾Í×èÈûÈÎÎñ
											       (OS_MSG_SIZE   *)&List_Msg_Size,            //·µ»ØÏûÏ¢³¤¶È
											       (CPU_TS        *)0,                    //·µ»ØÏûÏ¢±»·¢²¼µÄÊ±¼ä´Á
											       (OS_ERR        *)&err);                //·µ»Ø´íÎóÀ

		OS_CRITICAL_ENTER();      		
	  OSMutexPend ((OS_MUTEX  *)&List,                  //ÉêÇë»¥³âĞÅºÅÁ¿ mutex
								 (OS_TICK    )0,                       //ÎŞÆÚÏŞµÈ´ı
								 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //Èç¹ûÉêÇë²»µ½¾Í¶ÂÈûÈÎÎñ
								 (CPU_TS    *)0,                       //²»Ïë»ñµÃÊ±¼ä´Á
								 (OS_ERR    *)&err);                   //·µ»Ø´íÎóÀàĞÍ		
	
	  USART1_Send_Data(List_Msg,(u16)List_Msg_Size);
		macLED1_TOGGLE();	
		OSMutexPost ((OS_MUTEX  *)&List,                 //ÊÍ·Å»¥³âĞÅºÅÁ¿ mutex
								 (OS_OPT     )OS_OPT_POST_NONE,       //½øĞĞÈÎÎñµ÷¶È
								 (OS_ERR    *)&err);                  //·µ»Ø´íÎóÀàĞÍ	

		
		
		
		OSMemPut ((OS_MEM  *)&Mem,                                 //Ö¸ÏòÄÚ´æ¹ÜÀí¶ÔÏó
							(void    *)List_Msg,                                 //ÄÚ´æ¿éµÄÊ×µØÖ·
							(OS_ERR  *)&err);		                             //·µ»Ø´íÎóÀàĞÍ	
		
    OS_CRITICAL_EXIT();                                        //ÍË³öÁÙ½ç¶Î							
		
	}
}

static  void  AppTaskTCPServer (void *p_arg)
{
	
	OS_ERR      err;	
	(void)p_arg;
	
	
	W5500_GPIO_Init();
	W5500_Hardware_Reset();  //Ó²¼ş¸´Î»W5500		
	W5500_Parameters_Init();		//W5500³õÊ¼ÅäÖÃ

	printf(" Ò°»ğÍøÂçÊÊÅä°æ×÷ÎªTCP ·şÎñÆ÷£¬½¨Á¢ÕìÌı£¬µÈ´ıPC×÷ÎªTCP Client½¨Á¢Á¬½Ó \r\n");
	printf(" W5500¼àÌı¶Ë¿ÚÎª£º %d \r\n",local_port);
	printf(" Á¬½Ó³É¹¦ºó£¬TCP Client·¢ËÍÊı¾İ¸øW5500£¬W5500½«·µ»Ø¶ÔÓ¦Êı¾İ \r\n");	
		
	
	while (DEF_TRUE)
	{
		do_tcp_server();
		OSTimeDlyHMSM ( 0, 0, 0,50, OS_OPT_TIME_DLY, &err);	
	}


}	










