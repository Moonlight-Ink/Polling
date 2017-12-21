#ifndef __USART1_H
#define	__USART1_H


#include "stm32f10x.h"
#include <stdio.h>

extern uint8_t USART_Rx_Finsh;
extern uint8_t USART_Rx_Buffer[20];
extern uint8_t USART_Rx_Count;



/**************************USART��������********************************/
#define             macUSART_BAUD_RATE                       9600

#define             macUSARTx                                USART1
#define             macUSART_APBxClock_FUN                   RCC_APB2PeriphClockCmd
#define             macUSART_CLK                             RCC_APB2Periph_USART1
#define             macUSART_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macUSART_GPIO_CLK                        RCC_APB2Periph_GPIOA     
#define             macUSART_TX_PORT                         GPIOA   
#define             macUSART_TX_PIN                          GPIO_Pin_9
#define             macUSART_RX_PORT                         GPIOA 
#define             macUSART_RX_PIN                          GPIO_Pin_10
#define             macUSART_IRQ                             USART1_IRQn
#define             macUSART_INT_FUN                         USART1_IRQHandler

void USART1_Send(uint8_t data);
void USART1_Send_Data(volatile u8 *buf,u8 len);
void USARTx_Config( void );
extern  void NVIC_Configuration(void);
void Check_Device_Cmd_Buffer(uint8_t Check_Addr);
void TCP_Cmd_Control_Device(uint8_t Control_Addr,uint8_t Control_Relay_Num,uint8_t Control_Relay_State);

#endif /* __USART1_H */
