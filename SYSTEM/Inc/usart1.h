#ifndef __USART_H
#define __USART_H
#include "stm32f10x.h"                  // Device header
#include <stdarg.h>
#include <stdio.h>

#define USART_RCC  RCC_APB2Periph_USART1
#define USART_GPIO_RCC RCC_APB2Periph_GPIOA
#define USART_GPIO GPIOA
#define USART_X    USART1
#define USART_IRQn USART1_IRQn
#define USART_IRQHandler USART_IRQHandler
#define USART_TX   GPIO_Pin_9
#define USART_RX   GPIO_Pin_10

void Usart_Init(void);
void Send_Byte(uint8_t Byte);
void Send_Array(uint8_t *Array,uint16_t Length);
void Send_String(char*String);
void Send_Number(uint32_t Num,uint8_t Length);
void Usart_Printf(char *format,...);
uint8_t Usart_GetRxFlag(void);
uint8_t Usart_GetRxData(void);

uint8_t Get_id_Flag(void);//将获取id_Flag封装成函数
float RxPacket_Data_Handle(void);//数据包换算处理
#endif
