#include "bell.h"
#include "stm32f10x.h"
#include "delay.h"
#include "TIM.h"

unsigned char bell_ring_flag = 0;
unsigned int bell_times = 0;

//unsigned char bell_alarm_flag = 0;


//蜂鸣器初始化
void bell_init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_5);	//bell初始化，蜂鸣器低电平
}

void bell_short_ring(unsigned int times)
{
	bell_ring_flag = 1;
	bell_times = times*100;
}
