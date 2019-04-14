#include "model.h"
#include "stm32f10x.h"
#include <stm32f10x.h>
#include "OLED_I2C.h"
#include "delay.h"
#include "USART.h"
#include "RTC_Time.h"
#include "game.h"
#include "bell.h"
#include "dht12.h"
#include "TIM.h"
#include <string.h>
#include "key.h"


/***********************图标声明  codetab.h***************************************/
extern const unsigned char BMP_battery[];
extern const unsigned char alarm_flag[];
extern const unsigned char bell_flag[];

extern const unsigned char number_big[11][48];
extern const unsigned char *BMP_logo[];
extern unsigned char BMP_alarm_off[];
extern unsigned char BMP_alarm_on[];

/**
 0: 按  1: 键  3: 声  3: 音  4: 整  5: 点  6: 报
 7: 时  8: 亮  9: 度 10: 主 11: 题 12: 设 13: 置 
14: 贪 15: 吃 16: 蛇 17: 推 18: 箱 19: 子 20: 温 
21: 度 22: 湿 23: 度 24: 秒 25: 表 26: 蓝 27: 牙 
**/
extern unsigned char BMP_chinese[28][32];
/***********************图标声明  codetab.h***************************************/

//默认待机模式
enum models g_model = Standby;	

//温度湿度值
extern float Temprature, Humi;
extern unsigned char dht_tempr_arrary[];
extern unsigned char dht_humi_arrary[];

extern unsigned int i_1ms;			//1ms时标 (0-999)ms


//时间数组2
unsigned char screen_time[6] = {"00:00"};       //通知栏时间
unsigned char standby_day[11] = {"2019-02-01"}; //待机日期 
unsigned char standby_sec[8] = {"00:0010"};     //待机时间 

struct tm RTTime_toset;

//蜂鸣器声音
unsigned char sound_key_flag = 0;			//按键声音是否开启
unsigned char sound_whole_point_ring = 1; 	//整点报时是否开启
unsigned char sound_whole_point_flag = 1;	//只响一次标志位更新


unsigned char i_key = 0;	//按键选择
unsigned char i_choose = 0;	//功能选择
unsigned char once_flag = 1;	//显示一次
unsigned char select_temp = 0;	//保留进入Menu的值


unsigned int alarm_array[3][2] = {
	 6*60+30, 0,		// 6:30  OFF
	 7*60+10, 1,		// 7:00  ON
	14*60+20, 0		//14:20  OFF
};

unsigned char alarm_array_show[3][5] = {
	"0630", 	// 6:30  OFF
	"0730",		// 7:00  ON
	"1420"		//14:20  OFF
};

//闹钟检测
void is_alarm_ring()
{
	if (alarm_array[0][1] == 1)
	{
		if (RTTime.tm_hour*60 + RTTime.tm_min == alarm_array[1][0])
		{
			bell_alarm_ring();
		}
	}
	if (alarm_array[1][1] == 1)
	{
		if (RTTime.tm_hour*60 + RTTime.tm_min == alarm_array[1][0])
		{
			bell_alarm_ring();
		}		
	}
	if (alarm_array[2][1] == 1)
	{
		if (RTTime.tm_hour*60 + RTTime.tm_min == alarm_array[1][0])
		{
			bell_alarm_ring();
		}		
	}	
}

//更新通知栏时钟
void update_time_notice()
{
	RTTime = Time_GetCalendarTime();	//RTTime 在RTC_Time.h
	
//	//秒 第3 4位
//	screen_time[3] = RTTime.tm_sec/10 + '0';
//	screen_time[4] = RTTime.tm_sec%10 + '0';
	
	//分 第0 1位
	screen_time[3] = RTTime.tm_min/10 + '0';
	screen_time[4] = RTTime.tm_min%10 + '0';
	
	//时 第0 1位
	screen_time[0] = RTTime.tm_hour/10 + '0';
	screen_time[1] = RTTime.tm_hour%10 + '0';
	
	
	//整点报时
	if (RTTime.tm_min != 0)	//只响一次标志位更新
		sound_whole_point_flag = 1;
	
	if (sound_whole_point_ring == 1 && sound_whole_point_flag == 1 && RTTime.tm_min == 0)
	{
		sound_whole_point_flag = 0; //只响一次
		bell_short_ring(40);
	} 
	

}

