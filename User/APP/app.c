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
OS_MUTEX List;                         //…˘√˜ª•≥‚–≈∫≈¡ø
OS_SEM  SemOfPoll;
OS_MEM  Mem;                    //…˘√˜ƒ⁄¥Êπ‹¿Ì∂‘œÛ

uint8_t Array [ 3 ] [ 100 ];   //…˘√˜ƒ⁄¥Ê∑÷«¯¥Û–°
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

static  OS_TCB   AppTaskStartTCB;    //»ŒŒÒøÿ÷∆øÈ

       OS_TCB AppTaskCheckDeviceTCB;
static OS_TCB AppTaskListTCB;
static OS_TCB AppTaskTCPSERVERTCB;



/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];       //»ŒŒÒ∂—’ª

static  CPU_STK  AppTaskCheckDeviceStk[ APP_TASK_CHECK_DEVICE_SIZE ];
static  CPU_STK  AppTaskListStk[ APP_TASK_LIST_SIZE ];
static  CPU_STK  AppTaskTCPServerStk[ APP_TASK_TCP_SERVERT_SIZE ];



/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart  (void *p_arg);               //»ŒŒÒ∫Ø ˝…˘√˜

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

	
    OSInit(&err);                                                           //≥ı ºªØ uC/OS-III

	  /* ¥¥Ω®∆ º»ŒŒÒ */
    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                            //»ŒŒÒøÿ÷∆øÈµÿ÷∑
                 (CPU_CHAR   *)"App Task Start",                            //»ŒŒÒ√˚≥∆
                 (OS_TASK_PTR ) AppTaskStart,                               //»ŒŒÒ∫Ø ˝
                 (void       *) 0,                                          //¥´µ›∏¯»ŒŒÒ∫Ø ˝£®–Œ≤Œp_arg£©µƒ µ≤Œ
                 (OS_PRIO     ) APP_TASK_START_PRIO,                        //»ŒŒÒµƒ”≈œ»º∂
                 (CPU_STK    *)&AppTaskStartStk[0],                         //»ŒŒÒ∂—’ªµƒª˘µÿ÷∑
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,               //»ŒŒÒ∂—’ªø’º‰ £œ¬1/10 ±œﬁ÷∆∆‰‘ˆ≥§
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,                    //»ŒŒÒ∂—’ªø’º‰£®µ•Œª£∫sizeof(CPU_STK)£©
                 (OS_MSG_QTY  ) 5u,                                         //»ŒŒÒø…Ω” ’µƒ◊Ó¥Ûœ˚œ¢ ˝
                 (OS_TICK     ) 0u,                                         //»ŒŒÒµƒ ±º‰∆¨Ω⁄≈ƒ ˝£®0±Ìƒ¨»œ÷µOSCfg_TickRate_Hz/10£©
                 (void       *) 0,                                          //»ŒŒÒ¿©’π£®0±Ì≤ª¿©’π£©
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //»ŒŒÒ—°œÓ
                 (OS_ERR     *)&err);                                       //∑µªÿ¥ÌŒÛ¿‡–Õ

    OSStart(&err);                                                          //∆Ù∂Ø∂‡»ŒŒÒπ‹¿Ì£®Ωª”…uC/OS-IIIøÿ÷∆£©

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

    BSP_Init();                                                 //∞Âº∂≥ı ºªØ
    CPU_Init();                                                 //≥ı ºªØ CPU ◊Èº˛£® ±º‰¥¡°¢πÿ÷–∂œ ±º‰≤‚¡ø∫Õ÷˜ª˙√˚£©

    cpu_clk_freq = BSP_CPU_ClkFreq();                           //ªÒ»° CPU ƒ⁄∫À ±÷”∆µ¬ £®SysTick π§◊˜ ±÷”£©
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        //∏˘æ›”√ªß…Ë∂®µƒ ±÷”Ω⁄≈ƒ∆µ¬ º∆À„ SysTick ∂® ±∆˜µƒº∆ ˝÷µ
    OS_CPU_SysTickInit(cnts);                                   //µ˜”√ SysTick ≥ı ºªØ∫Ø ˝£¨…Ë÷√∂® ±∆˜º∆ ˝÷µ∫Õ∆Ù∂Ø∂® ±∆˜

    Mem_Init();                                                 //≥ı ºªØƒ⁄¥Êπ‹¿Ì◊Èº˛£®∂—ƒ⁄¥Ê≥ÿ∫Õƒ⁄¥Ê≥ÿ±Ì£©

