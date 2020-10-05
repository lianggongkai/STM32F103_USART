/*
* @files    output_usr.c/output_usr.h
*	@author   lianggongke
* @version  V1.0
* @date     2016/12/20
* @brief    本文件主要是为了简化操作使用stm32的IO作为输出模式。
*						主要做了输出对象初始化函数，Out输出，Out输出刷新
*						输出置1，输出置0，输出取反；
*
*
*/

#include "output_usr.h"
OutConterType OutCounter;

#define Inited  1
#define UnInit  0

/*
* @brief : 输出对象初始化。用定义的输出对象地址，指定端口，引脚号初始化输出。
*	@func  : 最多指定256个输出。最大输出频率2Mhz。推挽模式输出。
* @note  : 经过初始化后有初始化标志。以免造成HardFault;
*/
s8 InitOut(OutType* Out, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIOInitStruct;
	
	if(OutCounter.OutCount < 256){
		OutCounter.pOut[OutCounter.OutCount++] = Out;
		Out->IsOut = 0;
		Out->Cmd = OutOff;
		Out->GPIOx = GPIOx;
		Out->GPIO_Pin = GPIO_Pin;
	}
	else
		return -1;
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
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOx,&GPIOInitStruct);	
	Out->IsInit = Inited;
	return 0;
}

/*
* @brief : 输出检查，根据输入的输出对象地址，检查Out指令。
*	@func  : 根据Out指令判定输出。
*/
void OutCheck(OutType *Out)
{
	//未初始化直接退出
	if(Out->IsInit == UnInit)
		return;
	
	//没有使用out指令
	if(!Out->IsOut){
		return;
	}
	else{//使用了Out指令
		if(Out->Cmd == OutOn){//Out条件使能
			//GPIO_SetBits(Out->GPIOx,Out->GPIO_Pin);
			Out->GPIOx->BSRR = Out->GPIO_Pin;
		}
		else{//Out指令条件未使能
			//GPIO_ResetBits(Out->GPIOx,Out->GPIO_Pin);
			Out->GPIOx->BRR = Out->GPIO_Pin;
		}
	}
}

/*
* @brief : 扫描Out指令输出
*	@func  : 
*/
void OutAllCheck(void)
{
	u16 count = OutCounter.OutCount;
	while(count>0){
		OutCheck(OutCounter.pOut[--count]);
	}
}

/*
* @brief : 使能Out指令
*	@func  : 
*/
void Out(OutType* Out)
{
	if(Out->IsInit == UnInit)
		return;
	Out->IsOut = 1;
	Out->Cmd = OutOn;
}

/*
* @brief : 失能Out指令
*	@func  : 
*/
void ElseOut(OutType* Out){
	if(Out->IsInit == UnInit)
		return;
	Out->Cmd = OutOff;
}

/*
* @brief : 指定输出对象对应的引脚输出 高电平
*	@func  : 不使用out指令直接置1
*/
void Set(OutType* Out){
	if(Out->IsInit == UnInit)
		return;
	//Out->Cmd = CmdOn;
	Out->GPIOx->BSRR = Out->GPIO_Pin;
}

/*
* @brief : 指定输出对象对应的引脚输出 低电平
*	@func  : 不使用out指令直接置0
*/
void Reset(OutType* Out){
	if(Out->IsInit == UnInit)
		return;
	//Out->Cmd = CmdOff;
	Out->GPIOx->BRR = Out->GPIO_Pin;
}

/*
* @brief : 指定输出对象对应的引脚输出 反转电平
*	@func  : 不使用out指令直接取反
*/
void ALT(OutType* Out){
	if(Out->IsInit == UnInit)
		return;
	(Out->GPIOx->ODR & Out->GPIO_Pin) ? (Out->GPIOx->BRR  = Out->GPIO_Pin) \
																		: (Out->GPIOx->BSRR = Out->GPIO_Pin);
}




