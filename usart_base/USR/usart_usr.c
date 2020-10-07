#include "usart_usr.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "g_errno.h"
#include "fifo.h"

rfifo_t tx_fifo, rx_fifo;

/*initialize usart1 */
void usart1_init(uint8_t ucPort, uint32_t ulBaudRate, uint8_t ucDataBits,
    uint8_t eParity )
{
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    USART_ClockInitTypeDef USART_ClockInitStruct;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* enable AFIO GPIO USART clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
                            RCC_APB2Periph_AFIO |
                            RCC_APB2Periph_USART1,ENABLE);


    /* config usart tx gpio */
    GPIO_InitStruct.GPIO_Pin = USART1_TX;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* config usart rx gpio */
    GPIO_InitStruct.GPIO_Pin = USART1_RX;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* config baudrate, mode, parity, word, stop bit */
    USART_InitStruct.USART_BaudRate = ulBaudRate;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStruct.USART_Parity = eParity ;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b ;
    USART_Init(USART1, &USART_InitStruct);

    /* usart1 clock init */
    USART_ClockStructInit(&USART_ClockInitStruct);
    USART_ClockInit(USART1, &USART_ClockInitStruct);

    /* enable usart1 */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);
    USART_ClearFlag(USART1, USART_FLAG_TC);

    /* enable usart interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    rfifo_init(&rx_fifo);
    rfifo_init(&tx_fifo);
}

/*
 * note: in fifo_push function will not suitable because it has fifo pop here
 *       and in interrupt routine. So the fifo pop may compete key data in
 *       read index.
 * TODO : find a method avoid compete data.
 * avoid compete : by using start flag of fifo, in main function when operate
 *      fifo we have disabled interrupt, eg: in usart_write_byte() function, so
 *      there is no compete
 */
uint32_t usart_write_bytes(uint8_t *data, uint16_t num)
{
    uint32_t cnt = 0, ret;

    if (!data || !num) {
        return cnt;
    }

    for (cnt = 0; cnt < num; cnt++) {
        do {
            ret = usart_write_byte(data[cnt]);
        } while(ret != ERR_OK);
    }

    return num;
}

uint32_t usart_write_byte(uint8_t data)
{
    if (rfifo_push(&tx_fifo, data) == ERR_OK) {
        if (tx_fifo.start) {
            rfifo_pop(&tx_fifo, &data);
            USART_SendData(USART1, data);
            USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
        }
    } else {
        return ERR_FAIL;
    }
    return ERR_OK;
}

uint32_t usart_get_rfifo_bytes(void)
{
    return rfifo_len(&rx_fifo);
}

uint32_t usart_read_bytes(uint8_t *data, uint16_t num)
{
    uint16_t cnt = 0;
    uint8_t tmp;

    if (!data || !num || (rfifo_len(&rx_fifo) == 0)) {
        return 0;
    }

    for (; cnt < MIN(rfifo_len(&rx_fifo), num); cnt++) {
        if (rfifo_pop(&rx_fifo, &tmp) == ERR_OK) {
            data[cnt] = tmp;
        } else {
            break;
        }
    }

    return cnt;
}

uint32_t usart_read_byte(uint8_t *data)
{
    if (!data || (rfifo_len(&rx_fifo) == 0)) {
        return ERR_FAIL;
    }

    rfifo_pop(&rx_fifo, data);

    return ERR_OK;
}

void USART1_IRQHandler(void)
{
    uint8_t ch;
    /* receive data, push into rx fifo */
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        ch = (u8)USART_ReceiveData(USART1);
        rfifo_push(&rx_fifo, ch);
    }

    /* send data, pop from tx fifo */
    if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
    {
        /* pop data success, send poped data, or disable interrupt */
        if(rfifo_pop(&tx_fifo, &ch) == ERR_OK) {
            USART_SendData(USART1, ch);
        } else {
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
        }
    }
}


u8 strcount(const u8 *ch)
{
    u8 counter = 0;
    while( *(ch+counter) != 0x0d)
    {
        counter++;
    }
    return counter;
}


