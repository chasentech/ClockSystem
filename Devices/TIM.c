#include <stdio.h>
#include "stm32f10x_conf.h"
#include "TIM.h"
#include "RTC_Time.h"
#include "bell.h"



////��ʱ���ź�Ϊ72M��TIM_Prescaler=72��TIM_Period=1000����ʱ1ms
//void TIM3_init(unsigned short psc, unsigned short pre)
//{
//	/*************************GPIO*****************************/	
//	//GPIOʱ��
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
//	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);	//��ӳ��
//	
//	//beep GPIO
//	GPIO_InitTypeDef GPIO_InitStruct;
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;	//�������
//	GPIO_Init(GPIOB, &GPIO_InitStruct);
//	/*************************GPIO*****************************/	
//	
//	/*************************��ʱ��*****************************/	
//	//TIMʹ��ʱ��
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

//	//TIM����
//	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
//	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���
//	TIM_TimeBaseInitStruct.TIM_Prescaler = psc - 1; //��Ƶ72
//	TIM_TimeBaseInitStruct.TIM_Period = pre - 1;	//Ԥװ��ֵ
//	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);//��ʼ����ʱ��
//	
////	//NVIC
////	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
//	
//	//OC
//	TIM_OCInitTypeDef TIM_OCInitStruct;
//	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;	//ģʽ1
//	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;	//���ʹ��
//	TIM_OCInitStruct.TIM_Pulse = pre / 3;			//OC��ֵ
//	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;	//��ʼ���ԣ���
//	TIM_OC2Init(TIM3, &TIM_OCInitStruct);
//	
//	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);//ocʹ��
//	TIM_CtrlPWMOutputs(TIM3, ENABLE);	//PWNʹ��
//	TIM_ARRPreloadConfig(TIM3, ENABLE);	//�Զ�װ��
//	
//	
//	TIM_Cmd(TIM3, ENABLE);	//ʹ�ܶ�ʱ��
//	
//	/*************************��ʱ��*****************************/	
//}
//��ʱ���ź�Ϊ72M��TIM_Prescaler=72��TIM_Period=1000����ʱ1ms
void TIM2_init(unsigned short psc, unsigned short pre)
{
	//TIMʹ��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	//TIM����
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���
	TIM_TimeBaseInitStruct.TIM_Prescaler = psc - 1; //��Ƶ72
	TIM_TimeBaseInitStruct.TIM_Period = pre - 1;	//Ԥװ��ֵ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);//��ʼ����ʱ��
	
	
	TIM_Cmd(TIM2, ENABLE);	//ʹ�ܶ�ʱ��
	
	//NVIC
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}

void TIM2_NVIC_init(void)
{
	//ѡ����飬main�����ѷֺ�
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;	//�ж�Դ
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;	//��Ӧ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;	//ʹ��
	NVIC_Init(&NVIC_InitStruct);
}

unsigned int i_200us = 0;		//1usʱ�� (0-9)100us
unsigned int i_1ms = 0;			//1msʱ�� (0-999)ms

extern unsigned char bell_ring_flag;
extern unsigned int bell_times;
extern unsigned char bell_alarm_flag;
unsigned char bell_alarm_temp = 2;

void TIM2_IRQHandler()
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		TIM_ClearFlag(TIM2, TIM_IT_Update);
		
		if (bell_alarm_flag == 1)
		{
			if (bell_alarm_temp == 2)
			{
				bell_times = 10000;
				GPIO_WriteBit(GPIOB, GPIO_Pin_5, (BitAction)!GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_5));
				bell_times--;
				if (bell_times == 0)
				{
					bell_times = 10000;
					bell_alarm_temp = 1;
					GPIO_SetBits(GPIOB, GPIO_Pin_5);	//�رշ�����
				}				
			}
			else if (bell_alarm_temp == 1)
			{
				GPIO_SetBits(GPIOB, GPIO_Pin_5);	//�رշ�����
				bell_times--;
				if (bell_times == 0)
				{
					bell_times = 10000;
					bell_alarm_temp = 0;
					GPIO_SetBits(GPIOB, GPIO_Pin_5);	//�رշ�����
				}				
			}
			else if (bell_alarm_temp == 0) bell_alarm_temp = 2;
		}
		
		
		
		//��������
		if (bell_ring_flag == 1)
		{
			GPIO_WriteBit(GPIOB, GPIO_Pin_5, (BitAction)!GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_5));
			bell_times--;
			if (bell_times == 0)
			{
				bell_ring_flag = 0;
				GPIO_SetBits(GPIOB, GPIO_Pin_5);	//�رշ�����
			}
		}
		
		
		i_200us++;
		if (i_200us == 5)
		{
			i_200us = 0;
			
			i_1ms++; 	//1s
			if (i_1ms >= 1000) i_1ms = 0;			
		}
		

	}
	
//	if (TIM_GetFlagStatus(TIM2, TIM_FLAG_Update) == SET)	//��ʱ������
//	{
//		TIM_ClearFlag(TIM2, TIM_FLAG_Update);
//		i_led++;
//		if (i_led >= 100)	//500ms����500ms��
//		{
//			i_led = 0;
//			GPIO_WriteBit(GPIOE, GPIO_Pin_12, (BitAction)(~GPIO_ReadOutputDataBit(GPIOE, GPIO_Pin_12) & 0x01));
//		}
//	}
}
