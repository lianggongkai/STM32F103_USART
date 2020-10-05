/*
* @files    input_usr.c/input_usr.h
*	@author   lianggongke
* @version  V1.0
* @date     2016/12/20
* @brief    ���ļ���Ҫ��Ϊ�˼򻯲���ʹ��stm32��IO��Ϊ����ģʽ��
*						��Ҫ������������ʼ���������û����Ը��ݶ�ȡ�������
*						�ڵĲ�����ȡ��ǰ����״̬����������ʱ�䳤�ȡ�
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
	K->Filter = 50;//����ʱ��50ms
	K->GPIOx = GPIOx;
	K->GPIO_Pin = GPIO_Pin;
	K->IsInit = Inited;
	return;
}

//��ȡ����״̬
s8 ReadKey(KeyType *K)
{ 
	u32 CurTick = 0 ;
	u16 KeyBit = 0;
	
	//δ��ʼ��ֱ�ӷ��ع���
	if(K->IsInit == UnInit)
		return -1;
	
	CurTick= GetCurSysTick();
	//u16 KeyBit = GPIO_ReadInputDataBit(K->GPIOx,K->GPIO_Pin);
	//��ȡIO״̬
	KeyBit = ((K->GPIOx->IDR & K->GPIO_Pin) != ((uint32_t)Bit_RESET))?(u8)Bit_SET:(u8)Bit_RESET;
	
	//K�������²����ǵ�һ�ΰ���
	if( KeyBit == SIGNAL_IN && K->IsDown == KeyUp)
	{
		K->IsDown = KeyDown ;
		K->DownTick = CurTick;
		return 0;
	}
	
	//���K�����Ѿ����¹�
	if( K->IsDown == KeyDown )
	{
		//�ж�����ʱ���Ƿ񵽴�
		if( ((CurTick-K->DownTick) >= K->Filter) ||
				((0xffffffff - K->DownTick + CurTick) >= K->Filter)
			)
		{
			//������ɲ��Ұ��������ź�����
			if(KeyBit == SIGNAL_IN){
				K->Stat = KeyDown ;//�����ȶ�����
				((CurTick - K->DownTick) > 0) ? ( K->DownPeriod = CurTick - K->DownTick ) :\
						( K->DownPeriod = 0xffffffff - K->DownTick + CurTick );//������������ʱ�䳤��
			}
			else{//��������Ѿ�û���ź�
				K->Stat = KeyUp;//����״̬�ж�Ϊ��Ч
				K->IsDown = KeyUp;
				K->UpTick = CurTick;
				K->DownPeriod = 0;
			}
		}
	}
	return 0;
}

//��ȡ��ʼ����ȫ������״̬
void ReadAllKey(void)
{
	u16 count = KCounter.KeyCount;
	while(count>0){
		ReadKey(KCounter.pKey[--count]);
	}
}







