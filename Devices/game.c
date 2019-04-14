#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "OLED_I2C.h"
#include "delay.h"
#include "game.h"
#include "USART.h"
#include "bell.h"

unsigned char game_auto = 0;
static unsigned char sound_game_flag = 0;	//游戏声音是否开启

typedef struct Point
{
	int y;
	int x;
}Point;

#define SNAKE_NUM 50
#define MAP_SIZE_COL 40	//x
#define MAP_SIZE_ROW 16	//y

enum Direction
{
	Direction_None = 0,
	Direction_Up,
	Direction_Left,
	Direction_Down,
	Direction_Right
};

static unsigned char snake_map[MAP_SIZE_ROW][MAP_SIZE_COL] = {0};	//
static Point snake_array[SNAKE_NUM] = {0};		//
static unsigned char snake_lenth = 4;		//
static Point food;

static enum Direction direc = Direction_None;



static unsigned char paint_Y_tab[16] = 
{
//	0000(0), 0001(1), 0010(2), 0011(3), 
//	0100(4), 0101(5), 0110(6), 0111(7), 
//	1000(8), 1001(9), 1010(10),1011(11),
//	1100(12),1101(13),1110(14),1111(15),
	
	0x00, 0x03, 0x0c, 0x0f,
	0x30, 0x33, 0x3c, 0x3f,
	0xc0, 0xc3, 0xcc, 0xcf,
	0xf0, 0xf3, 0xfc, 0xff,
};

unsigned char my_pow(unsigned char a, unsigned char b)
{
	unsigned char result = 1;
	while (b--)
	{
		result *= a; 
	}
	return result;
}

//40*16
void draw_point(unsigned char y, unsigned char x, unsigned char value)
{
	snake_map[y][x] = value;
	OLED_SetPos(x*2 + 8, (y / 4) + 3);
	
	unsigned char temp = 0;
	unsigned char k = 0;
	for (k = 0; k < 4; k++)
	{
		if (snake_map[y-(y%4)+k][x] == 1 || snake_map[y-(y%4)+k][x] == 2)
			temp += my_pow(2, k);
	}
	
	WriteDat(paint_Y_tab[temp]);
	WriteDat(paint_Y_tab[temp]);
}

void wipe_point(unsigned char y, unsigned char x)
{
	snake_map[y][x] = 0;
	OLED_SetPos(x*2 + 8, (y / 4) + 3);
	
	unsigned char temp = 0;
	unsigned char k = 0;
	for (k = 0; k < 4; k++)
	{
		if (snake_map[y-(y%4)+k][x] == 1 || snake_map[y-(y%4)+k][x] == 2)
			temp += my_pow(2, k);
	}
	
	WriteDat(paint_Y_tab[temp]);
	WriteDat(paint_Y_tab[temp]);
}

void map_init()
{
	unsigned char y = 0;
	unsigned char x = 0;
	for (y = 0; y < MAP_SIZE_ROW; y++)
	{
		for (x = 0; x < MAP_SIZE_COL; x++)
		{
			snake_map[y][x] = 0;
		}
	}
}

void game_init()
{
	map_init();
	
	//不允许自动
	game_auto = 0;
	
	//清除屏幕
	unsigned char m,n;
	for(m=2;m<8;m++)
	{
		WriteCmd(0xb0+m);	//page0-page1
//		WriteCmd(0x00);		//low column start address
//		WriteCmd(0x10);		//high column start address
		
		WriteCmd(((0&0xf0)>>4)|0x10);
		WriteCmd((0&0x0f)|0x00);  //原来是：WriteCmd((x&0x0f)|0x01);有问题
		for(n=0;n<128;n++)
		{
			WriteDat(0x00);
		}
	}


	
	
	int i = 0;
	//游戏边界
	OLED_SetPos(0, 2);
	for (i = 0; i < 96; i++)	//上一横
	{
		WriteDat(0x81);
	}
	for (i = 96; i < 128; i++)	//上二行
	{
		WriteDat(0x01);
	}	
	OLED_SetPos(0, 7);
	for (i = 0; i < 96; i++)	//下一行
	{
		WriteDat(0x81);
	}
	for (i = 96; i < 128; i++)	//下二行
	{
		WriteDat(0x80);
	}
	
	
	for (i = 2; i < 8; i++)//左
	{
		OLED_SetPos(0, i);
		WriteDat(0xff);
		OLED_SetPos(7, i);
		WriteDat(0xff);
	}
	for (i = 2; i < 8; i++)//右
	{
		OLED_SetPos(127, i);
		WriteDat(0xff);
		OLED_SetPos(88, i);
		WriteDat(0xff);
	}
	
	//分割线，竖直
	for (i = 2; i < 8; i++)
	{
		OLED_SetPos(96, i);
		WriteDat(0xff);
	}

	//得分，水平
	OLED_SetPos(97, 4);
	for (i = 97; i < 127; i++)
	{
		WriteDat(0x80);
	}
	
	OLED_ShowStr(97, 3, (unsigned char *)"score", 1, 0);
	OLED_ShowStr(100, 6, (unsigned char *)"00", 1, 0);
	
	
	//食物
	food.y = 8;
	food.x = 20;
	draw_point(food.y, food.x, 2);

	//蛇身
	Point temp = {0, 0};
	snake_array[0] = temp;
	temp.x = 1;
	snake_array[1] = temp;
	temp.x = 2;
	snake_array[2] = temp;
	temp.x = 3;
	snake_array[3] = temp;
	
	draw_point(snake_array[0].y, snake_array[0].x, 1);
	draw_point(snake_array[1].y, snake_array[1].x, 1);
	draw_point(snake_array[2].y, snake_array[2].x, 1);
	draw_point(snake_array[3].y, snake_array[3].x, 1);
	
	//长度为4
	snake_lenth = 4;
	
	//方向初始化右
	direc = Direction_Right;
}