#if OS_CFG_STAT_TASK_EN > 0u                                    //»Áπ˚ πƒ‹£®ƒ¨»œ πƒ‹£©¡ÀÕ≥º∆»ŒŒÒ
    OSStatTaskCPUUsageInit(&err);                               //º∆À„√ª”–”¶”√»ŒŒÒ£®÷ª”–ø’œ–»ŒŒÒ£©‘À–– ± CPU µƒ£®◊Ó¥Û£©
#endif                                                          //»›¡ø£®æˆ∂® OS_Stat_IdleCtrMax µƒ÷µ£¨Œ™∫Û√Êº∆À„ CPU 
                                                                // π”√¬  π”√£©°£
    CPU_IntDisMeasMaxCurReset();                                //∏¥Œª£®«Â¡„£©µ±«∞◊Ó¥Ûπÿ÷–∂œ ±º‰

    
    /* ≈‰÷√ ±º‰∆¨¬÷◊™µ˜∂» */		
    OSSchedRoundRobinCfg((CPU_BOOLEAN   )DEF_ENABLED,          // πƒ‹ ±º‰∆¨¬÷◊™µ˜∂»
		                     (OS_TICK       )0,                    //∞— OSCfg_TickRate_Hz / 10 …ËŒ™ƒ¨»œ ±º‰∆¨÷µ
												 (OS_ERR       *)&err );               //∑µªÿ¥ÌŒÛ¿‡–Õ

		/* ¥¥Ω®ª•≥‚–≈∫≈¡ø mutex */
    OSMutexCreate ((OS_MUTEX  *)&List,           //÷∏œÚ–≈∫≈¡ø±‰¡øµƒ÷∏’Î
                   (CPU_CHAR  *)"List", //–≈∫≈¡øµƒ√˚◊÷
                   (OS_ERR    *)&err);            //¥ÌŒÛ¿‡–Õ
									 
									 
		/* ¥¥Ω®ƒ⁄¥Êπ‹¿Ì∂‘œÛ mem */
		OSMemCreate ((OS_MEM      *)&Mem,             //÷∏œÚƒ⁄¥Êπ‹¿Ì∂‘œÛ
								 (CPU_CHAR    *)"Mem For Test",   //√¸√˚ƒ⁄¥Êπ‹¿Ì∂‘œÛ
								 (void        *)Array,          //ƒ⁄¥Ê∑÷«¯µƒ ◊µÿ÷∑
								 (OS_MEM_QTY   )3,               //ƒ⁄¥Ê∑÷«¯÷–ƒ⁄¥ÊøÈ ˝ƒø
								 (OS_MEM_SIZE  )100,                //ƒ⁄¥ÊøÈµƒ◊÷Ω⁄ ˝ƒø
								 (OS_ERR      *)&err);            //∑µªÿ¥ÌŒÛ¿‡–Õ
	
  						 
     /*¥¥Ω®¬÷—Ø≤È—ØUsart ˝æ› «∑ÒΩ” ’ÕÍ±œ»ŒŒÒ*/		
    OSTaskCreate((OS_TCB     *)&AppTaskCheckDeviceTCB,                             //»ŒŒÒøÿ÷∆øÈµÿ÷∑
                 (CPU_CHAR   *)"App_Task_Check_Device",                             //»ŒŒÒ√˚≥∆
                 (OS_TASK_PTR ) AppTaskCheckDevice,                                //»ŒŒÒ∫Ø ˝
                 (void       *) 0,                                          //¥´µ›∏¯»ŒŒÒ∫Ø ˝£®–Œ≤Œp_arg£©µƒ µ≤Œ
                 (OS_PRIO     ) APP_TASK_CHECK_DEVICE_PRIO,                         //»ŒŒÒµƒ”≈œ»º∂
                 (CPU_STK    *)&AppTaskCheckDeviceStk[0],                          //»ŒŒÒ∂—’ªµƒª˘µÿ÷∑
                 (CPU_STK_SIZE) APP_TASK_CHECK_DEVICE_SIZE / 10,                //»ŒŒÒ∂—’ªø’º‰ £œ¬1/10 ±œﬁ÷∆∆‰‘ˆ≥§
                 (CPU_STK_SIZE) APP_TASK_CHECK_DEVICE_SIZE,                     //»ŒŒÒ∂—’ªø’º‰£®µ•Œª£∫sizeof(CPU_STK)£©
                 (OS_MSG_QTY  ) 50u,                                        //»ŒŒÒø…Ω” ’µƒ◊Ó¥Ûœ˚œ¢ ˝
                 (OS_TICK     ) 0u,                                         //»ŒŒÒµƒ ±º‰∆¨Ω⁄≈ƒ ˝£®0±Ìƒ¨»œ÷µOSCfg_TickRate_Hz/10£©
                 (void       *) 0,                                          //»ŒŒÒ¿©’π£®0±Ì≤ª¿©’π£©
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //»ŒŒÒ—°œÓ
                 (OS_ERR     *)&err);                                       //∑µªÿ¥ÌŒÛ¿‡–Õ


		/* ¥¥Ω® AppTaskPend »ŒŒÒ */
    OSTaskCreate((OS_TCB     *)&AppTaskListTCB,                             //»ŒŒÒøÿ÷∆øÈµÿ÷∑
                 (CPU_CHAR   *)"App Task Pend",                             //»ŒŒÒ√˚≥∆
                 (OS_TASK_PTR ) AppTaskList,                                //»ŒŒÒ∫Ø ˝
                 (void       *) 0,                                          //¥´µ›∏¯»ŒŒÒ∫Ø ˝£®–Œ≤Œp_arg£©µƒ µ≤Œ
                 (OS_PRIO     ) APP_TASK_LIST_PRIO,                         //»ŒŒÒµƒ”≈œ»º∂
                 (CPU_STK    *)&AppTaskListStk[0],                          //»ŒŒÒ∂—’ªµƒª˘µÿ÷∑
                 (CPU_STK_SIZE) APP_TASK_LIST_SIZE / 10,                //»ŒŒÒ∂—’ªø’º‰ £œ¬1/10 ±œﬁ÷∆∆‰‘ˆ≥§
                 (CPU_STK_SIZE) APP_TASK_LIST_SIZE,                     //»ŒŒÒ∂—’ªø’º‰£®µ•Œª£∫sizeof(CPU_STK)£©
                 (OS_MSG_QTY  ) 50u,                                        //»ŒŒÒø…Ω” ’µƒ◊Ó¥Ûœ˚œ¢ ˝
                 (OS_TICK     ) 0u,                                         //»ŒŒÒµƒ ±º‰∆¨Ω⁄≈ƒ ˝£®0±Ìƒ¨»œ÷µOSCfg_TickRate_Hz/10£©
                 (void       *) 0,                                          //»ŒŒÒ¿©’π£®0±Ì≤ª¿©’π£©
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //»ŒŒÒ—°œÓ
                 (OS_ERR     *)&err);                                       //∑µªÿ¥ÌŒÛ¿‡–Õ

     /*¥¥Ω®¬÷—Ø≤È—ØUsart ˝æ› «∑ÒΩ” ’ÕÍ±œ»ŒŒÒ*/		
    OSTaskCreate((OS_TCB     *)&AppTaskTCPSERVERTCB,                             //»ŒŒÒøÿ÷∆øÈµÿ÷∑
                 (CPU_CHAR   *)"App_Task_TCP_Server",                             //»ŒŒÒ√˚≥∆
                 (OS_TASK_PTR ) AppTaskTCPServer,                                //»ŒŒÒ∫Ø ˝
                 (void       *) 0,                                          //¥´µ›∏¯»ŒŒÒ∫Ø ˝£®–Œ≤Œp_arg£©µƒ µ≤Œ
                 (OS_PRIO     ) APP_TASK_TCP_SERVER_PRIO,                         //»ŒŒÒµƒ”≈œ»º∂
                 (CPU_STK    *)&AppTaskTCPServerStk[0],                          //»ŒŒÒ∂—’ªµƒª˘µÿ÷∑
                 (CPU_STK_SIZE) APP_TASK_TCP_SERVERT_SIZE / 10,                //»ŒŒÒ∂—’ªø’º‰ £œ¬1/10 ±œﬁ÷∆∆‰‘ˆ≥§
                 (CPU_STK_SIZE) APP_TASK_TCP_SERVERT_SIZE,                     //»ŒŒÒ∂—’ªø’º‰£®µ•Œª£∫sizeof(CPU_STK)£©
                 (OS_MSG_QTY  ) 50u,                                        //»ŒŒÒø…Ω” ’µƒ◊Ó¥Ûœ˚œ¢ ˝
                 (OS_TICK     ) 0u,                                         //»ŒŒÒµƒ ±º‰∆¨Ω⁄≈ƒ ˝£®0±Ìƒ¨»œ÷µOSCfg_TickRate_Hz/10£©
                 (void       *) 0,                                          //»ŒŒÒ¿©’π£®0±Ì≤ª¿©’π£©
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //»ŒŒÒ—°œÓ
                 (OS_ERR     *)&err);     


								 
														 
		OSTaskDel ( 0, & err );                     //…æ≥˝∆ º»ŒŒÒ±æ…Ì£¨∏√»ŒŒÒ≤ª‘Ÿ‘À––
		
		
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
	
	uint8_t i=0;
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
			OSMutexPend ((OS_MUTEX  *)&List,                  //…Í«Îª•≥‚–≈∫≈¡ø mutex
									 (OS_TICK    )0,                       //Œﬁ∆⁄œﬁµ»¥˝
									 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //»Áπ˚…Í«Î≤ªµΩæÕ∂¬»˚»ŒŒÒ
									 (CPU_TS    *)0,                       //≤ªœÎªÒµ√ ±º‰¥¡
									 (OS_ERR    *)&err);                   //∑µªÿ¥ÌŒÛ¿‡–Õ	

      Device_Exist=Find_Node(Addr);
			
		  OSMutexPost ((OS_MUTEX  *)&List,                 // Õ∑≈ª•≥‚–≈∫≈¡ø mutex
								   (OS_OPT     )OS_OPT_POST_NONE,       //Ω¯––»ŒŒÒµ˜∂»
								   (OS_ERR    *)&err);   	
			
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
			

			
		/* ◊Ë»˚»ŒŒÒ£¨µ»¥˝»ŒŒÒœ˚œ¢ */
		 Msg = OSTaskQPend ((OS_TICK        )5000,                    //Œﬁ∆⁄œﬁµ»¥˝
											  (OS_OPT         )OS_OPT_PEND_BLOCKING, //√ª”–œ˚œ¢æÕ◊Ë»˚»ŒŒÒ
											  (OS_MSG_SIZE   *)&Msg_size,            //∑µªÿœ˚œ¢≥§∂»
											  (CPU_TS        *)0,                    //∑µªÿœ˚œ¢±ª∑¢≤ºµƒ ±º‰¥¡
											  (OS_ERR        *)&err);                //∑µªÿ¥ÌŒÛ¿‡–Õ
			
		OS_CRITICAL_ENTER();                                       //Ω¯»Î¡ŸΩÁ∂Œ£¨±‹√‚¥Æø⁄¥Ú”°±ª¥Ú∂œ
		
		Find_Device=(*(Msg+3)==Addr?1:0);
		
		
    if(Device_Exist)
		{ 
			if(Find_Device)        //…Ë±∏¥Ê‘⁄”⁄¡¥±Ì≤¢«“≤È’“”–ªÿ∏¥
			{
			//∂‘±»…Ë±∏µƒ◊¥Ã¨–≈œ¢£¨ø¥ø¥ «∑Ò”––¬IOÃÌº”°¢–¬◊¥Ã¨∏¸–¬
				
			  OSMutexPend ((OS_MUTEX  *)&List,                  //…Í«Îª•≥‚–≈∫≈¡ø mutex
									   (OS_TICK    )0,                       //Œﬁ∆⁄œﬁµ»¥˝
									   (OS_OPT     )OS_OPT_PEND_BLOCKING,    //»Áπ˚…Í«Î≤ªµΩæÕ∂¬»˚»ŒŒÒ
									   (CPU_TS    *)0,                       //≤ªœÎªÒµ√ ±º‰¥¡
									   (OS_ERR    *)&err);                   //∑µªÿ¥ÌŒÛ¿‡–Õ		
				
        Updata_Node(Msg);

		    OSMutexPost ((OS_MUTEX  *)&List,                 // Õ∑≈ª•≥‚–≈∫≈¡ø mutex
								     (OS_OPT     )OS_OPT_POST_NONE,       //Ω¯––»ŒŒÒµ˜∂»
								     (OS_ERR    *)&err); 				
			
			}
			else                 //…Ë±∏¥Ê‘⁄”⁄¡¥±Ì£¨µ´ «≤È’“√ª”–ªÿ∏¥
			{
			//∂‘…Ë±∏Ω¯––offline≤Ÿ◊˜£¨…æ≥˝Ω⁄µ„
				
        OSMutexPend ((OS_MUTEX  *)&List,                  //…Í«Îª•≥‚–≈∫≈¡ø mutex
									   (OS_TICK    )0,                       //Œﬁ∆⁄œﬁµ»¥˝
									   (OS_OPT     )OS_OPT_PEND_BLOCKING,    //»Áπ˚…Í«Î≤ªµΩæÕ∂¬»˚»ŒŒÒ
									   (CPU_TS    *)0,                       //≤ªœÎªÒµ√ ±º‰¥¡
									   (OS_ERR    *)&err);                   //∑µªÿ¥ÌŒÛ¿‡–Õ		

		    Delete_Node(Addr);		

		    OSMutexPost ((OS_MUTEX  *)&List,                 // Õ∑≈ª•≥‚–≈∫≈¡ø mutex
								     (OS_OPT     )OS_OPT_POST_NONE,       //Ω¯––»ŒŒÒµ˜∂»
								     (OS_ERR    *)&err); 	
			}
		  
		}
		else 
		{
			if(Find_Device)    //…Ë±∏≤ª¥Ê‘⁄”⁄¡¥±Ì£¨µ´ «≤È’“”–ªÿ∏¥
			{
			//’‚∏ˆ «–¬µƒ…Ë±∏º”»Î£¨–Ë“™≤Â»ÎΩ⁄µ„£¨∏¸–¬◊¥Ã¨
			  OSMutexPend ((OS_MUTEX  *)&List,                  //…Í«Îª•≥‚–≈∫≈¡ø mutex
									   (OS_TICK    )0,                       //Œﬁ∆⁄œﬁµ»¥˝
									   (OS_OPT     )OS_OPT_PEND_BLOCKING,    //»Áπ˚…Í«Î≤ªµΩæÕ∂¬»˚»ŒŒÒ
									   (CPU_TS    *)0,                       //≤ªœÎªÒµ√ ±º‰¥¡
									   (OS_ERR    *)&err);                   //∑µªÿ¥ÌŒÛ¿‡–Õ		

        Insert_Node(Msg); 
				
		    OSMutexPost ((OS_MUTEX  *)&List,                 // Õ∑≈ª•≥‚–≈∫≈¡ø mutex
								     (OS_OPT     )OS_OPT_POST_NONE,       //Ω¯––»ŒŒÒµ˜∂»
								     (OS_ERR    *)&err); 				
				
				
			}
			else               //…Ë±∏≤ª¥Ê‘⁄”⁄¡¥±Ì£¨≤È’““≤√ª”–ªÿ∏¥   
			{
			//√ª”–’‚∏ˆ…Ë±∏£¨ºÃ–¯≤È’“œ¬“ª∏ˆ…Ë±∏
			
			}	
		}


		/* ÕÀªπƒ⁄¥ÊøÈ */
		OSMemPut ((OS_MEM  *)&Mem,                                 //÷∏œÚƒ⁄¥Êπ‹¿Ì∂‘œÛ
							(void    *)Msg,                                 //ƒ⁄¥ÊøÈµƒ ◊µÿ÷∑
							(OS_ERR  *)&err);		                             //∑µªÿ¥ÌŒÛ¿‡–Õ				

		macLED2_TOGGLE();	
		OS_CRITICAL_EXIT();                                        //ÕÀ≥ˆ¡ŸΩÁ∂Œ							
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
/* ◊Ë»˚»ŒŒÒ£¨µ»¥˝»ŒŒÒœ˚œ¢ */
		 List_Msg = OSTaskQPend ((OS_TICK        )0,                    //Œﬁ∆⁄œﬁµ»¥˝
											       (OS_OPT         )OS_OPT_PEND_BLOCKING, //√ª”–œ˚œ¢æÕ◊Ë»˚»ŒŒÒ
											       (OS_MSG_SIZE   *)&List_Msg_Size,            //∑µªÿœ˚œ¢≥§∂»
											       (CPU_TS        *)0,                    //∑µªÿœ˚œ¢±ª∑¢≤ºµƒ ±º‰¥¡
											       (OS_ERR        *)&err);                //∑µªÿ¥ÌŒÛ¿

		OS_CRITICAL_ENTER();      		
	  OSMutexPend ((OS_MUTEX  *)&List,                  //…Í«Îª•≥‚–≈∫≈¡ø mutex
								 (OS_TICK    )0,                       //Œﬁ∆⁄œﬁµ»¥˝
								 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //»Áπ˚…Í«Î≤ªµΩæÕ∂¬»˚»ŒŒÒ
								 (CPU_TS    *)0,                       //≤ªœÎªÒµ√ ±º‰¥¡
								 (OS_ERR    *)&err);                   //∑µªÿ¥ÌŒÛ¿‡–Õ		
	
	  USART1_Send_Data(List_Msg,(u16)List_Msg_Size);
		macLED1_TOGGLE();	
		OSMutexPost ((OS_MUTEX  *)&List,                 // Õ∑≈ª•≥‚–≈∫≈¡ø mutex
								 (OS_OPT     )OS_OPT_POST_NONE,       //Ω¯––»ŒŒÒµ˜∂»
								 (OS_ERR    *)&err);                  //∑µªÿ¥ÌŒÛ¿‡–Õ	

		
		
		
		OSMemPut ((OS_MEM  *)&Mem,                                 //÷∏œÚƒ⁄¥Êπ‹¿Ì∂‘œÛ
							(void    *)List_Msg,                                 //ƒ⁄¥ÊøÈµƒ ◊µÿ÷∑
							(OS_ERR  *)&err);		                             //∑µªÿ¥ÌŒÛ¿‡–Õ	
		
    OS_CRITICAL_EXIT();                                        //ÕÀ≥ˆ¡ŸΩÁ∂Œ							
		
	}
}

static  void  AppTaskTCPServer (void *p_arg)
{
	
	OS_ERR      err;	
	(void)p_arg;
	
	
	W5500_GPIO_Init();
	W5500_Hardware_Reset();  //”≤º˛∏¥ŒªW5500		
	W5500_Parameters_Init();		//W5500≥ı º≈‰÷√

	printf(" “∞ªÕ¯¬Á  ≈‰∞Ê◊˜Œ™TCP ∑˛ŒÒ∆˜£¨Ω®¡¢’ÏÃ˝£¨µ»¥˝PC◊˜Œ™TCP ClientΩ®¡¢¡¨Ω” \r\n");
	printf(" W5500º‡Ã˝∂Àø⁄Œ™£∫ %d \r\n",local_port);
	printf(" ¡¨Ω”≥…π¶∫Û£¨TCP Client∑¢ÀÕ ˝æ›∏¯W5500£¨W5500Ω´∑µªÿ∂‘”¶ ˝æ› \r\n");	
		
	
	while (DEF_TRUE)
	{
		do_tcp_server();
		OSTimeDlyHMSM ( 0, 0, 0,50, OS_OPT_TIME_DLY, &err);	
	}


}	










