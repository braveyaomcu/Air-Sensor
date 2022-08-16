#include "myiic.h"
#include "bsp_bh1750.h"
#include "bsp_dwt.h"

static uint8_t databuf[2];

brigh_t brigh;

void bh1750_init(void)
{
    IIC_Select(IIC_GENERAL);
    
    IIC_Init();
    IIC_Start();
    IIC_Send_Byte(BH1750_ADDR|0x00);
    if(IIC_Wait_Ack())
    {
        brigh.brigh_status=BRIGH_STATUS_ERR;
        return;
    }
    else
        brigh.brigh_status=BRIGH_STATUS_OK;
    IIC_Send_Byte(BH1750_POWER_ON);
    IIC_Stop();
}

void bh1750_start(void)
{
    IIC_Select(IIC_GENERAL);
    
    IIC_Start();
    IIC_Send_Byte(BH1750_ADDR|0x00);
    IIC_Wait_Ack();
    IIC_Send_Byte(BH1750_CHRES_MODE1);
    IIC_Wait_Ack();
    IIC_Stop();
}

void bh1750_read(void)
{
    
//    IIC_Start();
//    IIC_Send_Byte(BH1750_ADDR|0x00);
//    IIC_Wait_Ack();
//    IIC_Send_Byte(BH1750_CHRES_MODE1);
//    IIC_Wait_Ack();
//    IIC_Stop();
    
    //delay_ms(200);
    IIC_Select(IIC_GENERAL);
    
    IIC_Start();
    IIC_Send_Byte(BH1750_ADDR|0x01);
    IIC_Wait_Ack();
    databuf[1]=IIC_Read_Byte(1);
    databuf[0]=IIC_Read_Byte(0);
    IIC_Stop();
    
    brigh.result=databuf[1];
    brigh.result<<=8;
    brigh.result|=databuf[0];
    
    brigh.lux=brigh.result/1.2;
}

