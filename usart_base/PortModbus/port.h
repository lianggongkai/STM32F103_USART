#ifndef _PORT_H_
#define _PORT_H_

#include "inttypes.h"
#include "stdbool.h"
#include "assert.h"
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "misc.h"


typedef u8 UCHAR;
typedef s8 CHAR;
typedef u16 USHORT;
typedef s16 SHORT;
typedef u32 ULONG;
typedef s32 LONG;

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif


#define  ENTER_CRITICAL_SECTION()	__set_PRIMASK(1);//关中断
#define  EXIT_CRITICAL_SECTION()	__set_PRIMASK(0);//开中断
//#define  asser(expr) assert_param(expr);

#endif

