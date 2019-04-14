#include "stm32f10x_conf.h"
#include "time.h"
#include "RTC_Time.h"

#define FLASH_SIZE 64          //Flash大小

#if FLASH_SIZE<256
  #define SECTOR_SIZE           1024    //1024字节
#else 
  #define SECTOR_SIZE           2048    //2048字节
#endif




//读取指定地址的半字(16bit)
uint16_t FLASH_ReadHalfWord(uint32_t address)
{
  return *(__IO uint16_t*)address; 
}

//读取指定地址的全字(32bit)
uint32_t FLASH_ReadWord(uint32_t address)
{
  uint32_t temp1,temp2;
  temp1=*(__IO uint16_t*)address; 
  temp2=*(__IO uint16_t*)(address+2); 
  return (temp2<<16)+temp1;
}

//从指定地址开始读取多个数据
void FLASH_ReadMoreData(uint32_t startAddress,uint16_t *readData,uint16_t countToRead)
{
  uint16_t dataIndex;
  for(dataIndex=0;dataIndex<countToRead;dataIndex++)
  {
    readData[dataIndex]=FLASH_ReadHalfWord(startAddress+dataIndex*2);
  }
}

//从指定地址开始写入多个数据
void FLASH_WriteMoreData(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite)
{
  if(startAddress<FLASH_BASE||((startAddress+countToWrite*2)>=(FLASH_BASE+1024*FLASH_SIZE)))
  {
    return;//非法地址
  }
  FLASH_Unlock();         //解除写保护
  uint32_t offsetAddress=startAddress-FLASH_BASE;      //计算0X08000000后的实际偏移
  uint32_t sectorPosition=offsetAddress/SECTOR_SIZE;   //计算扇区地址，对于STM32F103VET6为0-255
  
  uint32_t sectorStartAddress=sectorPosition*SECTOR_SIZE+FLASH_BASE;    //对应扇区首地址

  FLASH_ErasePage(sectorStartAddress);//擦除这个扇区
  
  uint16_t dataIndex;
  for(dataIndex=0;dataIndex<countToWrite;dataIndex++)
  {
    FLASH_ProgramHalfWord(startAddress+dataIndex*2,writeData[dataIndex]);
  }
  
  FLASH_Lock();//上锁写保护
}





/*******************************************************************************
* Function Name  : Time_ConvUnixToCalendar(time_t t)
* Description    : 转换UNIX时间戳为日历时间
* Input 		 : u32 t  当前时间的UNIX时间戳
* Output		 : None
* Return		 : struct tm
*******************************************************************************/
struct tm Time_ConvUnixToCalendar(time_t t)
{
	struct tm *t_tm;
	t_tm = localtime(&t);
	t_tm->tm_year += 1900;	//localtime转换结果的tm_year是相对值，需要转成绝对值
	return *t_tm;
}

/*******************************************************************************
* Function Name  : Time_ConvCalendarToUnix(struct tm t)
* Description    : 写入RTC时钟当前时间
* Input 		 : struct tm t
* Output		 : None
* Return		 : time_t
*******************************************************************************/
time_t Time_ConvCalendarToUnix(struct tm t)
{
	t.tm_year -= 1900;  //外部tm结构体存储的年份为2008格式
						//而time.h中定义的年份格式为1900年开始的年份
						//所以，在日期转换时要考虑到这个因素。
	return mktime(&t);
}

/*******************************************************************************
* Function Name  : Time_GetUnixTime()
* Description    : 从RTC取当前时间的Unix时间戳值
* Input 		 : None
* Output		 : None
* Return		 : time_t t
*******************************************************************************/
time_t Time_GetUnixTime(void)
{
	return (time_t)RTC_GetCounter();
}

/*******************************************************************************
* Function Name  : Time_GetCalendarTime()
* Description    : 从RTC取当前时间的日历时间（struct tm）
* Input 		 : None
* Output		 : None
* Return		 : time_t t
*******************************************************************************/
struct tm Time_GetCalendarTime(void)
{
	time_t t_t;
	struct tm t_tm;

	t_t = (time_t)RTC_GetCounter();
	t_tm = Time_ConvUnixToCalendar(t_t);
	return t_tm;
}

/*******************************************************************************
* Function Name  : Time_SetUnixTime()
* Description    : 将给定的Unix时间戳写入RTC
* Input 		 : time_t t
* Output		 : None
* Return		 : None
*******************************************************************************/
void Time_SetUnixTime(time_t t)
{
	RTC_WaitForLastTask();
	RTC_SetCounter((u32)t);
	RTC_WaitForLastTask();
	return;
}

