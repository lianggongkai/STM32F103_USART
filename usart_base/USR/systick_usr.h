#ifndef _SYSTICK_USR_H_
#define _SYSTICK_USR_H_

#include "stm32f10x.h"
#include "core_cm3.h"
#include "stm32f10x_rcc.h"

/*
* 用户自定义的定时器使用方法
*	1，用TimerType定义定时器变量
* 2，调用SetTimer()函数设定定时时长
* 3, 在SysTickHandler中断函数中调用CheckAllTimer();
* 4, 调用EnableTimer()，启动定时器。
* 5, 在函数中查询定时器结构变量的Res，1表示定时到达
* 6，如果想循环使用定时器可以调用ResetTimer()，清除上次结果
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



