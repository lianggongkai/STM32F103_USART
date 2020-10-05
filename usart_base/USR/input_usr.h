#ifndef _INPUT_USR_H_
#define _INPUT_USR_H_

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

//�����������ģʽ���������룬���������ǵ͵�ƽ
//�����������ģʽ���������룬���������Ǹߵ�ƽ
#define SIGNAL_IN 0 //������
#define NO_SIGNAL 1 //������

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