//更新待机时钟
//flag = 1, 更新RTC时间，并更新数组
//flag = 0, 直接更新数组
void update_time_standby(struct tm RTTime_t, unsigned char *t1, unsigned char *t2, unsigned char flag)
{
	if (flag == 1)
	{
		RTTime_t = Time_GetCalendarTime();	//RTTime 在RTC_Time.h
		
		//整点报时
		if (RTTime_t.tm_min != 0)	//只响一次标志位更新
			sound_whole_point_flag = 1;
		
		if (sound_whole_point_ring == 1 && sound_whole_point_flag == 1 && RTTime_t.tm_min == 0)
		{
			sound_whole_point_flag = 0; //只响一次
			bell_short_ring(40);
		} 		
	}
	
	//秒
	t2[5] = RTTime_t.tm_sec/10 + '0';
	t2[6] = RTTime_t.tm_sec%10 + '0';
	
	//分 第0 1位
	t2[3] = RTTime_t.tm_min/10 + '0';
	t2[4] = RTTime_t.tm_min%10 + '0';
	
	//时 第0 1位
	t2[0] = RTTime_t.tm_hour/10 + '0';
	t2[1] = RTTime_t.tm_hour%10 + '0';
	
	//天
	t1[8] = RTTime_t.tm_mday/10 + '0';
	t1[9] = RTTime_t.tm_mday%10 + '0';

	//月
	t1[5] = (RTTime_t.tm_mon + 1)/10 + '0';
	t1[6] = (RTTime_t.tm_mon + 1)%10 + '0';
	
	//年
	t1[0] = RTTime_t.tm_year/1000 + '0';
	t1[1] = RTTime_t.tm_year%1000/100 + '0';
	t1[2] = RTTime_t.tm_year%100/10 + '0';
	t1[3] = RTTime_t.tm_year%10 + '0';
	

	
}

//设置时间
void set_time(int year, int month, int day, int hour, int min, int sec)
{
	struct tm time_value;
	time_value.tm_year = year;
	time_value.tm_mon = month - 1;
	time_value.tm_mday = day;
	time_value.tm_hour = hour;
	time_value.tm_min = min;
	time_value.tm_sec = sec;
    Time_SetCalendarTime(time_value);		
}

//待机时间显示
void standby_time_show()
{
	OLED_ShowStr(0, 2, standby_day, 2, 0);			//大号显示
	
	int i = 0;
	for (i = 0; i < 5; i++)
	{
		OLED_DrawBMP(0 + 12*i, 4, 12 + 12*i, 8,(unsigned char *)number_big[standby_sec[i] - '0'], 0);
	}
	OLED_ShowStr(60, 6, standby_sec + 5, 2, 0);			//秒
}

//500ms更新一次时间
void update_time_500ms()
{
	//500ms更新一次时间
	if (i_1ms % 500 == 0)
	{
		if (screen_time[2] == ':')
			screen_time[2] = ' ';
		else screen_time[2] = ':';
		
		update_time_notice();
		OLED_ShowStr(44, 0, screen_time, 2, 0);
	}

	//is_alarm_ring();	
}           

//待机模式
void model_Standby()
{
	unsigned char gif_i = 0;
	OLED_CLE_part();
	
//	printf("1111\n");
	while (1)
	{
		//退出待机模式
		if (i_key == 1)
		{
			i_key = 0;
			i_choose = 1;
			once_flag = 1;	//在这必须置一
			g_model = Menu;
			
			
			select_temp = 0;
			break;
		}
		
		
//		is_alarm_ring();
//		
//		if (i_key == 3)
//		{
//			bell_alarm_close();
//		}
		
		
		
		
		//待机界面初始化
		if (i_choose == 0 && once_flag == 1)
		{
			once_flag = 0;
			standby_time_show();
		}
		
		//500ms更新一次时间
		if (i_1ms % 500 == 0)	
		{
			OLED_ShowStr(44, 0, (unsigned char *)"     ", 2, 0);	//中部时间关闭
			update_time_standby(RTTime, standby_day, standby_sec, 1);
			standby_time_show();
			
			gif_show(gif_i++);//动态图
			if (gif_i >= 5) gif_i = 0;
			
			
//			uint16_t xxx = FLASH_ReadHalfWord(0x08000000);
//			printf("%d\n", xxx);
		}
	}
}


//显示某一个图标
void show_logo(unsigned char index)
{	
	int i = 0;
	if (index >= 0 && index < 3)
	{
		for (i = 0; i < 3; i++)
		{
			if (i == index) OLED_DrawBMP(0+32*i+10*i,3,32+32*i+10*i,7,(unsigned char *)BMP_logo[i], 1);
			else OLED_DrawBMP(0+32*i+10*i,3,32+32*i+10*i,7,(unsigned char *)BMP_logo[i], 0);
		}
	}
	if (index >= 3 && index < 6)
	{
		for (i = 0; i < 3; i++)
		{
			if ((i+3) == index) OLED_DrawBMP(0+32*i+10*i,3,32+32*i+10*i,7,(unsigned char *)BMP_logo[i+3], 1);
			else OLED_DrawBMP(0+32*i+10*i,3,32+32*i+10*i,7,(unsigned char *)BMP_logo[i+3], 0);
		}		
	}
	if (index >= 6 && index < 8)
	{
		OLED_DrawSpace(64+20,3,96+20,7);
		for (i = 0; i < 2; i++)
		{
			if ((i+6) == index) OLED_DrawBMP(0+32*i+10*i,3,32+32*i+10*i,7,(unsigned char *)BMP_logo[i+6], 1);
			else OLED_DrawBMP(0+32*i+10*i,3,32+32*i+10*i,7,(unsigned char *)BMP_logo[i+6], 0);
		}		
	}
}


