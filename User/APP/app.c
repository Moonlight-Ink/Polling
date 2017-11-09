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
OS_SEM  SemOfPoll;
OS_MEM  Mem;                    //�����ڴ�������
uint8_t Array [ 3 ] [ 100 ];   //�����ڴ������С
uint8_t Mem_Flag=0;

uint8_t Addr=0;
uint8_t Rx_Cnt=0;
uint8_t Check_Cnt=0;
//uint8_t Temp_Cnt=0;
uint32_t *Mem_blk,*Mem_blk1;

uint8_t Poll_Flag=1;
//uint8_t Temp_Buf[20]={0};
//uint8_t Rx_Buf[20]={0};
/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/

static  OS_TCB   AppTaskStartTCB;    //������ƿ�

static  OS_TCB   AppTaskUsartCheckTCB;
static  OS_TCB   AppTaskUsartPendTCB;
static  OS_TCB   AppTaskPollTCB;
static  OS_TCB   AppTaskPollPendTCB;

//OS_TCB   AppTaskPostTCB;
//OS_TCB   AppTaskPendTCB;

/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];       //�����ջ

static  CPU_STK  AppTaskUsartCheckStk[ APP_TASK_USART_CHECK_SIZE ];
static  CPU_STK  AppTaskUsartPendStk[ APP_TASK_USART_PEND_SIZE ];
static  CPU_STK  AppTaskPollStk[ APP_TASK_POLL_SIZE ];
static  CPU_STK  AppTaskPollPendStk[ APP_TASK_POLL_PEND_SIZE ];


//static  CPU_STK  AppTaskPostStk [ APP_TASK_POST_STK_SIZE ];
//static  CPU_STK  AppTaskPendStk [ APP_TASK_PEND_STK_SIZE ];


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart  (void *p_arg);               //����������

static  void  AppTaskUsartCheck (void *p_arg);
static  void  AppTaskUsartPend (void *p_arg);
static  void  AppTaskPoll (void *p_arg);
static  void  AppTaskPollPend (void *p_arg);
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


		/* �����ڴ������� mem */
		OSMemCreate ((OS_MEM      *)&Mem,             //ָ���ڴ�������
								 (CPU_CHAR    *)"Mem For Test",   //�����ڴ�������
								 (void        *)Array,          //�ڴ�������׵�ַ
								 (OS_MEM_QTY   )3,               //�ڴ�������ڴ����Ŀ
								 (OS_MEM_SIZE  )100,                //�ڴ����ֽ���Ŀ
								 (OS_ERR      *)&err);            //���ش�������
							 
		/* ������ֵ�ź��� SemOfKey */
    OSSemCreate((OS_SEM      *)&SemOfPoll,    //ָ���ź���������ָ��
               (CPU_CHAR    *)"SemOfPoll",    //�ź���������
               (OS_SEM_CTR   )0,             //�ź���������ָʾ�¼����������Ը�ֵΪ0����ʾ�¼���û�з���
               (OS_ERR      *)&err);         //��������								 
								 
								 
//			/* ���� AppTaskPost ���� */
//    OSTaskCreate((OS_TCB     *)&AppTaskPostTCB,                             //������ƿ��ַ
//                 (CPU_CHAR   *)"App Task Post",                             //��������
//                 (OS_TASK_PTR ) AppTaskPost,                                //������
//                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
//                 (OS_PRIO     ) APP_TASK_POST_PRIO,                         //��������ȼ�
//                 (CPU_STK    *)&AppTaskPostStk[0],                          //�����ջ�Ļ���ַ
//                 (CPU_STK_SIZE) APP_TASK_POST_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
//                 (CPU_STK_SIZE) APP_TASK_POST_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
//                 (OS_MSG_QTY  ) 5u,                                         //����ɽ��յ������Ϣ��
//                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
//                 (void       *) 0,                                          //������չ��0����չ��
//                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
//                 (OS_ERR     *)&err);                                       //���ش�������

