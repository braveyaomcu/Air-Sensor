#ifndef _BSP_SGP30_H
#define _BSP_SGP30_H

#include "myiic.h"

#define SGP30_ADDR  0x58<<1

#define SGP30_IAQ_INIT              0x2003
#define SGP30_IAQ_MEAS              0x2008
#define SGP30_IAQ_GET_BASELINE      0x2015
#define SGP30_IAQ_SET_BASELINE      0x201E
#define SGP30_SET_HUMIDITY          0x2061
#define SGP30_MEAS_TEST             0x2032
#define SGP30_GET_FEATURE_SET       0x202F
#define SGP30_MEAS_RAW              0x2050
#define SGP30_GET_TVOC_IN_BASELINE  0x20B3
#define SGP30_GET_TVOC_BASELINE     0x2077


#define SGP30_RESET 0x0006
#define SGP_GET_ID  0x3682

typedef enum {
    SGP30_STATUS_INIT,
    SGP30_STATUS_OK,
    SGP30_STATUS_ERR,
}sgp30_status_t;

typedef struct sgp30_data_st{
        
    uint16_t    co2;
    uint16_t    tvoc;
    
}sgp30_data_t;

typedef struct sgp30_obj_st{
    
    uint16_t    deviceID;
    uint16_t    featureSet;
    
    uint16_t    co2Baseline;
    uint16_t    tvocBaseline;
    
    sgp30_data_t    sgp30Data;
    sgp30_status_t  sgp30_status;
}sgp30_obj_t;


extern sgp30_obj_t sgp30;

void sgp30_init(void);
void sgp30_reset(void);


void sgp30_set_humidity(uint16_t humi);

void sgp30_get_meas(void);
void sgp30_get_feature(void);
void sgp30_get_id(void);

char sgp30_meas_test(void);
#endif
