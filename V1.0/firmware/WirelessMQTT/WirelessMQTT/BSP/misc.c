#include "misc.h"

#define MISC_LED_DELAY	500

static uint8_t	misc_led_status;

void led_pool(void)
{
	static uint32_t misc_delay=0;
	if(HAL_GetTick() - misc_delay < MISC_LED_DELAY )
		return;

	misc_delay=HAL_GetTick();

	switch(misc_led_status)
	{
		case 0:
			LED1_ON;
			misc_led_status=1;
			break;
		case 1:
			LED1_OFF;
			misc_led_status=0;
			break;
		default:
			break;
	}
    
}
