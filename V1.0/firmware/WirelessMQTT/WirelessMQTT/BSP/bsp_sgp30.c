#include "bsp_sgp30.h"
#include "bsp_dwt.h"
#include <string.h>

#define POLYNOMIAL      0x31

sgp30_obj_t sgp30;

/*
************************************************************
*	函数名称：	sgp30_CheckCrc
*
*	函数功能：	检查数据正确性
*
*	入口参数：	data：读取到的数据
*				nbrOfBytes：需要校验的数量
*
*	返回参数：	校验值
*

************************************************************
*/
static char sgp30_CheckCrc(char data[], char nbrOfBytes)
{
	
    char crc = 0xFF;
    char bit = 0;
    char byteCtr = 0;
	
    //calculates 8-Bit checksum with given polynomial
    for(byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr)
    {
        crc ^= (data[byteCtr]);
        for ( bit = 8; bit > 0; --bit)
        {
            if (crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL;
            else crc = (crc << 1);
        }
    }
	
	return crc;
}

static void sgp30_write_cmd(uint16_t cmd)
{   
    
    char buf[2];
    
    buf[0]=cmd>>8;
    buf[1]=cmd;
    
    IIC_Select(IIC_SGP);
    
    IIC_Start();
    
    IIC_Send_Byte(SGP30_ADDR|0x00);
    IIC_Wait_Ack();
    
    
    IIC_Send_Byte(buf[0]);
    IIC_Wait_Ack();

    IIC_Send_Byte(buf[1]);
    IIC_Wait_Ack();
   
}

static int sgp30_read_data(uint16_t *data)
{   
    char crc=0;
    char buf[2];
    
    IIC_Select(IIC_SGP);
    
    IIC_Start();
    IIC_Send_Byte(SGP30_ADDR|0x01);
    IIC_Wait_Ack();
    //调试发现
    delay_ms(1);
    
    buf[0]=IIC_Read_Byte(1);
    buf[1]=IIC_Read_Byte(1);
    crc=IIC_Read_Byte(1);

    
    IIC_Stop();
    
    //CRC校验
    if(crc == sgp30_CheckCrc(buf,2))
    {
        *data=buf[0];
        (*data)<<=8;
        (*data)|=buf[1];
       
    }
    else
        return 0;
    
    return 1;
}

static int sgp30_read_data2(uint16_t *data,uint16_t *data1)
{   
    char crc=0,crc1=0;
    char buf[2],buf1[2];
    
    IIC_Select(IIC_SGP);
    
    IIC_Start();
    IIC_Send_Byte(SGP30_ADDR|0x01);
    IIC_Wait_Ack();
    delay_ms(1);
    
    buf[0]=IIC_Read_Byte(1);
    buf[1]=IIC_Read_Byte(1);
    crc=IIC_Read_Byte(1);
    
    buf1[0]=IIC_Read_Byte(1);
    buf1[1]=IIC_Read_Byte(1);
    crc1=IIC_Read_Byte(0);
    
    IIC_Stop();
    
    //CRC校验
    if(crc == sgp30_CheckCrc(buf,2))
    {
        *data=buf[0];
        (*data)<<=8;
        (*data)|=buf[1];
       
    }
    else
        return 0;
    
    if(crc1 == sgp30_CheckCrc(buf1,2))
    {
        *data1=buf1[0];
        (*data1)<<=8;
        (*data1)|=buf1[1];
       
    }
    else
        return 0;
    
    return 1;
}

static void sgp30_write_data(uint16_t data)
{   
    char crc=0;
    char buf[2];

    buf[0]=data>>8;
    buf[1]=data;
    
    IIC_Select(IIC_SGP);
    
    crc = sgp30_CheckCrc(buf,2);
    
    IIC_Send_Byte(buf[0]);
    IIC_Wait_Ack();

    IIC_Send_Byte(buf[1]);
    IIC_Wait_Ack();
    
    IIC_Send_Byte(crc);
    IIC_Wait_Ack();
    
    IIC_Stop();
    
    
}

void sgp30_init(void)
{

    u8 data=3;
    
    IIC_Select(IIC_SGP);
    
    IIC_Init();
    
    memset(&sgp30,0,sizeof(sgp30));
    
    while(data)
    {
        IIC_Start();
    
        IIC_Send_Byte(SGP30_ADDR|0x00);
        if(IIC_Wait_Ack())
        {
            
            IIC_Stop();
            data--;
        }
        else
            break;
       
    }
    if(data == 0)
    {
        sgp30.sgp30_status=SGP30_STATUS_ERR;
        return;
    }
    else
        sgp30.sgp30_status=SGP30_STATUS_OK;
    data=SGP30_IAQ_INIT>>8;
    IIC_Send_Byte(data);
    IIC_Wait_Ack();
    
    data=SGP30_IAQ_INIT&0xFF;
    IIC_Send_Byte(data);
    IIC_Wait_Ack();

}

void sgp30_reset(void)
{
    u8 data;
    //reset
    IIC_Select(IIC_SGP);
    
    IIC_Start();
    
    IIC_Send_Byte(SGP30_ADDR|0x00);
    IIC_Wait_Ack();
    
    data=SGP30_RESET>>8;
    IIC_Send_Byte(data);
    IIC_Wait_Ack();
    
    data=SGP30_RESET&0xFF;
    IIC_Send_Byte(data);
    IIC_Wait_Ack();
    
    IIC_Stop();
}

void sgp30_get_id(void)
{
    
    sgp30_write_cmd(SGP_GET_ID);
    delay_ms(500);
    sgp30_read_data(&sgp30.deviceID);
    
}

static uint8_t sgp_tick=0;
static uint8_t sgp_status=0;
void sgp30_get_meas(void)
{
    IIC_Select(IIC_SGP);
    
    switch(sgp_status)
    {
        case 0:
            sgp30_write_cmd(SGP30_IAQ_MEAS);
            sgp_status++;
            break;
        case 1:
            sgp_tick++;
            if(sgp_tick>5)
            {
                sgp_status++;
                sgp_tick=0;
            }
            break;
        case 2:
            sgp30_read_data2(&sgp30.sgp30Data.co2,&sgp30.sgp30Data.tvoc);
            sgp_status=0;
    }
    
}

void sgp30_get_baseline(void)
{
    
    sgp30_write_cmd(SGP30_IAQ_GET_BASELINE);
    delay_ms(50);
    sgp30_read_data2(&sgp30.co2Baseline,&sgp30.tvocBaseline);
}

void sgp30_get_feature(void)
{  
    sgp30_write_cmd(SGP30_GET_FEATURE_SET);
    delay_ms(50);
    sgp30_read_data(&sgp30.featureSet);
}

//该指令用于自检和生产线运行
char sgp30_meas_test(void)
{
    uint16_t set=0;
    sgp30_write_cmd(SGP30_MEAS_TEST);
    delay_ms(500);
    sgp30_read_data(&set);
    
    if(set ==  0xD400)
        return 1;
    else        
        return 0;
}

void sgp30_set_baseline(uint16_t blco2,uint16_t bltvoc)
{
    sgp30_write_cmd(SGP30_IAQ_SET_BASELINE);
    delay_ms(50);
    sgp30_write_data(blco2);
    sgp30_write_data(bltvoc);
}

void sgp30_set_humidity(uint16_t humi)
{
    sgp30_write_cmd(SGP30_SET_HUMIDITY);
    delay_ms(50);
    sgp30_write_data(humi);
}
