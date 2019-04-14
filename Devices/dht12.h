#ifndef _DHT12_H_
#define _DHT12_H_

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

//IO方向设置
#define SDA_IN()  \
{\
	GPIO_InitTypeDef GPIO_InitStructure; \
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;\
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;\
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
	GPIO_Init(GPIOA, &GPIO_InitStructure);\
}//上拉输入
#define SDA_OUT() \
{\
	GPIO_InitTypeDef GPIO_InitStructure; \
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;\
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;\
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
	GPIO_Init(GPIOA, &GPIO_InitStructure);\
}//通用推挽输出


////IO方向设置
//#define SDA_IN() {GPIOB->CRH&=0XF0FFFFFF;GPIOB->CRH|=0x08000000;}	//上下拉输入
//#define SDA_OUT() {GPIOB->CRH&=0XF0FFFFFF;GPIOB->CRH|=0x03000000;}//通用推挽输出



//IO操作函数
#define IIC_SCL    PBout(13) //SCL输出
#define SEND_SDA   PAout(11) //SDA输出
#define READ_SDA   PAin(11)  //SDA输入





extern void dht12_init(void);
extern unsigned char DHT12_Rdata(void);
extern unsigned char DHT12_READ(void);

#endif
