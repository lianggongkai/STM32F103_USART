#ifndef __USART_USR_H_
#define __USART_USR_H_
#include "stm32f10x_usart.h"

#define USART1_TX  GPIO_Pin_9
#define USART1_RX  GPIO_Pin_10

#ifdef _ALIENTEK
    #define LED        GPIO_Pin_8
    #define LED_PORT   GPIOA
#else
    #define LED        GPIO_Pin_5
    #define LED_PORT   GPIOB
#endif

void usart1_init(uint8_t ucPort, uint32_t ulBaudRate, uint8_t ucDataBits,
    uint8_t eParity);

uint32_t usart_write_bytes(uint8_t *data, uint16_t num);

uint32_t usart_write_byte(uint8_t data);

uint32_t usart_get_rfifo_bytes(void);

uint32_t usart_read_bytes(uint8_t *data, uint16_t num);

uint32_t usart_read_byte(uint8_t *data);

#endif