//菜单模式
void model_Menu()
{
	unsigned char i_select = 0;  //0-7
	
	//进入Menu之前的值
	if (select_temp != 0)
	{
		i_select = select_temp - 2;
		
	}
	printf("enter menu\n");
	
	//bell_short_ring(1);
	
	OLED_CLE_part();
	while(1)
	{
		//500ms更新一次时间
		update_time_500ms();
		
		//退出菜单界面，返回待机界面
		if (i_key == 3)
		{
			i_key = 0;
			i_choose = 0;
			once_flag = 1; 	//在这必须置一，在串口中断中也置一了
			g_model = Standby;
			
			break;
		}
		
		//选择相应的功能
		if (i_key == 1 && i_choose != 1)	//确认
		{
			once_flag = 1;
			
			i_key = 0;
			
			
			//i_choose最为2-9，settime=2，About=9
			switch(i_choose)
			{
				case Settime: g_model = Settime; break;
				case Alarm: g_model = Alarm; break;
				case Set: g_model = Set; break;
				case Game: break;
				case Music: break;
				case Calendar: break;
				case Tools: g_model = Tools; break;
				case About: g_model = About; break;
				default: break;
			}	
			
			break;
		}
		
		//菜单界面初始化
		if (i_choose == 1 && once_flag == 1)
		{
			once_flag = 0;
			i_choose = i_select + 2;	//i_choose i_select 
			
			
			update_time_notice();
			OLED_ShowStr(44, 0, screen_time, 2, 0);
			
			OLED_CLE_part();
			
			
			show_logo(i_select);
		}
		

		switch(i_key)
		{
			case 4: if (i_select - 1 >= 0) 
					{
						i_key = 0;	//键值归零，防止多加
						once_flag = 0;	//防止初始化显示
						i_select--; 
						i_choose = i_select + 2;	//功能选择
						show_logo(i_select);
					} break;
			case 5: if (i_select - 1 >= 0) 
					{
						i_key = 0;
						once_flag = 0;
						i_select--; 
						i_choose = i_select + 2;
						show_logo(i_select);
					} break;
			case 6: if (i_select + 1 <= 7) 
					{
						i_key = 0;
						once_flag = 0;
						i_select++; 
						i_choose = i_select + 2;
						show_logo(i_select);
					} break;
			case 8: if (i_select + 1 <= 7) 
					{
						i_key = 0;
						once_flag = 0;
						i_select++; 
						i_choose = i_select + 2;
						show_logo(i_select);
					} break;
			default: once_flag = 0; break;
		}	
	}
}

//About界面
void model_About()
{
	OLED_CLE_part();
	
	while (1)
	{
		//500ms更新一次时间
		update_time_500ms();
		
		//退出About界面，返回菜单界面
		if (i_key == 3)
		{
			i_key = 0;
			i_choose = 1;
			once_flag = 1; 	//在这必须置一，在串口中断中也置一了
			g_model = Menu;
			
			select_temp = 9;
			break;
		}
		
		
		//About界面初始化
		if (i_choose == 9 && once_flag == 1)
		{
			once_flag = 0;
			
			OLED_ShowStr(0, 2, (unsigned char *)"Name: Oled Clock", 2, 0);
			OLED_ShowStr(0, 4, (unsigned char *)"Author: ZCL", 2, 0);
			OLED_ShowStr(0, 6, (unsigned char *)"Version: V1.0", 2, 0);
		}
	}
}


void tools_show_list(unsigned char index, unsigned char i_max)
{
	unsigned char i = 0;
	for (i = 0; i < i_max; i++)
	{
		if (i == index)
			OLED_ShowStr(112, i*2+2, (unsigned char *)"<<", 2, 0);
		else OLED_ShowStr(112, i*2+2, (unsigned char *)"  ", 2, 0);
	}
}

void t_and_h()
{
	OLED_CLE_part();
	while (1)
	{
		//500ms更新一次时间
		update_time_500ms();
		
		//退出界面
		if (i_key == 3)
		{
			i_key = 0;
			i_choose = 8; /*******/
			once_flag = 1; 	//在这必须置一，在串口中断中也置一了
			g_model = Tools;
			
			select_temp = 8;  /*******/
			break;
		}
		
		
		//界面初始化 /*******/
		if (once_flag == 1)	/*******/
		{
			once_flag = 0;
			
			DHT12_READ();//读取温湿度数据
			
			OLED_ShowStr(10, 3, dht_tempr_arrary, 2, 0);
			OLED_ShowStr(10, 6, dht_humi_arrary, 2, 0);
		}		
		
		
		if (i_1ms % 200 == 0)//200ms更新一次温湿度，节拍尽量错开
		{
			//bell_short_ring(10);
			DHT12_READ();//读取温湿度数据
			
			OLED_ShowStr(10, 3, dht_tempr_arrary, 2, 0);
			OLED_ShowStr(10, 6, dht_humi_arrary, 2, 0);
		}
		
	}
}