//		/* ���� AppTaskPend ���� */
//    OSTaskCreate((OS_TCB     *)&AppTaskPendTCB,                             //������ƿ��ַ
//                 (CPU_CHAR   *)"App Task Pend",                             //��������
//                 (OS_TASK_PTR ) AppTaskPend,                                //������
//                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
//                 (OS_PRIO     ) APP_TASK_PEND_PRIO,                         //��������ȼ�
//                 (CPU_STK    *)&AppTaskPendStk[0],                          //�����ջ�Ļ���ַ
//                 (CPU_STK_SIZE) APP_TASK_PEND_STK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
//                 (CPU_STK_SIZE) APP_TASK_PEND_STK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
//                 (OS_MSG_QTY  ) 50u,                                        //����ɽ��յ������Ϣ��
//                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
//                 (void       *) 0,                                          //������չ��0����չ��
//                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
//                 (OS_ERR     *)&err);                                       //���ش�������
   						 
     /*������ѯ��ѯUsart�����Ƿ�����������*/		
    OSTaskCreate((OS_TCB     *)&AppTaskUsartCheckTCB,                             //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Usart Check",                             //��������
                 (OS_TASK_PTR ) AppTaskUsartCheck,                                //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_USART_CHECK_PRIO,                         //��������ȼ�
                 (CPU_STK    *)&AppTaskUsartCheckStk[0],                          //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_USART_CHECK_SIZE / 10,                //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_USART_CHECK_SIZE,                     //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 50u,                                        //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);                                       //���ش�������
	 
	/* ���� AppTaskUsart ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskUsartPendTCB,                            //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Usart Pend",                            //��������
                 (OS_TASK_PTR ) AppTaskUsartPend,                               //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_USART_PEND_PRIO,                        //��������ȼ�
                 (CPU_STK    *)&AppTaskUsartPendStk[0],                         //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_USART_PEND_SIZE / 10,               //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_USART_PEND_SIZE,                    //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 50u,                                        //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);       								 
								 					 
	/* ���� AppTaskUsart ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskPollTCB,                            //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Poll",                            //��������
                 (OS_TASK_PTR ) AppTaskPoll,                               //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_POLL_PRIO,                        //��������ȼ�
                 (CPU_STK    *)&AppTaskPollStk[0],                         //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_POLL_SIZE / 10,               //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_POLL_SIZE,                    //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 50u,                                        //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);     

	/* ���� AppTaskUsart ���� */
    OSTaskCreate((OS_TCB     *)&AppTaskPollPendTCB,                            //������ƿ��ַ
                 (CPU_CHAR   *)"App Task Poll Pend",                            //��������
                 (OS_TASK_PTR ) AppTaskPollPend,                               //������
                 (void       *) 0,                                          //���ݸ����������β�p_arg����ʵ��
                 (OS_PRIO     ) APP_TASK_POLL_PEND_PRIO,                        //��������ȼ�
                 (CPU_STK    *)&AppTaskPollPendStk[0],                         //�����ջ�Ļ���ַ
                 (CPU_STK_SIZE) APP_TASK_POLL_PEND_SIZE / 10,               //�����ջ�ռ�ʣ��1/10ʱ����������
                 (CPU_STK_SIZE) APP_TASK_POLL_PEND_SIZE,                    //�����ջ�ռ䣨��λ��sizeof(CPU_STK)��
                 (OS_MSG_QTY  ) 50u,                                        //����ɽ��յ������Ϣ��
                 (OS_TICK     ) 0u,                                         //�����ʱ��Ƭ��������0��Ĭ��ֵOSCfg_TickRate_Hz/10��
                 (void       *) 0,                                          //������չ��0����չ��
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), //����ѡ��
                 (OS_ERR     *)&err);     								 
								 
								 
								 
								 
		OSTaskDel ( 0, & err );                     //ɾ����ʼ������������������
		
		
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
//	while (DEF_TRUE) {                                             //������
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
//		  OSTaskQPost ((OS_TCB      *)&AppTaskPendTCB,                 //Ŀ������Ŀ��ƿ�
//							  	 (void        *)&flag,             //��Ϣ����
//								   (OS_MSG_SIZE  )sizeof(flag),  //��Ϣ����
//								   (OS_OPT       )OS_OPT_POST_FIFO,                //������������Ϣ���е���ڶ�
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
//	while (DEF_TRUE) {                                           //������
//		/* �������񣬵ȴ�������Ϣ */
//		pMsg = OSTaskQPend ((OS_TICK        )0,                    //�����޵ȴ�
//											  (OS_OPT         )OS_OPT_PEND_BLOCKING, //û����Ϣ����������
//											  (OS_MSG_SIZE   *)&msg_size,            //������Ϣ����
//											  (CPU_TS        *)0,                  //������Ϣ��������ʱ���
//											  (OS_ERR        *)&err);                //���ش�������

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
static  void  AppTaskUsartCheck ( void * p_arg )
{
	OS_ERR      err;

	
	(void)p_arg;

					 
	while (DEF_TRUE) 
		{                                             //������
		  Check_Cnt=Rx_Cnt;
     
		  OSTimeDlyHMSM ( 0, 0, 0,10, OS_OPT_TIME_DLY, &err );
			
			if(Check_Cnt==Rx_Cnt && Rx_Cnt)
			{
				if(Poll_Flag)
				{
					OSTaskQPost ((OS_TCB      *)&AppTaskPollPendTCB,      //Ŀ������Ŀ��ƿ�
											 (void        *)Mem_blk,             //��Ϣ���ݵ��׵�ַ
											 (OS_MSG_SIZE  )Rx_Cnt,                     //��Ϣ����
											 (OS_OPT       )OS_OPT_POST_FIFO,      //������������Ϣ���е���ڶ�
											 (OS_ERR      *)&err);                 //���ش�������				
				}
				else
				{
	 			    /* ����������Ϣ������ AppTaskUsart */
					OSTaskQPost ((OS_TCB      *)&AppTaskUsartPendTCB,      //Ŀ������Ŀ��ƿ�
											 (void        *)Mem_blk,             //��Ϣ���ݵ��׵�ַ
											 (OS_MSG_SIZE  )Rx_Cnt,                     //��Ϣ����
											 (OS_OPT       )OS_OPT_POST_FIFO,      //������������Ϣ���е���ڶ�
											 (OS_ERR      *)&err);                 //���ش�������
				}				 
		 				
			}
	  }
}

