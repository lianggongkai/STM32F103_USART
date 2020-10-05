#ifndef _OUTPUT_USR_H_
#define _OUTPUT_USR_H_

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#define OutOn  1 //Out指令输出
#define OutOff 0 //Out指令不输出



typedef struct{
	u16 Cmd;
	u16 IsInit;
	u16 IsOut;
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;
} OutType;

typedef struct{
	OutType *pOut[256];
	u16 OutCount;
} OutConterType;

extern s8 InitOut(OutType* Out, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
extern void OutCheck(OutType* Out);
extern void OutAllCheck(void);
extern void Out(OutType* Output);
extern void ElseOut(OutType* Output);
extern void Set(OutType*);
extern void Reset(OutType*);
extern void ALT(OutType* Out);

#endif

