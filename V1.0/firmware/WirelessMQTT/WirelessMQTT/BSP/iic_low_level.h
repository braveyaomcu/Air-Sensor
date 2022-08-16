#ifndef _IIC_LOW_LEVEL_H
#define _IIC_LOW_LEVEL_H

#include "sys.h"

typedef enum{
    IIC_SDA_IN,
    IIC_SDA_OUT,
}iic_dir_t;

typedef struct  _iic_port_st{
    GPIO_TypeDef    *iic_scl_port;
    uint16_t        iic_scl_pin;
    
    GPIO_TypeDef    *iic_sda_port;
    uint16_t        iic_sda_pin;
}iic_port_t;

typedef struct _iic_app_st{
    
    iic_port_t  iic_port;
    
//    void (*iic_set_sda)(GPIO_PinState PinState);
//    void (*iic_set_scl)(GPIO_PinState PinState);
//    
//    GPIO_PinState (*iic_get_sda)(void);
//    
//    void (*iic_sda_dir)(iic_dir_t  iic_dir);
    
}iic_user_t;

#endif