void Stop_watch()
{
	OLED_CLE_part();
	while (1)
	{
		//500ms更新一次时间
		update_time_500ms();
		
		//退出界面
		if (i_key == 3)
		{
			i_key = 0;
			i_choose = 8; /*******/
			once_flag = 1; 	//在这必须置一，在串口中断中也置一了
			g_model = Tools;
			
			select_temp = 8;  /*******/
			break;
		}
		
		
		//界面初始化 /*******/
		if (once_flag == 1)	/*******/
		{
			once_flag = 0;
			
			OLED_ShowStr(0, 2, (unsigned char *)"NONE...", 2, 0);
		}
	}
}

void bluetooth()
{
	OLED_CLE_part();
	while (1)
	{
		//500ms更新一次时间
		update_time_500ms();
		
		//退出界面
		if (i_key == 3)
		{
			i_key = 0;
			i_choose = 8; /*******/
			once_flag = 1; 	//在这必须置一，在串口中断中也置一了
			g_model = Tools;
			
			select_temp = 8;  /*******/
			break;
		}
		
		
		//界面初始化 /*******/
		if (once_flag == 1)	/*******/
		{
			once_flag = 0;
			
			OLED_ShowStr(0, 2, (unsigned char *)"NONE...", 2, 0);
		}
	}
}


//Tools界面
void model_Tools()
{
	printf("enter tools\n");
	
	unsigned char i_select = 0;  //子菜单选项
	unsigned char i_select_max = 3;
	OLED_CLE_part();
	
	while (1)
	{
		//500ms更新一次时间
		update_time_500ms();
		
		//退出界面
		if (i_key == 3)
		{
			i_key = 0;
			i_choose = 1;/*******/
			once_flag = 1; 	//在这必须置一，在串口中断中也置一了
			g_model = Menu;
			
			select_temp = 8;  /*******/
			break;
		}
		
		
		//界面初始化 /*******/
		if (i_choose == 8 && once_flag == 1)	/*******/
		{
			i_choose = 0;	//默认置为0，待机模式，最后退出时会修改
			once_flag = 0;
			
			
			OLED_DrawBMP( 0, 2, 16, 4, BMP_chinese[20], 0);
			OLED_DrawBMP(16, 2, 32, 4, BMP_chinese[21], 0);
			OLED_DrawBMP(32, 2, 48, 4, BMP_chinese[22], 0);
			OLED_DrawBMP(48, 2, 64, 4, BMP_chinese[23], 0);
			
			OLED_DrawBMP( 0, 4, 16, 6, BMP_chinese[24], 0);
			OLED_DrawBMP(16, 4, 32, 6, BMP_chinese[25], 0);	
			
			OLED_DrawBMP( 0, 6, 16, 8, BMP_chinese[26], 0);
			OLED_DrawBMP(16, 6, 32, 8, BMP_chinese[27], 0);			
			
			tools_show_list(0, i_select_max);
		}
		
		switch(i_key)
		{
			case 6: if (i_select - 1 >= 0) {
						i_key = 0;	//键值归零，防止多加
						once_flag = 0;	//防止初始化显示
						i_select--; 
						tools_show_list(i_select, i_select_max);
					} break;
			case 8: if (i_select - 1 >= 0) {
						i_key = 0;
						once_flag = 0;
						i_select--; 
						tools_show_list(i_select, i_select_max);
					} break;
			case 4: if (i_select + 1 <= i_select_max - 1) {
						i_key = 0;
						once_flag = 0;
						i_select++; 
						tools_show_list(i_select, i_select_max);
					} break;
			case 5: if (i_select + 1 <= i_select_max - 1) {
						i_key = 0;
						once_flag = 0;
						i_select++; 
						tools_show_list(i_select, i_select_max);
					} break;
			default: once_flag = 0; break;
		}	
		
		//选择相应的功能
		if (i_key == 1)	//确认
		{
			once_flag = 1;

			switch(i_select)
			{
				case 0: t_and_h(); break;
				case 1: bluetooth(); break;
				case 2: Stop_watch(); break;
				default: break;
			}	
			break;
		}
	}
}

