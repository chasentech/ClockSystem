#ifndef __OLED_I2C_H
#define	__OLED_I2C_H

#include "stm32f10x.h"

#define OLED_ADDRESS	0x78 //通过调整0R电阻,屏可以0x78和0x7A两个地址 -- 默认0x78

void I2C_Configuration(void);
void I2C_WriteByte(uint8_t addr,uint8_t data);
void WriteCmd(unsigned char I2C_Command);
void WriteDat(unsigned char I2C_Data);
void OLED_Init(void);
void OLED_SetPos(unsigned char x, unsigned char y);
//void OLED_SetPos_Curr(unsigned char x, unsigned char y); //设置起始点坐标,精确到xy
void OLED_Fill(unsigned char fill_Data);
void OLED_CLE_part(void);
void OLED_CLS(void);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize, unsigned char flag);
void OLED_ShowStr_xix(unsigned char x, unsigned char y, unsigned char ch[], unsigned char num, unsigned char TextSize, unsigned char flag);//显示指定序列的字符串
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N);
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[], unsigned char flag);
void OLED_DrawSpace(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1);


void battery_show(unsigned char grade);
void gif_show(unsigned char i);

#endif