unsigned char snake_touch(Point point)
{
	int i = 0;
	for (i = 0; i < snake_lenth; i++)
	{
		if (point.x == snake_array[i].x && point.y == snake_array[i].y)
		{
			return 1;
		}
	}
	
	return 0;
}

void update_food()
{
	//随机种子
	//srand(gSystickMsecCount);
	
	food.y = rand()%16;
	food.x = rand()%40;
	draw_point(food.y, food.x, 2);
	
	
	//食物不能出现在蛇身上
	int i = 0;
	for (i = 0; i < snake_lenth; i++)
	{
		if (food.y == snake_array[i].y && food.x == snake_array[i].x)
		{
			food.y = rand()%16;
			food.x = rand()%40;
			draw_point(food.y, food.x, 2);
		}
	}
}

void snake_move(unsigned char select)
{
	switch(select)
	{
		case 'R': if (direc == Direction_Left) {
					if (sound_game_flag) bell_short_ring(1);
					return;
				}
				else direc = Direction_Right;   break;
		case 'L': if (direc == Direction_Right) {
					if (sound_game_flag) bell_short_ring(1);
					return;
				}
				else direc = Direction_Left;    break;
		case 'D': if (direc == Direction_Up) {
					if (sound_game_flag) bell_short_ring(1);
					return;
				}
				else direc = Direction_Down;    break;
		case 'U': if (direc == Direction_Down) {
					if (sound_game_flag) bell_short_ring(1);
					return;
				}
				else direc = Direction_Up;      break;
		default: break;
	}


	//目标点
	Point snake_temp = {0, 0};
	
	switch(select)
	{
		case 'R': snake_temp.y = snake_array[snake_lenth - 1].y;
				  snake_temp.x = snake_array[snake_lenth - 1].x + 1;  
				  break;
		case 'L': snake_temp.y = snake_array[snake_lenth - 1].y;
				  snake_temp.x = snake_array[snake_lenth - 1].x - 1;  
				  break;
		case 'D': snake_temp.y = snake_array[snake_lenth - 1].y + 1;
				  snake_temp.x = snake_array[snake_lenth - 1].x;  
				  break;
		case 'U': snake_temp.y = snake_array[snake_lenth - 1].y - 1;
				  snake_temp.x = snake_array[snake_lenth - 1].x;  
				  break;
		default: break;
	}
	
	
	
	//越界
	if (snake_temp.x >= MAP_SIZE_COL || snake_temp.x < 0 || 
		snake_temp.y >= MAP_SIZE_ROW || snake_temp.y < 0)
	{
		if (sound_game_flag) bell_short_ring(1);
		return;
	}
	
	//分数
	unsigned char socre_temp[3] = {0};
	
	
	//遇到食物
	if (snake_map[snake_temp.y][snake_temp.x] == 2)
	{
		if (sound_game_flag) bell_short_ring(4);
		snake_lenth++;
		if (snake_lenth >= 50)
		{
			if (sound_game_flag) bell_short_ring(10);
			
			//游戏结束
			game_init();

			return;
		}
		
		//蛇头更新
		snake_array[snake_lenth - 1] = snake_temp;
		draw_point(snake_array[snake_lenth - 1].y, snake_array[snake_lenth - 1].x, 1);
		
		//食物更新
		update_food();
		
		//更新分数
		socre_temp[0] = (snake_lenth-4) / 10 + '0';
		socre_temp[1] = (snake_lenth-4) % 10 + '0';
		socre_temp[2] = '\0';
		OLED_ShowStr(100, 6, (unsigned char *)socre_temp, 1, 0);
		
	}
	else if (snake_touch(snake_temp))		//蛇头碰到蛇身
	{
		if (sound_game_flag) bell_short_ring(10);
		
		//更新分数
		socre_temp[0] = '0';
		socre_temp[1] = '0';
		socre_temp[2] = '\0';
		OLED_ShowStr(100, 6, (unsigned char *)socre_temp, 1, 0);
		
		snake_lenth = 0;
		
		//游戏结束
		game_init();
		
		return;
	}
	else
	{
		//蛇尾map清除
		wipe_point(snake_array[0].y, snake_array[0].x);	//map
		
		//蛇身date更新
		int i = 0;
		for (i = 0; i < snake_lenth - 1; i++)
		{
			snake_array[i] = snake_array[i+1];
			draw_point(snake_array[i].y, snake_array[i].x, 1);//map
		}
		
		//蛇头date & map更新
		snake_array[snake_lenth - 1] = snake_temp;
		draw_point(snake_array[snake_lenth - 1].y, snake_array[snake_lenth - 1].x, 1);
		
		
	}
	
	
//	OLED_ShowStr(100, 6, (unsigned char *)socre_temp, 1);

}

//贪吃蛇手动
void snake_manual(unsigned char i)
{
	if (game_auto)
	{
		switch (direc)
		{
			case Direction_Left:  snake_move('L'); break;
			case Direction_Down:  snake_move('D'); break;
			case Direction_Right: snake_move('R'); break;
			case Direction_Up:    snake_move('U'); break;
			default: break;
		}		
	}
	
	switch (i)
	{
		case 4: snake_move('L'); break;
		case 5: snake_move('D'); break;
		case 6: snake_move('R'); break;
		case 8: snake_move('U'); break;
		default: break;
	}
}

void paint_test()
{
	

}