void settime_show_list_t(unsigned char index, unsigned char *t1, unsigned char *t2, unsigned char flag)
{
	if (index >= 0 && index < 4)
	{
		switch (index)
		{
			case 0: OLED_ShowStr_xix(0, 2, t1, 2, 2, flag); break;
			case 1: OLED_ShowStr_xix(2*8, 2, t1+2, 2, 2, flag); break;
			case 2: OLED_ShowStr_xix(5*8, 2, t1+5, 2, 2, flag); break;
			case 3: OLED_ShowStr_xix(8*8, 2, t1+8, 2, 2, flag); break;
			default: break;
		}		
	}
	else if (index == 4)
	{
		int i = 0;
		for (i = 0; i < 2; i++)
		{
			OLED_DrawBMP(0 + 12*i, 4, 12 + 12*i, 8,(unsigned char *)number_big[t2[i] - '0'], flag);
		}		
	}
	else if (index == 5)
	{
		int i = 0;
		for (i = 3; i < 5; i++)
		{
			OLED_DrawBMP(0 + 12*i, 4, 12 + 12*i, 8,(unsigned char *)number_big[t2[i] - '0'], flag);
		}		
	}
	else if (index == 6)
		OLED_ShowStr(60, 6, standby_sec + 5, 2, flag);			//秒
	else return;

}

void settime_show_list(unsigned char index, unsigned char *t1, unsigned char *t2)
{
	int i = 0;
	for (i = 0; i < 7; i++)
	{
		if (i == index) settime_show_list_t(i, t1, t2, 1);
		else settime_show_list_t(i, t1, t2, 0);
	}
}

void add_sub(struct tm *Time , unsigned char i, unsigned char opera)
{	
	//年
	if (i == 0 && opera == '+')
	{
		if (Time->tm_year < 9900)
			Time->tm_year += 100;
	}
	if (i == 0 && opera == '-')
	{
		if (Time->tm_year > 0)
			Time->tm_year -= 100;
	}
	if (i == 1 && opera == '+')
	{
		if (Time->tm_year%100 < 99)
			Time->tm_year += 1;
	}
	if (i == 1 && opera == '-')
	{
		if (Time->tm_year%100 > 0)
			Time->tm_year -= 1;
	}
	
	//月
	if (i == 2 && opera == '+')
	{
		if (Time->tm_mon < 11)
			Time->tm_mon += 1;
		else Time->tm_mon = 0;
	}
	if (i == 2 && opera == '-')
	{
		if (Time->tm_mon > 0)
			Time->tm_mon -= 1;
		else Time->tm_mon = 11;
	}
	//日
	if (i == 3 && opera == '+')
	{
		if (Time->tm_mday < 31)
			Time->tm_mday += 1;
		else Time->tm_mday = 0;
	}

	if (i == 3 && opera == '-')
	{
		if (Time->tm_mday > 1)
			Time->tm_mday -= 1;
		else Time->tm_mday = 31;
	}

	//时
	if (i == 4 && opera == '+')
	{
		if (Time->tm_hour < 23)
			Time->tm_hour += 1;
		else Time->tm_hour = 0;
	}

	if (i == 4 && opera == '-')
	{
		if (Time->tm_hour > 0)
			Time->tm_hour -= 1;
		else Time->tm_hour = 23;
	}

	//分
	if (i == 5 && opera == '+')
	{
		if (Time->tm_min < 59)
			Time->tm_min += 1;
		else Time->tm_min = 0;
	}

	if (i == 5 && opera == '-')
	{
		if (Time->tm_min > 0)
			Time->tm_min -= 1;
		else Time->tm_min = 59;
	}
}


void model_Settime()
{
	printf("enter set time\n");
	unsigned char i_select = 0;  //子菜单选项
	unsigned char i_select_max = 6;	 //最大条目
	unsigned char standby_day_t[11]; //待机日期 
	unsigned char standby_sec_t[8];  //待机时间 
	
	//复制当前时间
	RTTime_toset = RTTime;
	RTTime_toset.tm_sec = 0;
	strcpy((char *)standby_day_t, (char *)standby_day);
	strcpy((char *)standby_sec_t, (char *)standby_sec);

	
	OLED_CLE_part();
	
	while (1)
	{
		//500ms更新一次时间
		update_time_500ms();
		
		//退出界面
		if (i_key == 3)
		{
			i_key = 0;
			i_choose = 1;/*******/
			once_flag = 1; 	//在这必须置一，在串口中断中也置一了
			g_model = Menu;
			
			select_temp = 2;  /*******/
			break;
		}
		
		//界面初始化 /*******/
		if (i_choose == 2 && once_flag == 1)	/*******/
		{
			i_choose = 0;	//默认置为0，待机模式，最后退出时会修改
			once_flag = 0;
			
			update_time_standby(RTTime_toset, standby_day_t, standby_sec_t, 0);
			settime_show_list(i_select, standby_day_t, standby_sec_t);

		}
		
		switch(i_key)
		{
			case 4: if (i_select - 1 >= 0) {
						i_key = 0;	//键值归零，防止多加
						once_flag = 0;	//防止初始化显示
						i_select--;	
						settime_show_list(i_select, standby_day_t, standby_sec_t);
					} break;
			case 5:     i_key = 0;
						once_flag = 0;
						
						add_sub(&RTTime_toset, i_select, '-');
						update_time_standby(RTTime_toset, standby_day_t, standby_sec_t, 0);
						settime_show_list(i_select, standby_day_t, standby_sec_t);
					break;
			case 6: if (i_select + 1 <= i_select_max - 1) {
						i_key = 0;
						once_flag = 0;
						i_select++;
						settime_show_list(i_select, standby_day_t, standby_sec_t);
					} break;
			case 8:     i_key = 0;
						once_flag = 0;
				
						add_sub(&RTTime_toset, i_select, '+');
						update_time_standby(RTTime_toset, standby_day_t, standby_sec_t, 0);
						settime_show_list(i_select, standby_day_t, standby_sec_t);
					break;
			default: once_flag = 0; break;
		}
		
		//选择相应的功能
		if (i_key == 1)	//确认
		{
			once_flag = 1;
			
			//设置时间
			set_time(RTTime_toset.tm_year, RTTime_toset.tm_mon + 1, RTTime_toset.tm_mday, 
					 RTTime_toset.tm_hour, RTTime_toset.tm_min, RTTime_toset.tm_sec);
			
			//退出
			i_key = 0;
			i_choose = 1;/*******/
			once_flag = 1; 	//在这必须置一，在串口中断中也置一了
			g_model = Menu;
			
			select_temp = 2;  /*******/

			
			break;
		}
	}	
}

