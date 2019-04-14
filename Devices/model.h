#ifndef _MODEL_H_
#define _MODEL_H_

#include "model.h"

//����ģʽ
enum models
{
	Standby = 0,//����ģʽ
	Menu,		//ѡ����ģʽ
	Settime, 	//����ʱ��ģʽ
	Alarm,	//����ģʽ
	Set,	//����ģʽ
	Game,	//��Ϸģʽ
	Music,	//����ģʽ
	Calendar,//����ģʽ
	Tools,	//������ģʽ
	About	//����ģʽ
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
