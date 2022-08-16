#include "myiic.h"
#include "iic_low_level.h"
#include "bsp_dwt.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//IIC驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/9
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////



iic_user_t  iic_user[IIC_MAX];


static iic_user_t *p;
static GPIO_PinState _iic_get_sda(void)
{
    return HAL_GPIO_ReadPin(p->iic_port.iic_sda_port,p->iic_port.iic_sda_pin);
}

static void _iic_set_sda(GPIO_PinState PinState)
{
    HAL_GPIO_WritePin(p->iic_port.iic_sda_port,p->iic_port.iic_sda_pin,PinState);
}

static void _iic_set_scl(GPIO_PinState PinState)
{
    HAL_GPIO_WritePin(p->iic_port.iic_scl_port,p->iic_port.iic_scl_pin,PinState);
}

static void _iic_sda_dir(iic_dir_t x)
{
    uint32_t    temp=0x0000000F;
    uint16_t    temp_pin;
    uint8_t     pinx=0;
    
    temp_pin=p->iic_port.iic_sda_pin>>1;
    while(temp_pin)
    {
        temp_pin>>=1;
        pinx++;
    }
    
    pinx=pinx%8;
    pinx=pinx*4;
    temp=temp<<(pinx*1);
    if(p->iic_port.iic_sda_pin <GPIO_PIN_8)
    {
        //temp=temp<<(pinx*4);
        p->iic_port.iic_sda_port->CRL&=~temp;
        if(x == IIC_SDA_IN)
            p->iic_port.iic_sda_port->CRL|=(u32)8<<pinx;
        else
            p->iic_port.iic_sda_port->CRL|=(u32)3<<pinx;
    }
    else
    {
        p->iic_port.iic_sda_port->CRH&=~temp;
        if(x == IIC_SDA_IN)
            p->iic_port.iic_sda_port->CRH|=(u32)8<<pinx;
        else
            p->iic_port.iic_sda_port->CRH|=(u32)3<<pinx;
    }
}

void iic_ll_init(void)
{
    iic_user[IIC_SGP].iic_port.iic_scl_port=GPIOB;
    iic_user[IIC_SGP].iic_port.iic_scl_pin=GPIO_PIN_5;
    iic_user[IIC_SGP].iic_port.iic_sda_port=GPIOB;
    iic_user[IIC_SGP].iic_port.iic_sda_pin=GPIO_PIN_4;
    
    iic_user[IIC_GENERAL].iic_port.iic_scl_port=GPIOB;
    iic_user[IIC_GENERAL].iic_port.iic_scl_pin=GPIO_PIN_8;
    iic_user[IIC_GENERAL].iic_port.iic_sda_port=GPIOB;
    iic_user[IIC_GENERAL].iic_port.iic_sda_pin=GPIO_PIN_9;
    
//    for(int i=0;i<IIC_MAX;i++)
//    {
//        p=&iic_user[i];
//        IIC_Init();
//    }
    
}

//初始化IIC
void IIC_Init(void)
{	
    
    
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    HAL_GPIO_WritePin(p->iic_port.iic_scl_port, p->iic_port.iic_scl_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(p->iic_port.iic_sda_port, p->iic_port.iic_sda_pin, GPIO_PIN_SET);
    
    GPIO_InitStruct.Pin = p->iic_port.iic_scl_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(p->iic_port.iic_scl_port, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = p->iic_port.iic_sda_pin;
    HAL_GPIO_Init(p->iic_port.iic_sda_port, &GPIO_InitStruct);
	
}

void IIC_Select(iic_select_t x)
{
    p=&iic_user[x];
}

//产生IIC起始信号
void IIC_Start(void)
{
    _iic_sda_dir(IIC_SDA_OUT);
    _iic_set_sda(GPIO_PIN_SET);
    _iic_set_scl(GPIO_PIN_SET);
    delay_us(4);
    _iic_set_sda(GPIO_PIN_RESET);
    delay_us(4);
    _iic_set_scl(GPIO_PIN_RESET);
    
//	SDA_OUT();     //sda线输出
//	IIC_SDA=1;	  	  
//	IIC_SCL=1;
//	delay_us(4);
// 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
//	delay_us(4);
//	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
    _iic_sda_dir(IIC_SDA_OUT);
    _iic_set_scl(GPIO_PIN_RESET);
    _iic_set_sda(GPIO_PIN_RESET);
    delay_us(4);
    _iic_set_scl(GPIO_PIN_SET);
    _iic_set_sda(GPIO_PIN_SET);
    delay_us(4);
    
//	SDA_OUT();//sda线输出
//	IIC_SCL=0;
//	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
// 	delay_us(4);
//	IIC_SCL=1; 
//	IIC_SDA=1;//发送I2C总线结束信号
//	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
    
    _iic_sda_dir(IIC_SDA_IN);
    
    _iic_set_sda(GPIO_PIN_SET);delay_us(1);
    _iic_set_scl(GPIO_PIN_SET);delay_us(1);
    while(_iic_get_sda())
//	SDA_IN();      //SDA设置为输入  
//	IIC_SDA=1;delay_us(1);	   
//	IIC_SCL=1;delay_us(1);	 
//	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
    _iic_set_scl(GPIO_PIN_RESET);
	//IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
    _iic_set_scl(GPIO_PIN_RESET);
    _iic_sda_dir(IIC_SDA_OUT);
    _iic_set_sda(GPIO_PIN_RESET);
    delay_us(2);
    _iic_set_scl(GPIO_PIN_SET);
    delay_us(2);
    _iic_set_scl(GPIO_PIN_RESET);
    
//	IIC_SCL=0;
//	SDA_OUT();
//	IIC_SDA=0;
//	delay_us(2);
//	IIC_SCL=1;
//	delay_us(2);
//	IIC_SCL=0;
}
//不产生ACK应答		    
void IIC_NAck(void)
{
    _iic_set_scl(GPIO_PIN_RESET);
    _iic_sda_dir(IIC_SDA_OUT);
    _iic_set_sda(GPIO_PIN_SET);
    delay_us(2);
    _iic_set_scl(GPIO_PIN_SET);
    delay_us(2);
    _iic_set_scl(GPIO_PIN_RESET);
    
//	IIC_SCL=0;
//	SDA_OUT();
//	IIC_SDA=1;
//	delay_us(2);
//	IIC_SCL=1;
//	delay_us(2);
//	IIC_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
    _iic_sda_dir(IIC_SDA_OUT);
    _iic_set_scl(GPIO_PIN_RESET);
    
	//SDA_OUT(); 	    
    //IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			//IIC_SDA=1;
            _iic_set_sda(GPIO_PIN_SET);
		else
			//IIC_SDA=0;
            _iic_set_sda(GPIO_PIN_RESET);
		txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		//IIC_SCL=1;
        _iic_set_scl(GPIO_PIN_SET);
		delay_us(2); 
		//IIC_SCL=0;	
        _iic_set_scl(GPIO_PIN_RESET);
		delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
    _iic_sda_dir(IIC_SDA_IN);
	//SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        //IIC_SCL=0; 
        _iic_set_scl(GPIO_PIN_RESET);
        delay_us(2);
		//IIC_SCL=1;
        _iic_set_scl(GPIO_PIN_SET);
        receive<<=1;
        //if(READ_SDA)receive++;  
        if(_iic_get_sda()) receive++;          
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}



























