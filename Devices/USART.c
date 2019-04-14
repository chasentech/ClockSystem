#include <stdio.h>
#include <string.h>
#include "stm32f10x_conf.h"
#include "USART.h"
#include "OLED_I2C.h"
#include "bell.h"


extern unsigned char i_key;
extern unsigned char once_flag;

extern unsigned char sound_key_flag;


//描述：库函数printf重定义到USART1
int fputc(int ch, FILE *f)
{
	/* 将Printf内容发往串口 */
	USART_SendData(USART1, (unsigned char) ch);
	while (!(USART1->SR & USART_FLAG_TXE));
	
	return (ch);
}

////加入以下代码,支持printf函数
////重定义fputc函数 
//int fputc(int ch, FILE *f)
//{      
//	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
//    USART1->DR = (u8) ch;      
//	return ch;
//}



void USART1_init(void)
{
	//时钟配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	
	//USART GPIO
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;	//复用输出 TX
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入 RX
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	
	//时钟配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	//串口配置
	USART_InitTypeDef USART_InitStruc;
	USART_InitStruc.USART_BaudRate = 115200;	//波特率
	USART_InitStruc.USART_WordLength = USART_WordLength_8b;	//数据位
	USART_InitStruc.USART_StopBits = USART_StopBits_1;	//停止位
	USART_InitStruc.USART_Parity = USART_Parity_No;		//校验位
	USART_InitStruc.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//模式收发
	USART_InitStruc.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流
	USART_Init(USART1, &USART_InitStruc);
	
	USART_Cmd(USART1, ENABLE);	//使能USART1
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//开启中断
}

void USART2_init(void)
{
	//时钟配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	
	//USART GPIO
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;	//复用输出 TX
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入 RX
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	
	//时钟配置
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	//串口配置
	USART_InitTypeDef USART_InitStruc;
	USART_InitStruc.USART_BaudRate = 115200;	//波特率
	USART_InitStruc.USART_WordLength = USART_WordLength_8b;	//数据位
	USART_InitStruc.USART_StopBits = USART_StopBits_1;	//停止位
	USART_InitStruc.USART_Parity = USART_Parity_No;		//校验位
	USART_InitStruc.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//模式收发
	USART_InitStruc.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流
	USART_Init(USART2, &USART_InitStruc);
	
	USART_Cmd(USART2, ENABLE);	//使能USART1
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	//开启中断
}

void USART_NVIC_init(void)
{
//	//选择分组，只能分一次，主函数分
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;	//中断源
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;	//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;	//响应优先级
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;	//使能
	NVIC_Init(&NVIC_InitStruct);
	

	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;	//中断源
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;	//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;	//响应优先级
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;	//使能
	NVIC_Init(&NVIC_InitStruct);
	
}






/*串口调试
		   8上

	4左	   5下    6右

	1确认        3返回

*/
void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)	//接收中断
	{
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		
		if (USART_ReceiveData(USART1) == '1')
		{
			once_flag = 1;
			i_key = 1;
			
			if (sound_key_flag) bell_short_ring(1);
			printf("OK\n");
		}
		if (USART_ReceiveData(USART1) == '3')
		{
			once_flag = 1;
			i_key = 3;
			
			if (sound_key_flag) bell_short_ring(1);
			printf("Cancel\n");
		}
		
		if (USART_ReceiveData(USART1) == '4')
		{
			once_flag = 1;
			i_key = 4;
			
			if (sound_key_flag) bell_short_ring(1);
			printf("L\n");
		}
		if (USART_ReceiveData(USART1) == '5')
		{
			once_flag = 1;
			i_key = 5;
			
			if (sound_key_flag) bell_short_ring(1);
			printf("D\n");
		}
		if (USART_ReceiveData(USART1) == '6')
		{
			once_flag = 1;
			i_key = 6;
			
			if (sound_key_flag) bell_short_ring(1);
			printf("R\n");
		}
		if (USART_ReceiveData(USART1) == '8')
		{
			once_flag = 1;
			i_key = 8;
			
			if (sound_key_flag) bell_short_ring(1);
			printf("U\n");
		}
//		if (USART_ReceiveData(USART1) == '7')
//		{
//			 OLED_OFF();
//		}
//		if (USART_ReceiveData(USART1) == '9')
//		{
//			 OLED_ON();
//			OLED_ShowStr(0, 2, (unsigned char *)"Name: Oled Clock", 2, 0);
//		}
		
	}
}

void USART2_IRQHandler(void)
{
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)	//接收中断
	{
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		
		if (USART_ReceiveData(USART2) == '1')
		{
			once_flag = 1;
			i_key = 1;
			
			if (sound_key_flag) bell_short_ring(1);
		}
		if (USART_ReceiveData(USART2) == '3')
		{
			once_flag = 1;
			i_key = 3;
			
			if (sound_key_flag) bell_short_ring(1);
		}
		
		if (USART_ReceiveData(USART2) == '4')
		{
			once_flag = 1;
			i_key = 4;
			
			if (sound_key_flag) bell_short_ring(1);
		}
		if (USART_ReceiveData(USART2) == '5')
		{
			once_flag = 1;
			i_key = 5;
			
			if (sound_key_flag) bell_short_ring(1);
		}
		if (USART_ReceiveData(USART2) == '6')
		{
			once_flag = 1;
			i_key = 6;
			
			if (sound_key_flag) bell_short_ring(1);
		}
		if (USART_ReceiveData(USART2) == '8')
		{
			once_flag = 1;
			i_key = 8;
			
			if (sound_key_flag) bell_short_ring(1);
		}
		
	}
}

//void USART1_send_datas(unsigned char *data)	//发送函数
//{
//	int i = 0;
//	for (i = 0; data[i] != '\0'; i++)
//	{
//		USART_SendData(USART1, data[i]);
//		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); //等待发送完成
//		USART_ClearITPendingBit(USART1, USART_FLAG_TC);	//TC | TXE
//	}
//}





