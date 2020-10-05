/*
Author : Program By Lianggongke;
Goal   : Led on off test
Key tip : First enable GPIO's RCC,
                    Second config the GPIO mode
                    Then you can use the GPIO
                    SysTick Config in two ways;
                    One : Use the CounterFlag(in this case can't enable TICKINT)
                    TWO : Use the SysTickHandler interrupt function to deal with;
                                ( In this case ,you must enable TICKINT )
*/

#include "stm32f10x.h"
//#include "stm32f10x_conf.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "systick_usr.h"
#include "input_usr.h"
#include "output_usr.h"
#include "usart_usr.h"
#include "string.h"
#include "mb.h"
#include "mbport.h"

#include "stdio.h"

#if 0
#pragma import(__use_no_semihosting_swi)
#pragma import(__use_no_semihosting)

_sys_exit(int x) {
    x = x;
}

struct __FILE  {
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};
/* FILE is typedef¡¯ d in stdio.h. */
FILE __stdout;

_ttywrch(int ch){
        ch = ch;
};
#endif

TimerType T1,T2,T3;
KeyType K1;
OutType RedLed,GreenLed,GreenLed1;
u16 Tx[1025] = { 1,2,3,4,5,6,7,8,9,10,11,23,13,45,67,8,9,0,123,345};
u16 Rx[1025] =  { 1000,2000};

float ff = 1.0f;
int main()
{
    int x = 0;
    char buffer[1000] = {0};
    SysTick_Init();
    InitKey(&K1,GPIOC, GPIO_Pin_5);
    InitOut(&RedLed,GPIOA,GPIO_Pin_8);
    InitOut(&GreenLed,GPIOD,GPIO_Pin_1);
    InitOut(&GreenLed1,GPIOB,GPIO_Pin_5);

    //eMBInit(MB_RTU,0X02,0X00,19200,MB_PAR_NONE);
    //eMBEnable(0, 19200, 8, MB_PAR_NONE);
    usart1_init(0, 38400, 8, 0);
//  Set(&RedLed);
//  Set(&GreenLed);
    if(!SetTimer(&T1,200)){
        return -1;
    }
    if(!SetTimer(&T2,100)){
        return -1;
    }

    EnableTimer(&T1);
    //USART1_NVIC_Init();
    while(1)
    {
        //eMBPoll();
        if(T1.Res){
            ResetTimer(&T1);
            EnableTimer(&T1);
            ALT(&GreenLed1);
            Tx[5]++;
            //sprintf(buffer, "%d * %d = %d\n", x, x, x * x);
            //sprintf(buffer, "hello-%d\n", x);
            sprintf(buffer, "hello cola-==%d\n", x % 10);
            x += 1;
            usart_write_bytes((uint8_t *)buffer);
        }
    }
}



