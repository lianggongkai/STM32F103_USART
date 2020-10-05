/*
* @files    output_usr.c/output_usr.h
*	@author   lianggongke
* @version  V1.0
* @date     2016/12/20
* @brief    ���ļ���Ҫ��Ϊ�˼򻯲���ʹ��stm32��IO��Ϊ���ģʽ��
*						��Ҫ������������ʼ��������Out�����Out���ˢ��
*						�����1�������0�����ȡ����
*
*
*/

#include "output_usr.h"
OutConterType OutCounter;

#define Inited  1
#define UnInit  0

/*
* @brief : ��������ʼ�����ö������������ַ��ָ���˿ڣ����źų�ʼ�������
*	@func  : ���ָ��256�������������Ƶ��2Mhz������ģʽ�����
* @note  : ������ʼ�����г�ʼ����־���������HardFault;
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
* @brief : �����飬�����������������ַ�����Outָ�
*	@func  : ����Outָ���ж������
*/
void OutCheck(OutType *Out)
{
	//δ��ʼ��ֱ���˳�
	if(Out->IsInit == UnInit)
		return;
	
	//û��ʹ��outָ��
	if(!Out->IsOut){
		return;
	}
	else{//ʹ����Outָ��
		if(Out->Cmd == OutOn){//Out����ʹ��
			//GPIO_SetBits(Out->GPIOx,Out->GPIO_Pin);
			Out->GPIOx->BSRR = Out->GPIO_Pin;
		}
		else{//Outָ������δʹ��
			//GPIO_ResetBits(Out->GPIOx,Out->GPIO_Pin);
			Out->GPIOx->BRR = Out->GPIO_Pin;
		}
	}
}

/*
* @brief : ɨ��Outָ�����
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
* @brief : ʹ��Outָ��
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
* @brief : ʧ��Outָ��
*	@func  : 
*/
void ElseOut(OutType* Out){
	if(Out->IsInit == UnInit)
		return;
	Out->Cmd = OutOff;
}

/*
* @brief : ָ����������Ӧ��������� �ߵ�ƽ
*	@func  : ��ʹ��outָ��ֱ����1
*/
void Set(OutType* Out){
	if(Out->IsInit == UnInit)
		return;
	//Out->Cmd = CmdOn;
	Out->GPIOx->BSRR = Out->GPIO_Pin;
}

/*
* @brief : ָ����������Ӧ��������� �͵�ƽ
*	@func  : ��ʹ��outָ��ֱ����0
*/
void Reset(OutType* Out){
	if(Out->IsInit == UnInit)
		return;
	//Out->Cmd = CmdOff;
	Out->GPIOx->BRR = Out->GPIO_Pin;
}

/*
* @brief : ָ����������Ӧ��������� ��ת��ƽ
*	@func  : ��ʹ��outָ��ֱ��ȡ��
*/
void ALT(OutType* Out){
	if(Out->IsInit == UnInit)
		return;
	(Out->GPIOx->ODR & Out->GPIO_Pin) ? (Out->GPIOx->BRR  = Out->GPIO_Pin) \
																		: (Out->GPIOx->BSRR = Out->GPIO_Pin);
}




