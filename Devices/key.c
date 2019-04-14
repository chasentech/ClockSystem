#include "key.h"
#include "usart.h"
#include "bell.h"
#include "delay.h"

extern unsigned char i_key;
extern unsigned char once_flag;

extern unsigned char sound_key_flag;


void key_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//�ж�
	EXTI_InitTypeDef EXTI_InitStructure;
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);//A1
  	EXTI_InitStructure.EXTI_Line = EXTI_Line1; //1
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource2);//A2
  	EXTI_InitStructure.EXTI_Line = EXTI_Line2; //1
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource3);//A3
  	EXTI_InitStructure.EXTI_Line = EXTI_Line3; //1
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4);//A4
  	EXTI_InitStructure.EXTI_Line = EXTI_Line4; //1
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource5);//A5
  	EXTI_InitStructure.EXTI_Line = EXTI_Line5; //1
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource6);//A6
  	EXTI_InitStructure.EXTI_Line = EXTI_Line6; //1
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���


}

//�����ⲿ�ж�
//https://www.cnblogs.com/zhoubatuo/p/6134166.html
void key_nvic()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�2 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�2 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�2 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//��ռ���ȼ�2 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;	//��ռ���ȼ�2 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}

void EXTI1_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line1)!=RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line1);  //���EXTI0��·����λ
		
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0)
		{
			DelayMs(5);
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0)
			{
				once_flag = 1;
				i_key = 1;
				
				if (sound_key_flag) bell_short_ring(1);
				//printf("OK\n");	
			}			
		}

	}
}
void EXTI2_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line2)!=RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line2);  //���EXTI0��·����λ
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0)
		{
			DelayMs(5);
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0)
			{
				once_flag = 1;
				i_key = 3;
				
				if (sound_key_flag) bell_short_ring(1);
				//printf("Cancel\n");
			}
		}
	}
}
void EXTI3_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line3)!=RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line3);  //���EXTI0��·����λ
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == 0)
		{
			DelayMs(5);
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == 0)
			{
				once_flag = 1;
				i_key = 5;
				
				if (sound_key_flag) bell_short_ring(1);
				//printf("D\n");
			}
		}
	}
}
void EXTI4_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line4)!=RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line4);  //���EXTI0��·����λ
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0)
		{
			DelayMs(5);
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0)
			{
				once_flag = 1;
				i_key = 4;
				
				if (sound_key_flag) bell_short_ring(1);
				//printf("L\n");
			}
		}
	}
}



void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line5)!=RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line5);  //���EXTI0��·����λ
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 0)
		{
			DelayMs(5);
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 0)
			{
				once_flag = 1;
				i_key = 6;
				
				if (sound_key_flag) bell_short_ring(1);
				//printf("R\n");
			}
		}
	}
	
	if(EXTI_GetITStatus(EXTI_Line6)!=RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line6);  //���EXTI0��·����λ
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 0)
		{
			DelayMs(5);
			if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 0)
			{
				once_flag = 1;
				i_key = 8;
				
				if (sound_key_flag) bell_short_ring(1);
				//printf("U\n");
			}
		}
	}
}
