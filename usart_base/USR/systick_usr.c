/*
* @brief : ��ʱ������Ķ����Լ�������
*	@func  : ϵͳ�δ�ʱ����ʼ�����жϿ������жϹرգ���ȡ��ǰ�δ���ֵ
*          ���ö�ʱ����������ʱ������λ��ʱ�������µ�����ʱ��������ȫ����ʱ��
*/

#include "stm32f10x.h"
#include "core_cm3.h"
#include "stm32f10x_rcc.h"
#include "systick_usr.h"
#include "input_usr.h"
#include "output_usr.h"

volatile static u32 timeMs;//��ǰϵͳ�δ���ֵ
volatile static TimerCounter TCounter;//��ǰ��ʱ��������¼

/*
* @brief : ϵͳ�δ�ʱ����ʼ�����Ȳ����ж�
*/
void SysTick_Init(void)
{
	if(SysTick_Config(SystemCoreClock/1000)){
		while(1){};//systick Init failed
	}
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	TCounter.Counter = 0;
	
	//��ʹ���ж�ʱ��Ҫ�ر��ж����󣬷����ѯCounterFlagʧЧ
	//SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;	
}

/*
* @brief : ϵͳ�δ�ʱ���жϿ���
*/
void SysTick_Enable(void)
{
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

/*
* @brief : ϵͳ�δ�ʱ���ر�
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
* @brief  : �ú�����ȡȫ�ֱ���ϵͳ�δ���ֵ
* @return : ���ص�ǰϵͳ�δ���ֵ
*/
u32 GetCurSysTick(void){
	u32 CurSysTick = 0;
	__set_PRIMASK(1);//���ж�
	 CurSysTick = timeMs;
	__set_PRIMASK(0);//���ж�
	return CurSysTick;
}

/*
* @brief  : ��ȷ��ʱ����ȷ��1ms
* @param  : ��ʱʱ��
* @return : ��ʱ���ﷵ��1������Ϊ0
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
* @brief : �ò�ѯCountFlag���ľ�ȷ��ʱ,ѭ���ȴ�
*					 ֱ����ʱʱ�䵽��
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
* @brief : ����ָ���Ķ�ʱ���Ķ�ʱʱ��
* @param : T Ϊ�û�����Ķ�ʱ���ṹ������ַ
*          Ticks Ϊ��ʱʱ�䳤��
* @return : ���óɹ�����1�����򷵻�0
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
* @brief : �����û��Զ���Ķ�ʱ��
*/
void EnableTimer(TimerType *T)
{
	T->Enable = 1;
}

/*
* @brief : ��λ�û��Զ��嶨ʱ��
*/
void ResetTimer(TimerType *T)
{
	T->CurTime = 0;
	T->Res = 0;
	T->IsFirst = 0;
	T->Enable = 0;
}

/*
* @brief : �������û���ʱ������
*/
void CheckTimer(TimerType *T)
{
	if(!T->Enable){//���û�п���ֱ���˳�
		return;
	}
	//��һ�ν����ȡ��ʱ��ʼֵ
	if(!T->IsFirst){
		T->FirstIn = GetCurSysTick();
		T->IsFirst = 1;
	}
	//��ȡ��ǰ�δ�ֵ
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
* @brief : ����ȫ���û��Զ���Ķ�ʱ��,��SysTickHandler�жϺ����е���
*/
void CheckAllTimer(void)
{
	u16 count = TCounter.Counter;
	while(count>0){
		CheckTimer(TCounter.Addr[--count]);
	}
}

//�жϷ�����
void SysTick_Handler(void)
{
	timeMs++;
	CheckAllTimer();
	ReadAllKey();
	OutAllCheck();
}

