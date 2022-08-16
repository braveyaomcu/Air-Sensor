#ifndef _BSP_BH1750_H
#define _BSP_BH1750_H

#include "sys.h"

#define BH1750_ADDR         0x46

#define BH1750_POWER_DOWN   0x00
#define BH1750_POWER_ON     0x01
#define BH1750_RESET        0x07

#define BH1750_CHRES_MODE1  0x10
#define BH1750_CHRES_MODE2  0x11
#define BH1750_CLRES_MODE   0x13

#define BH1750_PHRES_MODE1  0x20
#define BH1750_PHRES_MODE2  0x21
#define BH1750_PLRES_MODE   0x23

typedef enum{
    BRIGH_STATUS_INIT,
    BRIGH_STATUS_OK,
    BRIGH_STATUS_ERR,
}brigh_status_t;

typedef struct _brigh_st{
    uint16_t result;
    float   lux;
    brigh_status_t  brigh_status;
}brigh_t;

extern brigh_t brigh;

void bh1750_init(void);
void bh1750_start(void);
void bh1750_read(void);

#endif
