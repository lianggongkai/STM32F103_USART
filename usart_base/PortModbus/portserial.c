/*
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2006 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: mbport.h,v 1.19 2010/06/06 13:54:40 wolti Exp $
 */
#include "mbport.h"
#include "output_usr.h"
#include "mbrtu.h"
#define USART1_TX  GPIO_Pin_9
#define USART1_RX  GPIO_Pin_10

BOOL xMBPortSerialInit( UCHAR ucPort, ULONG ulBaudRate,
												UCHAR ucDataBits, eMBParity eParity )
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	USART_ClockInitTypeDef USART_ClockInitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 打开发送接收脚的时钟，以及USART1的时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
							RCC_APB2Periph_AFIO |
							RCC_APB2Periph_USART1,ENABLE);


	/* 配置USART1的发送脚 */
	GPIO_InitStruct.GPIO_Pin = USART1_TX;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);

	/* 配置USART1的接收脚 */
	GPIO_InitStruct.GPIO_Pin = USART1_RX;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStruct);

	/* 配置USART1的波特率，模式，校验，字长等 */
	//USART_StructInit(&USART_InitStruct);
	USART_InitStruct.USART_BaudRate = ulBaudRate;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStruct.USART_Parity = eParity ;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b ;
	USART_Init(USART1,&USART_InitStruct);

	/* 配置USART1的时钟并初始化 */
	USART_ClockStructInit(&USART_ClockInitStruct);
	USART_ClockInit(USART1,&USART_ClockInitStruct);

	/* 打开USART1 */
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	USART_Cmd(USART1,ENABLE);
	USART_ClearFlag(USART1,USART_FLAG_TC);


	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	return true;
}

void vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
	if(xRxEnable){
		USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	}
	else{
		//USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);
	}
	if(xTxEnable){
		USART_ITConfig(USART1,USART_IT_TC,ENABLE);
	}
	else{
		USART_ITConfig(USART1,USART_IT_TC,DISABLE);
	}
}

extern OutType GreenLed,RedLed;
BOOL xMBPortSerialGetByte( CHAR * pucByte ){
	*pucByte = USART_ReceiveData(USART1);

	return true;
}

BOOL xMBPortSerialPutByte( CHAR ucByte ){
	USART_SendData(USART1,ucByte);
	/*while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {
  } */ //这个等待过程只可以加在不使用发送完成中断的情况。
	//移植时候要仔细检查，这个问题困扰了很大一阵子。
	return true;
}


/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call
 * xMBPortSerialPutByte( ) to send the character.
 */
static void prvvUARTTxReadyISR( void )
{
  pxMBFrameCBTransmitterEmpty();
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void prvvUARTRxISR( void )
{
  pxMBFrameCBByteReceived();
}


void USART1_IRQHandler_redefine_1111111111(void)
{
  if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
  {
		//ALT(&GreenLed);
		prvvUARTRxISR();// xMBRTUReceiveFSM();
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
  }

  if(USART_GetITStatus(USART1, USART_IT_TC) == SET)
  {
		//ALT(&RedLed);
    prvvUARTTxReadyISR();//xMBRTUTransmitFSM();
    USART_ClearITPendingBit(USART1, USART_IT_TC);
  }
}


