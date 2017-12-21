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
OS_MUTEX List;                         //声明互斥信号量
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

static OS_TCB AppTaskStartTCB;    //任务控制块
 OS_TCB AppTaskUSART1CheckTCB;
static OS_TCB AppTaskTCPServerTCB;
 OS_TCB AppTaskPollDeviceTCB;


/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];       //任务堆栈
static  CPU_STK  AppTaskUSART1CheckStk[ APP_TASK_USART1_CHECK_SIZE ];
static  CPU_STK  AppTaskTCPServerStk[ APP_TASK_TCP_SERVERT_SIZE ];
static  CPU_STK  AppTaskTCPPollDeviceStk[ APP_TASK_TCP_POLL_DEVICE_SIZE ];


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart  (void *p_arg);               //任务函数声明
static  void  AppTaskUSART1Check (void *p_arg);
static  void  AppTaskTCPServer (void *p_arg);
static  void  AppTaskTCPPollDevice (void *p_arg);
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

	
    OSInit(&err);                                                           //初始化 uC/OS-III

	  /* 创建起始任务 */
    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                            //任务控制块地址
                 (CPU_CHAR   *)"App Task Start",                            //任务名称
                 (OS_TASK_PTR ) AppTaskStart,                               //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_START_PRIO,                        //任务的优先级
                 (CPU_STK    *)&AppTaskStartStk[0],                         //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,               //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,                    //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 5u,                                         //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型

    OSStart(&err);                                                          //启动多任务管理（交由uC/OS-III控制）

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

    BSP_Init();                                                 //板级初始化
    CPU_Init();                                                 //初始化 CPU 组件（时间戳、关中断时间测量和主机名）

    cpu_clk_freq = BSP_CPU_ClkFreq();                           //获取 CPU 内核时钟频率（SysTick 工作时钟）
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        //根据用户设定的时钟节拍频率计算 SysTick 定时器的计数值
    OS_CPU_SysTickInit(cnts);                                   //调用 SysTick 初始化函数，设置定时器计数值和启动定时器

    Mem_Init();                                                 //初始化内存管理组件（堆内存池和内存池表）

#if OS_CFG_STAT_TASK_EN > 0u                                    //如果使能（默认使能）了统计任务
    OSStatTaskCPUUsageInit(&err);                               //计算没有应用任务（只有空闲任务）运行时 CPU 的（最大）
#endif                                                          //容量（决定 OS_Stat_IdleCtrMax 的值，为后面计算 CPU 
                                                                //使用率使用）。
    CPU_IntDisMeasMaxCurReset();                                //复位（清零）当前最大关中断时间

    
    /* 配置时间片轮转调度 */		
    OSSchedRoundRobinCfg((CPU_BOOLEAN   )DEF_ENABLED,          //使能时间片轮转调度
		                     (OS_TICK       )0,                    //把 OSCfg_TickRate_Hz / 10 设为默认时间片值
												 (OS_ERR       *)&err );               //返回错误类型

		/* 创建互斥信号量 mutex */
    OSMutexCreate ((OS_MUTEX  *)&List,           //指向信号量变量的指针
                   (CPU_CHAR  *)"List", //信号量的名字
                   (OS_ERR    *)&err);            //错误类型
									 
				/* 创建互斥信号量 mutex */
    OSMutexCreate ((OS_MUTEX  *)&Usart,           //指向信号量变量的指针
                   (CPU_CHAR  *)"Usart", //信号量的名字
                   (OS_ERR    *)&err);            //错误类型
									 
		/* 创建互斥信号量 mutex */
    OSMutexCreate ((OS_MUTEX  *)&Socket,           //指向信号量变量的指针
                   (CPU_CHAR  *)"Socket", //信号量的名字
                   (OS_ERR    *)&err);            //错误类型									 
									 
	

	
     /*创建轮询查询Usart数据是否接收完毕任务*/		
    OSTaskCreate((OS_TCB     *)&AppTaskTCPServerTCB,                             //任务控制块地址
                 (CPU_CHAR   *)"App_Task_TCP_Server",                             //任务名称
                 (OS_TASK_PTR ) AppTaskTCPServer,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_TCP_SERVER_PRIO,                         //任务的优先级
                 (CPU_STK    *)&AppTaskTCPServerStk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_TCP_SERVERT_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_TCP_SERVERT_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 50u,                                        //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);     
	
  						 
//     /*创建轮询查询Usart数据是否接收完毕任务*/		
//    OSTaskCreate((OS_TCB     *)&AppTaskUSART1CheckTCB,                             //任务控制块地址
//                 (CPU_CHAR   *)"App_Task_Check_Device",                             //任务名称
//                 (OS_TASK_PTR ) AppTaskUSART1Check,                                //任务函数
//                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
//                 (OS_PRIO     ) APP_TASK_USART1_CHECK_PRIO,                         //任务的优先级
//                 (CPU_STK    *)&AppTaskUSART1CheckStk[0],                          //任务堆栈的基地址
//                 (CPU_STK_SIZE) APP_TASK_USART1_CHECK_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
//                 (CPU_STK_SIZE) APP_TASK_USART1_CHECK_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
//                 (OS_MSG_QTY  ) 50u,                                        //任务可接收的最大消息数
//                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
//                 (void       *) 0,                                          //任务扩展（0表不扩展）
//                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
//                 (OS_ERR     *)&err);                                       //返回错误类型

     /*创建轮询查询Usart数据是否接收完毕任务*/		
    OSTaskCreate((OS_TCB     *)&AppTaskPollDeviceTCB,                             //任务控制块地址
                 (CPU_CHAR   *)"App_Task_Poll_Device",                             //任务名称
                 (OS_TASK_PTR ) AppTaskTCPPollDevice,                                //任务函数
                 (void       *) 0,                                          //传递给任务函数（形参p_arg）的实参
                 (OS_PRIO     ) APP_TASK_POLL_DEVICE_PRIO,                         //任务的优先级
                 (CPU_STK    *)&AppTaskTCPPollDeviceStk[0],                          //任务堆栈的基地址
                 (CPU_STK_SIZE) APP_TASK_TCP_POLL_DEVICE_SIZE / 10,                //任务堆栈空间剩下1/10时限制其增长
                 (CPU_STK_SIZE) APP_TASK_TCP_POLL_DEVICE_SIZE,                     //任务堆栈空间（单位：sizeof(CPU_STK)）
                 (OS_MSG_QTY  ) 50u,                                        //任务可接收的最大消息数
                 (OS_TICK     ) 0u,                                         //任务的时间片节拍数（0表默认值OSCfg_TickRate_Hz/10）
                 (void       *) 0,                                          //任务扩展（0表不扩展）
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //任务选项
                 (OS_ERR     *)&err);                                       //返回错误类型


								 
														 
		OSTaskDel ( 0, & err );                     //删除起始任务本身，该任务不再运行
		
		
}

