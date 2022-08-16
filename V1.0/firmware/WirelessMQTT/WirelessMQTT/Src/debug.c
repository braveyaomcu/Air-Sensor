#include <stdio.h>
#include "usart.h"

int fputc(int ch,FILE *p)
{
    HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,1000);
    return ch;
}

void printbuf(uint8_t *buf,size_t len)
{
    for(int i=0;i<len;i++)
    {
        printf("%#04x ",buf[i]);
    }
}


