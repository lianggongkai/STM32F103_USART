/*
* @brief : 定时器对象的定义以及操作。
*	@func  : 系统滴答定时器初始化，中断开启，中断关闭，获取当前滴答数值
*          设置定时器，启动定时器，复位定时器，更新单个定时器，更新全部定时器
*/

#include "stm32f10x.h"
#include "core_cm3.h"
#include "stm32f10x_rcc.h"
#include "systick_usr.h"
#include "input_usr.h"
#include "output_usr.h"

volatile static u32 timeMs;//当前系统滴答数值
volatile static TimerCounter TCounter;//当前定时器数量记录

/*
* @brief : 系统滴答定时器初始化，先不开中断
*/
void SysTick_Init(void)
{
	if(SysTick_Config(SystemCoreClock/1000)){
		while(1){};//systick Init failed
	}
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	TCounter.Counter = 0;
	
	//不使用中断时候要关闭中断请求，否则查询CounterFlag失效
	//SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;	
}

/*
* @brief : 系统滴答定时器中断开启
*/
void SysTick_Enable(void)
{
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

/*
* @brief : 系统滴答定时器关闭
*/
void SysTick_Disable(void)
{
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

//Check systick on
//If systick on return 1
int IfSysTickOn(void)
{	
	if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
		return 1;
	else
		return 0;
}

/*
* @brief  : 用函数获取全局变量系统滴答数值
* @return : 返回当前系统滴答数值
*/
u32 GetCurSysTick(void){
	u32 CurSysTick = 0;
	__set_PRIMASK(1);//关中断
	 CurSysTick = timeMs;
	__set_PRIMASK(0);//开中断
	return CurSysTick;
}

/*
* @brief  : 精确延时，精确度1ms
* @param  : 延时时长
* @return : 延时到达返回1，否则为0
*/
int TimeDelayMs(u32 ms){
	static u32 timeIn = 0; 
	static u32 flag = 0;
	
	SysTick_Enable();
	if( flag == 0){
		timeIn= GetCurSysTick();
		flag = 1;
	}
	if( flag == 1){
		u32 CurTick = GetCurSysTick();
		if( CurTick > timeIn ){
			u32 tmp = CurTick - timeIn ;
			if(tmp == ms)
			{
				flag = 0;
				return 1;
			}
		}
		if( CurTick < timeIn ){
			u32 tmp1 = 0xffffffff -timeIn + CurTick ;
			if( tmp1 == ms){
				flag = 0;
				return 1;
			}
		}
		if( CurTick == timeIn ){
			u32 tmp1 = 0xffffffff;
			if( tmp1 == ms){
				flag = 0;
				return 1;
			}
		}
	}
	return 0;
}

/*
* @brief : 用查询CountFlag做的精确延时,循环等待
*					 直到延时时间到。
*/
void DelayMs(u16 ms){
	SysTick_Enable();
	while(ms>0){
		if( IfSysTickOn())
		{
			//SysTick->CTRL &= ~SysTick_CTRL_COUNTFLAG_Msk;
			ms--;
		}
	}
	SysTick_Disable();
}

/*
* @brief : 设置指定的定时器的定时时长
* @param : T 为用户定义的定时器结构变量地址
*          Ticks 为定时时间长度
* @return : 设置成功返回1，否则返回0
*/
u16 SetTimer(TimerType *T,u32 Ticks){
	if(TCounter.Counter < 256){
		SysTick_Enable();
		T->SetTime = Ticks;
		T->IsFirst = 0;
		T->Res = 0;
		T->Enable = 0;
		TCounter.Addr[TCounter.Counter++] = T;
		return 1;
	}
	else
		return 0;//Fail to SetTimer
}

/*
* @brief : 启动用户自定义的定时器
*/
void EnableTimer(TimerType *T)
{
	T->Enable = 1;
}

/*
* @brief : 复位用户自定义定时器
*/
void ResetTimer(TimerType *T)
{
	T->CurTime = 0;
	T->Res = 0;
	T->IsFirst = 0;
	T->Enable = 0;
}

/*
* @brief : 检查更新用户定时器参数
*/
void CheckTimer(TimerType *T)
{
	if(!T->Enable){//如果没有开启直接退出
		return;
	}
	//第一次进入获取定时开始值
	if(!T->IsFirst){
		T->FirstIn = GetCurSysTick();
		T->IsFirst = 1;
	}
	//获取当前滴答值
	T->CurTime = GetCurSysTick();
	if(!T->Res)
	{
		if( T->CurTime > T->FirstIn )
		{
			u32 tmp = T->CurTime - T->FirstIn ;
			if(tmp >= T->SetTime )
			{
				T->Res = 1;
				return;
			}
		}
		if( T->CurTime < T->FirstIn )
		{
			u32 tmp1 = 0xffffffff -T->FirstIn + T->CurTime ;
			if( tmp1 >= T->SetTime){
				T->Res = 1;
				return;
			}
		}
		if( T->CurTime == T->FirstIn )
		{
			u32 tmp1 = 0xffffffff;
			if( tmp1 == T->SetTime){
				T->Res = 1;
				return;
			}
		}
	}
}

/*
* @brief : 更新全部用户自定义的定时器,在SysTickHandler中断函数中调用
*/
void CheckAllTimer(void)
{
	u16 count = TCounter.Counter;
	while(count>0){
		CheckTimer(TCounter.Addr[--count]);
	}
}

//中断服务函数
void SysTick_Handler(void)
{
	timeMs++;
	CheckAllTimer();
	ReadAllKey();
	OutAllCheck();
}

