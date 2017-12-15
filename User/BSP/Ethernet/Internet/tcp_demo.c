/**
************************************************************************************************
* @file   		tcp_demo.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-02-14
* @brief   		TCP ��ʾ����
* @attention  
************************************************************************************************
**/

#include <stdio.h>
#include <string.h>
#include "tcp_demo.h"
#include "W5500_conf.h"
#include "w5500.h"
#include "socket.h"
#include "bsp_cjson.h"




uint8 TCP_Rev_Buffer[500];				                              	         /*����һ��2KB�Ļ���*/
uint8 TCP_Send_Flag=0;
uint8 TCP_Send_Buffer[500]={0};
uint16 TCP_Send_Cnt=0;
/**
*@brief		TCP Server�ػ���ʾ������
*@param		��
*@return	��
*/
	uint16 len=0;  
void do_tcp_server(void)
{	

	switch(getSn_SR(SOCK_TCPS))											            	/*��ȡsocket��״̬*/
	{
		case SOCK_CLOSED:													                  /*socket���ڹر�״̬*/
			socket(SOCK_TCPS ,Sn_MR_TCP,local_port,Sn_MR_ND);	        /*��socket*/
		  break;     
    
		case SOCK_INIT:														                  /*socket�ѳ�ʼ��״̬*/
			listen(SOCK_TCPS);												                /*socket��������*/
		  break;
		
		case SOCK_ESTABLISHED:												              /*socket�������ӽ���״̬*/
		
			if(getSn_IR(SOCK_TCPS) & Sn_IR_CON)
			{
				setSn_IR(SOCK_TCPS, Sn_IR_CON);								          /*��������жϱ�־λ*/
			}
			len=getSn_RX_RSR(SOCK_TCPS);									            /*����lenΪ�ѽ������ݵĳ���*/
			if(len>0)
			{
				recv(SOCK_TCPS,TCP_Rev_Buffer,len);								              	/*��������Client������*/
				
				TCP_Cmd_Cjson_Analyze(TCP_Rev_Buffer,len);
				
				memset(TCP_Rev_Buffer,0,len);
				len=0;
				
//				TCP_Rev_Buffer[len]=0x00; 											                  /*����ַ���������*/
//				printf("%s\r\n",TCP_Rev_Buffer);
//				send(SOCK_TCPS,TCP_Rev_Buffer,len);									              /*��Client��������*/
		  }
			
			if(TCP_Send_Flag)
			{
			  TCP_Send_Flag = 0;
				send(SOCK_TCPS,TCP_Send_Buffer,TCP_Send_Cnt);
			 
				memset(TCP_Send_Buffer,0,TCP_Send_Cnt);
				TCP_Send_Cnt = 0;				
			}
			
			
		  break;
		
		case SOCK_CLOSE_WAIT:												                /*socket���ڵȴ��ر�״̬*/
			close(SOCK_TCPS);
		  break;
	}
}

///**
//*@brief		TCP Client�ػ���ʾ������
//*@param		��
//*@return	��
//*/
//void do_tcp_client(void)
//{	
//   uint16 len=0;	

//	switch(getSn_SR(SOCK_TCPC))								  				         /*��ȡsocket��״̬*/
//	{
//		case SOCK_CLOSED:											        		         /*socket���ڹر�״̬*/
//			socket(SOCK_TCPC,Sn_MR_TCP,local_port++,Sn_MR_ND);
//		  break;
//		
//		case SOCK_INIT:													        	         /*socket���ڳ�ʼ��״̬*/
//			connect(SOCK_TCPC,remote_ip,remote_port);                /*socket���ӷ�����*/ 
//		  break;
//		
//		case SOCK_ESTABLISHED: 												             /*socket�������ӽ���״̬*/
//			if(getSn_IR(SOCK_TCPC) & Sn_IR_CON)
//			{
//				setSn_IR(SOCK_TCPC, Sn_IR_CON); 							         /*��������жϱ�־λ*/
//			}
//		
//			len=getSn_RX_RSR(SOCK_TCPC); 								  	         /*����lenΪ�ѽ������ݵĳ���*/
//			if(len>0)
//			{
//				recv(SOCK_TCPC,buff,len); 							   		         /*��������Server������*/
//				buff[len]=0x00;  											                 /*����ַ���������*/
//				printf("%s\r\n",buff);
//				send(SOCK_TCPC,buff,len);								     	         /*��Server��������*/
//			}		  
//		  break;
//			
//		case SOCK_CLOSE_WAIT: 											    	         /*socket���ڵȴ��ر�״̬*/
//			close(SOCK_TCPC);
//		  break;

//	}
//}