void alarm_data2show()
{
	alarm_array_show[0][0] = (alarm_array[0][0] / 60) / 10 + '0';
	alarm_array_show[0][1] = (alarm_array[0][0] / 60) % 10 + '0';
	alarm_array_show[0][2] = (alarm_array[0][0] % 60) / 10 + '0';
	alarm_array_show[0][3] = (alarm_array[0][0] % 60) % 10 + '0';
	
	alarm_array_show[1][0] = (alarm_array[1][0] / 60) / 10 + '0';
	alarm_array_show[1][1] = (alarm_array[1][0] / 60) % 10 + '0';
	alarm_array_show[1][2] = (alarm_array[1][0] % 60) / 10 + '0';
	alarm_array_show[1][3] = (alarm_array[1][0] % 60) % 10 + '0';
	                                           
	alarm_array_show[2][0] = (alarm_array[2][0] / 60) / 10 + '0';
	alarm_array_show[2][1] = (alarm_array[2][0] / 60) % 10 + '0';
	alarm_array_show[2][2] = (alarm_array[2][0] % 60) / 10 + '0';
	alarm_array_show[2][3] = (alarm_array[2][0] % 60) % 10 + '0';
	
}

void alarm_show_list_t(unsigned char index, unsigned char flag)
{
	switch (index)
	{
		case 0: OLED_ShowStr_xix( 0, 2, (unsigned char *)alarm_array_show[0], 2, 2, flag); break;
		case 1: OLED_ShowStr_xix(24, 2, (unsigned char *)alarm_array_show[0]+2, 2, 2, flag); break;
		case 2: if (alarm_array[0][1]) OLED_DrawBMP(88, 2, 120, 4, BMP_alarm_on, 0);
				else OLED_DrawBMP(88, 2, 120, 4, BMP_alarm_off, 0);
				if (flag == 1)
				{
					OLED_ShowStr(80, 2, (unsigned char *)">", 2, 0);
					OLED_ShowStr(120, 2, (unsigned char *)"<", 2, 0);
				} else {
					OLED_ShowStr(80, 2, (unsigned char *)" ", 2, 0);
					OLED_ShowStr(120, 2, (unsigned char *)" ", 2, 0);				
				}break;
		case 3: OLED_ShowStr_xix( 0, 4, (unsigned char *)alarm_array_show[1], 2, 2, flag); break;
		case 4: OLED_ShowStr_xix(24, 4, (unsigned char *)alarm_array_show[1]+2, 2, 2, flag); break;
		case 5: if (alarm_array[1][1]) OLED_DrawBMP(88, 4, 120, 6, BMP_alarm_on, 0);
				else OLED_DrawBMP(88, 4, 120, 6, BMP_alarm_off, 0);
				if (flag == 1)
				{
					OLED_ShowStr(80, 4, (unsigned char *)">", 2, 0);
					OLED_ShowStr(120, 4, (unsigned char *)"<", 2, 0);
				} else {
					OLED_ShowStr(80, 4, (unsigned char *)" ", 2, 0);
					OLED_ShowStr(120, 4, (unsigned char *)" ", 2, 0);				
				}break;
		case 6: OLED_ShowStr_xix( 0, 6, (unsigned char *)alarm_array_show[2], 2, 2, flag); break;
		case 7: OLED_ShowStr_xix(24, 6, (unsigned char *)alarm_array_show[2]+2, 2, 2, flag); break;
		case 8: if (alarm_array[2][1]) OLED_DrawBMP(88, 6, 120, 8, BMP_alarm_on, 0);
				else OLED_DrawBMP(88, 6, 120, 8, BMP_alarm_off, 0);
				if (flag == 1)
				{
					OLED_ShowStr(80, 6, (unsigned char *)">", 2, 0);
					OLED_ShowStr(120, 6, (unsigned char *)"<", 2, 0);
				} else {
					OLED_ShowStr(80, 6, (unsigned char *)" ", 2, 0);
					OLED_ShowStr(120, 6, (unsigned char *)" ", 2, 0);				
				}break;
		default: break;
	}
}