/*
*********************************************************************************************************
*                                 USART CHECK TASK
*********************************************************************************************************
*/

static  void  AppTaskTCPPollDevice( void * p_arg )
{
	OS_ERR      err;
	CPU_SR_ALLOC();
	uint8_t Poll_Addr = 0x01;
	(void)p_arg;

	while (DEF_TRUE) 
	{ 
		if(Poll_Addr>0x10)
		{
			Poll_Addr=1;
		}	
		
    Check_Device_Cmd_Buffer(Poll_Addr);
		
		OSTaskSemPend ((OS_TICK   )50,                     //无期限等待
									 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //如果信号量不可用就等待
									 (CPU_TS   *)0,                   //获取信号量被发布的时间戳
									 (OS_ERR   *)&err);                 //返回错误类型		

		OS_CRITICAL_ENTER();                                       //?????,?????????    
    
		if(USART_Rx_Count)                              //串口收到cmd为0x20的回复数据
		{
			if(USART_Rx_Buffer[3] == Poll_Addr)
			{
		    if(Find_Node(Poll_Addr))
				{
				  Check_Node_Status(USART_Rx_Buffer);							
				}
				else
				{
				  Insert_Node(USART_Rx_Buffer); 								
				}
        Poll_Addr++;				
			}
			else
			{
				if(Find_Node(USART_Rx_Buffer[3]))
				{
				  Check_Node_Status(USART_Rx_Buffer);			
				}
			}
		}
		else
		{
		  if(Find_Node(Poll_Addr))
      {
		    Delete_Node(Poll_Addr);	
			}				
        Poll_Addr++;				
		}
//		
//		Device_Exist=Find_Node(Poll_Addr);		
// 	   
//		 
//		 
//    if(USART_Rx_Buffer[3] == Poll_Addr)
//		{
//	    if(Device_Exist)
//			{
//       Check_Node_Status(USART_Rx_Buffer);				
//			}
//			else
//			{
//			  Insert_Node(USART_Rx_Buffer); 		
//			}					
//		}
//		else
//		{
//			if(Device_Exist)
//			{
//		    Delete_Node(Poll_Addr);							
//			}
//		}
//		if(USART_Rx_Buffer[3] == Query_Address)
//    {
//		 Query_Address = 0;
//		}
//    Find_Device = 0;	
		memset(USART_Rx_Buffer,0,USART_Rx_Count);
		USART_Rx_Count = 0;
		
//		Poll_Addr++;	


//		
//		if(Find_Device)
//		{
//		  if(Device_Exist)
//			{
//       Check_Node_Status(USART_Rx_Buffer);				
//			}
//			else
//			{
//			  Insert_Node(USART_Rx_Buffer); 		
//			}
//      Find_Device = 0;	
//			memset(USART_Rx_Buffer,0,USART_Rx_Count);
//			USART_Rx_Count = 0;
//		}
//		else
//		{
//			if(Device_Exist)
//			{
//		    Delete_Node(Addr);							
//			}
//		}		
//		Addr++;	
		OS_CRITICAL_EXIT();                                        //?????			
	}
}



