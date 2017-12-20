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
OS_MUTEX List;                         //���������ź���
OS_MUTEX Usart;
OS_MUTEX Socket;

OS_SEM  SemOfPoll;

 Node *Head;

	uint8_t Device_Exist=0;
	uint8_t Find_Device=0;
	uint8_t Addr=0x01;
/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/

static OS_TCB AppTaskStartTCB;    //������ƿ�
 OS_TCB AppTaskUSART1CheckTCB;
static OS_TCB AppTaskTCPServerTCB;
//static OS_TCB AppTaskPollDeviceTCB;


/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];       //�����ջ
static  CPU_STK  AppTaskUSART1CheckStk[ APP_TASK_USART1_CHECK_SIZE ];
static  CPU_STK  AppTaskTCPServerStk[ APP_TASK_TCP_SERVERT_SIZE ];
//static  CPU_STK  AppTaskTCPPollDeviceStk[ APP_TASK_TCP_POLL_DEVICE_SIZE ];


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart  (void *p_arg);               //����������
static  void  AppTaskUSART1Check (void *p_arg);
static  void  AppTaskTCPServer (void *p_arg);
//static  void  AppTaskTCPPollDevice (void *p_arg);
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

	
    OSInit(&err);                                                           //��ʼ�� uC/OS-III

	  /* ������ʼ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                            //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Start",                            //��������
                 (OS_TASK_PTR ) AppTaskStart,                               //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_START_PRIO,                        //��������ȼ�
                 (CPU_STK    *)&AppTaskStartStk[0],                         //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,               //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,                    //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������

    OSStart(&err);                                                          //�����������������uC/OS-III���ƣ�

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

    BSP_Init();                                                 //�弶��ʼ��
    CPU_Init();                                                 //��ʼ�� CPU �����ʱ��������ж�ʱ���������������

    cpu_clk_freq = BSP_CPU_ClkFreq();                           //��ȡ CPU �ں�ʱ��Ƶ�ʣ�SysTick ����ʱ�ӣ�
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        //�����û��趨��ʱ�ӽ���Ƶ�ʼ��� SysTick ��ʱ���ļ���ֵ
    OS_CPU_SysTickInit(cnts);                                   //���� SysTick ��ʼ�����������ö�ʱ������ֵ��������ʱ��

    Mem_Init();                                                 //��ʼ���ڴ������������ڴ�غ��ڴ�ر�

#if OS_CFG_STAT_TASK_EN > 0u                                    //���ʹ�ܣ�Ĭ��ʹ�ܣ���ͳ������
    OSStatTaskCPUUsageInit(&err);                               //����û��Ӧ������ֻ�п�����������ʱ CPU �ģ����
#endif                                                          //���������� OS_Stat_IdleCtrMax ��ֵ��Ϊ������� CPU 
                                                                //ʹ����ʹ�ã���
    CPU_IntDisMeasMaxCurReset();                                //��λ�����㣩��ǰ�����ж�ʱ��

    
    /* ����ʱ��Ƭ��ת���� */		
    OSSchedRoundRobinCfg((CPU_BOOLEAN   )DEF_ENABLED,          //ʹ��ʱ��Ƭ��ת����
		                     (OS_TICK       )0,                    //�� OSCfg_TickRate_Hz / 10 ��ΪĬ��ʱ��Ƭֵ
												 (OS_ERR       *)&err );               //���ش�������

		/* ���������ź��� mutex */
    OSMutexCreate ((OS_MUTEX  *)&List,           //ָ���ź���������ָ��
                   (CPU_CHAR  *)"List", //�ź���������
                   (OS_ERR    *)&err);            //��������
									 
				/* ���������ź��� mutex */
    OSMutexCreate ((OS_MUTEX  *)&Usart,           //ָ���ź���������ָ��
                   (CPU_CHAR  *)"Usart", //�ź���������
                   (OS_ERR    *)&err);            //��������
									 
		/* ���������ź��� mutex */
    OSMutexCreate ((OS_MUTEX  *)&Socket,           //ָ���ź���������ָ��
                   (CPU_CHAR  *)"Socket", //�ź���������
                   (OS_ERR    *)&err);            //��������									 
									 
	

	
     /*������ѯ��ѯUsart�����Ƿ�����������*/		
    OSTaskCreate((OS_TCB     *)&AppTaskTCPServerTCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App_Task_TCP_Server",                             //��������
                 (OS_TASK_PTR ) AppTaskTCPServer,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_TCP_SERVER_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskTCPServerStk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_TCP_SERVERT_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_TCP_SERVERT_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 50u,                                        //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);     
	
  						 
     /*������ѯ��ѯUsart�����Ƿ�����������*/		
    OSTaskCreate((OS_TCB     *)&AppTaskUSART1CheckTCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App_Task_Check_Device",                             //��������
                 (OS_TASK_PTR ) AppTaskUSART1Check,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_USART1_CHECK_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskUSART1CheckStk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_USART1_CHECK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_USART1_CHECK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 50u,                                        //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������

//     /*������ѯ��ѯUsart�����Ƿ�����������*/		
//    OSTaskCreate((OS_TCB     *)&AppTaskPollDeviceTCB,                             //������ƿ��ַ
//                 (CPU_CHAR   *)"App_Task_Poll_Device",                             //��������
//                 (OS_TASK_PTR ) AppTaskTCPPollDevice,                                //������
//                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
//                 (OS_PRIO     ) APP_TASK_POLL_DEVICE_PRIO,                         //��������ȼ�
//                 (CPU_STK    *)&AppTaskTCPPollDeviceStk[0],                          //�����ջ�Ļ���ַ
//                 (CPU_STK_SIZE) APP_TASK_TCP_POLL_DEVICE_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
//                 (CPU_STK_SIZE) APP_TASK_TCP_POLL_DEVICE_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
//                 (OS_MSG_QTY  ) 50u,                                        //����ɽ��յ������Ϣ��
//                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
//                 (void       *) 0,                                          //������չ��0����չ��
//                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
//                 (OS_ERR     *)&err);                                       //���ش�������


								 
														 
		OSTaskDel ( 0, & err );                     //ɾ����ʼ������������������
		
		
}

/*
*********************************************************************************************************
*                                 USART CHECK TASK
*********************************************************************************************************
*/

//static  void  AppTaskTCPPollDevice( void * p_arg )
//{
//	(void)p_arg;

//	while (DEF_TRUE) 
//	{ 
//	}
//}



static  void  AppTaskUSART1Check( void * p_arg )
{
	OS_ERR      err;
	CPU_SR_ALLOC();
	
//	uint8_t i=0;

	uint8_t Check_Temp[20]={0};
	
	(void)p_arg;
			 
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
	
		  OSTaskSemPend ((OS_TICK   )3000,                     //�����޵ȴ�
									   (OS_OPT    )OS_OPT_PEND_BLOCKING,  //����ź��������þ͵ȴ�
									   (CPU_TS   *)0,                   //��ȡ�ź�����������ʱ���
									   (OS_ERR   *)&err);                 //���ش�������			
//	  	OSTimeDlyHMSM ( 0, 0, 3,0, OS_OPT_TIME_DLY, &err);	

			
		OS_CRITICAL_ENTER();                                       //�����ٽ�Σ����⴮�ڴ�ӡ�����
		
//		if(USART_Rx_Finsh)
//		{
//		  if(USART_Rx_Buffer[4] == 0x20)
//			{
//			  if(USART_Rx_Buffer[3] == 	Addr)
//				{
//				  if(Device_Exist)
//					{
//            Updata_Node(USART_Rx_Buffer);											
//					}
//					else
//					{
//            Insert_Node(USART_Rx_Buffer); 						
//					}
//				}
//				else
//				{
//					if(Device_Exist)
//					{
//		        Delete_Node(Addr);							
//					}
//				}
//			}
//			else
//			{
//			
//			
//			}
//			memset(USART_Rx_Buffer,0,USART_Rx_Count);
//			USART_Rx_Count = 0;
//			USART_Rx_Finsh = 0;
//		}
//		
//		
////		Find_Device=(*(Msg+3)==Addr?1:0);
////		
////		
    if(Device_Exist)
		{ 
			if(Find_Device)        //�豸�����������Ҳ����лظ�
			{
			//�Ա��豸��״̬��Ϣ�������Ƿ�����IO��ӡ���״̬����			
        Updata_Node(USART_Rx_Buffer);		
			}
			else                 //�豸�������������ǲ���û�лظ�
			{
			//���豸����offline������ɾ���ڵ�
		    Delete_Node(Addr);		
			}
		  
		}
		else 
		{
			if(Find_Device)    //�豸���������������ǲ����лظ�
			{
			//������µ��豸���룬��Ҫ����ڵ㣬����״̬
        Insert_Node(USART_Rx_Buffer); 	
			}
		}

		memset(USART_Rx_Buffer,0,USART_Rx_Count);
		USART_Rx_Count = 0;
		Find_Device = 0;
		
		macLED2_TOGGLE();	
		OS_CRITICAL_EXIT();                                        //�˳��ٽ��							
	  }
}

