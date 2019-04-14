#include <stdio.h>
#include "stm32f10x_conf.h"
#include "TIM.h"
#include "RTC_Time.h"
#include "bell.h"



////若时钟信号为72M，TIM_Prescaler=72，TIM_Period=1000，则定时1ms
//void TIM3_init(unsigned short psc, unsigned short pre)
//{
//	/*************************GPIO*****************************/	
//	//GPIO时钟
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
//	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);	//重映射
//	
//	//beep GPIO
//	GPIO_InitTypeDef GPIO_InitStruct;
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;	//复用输出
//	GPIO_Init(GPIOB, &GPIO_InitStruct);
//	/*************************GPIO*****************************/	
//	
//	/*************************定时器*****************************/	
//	//TIM使能时钟
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

//	//TIM配置
//	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
//	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; //向上计数
//	TIM_TimeBaseInitStruct.TIM_Prescaler = psc - 1; //分频72
//	TIM_TimeBaseInitStruct.TIM_Period = pre - 1;	//预装载值
//	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);//初始化定时器
//	
////	//NVIC
////	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
//	
//	//OC
//	TIM_OCInitTypeDef TIM_OCInitStruct;
//	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;	//模式1
//	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;	//输出使能
//	TIM_OCInitStruct.TIM_Pulse = pre / 3;			//OC的值
//	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;	//初始极性，高
//	TIM_OC2Init(TIM3, &TIM_OCInitStruct);
//	
//	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);//oc使能
//	TIM_CtrlPWMOutputs(TIM3, ENABLE);	//PWN使能
//	TIM_ARRPreloadConfig(TIM3, ENABLE);	//自动装载
//	
//	
//	TIM_Cmd(TIM3, ENABLE);	//使能定时器
//	
//	/*************************定时器*****************************/	
//}
//若时钟信号为72M，TIM_Prescaler=72，TIM_Period=1000，则定时1ms
void TIM2_init(unsigned short psc, unsigned short pre)
{
	//TIM使能时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	//TIM配置
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; //向上计数
	TIM_TimeBaseInitStruct.TIM_Prescaler = psc - 1; //分频72
	TIM_TimeBaseInitStruct.TIM_Period = pre - 1;	//预装载值
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);//初始化定时器
	
	
	TIM_Cmd(TIM2, ENABLE);	//使能定时器
	
	//NVIC
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}

void TIM2_NVIC_init(void)
{
	//选择分组，main函数已分好
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;	//中断源
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;	//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;	//响应优先级
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;	//使能
	NVIC_Init(&NVIC_InitStruct);
}

unsigned int i_200us = 0;		//1us时标 (0-9)100us
unsigned int i_1ms = 0;			//1ms时标 (0-999)ms

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
					GPIO_SetBits(GPIOB, GPIO_Pin_5);	//关闭蜂鸣器
				}				
			}
			else if (bell_alarm_temp == 1)
			{
				GPIO_SetBits(GPIOB, GPIO_Pin_5);	//关闭蜂鸣器
				bell_times--;
				if (bell_times == 0)
				{
					bell_times = 10000;
					bell_alarm_temp = 0;
					GPIO_SetBits(GPIOB, GPIO_Pin_5);	//关闭蜂鸣器
				}				
			}
			else if (bell_alarm_temp == 0) bell_alarm_temp = 2;
		}
		
		
		
		//蜂鸣器响
		if (bell_ring_flag == 1)
		{
			GPIO_WriteBit(GPIOB, GPIO_Pin_5, (BitAction)!GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_5));
			bell_times--;
			if (bell_times == 0)
			{
				bell_ring_flag = 0;
				GPIO_SetBits(GPIOB, GPIO_Pin_5);	//关闭蜂鸣器
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
	
//	if (TIM_GetFlagStatus(TIM2, TIM_FLAG_Update) == SET)	//定时器闪灯
//	{
//		TIM_ClearFlag(TIM2, TIM_FLAG_Update);
//		i_led++;
//		if (i_led >= 100)	//500ms亮，500ms灭
//		{
//			i_led = 0;
//			GPIO_WriteBit(GPIOE, GPIO_Pin_12, (BitAction)(~GPIO_ReadOutputDataBit(GPIOE, GPIO_Pin_12) & 0x01));
//		}
//	}
}
