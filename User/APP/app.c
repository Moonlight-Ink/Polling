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
OS_SEM  SemOfPoll;
OS_MEM  Mem;                    //�����ڴ�������

uint8_t Array [ 3 ] [ 100 ];   //�����ڴ������С
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

static  OS_TCB   AppTaskStartTCB;    //������ƿ�

       OS_TCB AppTaskCheckDeviceTCB;
static OS_TCB AppTaskListTCB;
static OS_TCB AppTaskTCPSERVERTCB;



/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];       //�����ջ

static  CPU_STK  AppTaskCheckDeviceStk[ APP_TASK_CHECK_DEVICE_SIZE ];
static  CPU_STK  AppTaskListStk[ APP_TASK_LIST_SIZE ];
static  CPU_STK  AppTaskTCPServerStk[ APP_TASK_TCP_SERVERT_SIZE ];



/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart  (void *p_arg);               //����������

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
									 
									 
		/* �����ڴ������� mem */
		OSMemCreate ((OS_MEM      *)&Mem,             //ָ���ڴ�������
								 (CPU_CHAR    *)"Mem For Test",   //�����ڴ�������
								 (void        *)Array,          //�ڴ�������׵�ַ
								 (OS_MEM_QTY   )3,               //�ڴ�������ڴ����Ŀ
								 (OS_MEM_SIZE  )100,                //�ڴ����ֽ���Ŀ
								 (OS_ERR      *)&err);            //���ش�������
	
  						 
     /*������ѯ��ѯUsart�����Ƿ�����������*/		
    OSTaskCreate((OS_TCB     *)&AppTaskCheckDeviceTCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App_Task_Check_Device",                             //��������
                 (OS_TASK_PTR ) AppTaskCheckDevice,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_CHECK_DEVICE_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskCheckDeviceStk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_CHECK_DEVICE_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_CHECK_DEVICE_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 50u,                                        //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������


		/* ���� AppTaskPend ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskListTCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Pend",                             //��������
                 (OS_TASK_PTR ) AppTaskList,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_LIST_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskListStk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_LIST_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_LIST_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 50u,                                        //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������

     /*������ѯ��ѯUsart�����Ƿ�����������*/		
    OSTaskCreate((OS_TCB     *)&AppTaskTCPSERVERTCB,                             //������ƿ��ַ
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


								 
														 
		OSTaskDel ( 0, & err );                     //ɾ����ʼ������������������
		
		
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
			OSMutexPend ((OS_MUTEX  *)&List,                  //���뻥���ź��� mutex
									 (OS_TICK    )0,                       //�����޵ȴ�
									 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //������벻���Ͷ�������
									 (CPU_TS    *)0,                       //������ʱ���
									 (OS_ERR    *)&err);                   //���ش�������	

      Device_Exist=Find_Node(Addr);
			
		  OSMutexPost ((OS_MUTEX  *)&List,                 //�ͷŻ����ź��� mutex
								   (OS_OPT     )OS_OPT_POST_NONE,       //�����������
								   (OS_ERR    *)&err);   	
			
			Check_Temp[0]=0xad;
			Check_Temp[1]=0xda;
			Check_Temp[2]=0x02;
			Check_Temp[3]=Addr;
			Check_Temp[4]=0x20;
			Check_Temp[5]=0x03;
		 	
      USART1_Send_Data(Check_Temp,6);


			
		/* �������񣬵ȴ�������Ϣ */
		 Msg = OSTaskQPend ((OS_TICK        )5000,                    //�����޵ȴ�
											  (OS_OPT         )OS_OPT_PEND_BLOCKING, //û����Ϣ����������
											  (OS_MSG_SIZE   *)&Msg_size,            //������Ϣ����
											  (CPU_TS        *)0,                    //������Ϣ��������ʱ���
											  (OS_ERR        *)&err);                //���ش�������
			
		OS_CRITICAL_ENTER();                                       //�����ٽ�Σ����⴮�ڴ�ӡ�����
		
		Find_Device=(*(Msg+3)==Addr?1:0);
		
		
    if(Device_Exist)
		{ 
			if(Find_Device)        //�豸�����������Ҳ����лظ�
			{
			//�Ա��豸��״̬��Ϣ�������Ƿ�����IO��ӡ���״̬����
				
			  OSMutexPend ((OS_MUTEX  *)&List,                  //���뻥���ź��� mutex
									   (OS_TICK    )0,                       //�����޵ȴ�
									   (OS_OPT     )OS_OPT_PEND_BLOCKING,    //������벻���Ͷ�������
									   (CPU_TS    *)0,                       //������ʱ���
									   (OS_ERR    *)&err);                   //���ش�������		
				
        Updata_Node(Msg);

		    OSMutexPost ((OS_MUTEX  *)&List,                 //�ͷŻ����ź��� mutex
								     (OS_OPT     )OS_OPT_POST_NONE,       //�����������
								     (OS_ERR    *)&err); 				
			
			}
			else                 //�豸�������������ǲ���û�лظ�
			{
			//���豸����offline������ɾ���ڵ�
				
        OSMutexPend ((OS_MUTEX  *)&List,                  //���뻥���ź��� mutex
									   (OS_TICK    )0,                       //�����޵ȴ�
									   (OS_OPT     )OS_OPT_PEND_BLOCKING,    //������벻���Ͷ�������
									   (CPU_TS    *)0,                       //������ʱ���
									   (OS_ERR    *)&err);                   //���ش�������		

		    Delete_Node(Addr);		

		    OSMutexPost ((OS_MUTEX  *)&List,                 //�ͷŻ����ź��� mutex
								     (OS_OPT     )OS_OPT_POST_NONE,       //�����������
								     (OS_ERR    *)&err); 	
			}
		  
		}
		else 
		{
			if(Find_Device)    //�豸���������������ǲ����лظ�
			{
			//������µ��豸���룬��Ҫ����ڵ㣬����״̬
			  OSMutexPend ((OS_MUTEX  *)&List,                  //���뻥���ź��� mutex
									   (OS_TICK    )0,                       //�����޵ȴ�
									   (OS_OPT     )OS_OPT_PEND_BLOCKING,    //������벻���Ͷ�������
									   (CPU_TS    *)0,                       //������ʱ���
									   (OS_ERR    *)&err);                   //���ش�������		

        Insert_Node(Msg); 
				
		    OSMutexPost ((OS_MUTEX  *)&List,                 //�ͷŻ����ź��� mutex
								     (OS_OPT     )OS_OPT_POST_NONE,       //�����������
								     (OS_ERR    *)&err); 				
				
				
			}
			else               //�豸����������������Ҳû�лظ�   
			{
			//û������豸������������һ���豸
			
			}	
		}


		/* �˻��ڴ�� */
		OSMemPut ((OS_MEM  *)&Mem,                                 //ָ���ڴ�������
							(void    *)Msg,                                 //�ڴ����׵�ַ
							(OS_ERR  *)&err);		                             //���ش�������				

		macLED2_TOGGLE();	
		OS_CRITICAL_EXIT();                                        //�˳��ٽ��							
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
/* �������񣬵ȴ�������Ϣ */
		 List_Msg = OSTaskQPend ((OS_TICK        )0,                    //�����޵ȴ�
											       (OS_OPT         )OS_OPT_PEND_BLOCKING, //û����Ϣ����������
											       (OS_MSG_SIZE   *)&List_Msg_Size,            //������Ϣ����
											       (CPU_TS        *)0,                    //������Ϣ��������ʱ���
											       (OS_ERR        *)&err);                //���ش����

		OS_CRITICAL_ENTER();      		
	  OSMutexPend ((OS_MUTEX  *)&List,                  //���뻥���ź��� mutex
								 (OS_TICK    )0,                       //�����޵ȴ�
								 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //������벻���Ͷ�������
								 (CPU_TS    *)0,                       //������ʱ���
								 (OS_ERR    *)&err);                   //���ش�������		
	
	  USART1_Send_Data(List_Msg,(u16)List_Msg_Size);
		macLED1_TOGGLE();	
		OSMutexPost ((OS_MUTEX  *)&List,                 //�ͷŻ����ź��� mutex
								 (OS_OPT     )OS_OPT_POST_NONE,       //�����������
								 (OS_ERR    *)&err);                  //���ش�������	

		
		
		
		OSMemPut ((OS_MEM  *)&Mem,                                 //ָ���ڴ�������
							(void    *)List_Msg,                                 //�ڴ����׵�ַ
							(OS_ERR  *)&err);		                             //���ش�������	
		
    OS_CRITICAL_EXIT();                                        //�˳��ٽ��							
		
	}
}

static  void  AppTaskTCPServer (void *p_arg)
{
	
	OS_ERR      err;	
	(void)p_arg;
	
	
	W5500_GPIO_Init();
	W5500_Hardware_Reset();  //Ӳ����λW5500		
	W5500_Parameters_Init();		//W5500��ʼ����

	printf(" Ұ�������������ΪTCP �������������������ȴ�PC��ΪTCP Client�������� \r\n");
	printf(" W5500�����˿�Ϊ�� %d \r\n",local_port);
	printf(" ���ӳɹ���TCP Client�������ݸ�W5500��W5500�����ض�Ӧ���� \r\n");	
		
	
	while (DEF_TRUE)
	{
		do_tcp_server();
		OSTimeDlyHMSM ( 0, 0, 0,50, OS_OPT_TIME_DLY, &err);	
	}


}	










