#include "bsp_dwt.h"
#include "stm32f1xx_hal.h"

#if USE_DWT_DELAY


#define  DWT_CR      *(__IO uint32_t *)0xE0001000
#define  DWT_CYCCNT  *(__IO uint32_t *)0xE0001004
#define  DEM_CR      *(__IO uint32_t *)0xE000EDFC

#define  ITM_LAR     *(__IO uint32_t *)0xE0001FB0


#define  DEM_CR_TRCENA                   (1 << 24)
#define  DWT_CR_CYCCNTENA                (1 <<  0)

void dwt_init(void)
{
    /* ʹ��DWT���� */
    DEM_CR |= (uint32_t)DEM_CR_TRCENA;                

    //ITM_LAR         = 0xC5ACCE55;
    /* DWT CYCCNT�Ĵ���������0 */
    DWT_CYCCNT = (uint32_t)0u;
    
    /* ʹ��Cortex-M DWT CYCCNT�Ĵ��� */
    DWT_CR |= (uint32_t)DWT_CR_CYCCNTENA;
}

uint32_t dwt_read(void)
{        
  return ((uint32_t)DWT_CYCCNT);
}

void dwt_delay_us(__IO uint32_t us)
{
  uint32_t ticks;
  uint32_t told,tnow,tcnt=0;

  /* �ں����ڲ���ʼ��ʱ����Ĵ����� */  
#if (CPU_TS_INIT_IN_DELAY_FUNCTION)  
  /* ��ʼ��ʱ��������� */
  dwt_init();
#endif
  
  ticks = us * (get_cpu_clk_freq() / 1000000);  /* ��Ҫ�Ľ����� */      
  tcnt = 0;
  told = (uint32_t)dwt_read();         /* �ս���ʱ�ļ�����ֵ */

  while(1)
  {
    tnow = (uint32_t)dwt_read();  
    if(tnow != told)
    { 
        /* 32λ�������ǵ��������� */    
      if(tnow > told)
      {
        tcnt += tnow - told;  
      }
      /* ����װ�� */
      else 
      {
        tcnt += UINT32_MAX - told + tnow; 
      } 
      
      told = tnow;

      /*ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳� */
      if(tcnt >= ticks)break;
    }  
  }
}

#endif