void alarm_show_list(int index)
{
	int i = 0;
	for (i = 0; i < 9; i++)
	{
		if (i == index) alarm_show_list_t(i, 1);
		else alarm_show_list_t(i, 0);
	}
}

void alarm_opera_index(unsigned char index, unsigned char opera)
{
	if (index == 2) alarm_array[0][1] = !alarm_array[0][1];
	if (index == 5) alarm_array[1][1] = !alarm_array[1][1];
	if (index == 8) alarm_array[2][1] = !alarm_array[2][1];
	
	//第一个闹钟
	if (index == 0 && opera == '+')
	{
		if (alarm_array[0][0] < 23*60)
			alarm_array[0][0] += 60;
	}
	if (index == 0 && opera == '-')
	{
		if (alarm_array[0][0] > 1*60)
			alarm_array[0][0] -= 60;
	}	
	if (index == 1 && opera == '+')
	{
		if (alarm_array[0][0] < 1439)
			alarm_array[0][0] += 1;
	}
	if (index == 1 && opera == '-')
	{
		if (alarm_array[0][0] > 0)
			alarm_array[0][0] -= 1;
	}
	//第二个闹钟
	if (index == 3 && opera == '+')
	{
		if (alarm_array[1][0] < 23*60)
			alarm_array[1][0] += 60;
	}
	if (index == 3 && opera == '-')
	{
		if (alarm_array[1][0] > 1*60)
			alarm_array[1][0] -= 60;
	}	
	if (index == 4 && opera == '+')
	{
		if (alarm_array[1][0] < 1439)
			alarm_array[1][0] += 1;
	}
	if (index == 4 && opera == '-')
	{
		if (alarm_array[1][0] > 0)
			alarm_array[1][0] -= 1;
	}
	//第三个闹钟
	if (index == 6 && opera == '+')
	{
		if (alarm_array[2][0] < 23*60)
			alarm_array[2][0] += 60;
	}
	if (index == 6 && opera == '-')
	{
		if (alarm_array[2][0] > 1*60)
			alarm_array[2][0] -= 60;
	}	
	if (index == 7 && opera == '+')
	{
		if (alarm_array[2][0] < 1439)
			alarm_array[2][0] += 1;
	}
	if (index == 7 && opera == '-')
	{
		if (alarm_array[2][0] > 0)
			alarm_array[2][0] -= 1;
	}
	
}

void model_Alarm()
{
	printf("enter alarm\n");
	
	unsigned char i_select = 0;  //子菜单选项
	unsigned char i_select_max = 9;
	OLED_CLE_part();
	
	while (1)
	{
		//500ms更新一次时间
		update_time_500ms();
		
		//退出界面
		if (i_key == 3 || i_key == 1)	//闹钟模式确认退出都可以退出
		{
			i_key = 0;
			i_choose = 1;/*******/
			once_flag = 1; 	//在这必须置一，在串口中断中也置一了
			g_model = Menu;
			
			select_temp = 3;  /*******/
			break;
		}
		
		
		//界面初始化 /*******/
		if (i_choose == 3 && once_flag == 1)	/*******/
		{
			i_choose = 0;	//默认置为0，待机模式，最后退出时会修改
			once_flag = 0;
			
			
			OLED_ShowStr_xix(16, 2, (unsigned char *)":", 1, 2, 0); OLED_ShowStr(50, 2, (unsigned char *)"==", 2, 0);
			OLED_ShowStr_xix(16, 4, (unsigned char *)":", 1, 2, 0); OLED_ShowStr(50, 4, (unsigned char *)"==", 2, 0);
			OLED_ShowStr_xix(16, 6, (unsigned char *)":", 1, 2, 0); OLED_ShowStr(50, 6, (unsigned char *)"==", 2, 0);
			
			alarm_data2show();
			alarm_show_list(i_select);
			
		}
		
		switch(i_key)
		{
			case 4: if (i_select - 1 >= 0) {
						i_key = 0;	//键值归零，防止多加
						once_flag = 0;	//防止初始化显示
						i_select--; 
						alarm_show_list(i_select);
					} break;
			case 8: i_key = 0;
					once_flag = 0;
			
					alarm_opera_index(i_select, '+');
					alarm_data2show();
					alarm_show_list(i_select);
					break;
			case 6: if (i_select + 1 <= i_select_max - 1) {
						i_key = 0;
						once_flag = 0;
						i_select++; 
						alarm_show_list(i_select);
					} break;
			case 5: i_key = 0;
					once_flag = 0;

					alarm_opera_index(i_select, '-');
					alarm_data2show();
					alarm_show_list(i_select);
					break;
			default: once_flag = 0; break;
		}
	}	
}