/*******************************************************************************
* Function Name  : Time_SetCalendarTime()
* Description    : 将给定的Calendar格式时间转换成UNIX时间戳写入RTC
* Input 		 : struct tm t
* Output		 : None
* Return		 : None
*******************************************************************************/
void Time_SetCalendarTime(struct tm t)
{
	Time_SetUnixTime(Time_ConvCalendarToUnix(t));
	return;
}

////////////////////////////////////////////////////////////////////////////////
// RTC时钟初始化！
////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************
* Function Name  : RTC_Configuration
* Description    : 来重新配置RTC和BKP，仅在检测到后备寄存器数据丢失时使用
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_Configuration(void)
{
	//启用PWR和BKP的时钟（from APB1）
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	//后备域解锁
	PWR_BackupAccessCmd(ENABLE);
	//备份寄存器模块复位
	BKP_DeInit();
	//外部32.768K其哟偶那个
	RCC_LSEConfig(RCC_LSE_ON);
	//等待稳定
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
	//RTC时钟源配置成LSE（外部32.768K）
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	//RTC开启
	RCC_RTCCLKCmd(ENABLE);
	//开启后需要等待APB1时钟与RTC时钟同步，才能读写寄存器
	RTC_WaitForSynchro();
	//读写寄存器前，要确定上一个操作已经结束
	RTC_WaitForLastTask();

	//设置RTC分频器，使RTC时钟为1Hz
	//RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1)
	RTC_SetPrescaler(32767);
	//等待寄存器写入完成
	RTC_WaitForLastTask();
	//使能秒中断
	RTC_ITConfig(RTC_IT_SEC, ENABLE);    
	//等待写入完成
	RTC_WaitForLastTask();
	return;
}

/*******************************************************************************
* Function Name  : RTC_Config
* Description    : 上电时调用本函数，自动检查是否需要RTC初始化， 
* 					若需要重新初始化RTC，则调用RTC_Configuration()完成相应操作
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_Config(void)
{
	//我们在BKP的后备寄存器1中，存了一个特殊字符0xA5A5
	//第一次上电或后备电源掉电后，该寄存器数据丢失，
	//表明RTC数据丢失，需要重新配置
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
	{
		//重新配置RTC
		RTC_Configuration();
		//配置完成后，向后备寄存器中写特殊字符0xA5A5
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
	}
	else
	{
		//若后备寄存器没有掉电，则无需重新配置RTC
		//这里我们可以利用RCC_GetFlagStatus()函数查看本次复位类型
		if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
		{
			//这是上电复位
		}
		else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
		{
			//这是外部RST管脚复位
		}
		//清除RCC中复位标志
		RCC_ClearFlag();

		//虽然RTC模块不需要重新配置，且掉电后依靠后备电池依然运行
		RCC_RTCCLKCmd(ENABLE);
		//等待RTC时钟与APB1时钟同步
		RTC_WaitForSynchro();

		//使能秒中断
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		//等待操作完成
		RTC_WaitForLastTask();
	}
    RCC_ClearFlag();

    /* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);

#ifdef RTCClockOutput_Enable
	/* Disable the Tamper Pin */
	BKP_TamperPinCmd(DISABLE); /* To output RTCCLK/64 on Tamper pin, the tamper
								 functionality must be disabled */
	/* Enable RTC Clock Output on Tamper Pin */
	BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
#endif
	return;
}

//计算某一天是星期几？  y 年   m 月   d 日
unsigned char CaculateWeekDay(int y,int m, int d)
{
	u8 weekstr=0;
	int week = 0;
	if(m==1||m==2) {
	m+=12;
	y--;
	}
	week=(d+2*m+3*(m+1)/5+y+y/4-y/100+y/400)%7;

	switch(week)
	{
		case 0: weekstr=1; break;
		case 1: weekstr=2; break;
		case 2: weekstr=3; break;
		case 3: weekstr=4; break;
		case 4: weekstr=5; break;
		case 5: weekstr=6; break;
		case 6: weekstr=7; break;
	}
	return weekstr;
 }

 struct tm RTTime;
 void RTC_TimeInit(void)
 {
	RTTime.tm_year = 19 + 2000;
	RTTime.tm_mon = 1 - 1;
	RTTime.tm_mday = 1;
	RTTime.tm_hour = 0;
	RTTime.tm_min = 0;
	RTTime.tm_sec = 0;
    Time_SetCalendarTime(RTTime);
 }
