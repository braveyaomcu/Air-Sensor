#ifndef _MQTT_APP_H
#define _MQTT_APP_H

#include "at_command.h"
#include "debug.h"

#include "MQTTConnect.h"
#include "MQTTPacket.h"
#include "MQTTFormat.h"
#include "cjson.h"


#define MQTT_SERVER_ADDRESS  "mq.tlink.io"
#define MQTT_SERVER_PORT     1883

#define MQTT_CLIENT_ID          "ZE063M9HWQU7C8E9"
#define MQTT_CLIENT_USER_NAME   "18807488110"
#define MQTT_CLIENT_USER_PWD    "abc123456"

#define MQTT_CLIENT_USER_SUB1   "ZE063M9HWQU7C8E9/+"
#define MQTT_CLIENT_USER_PUB1   "ZE063M9HWQU7C8E9"

typedef enum{
    MQTT_TRANS_NONE,
    MQTT_TRANS_LTE,
    MQTT_TRANS_LAN,
    MQTT_TRANS_WIFI,
}eMQTrans_t;

typedef enum{
    
    MQTT_STATUS_INIT,
    
    MQTT_STATUS_NET_INIT_OK,
    MQTT_STATUS_NET_INIT_ERR,
    
    MQTT_STATUS_NET_CONNECT_OK,
    MQTT_STATUS_NET_CONNECT_ERR,
    
    MQTT_STATUS_SERVER_CONNECT_OK,
    MQTT_STATUS_SERVER_CONNECT_ERR,
    
    
    MQTT_STATUS_TRANS_TYPE_ERR,
}eMQStatus_t;

typedef struct _mqtt_trans_st{
    
    bool    (*mqtt_trans_init)(void);
    bool    (*mqtt_trans_conn)(char *ip,uint16_t port);
    int     (*mqtt_trans_send)(uint8_t *buf,size_t len);
    int     (*mqtt_trans_recv)(uint8_t *buf,size_t len);
    int     (*mqtt_trans_status)(void);
    bool    (*mqtt_trans_disconn)(void);
    
}mqtt_trans_t;

typedef struct _mqtt_sub_handler_st{
    char *  *topics;
    void (*pfunc)(void);
}mqtt_sub_handle_t;

typedef struct _mqtt_app_st{
    
    //hardware interface
    eMQTrans_t      MQTrans;
    mqtt_trans_t    mqtt_trans;
    
    eMQStatus_t     MQStatus;
    //basic sets
    
    char            pJsonFormat[256];
    void            (*pJsonCreate)(char *buf);
    MQTTPacket_connectData  *pClient;
    
    uint32_t        tick;
    // asist
    uint32_t        transCnt;
    
    
}mqtt_app_t;

void mqtt_client_init(MQTTPacket_connectData *x);
void mqtt_client_pool(mqtt_app_t *obj);
bool mqtt_app_init(mqtt_app_t *obj,eMQTrans_t trans);
void mqtt_app_publish(mqtt_app_t *obj,char * tpc);

#endif
