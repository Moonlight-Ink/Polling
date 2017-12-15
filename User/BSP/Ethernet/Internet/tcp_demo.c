/**
************************************************************************************************
* @file   		tcp_demo.c
* @author  		WIZnet Software Team 
* @version 		V1.0
* @date    		2015-02-14
* @brief   		TCP 演示函数
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




uint8 TCP_Rev_Buffer[500];				                              	         /*定义一个2KB的缓存*/
uint8 TCP_Send_Flag=0;
uint8 TCP_Send_Buffer[500]={0};
uint16 TCP_Send_Cnt=0;
/**
*@brief		TCP Server回环演示函数。
*@param		无
*@return	无
*/
	uint16 len=0;  
void do_tcp_server(void)
{	

	switch(getSn_SR(SOCK_TCPS))											            	/*获取socket的状态*/
	{
		case SOCK_CLOSED:													                  /*socket处于关闭状态*/
			socket(SOCK_TCPS ,Sn_MR_TCP,local_port,Sn_MR_ND);	        /*打开socket*/
		  break;     
    
		case SOCK_INIT:														                  /*socket已初始化状态*/
			listen(SOCK_TCPS);												                /*socket建立监听*/
		  break;
		
		case SOCK_ESTABLISHED:												              /*socket处于连接建立状态*/
		
			if(getSn_IR(SOCK_TCPS) & Sn_IR_CON)
			{
				setSn_IR(SOCK_TCPS, Sn_IR_CON);								          /*清除接收中断标志位*/
			}
			len=getSn_RX_RSR(SOCK_TCPS);									            /*定义len为已接收数据的长度*/
			if(len>0)
			{
				recv(SOCK_TCPS,TCP_Rev_Buffer,len);								              	/*接收来自Client的数据*/
				
				TCP_Cmd_Cjson_Analyze(TCP_Rev_Buffer,len);
				
				memset(TCP_Rev_Buffer,0,len);
				len=0;
				
//				TCP_Rev_Buffer[len]=0x00; 											                  /*添加字符串结束符*/
//				printf("%s\r\n",TCP_Rev_Buffer);
//				send(SOCK_TCPS,TCP_Rev_Buffer,len);									              /*向Client发送数据*/
		  }
			
			if(TCP_Send_Flag)
			{
			  TCP_Send_Flag = 0;
				send(SOCK_TCPS,TCP_Send_Buffer,TCP_Send_Cnt);
			 
				memset(TCP_Send_Buffer,0,TCP_Send_Cnt);
				TCP_Send_Cnt = 0;				
			}
			
			
		  break;
		
		case SOCK_CLOSE_WAIT:												                /*socket处于等待关闭状态*/
			close(SOCK_TCPS);
		  break;
	}
}

///**
//*@brief		TCP Client回环演示函数。
//*@param		无
//*@return	无
//*/
//void do_tcp_client(void)
//{	
//   uint16 len=0;	

//	switch(getSn_SR(SOCK_TCPC))								  				         /*获取socket的状态*/
//	{
//		case SOCK_CLOSED:											        		         /*socket处于关闭状态*/
//			socket(SOCK_TCPC,Sn_MR_TCP,local_port++,Sn_MR_ND);
//		  break;
//		
//		case SOCK_INIT:													        	         /*socket处于初始化状态*/
//			connect(SOCK_TCPC,remote_ip,remote_port);                /*socket连接服务器*/ 
//		  break;
//		
//		case SOCK_ESTABLISHED: 												             /*socket处于连接建立状态*/
//			if(getSn_IR(SOCK_TCPC) & Sn_IR_CON)
//			{
//				setSn_IR(SOCK_TCPC, Sn_IR_CON); 							         /*清除接收中断标志位*/
//			}
//		
//			len=getSn_RX_RSR(SOCK_TCPC); 								  	         /*定义len为已接收数据的长度*/
//			if(len>0)
//			{
//				recv(SOCK_TCPC,buff,len); 							   		         /*接收来自Server的数据*/
//				buff[len]=0x00;  											                 /*添加字符串结束符*/
//				printf("%s\r\n",buff);
//				send(SOCK_TCPC,buff,len);								     	         /*向Server发送数据*/
//			}		  
//		  break;
//			
//		case SOCK_CLOSE_WAIT: 											    	         /*socket处于等待关闭状态*/
//			close(SOCK_TCPC);
//		  break;

//	}
//}