static  void  AppTaskUSART1Check( void * p_arg )
{
	OS_ERR      err;

	(void)p_arg;
			 
	while (DEF_TRUE) 
	{      
    	

	  OSTaskSemPend ((OS_TICK   )0,                     //无期限等待
									 (OS_OPT    )OS_OPT_PEND_BLOCKING,  //如果信号量不可用就等待
									 (CPU_TS   *)0,                   //获取信号量被发布的时间戳
									 (OS_ERR   *)&err);                 //返回错误类型	

	  OSTimeDlyHMSM ( 0, 0, 0,10, OS_OPT_TIME_DLY, &err);					



		
		if(USART_Rx_Buffer[4] == 0x20)
		{
//			if(USART_Rx_Buffer[3] == 	Poll_Addr)
//			{
				if(!Find_Device)
				{
					Find_Device = 1;
				}
				
//				if(Device_Exist)
//				{
//          Updata_Node(USART_Rx_Buffer);											
//				}
//				else
//				{
//          Insert_Node(USART_Rx_Buffer); 						
//				}
//			  
				OSTaskSemPost((OS_TCB  *)&AppTaskTCPPollDevice,                              //目标任务
									  	(OS_OPT   )OS_OPT_POST_NONE,                             //没选项要求
										  (OS_ERR  *)&err);   						
			}
//			else
//			{
//				if(Device_Exist)
//				{
//		      Delete_Node(Addr);							
//				}
//			}
//		}
		else
		{
			
			
		}

			


 		
//		
//		
////		Find_Device=(*(Msg+3)==Addr?1:0);
////		
////		
//    if(Device_Exist)
//		{ 
//			if(Find_Device)        //设备存在于链表并且查找有回复
//			{
//			//对比设备的状态信息，看看是否有新IO添加、新状态更新			
//        Updata_Node(USART_Rx_Buffer);		
//			}
//			else                 //设备存在于链表，但是查找没有回复
//			{
//			//对设备进行offline操作，删除节点
//		    Delete_Node(Addr);		
//			}
//		  
//		}
//		else 
//		{
//			if(Find_Device)    //设备不存在于链表，但是查找有回复
//			{
//			//这个是新的设备加入，需要插入节点，更新状态
//        Insert_Node(USART_Rx_Buffer); 	
//			}
//		}

//		memset(USART_Rx_Buffer,0,USART_Rx_Count);
//		USART_Rx_Count = 0;
//		Find_Device = 0;
//		
		macLED2_TOGGLE();	
//		OS_CRITICAL_EXIT();                                        //退出临界段							
	  }
}

static  void  AppTaskTCPServer (void *p_arg)
{
	
	OS_ERR      err;
	(void)p_arg;
		
	Load_Net_Parameters();		//装载网络参数	
	W5500_Hardware_Reset();		//硬件复位W5500	
	W5500_Initialization();		//W5500初始配置
	
	
	while (DEF_TRUE)
	{
		W5500_Socket_Set();            //W5500端口初始化配置		
		
		if(W5500_Interrupt)            //如果W5500_Interrupt标志位置位，表示中断发生，处理W5500中断		
		{
			W5500_Interrupt_Process(); //W5500中断处理程序
		}
		if((S0_Data & S_RECEIVE) == S_RECEIVE)//如果Socket0接收到数据
		{
			S0_Data&=~S_RECEIVE;              //清楚接收数据标志位
			Process_Socket_Recive_Data(0);           //W5500处理接收数据的函数		
		}
//		else if(time >= 500)                  //定时发送字符串
//		{
//			time=0;                           //清除time                     
//			if(S0_State == (S_INIT|S_CONN))   //如果端口初始化成功，且连接成功
//			{
//				S0_Data&=~S_TRANSMITOK;	                                             //清除发送标志	
//				Write_SOCK_Data_Buffer(0, TxPtr, strlen((const char*)TxPtr));  //端口0发送数据
//			}			                      
//		}
		OSTimeDlyHMSM ( 0, 0, 0,10, OS_OPT_TIME_DLY, &err);	
	}
}	










