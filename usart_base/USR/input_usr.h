#ifndef _INPUT_USR_H_
#define _INPUT_USR_H_

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

//如果设置输入模式是上拉输入，则有输入是低电平
//如果设置输入模式是下拉输入，则有输入是高电平
#define SIGNAL_IN 0 //有输入
#define NO_SIGNAL 1 //无输入

enum KeyS{
	KeyUp = 0,
	KeyDown = 1
};

typedef struct{
	u16 Stat;
	u16 Filter;
	u16 IsDown;
	u16 IsInit;
	u32 DownTick;
	u32 UpTick;
	u32 DownPeriod;
	
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;
} KeyType;

typedef struct{
	KeyType *pKey[256];
	u16 KeyCount;
} KeyConterType;

extern void InitKey(KeyType*,GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
extern s8 ReadKey(KeyType *K);
extern void ReadAllKey(void);

#endif