static  void  AppTaskTCPServer (void *p_arg)
{
	
	OS_ERR      err;
	(void)p_arg;
		
	Load_Net_Parameters();		//װ���������	
	W5500_Hardware_Reset();		//Ӳ����λW5500	
	W5500_Initialization();		//W5500��ʼ����
	
	
	while (DEF_TRUE)
	{
		W5500_Socket_Set();            //W5500�˿ڳ�ʼ������		
		
		if(W5500_Interrupt)            //���W5500_Interrupt��־λ��λ����ʾ�жϷ���������W5500�ж�		
		{
			W5500_Interrupt_Process(); //W5500�жϴ������
		}
		if((S0_Data & S_RECEIVE) == S_RECEIVE)//���Socket0���յ�����
		{
			S0_Data&=~S_RECEIVE;              //����������ݱ�־λ
			Process_Socket_Recive_Data(0);           //W5500����������ݵĺ���		
		}
//		else if(time >= 500)                  //��ʱ�����ַ���
//		{
//			time=0;                           //���time                     
//			if(S0_State == (S_INIT|S_CONN))   //����˿ڳ�ʼ���ɹ��������ӳɹ�
//			{
//				S0_Data&=~S_TRANSMITOK;	                                             //������ͱ�־	
//				Write_SOCK_Data_Buffer(0, TxPtr, strlen((const char*)TxPtr));  //�˿�0��������
//			}			                      
//		}
		OSTimeDlyHMSM ( 0, 0, 0,10, OS_OPT_TIME_DLY, &err);	
	}
}	










