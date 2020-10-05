#include "usart_usr.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "g_errno.h"
#include "fifo.h"

#define _USART1_IRQ 1

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
    GPIO_Init(GPIOA,&GPIO_InitStruct);

    /* config usart rx gpio */
    GPIO_InitStruct.GPIO_Pin = USART1_RX;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA,&GPIO_InitStruct);

    /* config baudrate, mode, parity, word, stop bit */
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

    rfifo_init(&rx_fifo);
    rfifo_init(&tx_fifo);
}

#ifdef _USART1_IRQ //如果定义了串口1中断
void USART1_NVIC_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStruct;
    //选择串口中断通道
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    //设置优先级，最低优先级
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x0f;
    //设置次级优先级，最低优先级
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x0f;
    //开启中断通道
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    //更新中断控制器
    NVIC_Init(&NVIC_InitStruct);
    //关闭USART1的发送中断
    USART_ITConfig(USART1,USART_IT_TXE,DISABLE);
    //开启USART1的接收中断
    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);

}

/*
 * note: in fifo_push function will not suitable because it has fifo pop here
 *       and in interrupt routine. So the fifo pop may compete key data in
 *       read index.
 * TODO : find a method avoid compete data.
 */
uint32_t usart_write_bytes(uint8_t *data)
{
    uint32_t ret = ERR_OK;

    if (!data) {
        return ret;
    }

    for (; *data != '\0'; data++) {
        do {
            ret = usart_write_byte(*data);
        } while(ret != ERR_OK);
    }

    return ret;
}

uint32_t usart_write_byte(uint8_t data)
{
    if (rfifo_push(&tx_fifo, data) == ERR_OK) {
        if (tx_fifo.start) {
            //tx_fifo.start = 0;
            rfifo_pop(&tx_fifo, &data);
            USART_SendData(USART1, data);
            USART_ITConfig(USART1,USART_IT_TXE, ENABLE);
        }
    } else {
        return ERR_FAIL;
    }
    return ERR_OK;
}

uint8_t usart_read_bytes(uint8_t *data)
{
    uint8_t idx = 0, tmp;

    if (!data || (rfifo_len(&rx_fifo) == 0)) {
        return 0;
    }

    do {
        if (rfifo_pop(&rx_fifo, &tmp) == ERR_OK) {
            data[idx++] = tmp;
        } else {
            break;
        }
    } while(rfifo_len(&rx_fifo));
    USART_ITConfig(USART1,USART_IT_TXE,ENABLE);

    return idx;
}

uint8_t usart_read_byte(uint8_t *data)
{
    if (!data || (rfifo_len(&rx_fifo) == 0)) {
        return ERR_FAIL;
    }

    rfifo_pop(&rx_fifo, data);

    return ERR_OK;
}

#endif

void USART1_IRQHandler(void)
{
    u8 ch;
    //接收中断，向接收缓冲区Put数据
    if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)
    {
        ch = (u8)USART_ReceiveData(USART1);
        rfifo_push(&rx_fifo, ch);
    }
    //发送中断，从发送缓冲区Get数据
    if(USART_GetITStatus(USART1,USART_IT_TXE) != RESET)
    {
        if(rfifo_pop(&tx_fifo,&ch) == ERR_OK)//是否成功Get到数据
        {
            USART_SendData(USART1,ch);
            //tx_fifo.start = 0;
        }
        else{//如果没有Get到数据，说明发送缓冲区空了。关闭发送中断
            USART_ITConfig(USART1,USART_IT_TXE,DISABLE);
            //tx_fifo.start = 1;
        }
    }
    ch = 1;
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


