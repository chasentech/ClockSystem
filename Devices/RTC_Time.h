#ifndef _RTC_TIME_H_
#define _RTC_TIME_H_
#include <time.h>
#include "stm32f10x_conf.h"

//#define RTCClockSource_LSI   /* Use the internal 32 KHz oscillator as RTC clock source */
#define RTCClockSource_LSE   /* Use the external 32.768 KHz oscillator as RTC clock source */
//#define RTCClockOutput_Enable  /* RTC Clock/64 is output on tamper pin(PC.13) */

extern struct tm RTTime; //RTC_Time.c

struct tm Time_ConvUnixToCalendar(time_t t);
time_t Time_ConvCalendarToUnix(struct tm t);
time_t Time_GetUnixTime(void);
struct tm Time_GetCalendarTime(void);
void Time_SetUnixTime(time_t);
void Time_SetCalendarTime(struct tm t);

void RTC_Configuration(void);
void RTC_Config(void);
unsigned char CaculateWeekDay(int y,int m, int d);
void RTC_TimeInit(void);



//读取指定地址的半字(16bit)
uint16_t FLASH_ReadHalfWord(uint32_t address);

//读取指定地址的全字(32bit)
uint32_t FLASH_ReadWord(uint32_t address);

//从指定地址开始读取多个数据
void FLASH_ReadMoreData(uint32_t startAddress,uint16_t *readData,uint16_t countToRead);

//从指定地址开始写入多个数据
void FLASH_WriteMoreData(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite);

#endif
