#ifndef _BSP_DWT_H
#define _BSP_DWT_H

#include <stdint.h>

#define USE_DWT_DELAY   1

#if USE_DWT_DELAY
#define USE_TICK_DELAY		0		/* ��ʹ��SysTick��ʱ */
#else
#define USE_TICK_DELAY		1		/* ʹ��SysTick��ʱ */
#endif

#if USE_DWT_DELAY

/* ��ȡ�ں�ʱ��Ƶ�� */            
#define get_cpu_clk_freq()          (72000000U)
#define system_clk_freq             (72000000U)

#define CPU_TS_INIT_IN_DELAY_FUNCTION   0  

uint32_t dwt_read(void);
void dwt_init(void);

void dwt_delay_us(uint32_t us);
#define dwt_delay_ms(ms)     dwt_delay_us(ms*1000)
#define dwt_delay_s(s)       dwt_delay_ms(s*1000)

#define delay_ms(ms)  	dwt_delay_ms(ms)
#define delay_us(us)  	dwt_delay_us(us)
#define delay_s(s)  	dwt_delay_s(s)

#endif

#endif
