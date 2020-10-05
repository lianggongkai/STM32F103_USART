#ifndef _SYSTICK_USR_H_
#define _SYSTICK_USR_H_

#include "stm32f10x.h"
#include "core_cm3.h"
#include "stm32f10x_rcc.h"

/*
* �û��Զ���Ķ�ʱ��ʹ�÷���
*	1����TimerType���嶨ʱ������
* 2������SetTimer()�����趨��ʱʱ��
* 3, ��SysTickHandler�жϺ����е���CheckAllTimer();
* 4, ����EnableTimer()��������ʱ����
* 5, �ں����в�ѯ��ʱ���ṹ������Res��1��ʾ��ʱ����
* 6�������ѭ��ʹ�ö�ʱ�����Ե���ResetTimer()������ϴν��
*/

typedef struct{
	volatile u32 SetTime;
	volatile u32 CurTime;
	volatile u32 FirstIn;
	volatile u16 IsFirst;
	volatile u16 Res;
	volatile u16 Enable;
	//u16 (*func)(u32);
} TimerType;

typedef struct{
	TimerType* Addr[256];
	s16 Counter;	
} TimerCounter;

void SysTick_Init(void);
void SysTick_Enable(void);
void SysTick_Disable(void);
int IfSysTickOn(void);
void DelayMs(u16 ms);
int TimeDelayMs(u32 ms);
u32 GetCurSysTick(void);

void ResetTimer(TimerType *);
u16 SetTimer(TimerType *,u32);
void CheckTimer(TimerType *);
void CheckAllTimer(void);
void EnableTimer(TimerType *);


#endif