static  void  AppTaskUsartPend ( void * p_arg )
{
  
	OS_ERR         err;
	OS_MSG_SIZE    Msg_size;
	CPU_SR_ALLOC();
	
	uint32_t *Msg;

	
	(void)p_arg;

					 
	while (DEF_TRUE) {                                           //������
		/* �������񣬵ȴ�������Ϣ */
		 Msg = OSTaskQPend ((OS_TICK        )0,                    //�����޵ȴ�
											  (OS_OPT         )OS_OPT_PEND_BLOCKING, //û����Ϣ����������
											  (OS_MSG_SIZE   *)&Msg_size,            //������Ϣ����
											  (CPU_TS        *)0,                    //������Ϣ��������ʱ���
											  (OS_ERR        *)&err);                //���ش�������

		OS_CRITICAL_ENTER();                                       //�����ٽ�Σ����⴮�ڴ�ӡ�����
		
//		for(i=0;i<Msg_size;i++)
//		{
//			Rx_Buf[i]=*(Msg+i);
//		}
		//USART1_Send_Data(Rx_Buf,(u16)Msg_size);
		USART1_Send_Data(Msg,(u16)Msg_size);		
		OS_CRITICAL_EXIT();                                        //�˳��ٽ��
		
		/* �˻��ڴ�� */
		OSMemPut ((OS_MEM  *)&Mem,                                 //ָ���ڴ�������
							(void    *)Msg,                                 //�ڴ����׵�ַ
							(OS_ERR  *)&err);		                             //���ش�������
		Rx_Cnt=0;		
		Mem_Flag=0;					
		
	}
	
}

static void  AppTaskPoll(void *p_arg)
{
  OS_ERR err;
	
	uint8_t Val=0;
  
	uint8_t Poll_Buf[10]={0};
	
	(void)p_arg;
	
	while(DEF_TRUE)
	{
		if(Addr>0x3f)
		{
		  Addr=0;		
		}
		
		Poll_Buf[0]=0xad;
		Poll_Buf[1]=0xda;
		Poll_Buf[2]=0x03;
		Poll_Buf[3]=Addr;
		Poll_Buf[4]=0x20;
		Poll_Buf[5]=0xff;		
		

////		USART1_Send_Data(Mem_blk1,6);
		USART1_Send_Data1(Poll_Buf,6);
////		Val=0;
////		memset(cc,0,10);

		//OSTimeDlyHMSM ( 0, 0, 1, 0, OS_OPT_TIME_DLY, &err );
		OSSemPend((OS_SEM *)&SemOfPoll,
		          (OS_TICK )1000,
		          (OS_OPT  )OS_OPT_PEND_BLOCKING,
		          (CPU_TS *)0,
		          (OS_ERR *)&err);
		Addr++;					
	}


}

static void  AppTaskPollPend(void *p_arg)
{
	OS_ERR         err;	
	
	OS_MSG_SIZE    Msg_size;
	
	CPU_SR_ALLOC();
	
	uint32_t *Msg;
	
	(void)p_arg;
	
	while (DEF_TRUE) {                                           //������
		/* �������񣬵ȴ�������Ϣ */
		 Msg = OSTaskQPend ((OS_TICK        )0,                    //�����޵ȴ�
											  (OS_OPT         )OS_OPT_PEND_BLOCKING, //û����Ϣ����������
											  (OS_MSG_SIZE   *)&Msg_size,            //������Ϣ����
											  (CPU_TS        *)0,                    //������Ϣ��������ʱ���
											  (OS_ERR        *)&err);                //���ش�������
		
		
		OS_CRITICAL_ENTER(); 
		if(*(Msg+3)==Addr)
		{
		 OSSemPost((OS_SEM *)&SemOfPoll,
			         (OS_OPT  )OS_OPT_POST_ALL,
			         (OS_ERR *)&err);
		 macLED1_TOGGLE();  	
		}
//		USART1_Send_Data(Msg,(u16)Msg_size);
//		USART1_Send(*(Msg+3));

		macLED2_TOGGLE();  	
		OS_CRITICAL_EXIT(); 


    
 		/* �˻��ڴ�� */
		OSMemPut ((OS_MEM  *)&Mem,                                 //ָ���ڴ�������
							(void    *)Msg,                                 //�ڴ����׵�ַ
							(OS_ERR  *)&err);		                             //���ش�������
		Rx_Cnt=0;		
		Mem_Flag=0;			

		
	}

	
}








