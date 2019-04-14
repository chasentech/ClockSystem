/************************************************************************************
bell  PB3
IIC   PB6--SCL PB7--SDA
DHT12 PB14

stm32使用内部flash保存参数
https://www.cnblogs.com/foxclever/p/5784169.html


*************************************************************************************/
#include "stm32f10x.h"
#include "OLED_I2C.h"
#include "delay.h"
#include "USART.h"
#include "RTC_Time.h"
#include "game.h"
#include "bell.h"
#include "dht12.h"
#include "TIM.h"
#include "model.h"
#include "led.h"
#include "key.h"

#define TEST 0

/***********************图标声明  codetab.h***************************************/
extern const unsigned char BMP_battery[];
extern const unsigned char alarm_flag[];
extern const unsigned char bell_flag[];

extern const unsigned char number_big[11][48];
extern const unsigned char *BMP_logo[];
/***********************图标声明  codetab.h***************************************/

extern enum models g_model;


//时间数组
extern unsigned char screen_time[6];       //通知栏时间
extern unsigned char standby_day[11]; //待机日期 
extern unsigned char standby_sec[8];     //待机时间 


extern unsigned char sound_flag;		//声音是否开启
extern unsigned int i_1ms;			//1ms时标 (0-999)ms

extern unsigned char i_key;	//按键选择
extern unsigned char i_choose;	//功能选择
extern unsigned char once_flag;	//显示一次
extern unsigned char select_temp;	//保留进入Menu的值


//初始化
void init()
{
	//中断分组 4 4
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//RTC实时时钟
	RTC_Config();
	
	TIM2_init(72, 200); //200us为了蜂鸣器，最后还是1ms时标
	TIM2_NVIC_init();
	
	//串口1初始化
	USART1_init();
	USART2_init();
	USART_NVIC_init();
	
	bell_init();
	dht12_init();
	LED_GPIO_Config();
	key_init();
	key_nvic();

	
	DelayInit();//延时函数初始化
	I2C_Configuration();//IIC初始化
	OLED_Init();//OLED初始化
	
	OLED_Fill(0x00);//全屏灭
	
	//通知栏信息显示
	OLED_DrawBMP(0, 0, 16, 2,(unsigned char *)alarm_flag, 0);	//闹钟显示
	OLED_DrawBMP(18, 0, 34, 2,(unsigned char *)bell_flag, 0);	//报时显示
	OLED_ShowStr(44, 0, screen_time, 2, 0);	//中部时间显示
	OLED_DrawBMP(96, 0, 128, 2,(unsigned char *)BMP_battery, 0);
	battery_show(2);	//电量显示
	
//	set_time(2019, 2, 28, 11, 28, 00);


//	//读取指定地址的半字(16bit)
//	uint16_t FLASH_ReadHalfWord(uint32_t address);

//	//读取指定地址的全字(32bit)
//	uint32_t FLASH_ReadWord(uint32_t address);

//	//从指定地址开始读取多个数据
//	void FLASH_ReadMoreData(uint32_t startAddress,uint16_t *readData,uint16_t countToRead);

//	//从指定地址开始写入多个数据
//	void FLASH_WriteMoreData(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite);


//	uint16_t xxx = 0x89;
//	FLASH_WriteMoreData(0X08000000, &xxx, 1);

	
	printf("000\n");

//	game_init();
}

int main(void)
{

//  //测试模式用
//	//中断分组 4 4
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

//	
//	//串口1初始化
//	USART1_init();
//	USART_NVIC_init();
//	
//	printf("...\n");
//	LED_GPIO_Config();
	
	init();
	g_model = Standby;
	
	while(1)
	{
//		printf("111\n");
		switch(g_model)
		{
			case Standby: model_Standby(); break;
			case Menu: model_Menu(); break;
			case Settime: model_Settime(); break;
			case Alarm: model_Alarm(); break;
			case Set: model_Set(); break;
			case Game: break;
			case Music: break;
			case Calendar: break;
			case Tools: model_Tools(); break;
			case About: model_About(); break;
			default: break;
		}
	}
}
