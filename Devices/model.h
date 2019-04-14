#ifndef _MODEL_H_
#define _MODEL_H_

#include "model.h"

//工作模式
enum models
{
	Standby = 0,//待机模式
	Menu,		//选择功能模式
	Settime, 	//设置时间模式
	Alarm,	//闹钟模式
	Set,	//设置模式
	Game,	//游戏模式
	Music,	//音乐模式
	Calendar,//日历模式
	Tools,	//工具箱模式
	About	//关于模式
};


void set_time(int year, int month, int day, int hour, int min, int sec);

void model_Standby(void);
void model_Menu(void);
void model_Settime(void);
void model_Alarm(void);
void model_Set(void);
void model_Game(void);
void model_Music(void);
void model_Calendar(void);
void model_Tools(void);
void model_About(void);

#endif
