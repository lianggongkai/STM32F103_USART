/*
* @files    input_usr.c/input_usr.h
*	@author   lianggongke
* @version  V1.0
* @date     2016/12/20
* @brief    本文件主要是为了简化操作使用stm32的IO作为输入模式。
*						主要做了输入对象初始化函数；用户可以根据读取输入对象
*						内的参数获取当前按键状态，按键按下时间长度。
*
*
*/

#include "input_usr.h"
#include "systick_usr.h"

KeyConterType KCounter ;

#define Inited  1
#define UnInit  0

void InitKey(KeyType* K,GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIOInitStruct;
	if(KCounter.KeyCount < 256){
		KCounter.pKey[KCounter.KeyCount++] = K;
	}
	else
		return;
	if(GPIOx == GPIOA)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	if(GPIOx == GPIOB)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	if(GPIOx == GPIOC) 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	if(GPIOx == GPIOD)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	if(GPIOx == GPIOE)  
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	if(GPIOx == GPIOF) 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,ENABLE);
	if(GPIOx == GPIOG)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);
	GPIOInitStruct.GPIO_Pin = GPIO_Pin;
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_IPU;
	//GPIOInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOx,&GPIOInitStruct);
	K->Stat = KeyUp;
	K->Filter = 50;//消抖时间50ms
	K->GPIOx = GPIOx;
	K->GPIO_Pin = GPIO_Pin;
	K->IsInit = Inited;
	return;
}

//读取按键状态
s8 ReadKey(KeyType *K)
{ 
	u32 CurTick = 0 ;
	u16 KeyBit = 0;
	
	//未初始化直接返回故障
	if(K->IsInit == UnInit)
		return -1;
	
	CurTick= GetCurSysTick();
	//u16 KeyBit = GPIO_ReadInputDataBit(K->GPIOx,K->GPIO_Pin);
	//读取IO状态
	KeyBit = ((K->GPIOx->IDR & K->GPIO_Pin) != ((uint32_t)Bit_RESET))?(u8)Bit_SET:(u8)Bit_RESET;
	
	//K按键按下并且是第一次按下
	if( KeyBit == SIGNAL_IN && K->IsDown == KeyUp)
	{
		K->IsDown = KeyDown ;
		K->DownTick = CurTick;
		return 0;
	}
	
	//如果K按键已经按下过
	if( K->IsDown == KeyDown )
	{
		//判断消抖时间是否到达
		if( ((CurTick-K->DownTick) >= K->Filter) ||
				((0xffffffff - K->DownTick + CurTick) >= K->Filter)
			)
		{
			//消抖完成并且按键还有信号输入
			if(KeyBit == SIGNAL_IN){
				K->Stat = KeyDown ;//按键稳定按下
				((CurTick - K->DownTick) > 0) ? ( K->DownPeriod = CurTick - K->DownTick ) :\
						( K->DownPeriod = 0xffffffff - K->DownTick + CurTick );//更新连续按下时间长度
			}
			else{//消抖完成已经没有信号
				K->Stat = KeyUp;//按键状态判定为无效
				K->IsDown = KeyUp;
				K->UpTick = CurTick;
				K->DownPeriod = 0;
			}
		}
	}
	return 0;
}

//读取初始化的全部按键状态
void ReadAllKey(void)
{
	u16 count = KCounter.KeyCount;
	while(count>0){
		ReadKey(KCounter.pKey[--count]);
	}
}







