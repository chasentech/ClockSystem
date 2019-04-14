#ifndef _TIM_H_
#define _TIM_H_

extern unsigned int i_delay;

//extern void TIM3_init(unsigned short psc, unsigned short pre);
extern void TIM2_init(unsigned short psc, unsigned short pre);
extern void TIM2_NVIC_init(void);

#endif
