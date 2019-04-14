/************************************************************************************
bell  PB3
IIC   PB6--SCL PB7--SDA
DHT12 PB14

stm32ʹ���ڲ�flash�������
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

/***********************ͼ������  codetab.h***************************************/
extern const unsigned char BMP_battery[];
extern const unsigned char alarm_flag[];
extern const unsigned char bell_flag[];

extern const unsigned char number_big[11][48];
extern const unsigned char *BMP_logo[];
/***********************ͼ������  codetab.h***************************************/

extern enum models g_model;


//ʱ������
extern unsigned char screen_time[6];       //֪ͨ��ʱ��
extern unsigned char standby_day[11]; //�������� 
extern unsigned char standby_sec[8];     //����ʱ�� 


extern unsigned char sound_flag;		//�����Ƿ���
extern unsigned int i_1ms;			//1msʱ�� (0-999)ms

extern unsigned char i_key;	//����ѡ��
extern unsigned char i_choose;	//����ѡ��
extern unsigned char once_flag;	//��ʾһ��
extern unsigned char select_temp;	//��������Menu��ֵ


//��ʼ��
void init()
{
	//�жϷ��� 4 4
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//RTCʵʱʱ��
	RTC_Config();
	
	TIM2_init(72, 200); //200usΪ�˷������������1msʱ��
	TIM2_NVIC_init();
	
	//����1��ʼ��
	USART1_init();
	USART2_init();
	USART_NVIC_init();
	
	bell_init();
	dht12_init();
	LED_GPIO_Config();
	key_init();
	key_nvic();

	
	DelayInit();//��ʱ������ʼ��
	I2C_Configuration();//IIC��ʼ��
	OLED_Init();//OLED��ʼ��
	
	OLED_Fill(0x00);//ȫ����
	
	//֪ͨ����Ϣ��ʾ
	OLED_DrawBMP(0, 0, 16, 2,(unsigned char *)alarm_flag, 0);	//������ʾ
	OLED_DrawBMP(18, 0, 34, 2,(unsigned char *)bell_flag, 0);	//��ʱ��ʾ
	OLED_ShowStr(44, 0, screen_time, 2, 0);	//�в�ʱ����ʾ
	OLED_DrawBMP(96, 0, 128, 2,(unsigned char *)BMP_battery, 0);
	battery_show(2);	//������ʾ
	
//	set_time(2019, 2, 28, 11, 28, 00);


//	//��ȡָ����ַ�İ���(16bit)
//	uint16_t FLASH_ReadHalfWord(uint32_t address);

//	//��ȡָ����ַ��ȫ��(32bit)
//	uint32_t FLASH_ReadWord(uint32_t address);

//	//��ָ����ַ��ʼ��ȡ�������
//	void FLASH_ReadMoreData(uint32_t startAddress,uint16_t *readData,uint16_t countToRead);

//	//��ָ����ַ��ʼд��������
//	void FLASH_WriteMoreData(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite);


//	uint16_t xxx = 0x89;
//	FLASH_WriteMoreData(0X08000000, &xxx, 1);

	
	printf("000\n");

//	game_init();
}

int main(void)
{

//  //����ģʽ��
//	//�жϷ��� 4 4
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

//	
//	//����1��ʼ��
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
