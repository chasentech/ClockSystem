#ifndef _USART_H_
#define _USART_H_

#include <stdio.h>
#include <USART.h>

//extern unsigned char USART1_rece_buff[];


extern void USART1_init(void);
extern void USART2_init(void);
extern void USART_NVIC_init(void);
//extern void USART1_send_datas(unsigned char *data);


#endif