void set_show_list_t(unsigned char index, unsigned char flag)
{
	switch (index)
	{
		case 0: 
			OLED_DrawBMP( 0, 2, 16, 4, BMP_chinese[0], flag);
			OLED_DrawBMP(16, 2, 32, 4, BMP_chinese[1], flag);
			OLED_DrawBMP(32, 2, 48, 4, BMP_chinese[2], flag);
			OLED_DrawBMP(48, 2, 64, 4, BMP_chinese[3], flag); break;
		case 1: if (sound_key_flag) OLED_DrawBMP(88, 2, 120, 4, BMP_alarm_on, 0);
				else OLED_DrawBMP(88, 2, 120, 4, BMP_alarm_off, 0);
				if (flag == 1)
				{
					OLED_ShowStr(80, 2, (unsigned char *)">", 2, 0);
					OLED_ShowStr(120, 2, (unsigned char *)"<", 2, 0);
				} else {
					OLED_ShowStr(80, 2, (unsigned char *)" ", 2, 0);
					OLED_ShowStr(120, 2, (unsigned char *)" ", 2, 0);				
				}break;
		case 2: 
			OLED_DrawBMP( 0, 4, 16, 6, BMP_chinese[4], flag);
			OLED_DrawBMP(16, 4, 32, 6, BMP_chinese[5], flag);
			OLED_DrawBMP(32, 4, 48, 6, BMP_chinese[6], flag);
			OLED_DrawBMP(48, 4, 64, 6, BMP_chinese[7], flag); break;
		case 3: if (sound_whole_point_ring) OLED_DrawBMP(88, 4, 120, 6, BMP_alarm_on, 0);
				else OLED_DrawBMP(88, 4, 120, 6, BMP_alarm_off, 0);
				if (flag == 1)
				{
					OLED_ShowStr(80, 4, (unsigned char *)">", 2, 0);
					OLED_ShowStr(120, 4, (unsigned char *)"<", 2, 0);
				} else {
					OLED_ShowStr(80, 4, (unsigned char *)" ", 2, 0);
					OLED_ShowStr(120, 4, (unsigned char *)" ", 2, 0);				
				}break;
		case 4: 
			OLED_DrawBMP( 0, 6, 16, 8, BMP_chinese[8], flag);
			OLED_DrawBMP(16, 6, 32, 8, BMP_chinese[9], flag);
			OLED_DrawBMP(32, 6, 48, 8, BMP_chinese[12], flag);
			OLED_DrawBMP(48, 6, 64, 8, BMP_chinese[13], flag);	break;
		case 5: 
			OLED_ShowStr(88, 6, (unsigned char *)"....", 2, flag); break;
		default: break;
	}
}

void set_show_list(unsigned char index, unsigned char i_max)
{
	unsigned char i = 0;
	for (i = 0; i < i_max; i++)
	{
		if (i == index)
			set_show_list_t(i, 1);
		else set_show_list_t(i, 0);
	}
}

void model_opera_index(unsigned char index)
{
	if (index == 1) sound_key_flag = !sound_key_flag;
	if (index == 3) sound_whole_point_ring = !sound_whole_point_ring;
}

void model_Set()
{
	printf("enter set\n");
	
	unsigned char i_select = 0;  //子菜单选项
	unsigned char i_select_max = 6;
	OLED_CLE_part();
	
	while (1)
	{
		//500ms更新一次时间
		update_time_500ms();
		
		//退出界面
		if (i_key == 3)
		{
			i_key = 0;
			i_choose = 1;/*******/
			once_flag = 1; 	//在这必须置一，在串口中断中也置一了
			g_model = Menu;
			
			select_temp = 4;  /*******/
			break;
		}
		
		
		//界面初始化 /*******/
		if (i_choose == 4 && once_flag == 1)	/*******/
		{
			i_choose = 0;	//默认置为0，待机模式，最后退出时会修改
			once_flag = 0;
			
			set_show_list(i_select, i_select_max);
		}
		
		switch(i_key)
		{
			case 4: if (i_select - 1 >= 0) {
						i_key = 0;	//键值归零，防止多加
						once_flag = 0;	//防止初始化显示
						i_select--; 
						set_show_list(i_select, i_select_max);
					} break;
			case 8:     i_key = 0;
						once_flag = 0;
						
						model_opera_index(i_select); 
						set_show_list(i_select, i_select_max);break;
			case 6: if (i_select + 1 <= i_select_max - 1) {
						i_key = 0;
						once_flag = 0;
						i_select++; 
						set_show_list(i_select, i_select_max);
					} break;
			case 5: 	i_key = 0;
						once_flag = 0;
						
						model_opera_index(i_select); 
						set_show_list(i_select, i_select_max);break;
			default: once_flag = 0; break;
		}
	}
}


