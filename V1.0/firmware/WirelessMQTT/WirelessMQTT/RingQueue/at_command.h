#ifndef _AT_COMMAND_H
#define _AT_COMMAND_H

#include "usart.h"
#include "bsp_dwt.h"
#include "chr_fifo.h"

#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>

typedef enum {
    WIFI_MODE_ERROR=0,
    WIFI_MODE_STA=1,
    WIFI_MODE_AP,
    WIFI_MODE_ALL,
}wifi_mode_t;

typedef struct net_connect_st{

    uint8_t     linkId:3;
    uint8_t     linkStatus:3;
    uint8_t     teType:1;
    uint8_t     multiMode:1;
    char        linkType[5];
    char        remoteIp[16];
    uint16_t    remotePort;
    uint16_t    localPort;
        
}net_connect_t;

typedef struct esp8288_conf_st{
    
    wifi_mode_t netMode;
    char        stationIp[16];
    char        stationGateway[16];
    
    char        apIP[16];
    char        apMAC[18];
    
    net_connect_t   netConnect;
    
}esp8266_conf_t;

bool at_agent_reset(void);
bool at_agent_restore(void);
bool at_agent_set_mode(wifi_mode_t x);
bool at_agent_joint_ap(char *ssid,char *pwd);
bool at_agent_get_ip(void);
bool at_agent_get_connect_status(void);
bool at_agent_tcp_connect(char *ip,uint16_t port);

bool esp8266_check(void);

bool esp8266_tcp_init(void);
bool esp8266_tcp_connect(char *ip,uint16_t port);
int esp8266_tcp_send(uint8_t *buf,size_t len);
int esp8266_tcp_recv(uint8_t *buf,size_t len);
int esp8266_tcp_status(void);
bool esp8266_tcp_disconnect(void);

bool lte_check(void);

bool lte_tcp_init(void);
bool lte_tcp_connect(char *ip,uint16_t port);
int lte_tcp_send(uint8_t *buf,size_t len);
int lte_tcp_recv(uint8_t *buf,size_t len);
int at_lte_get_status(void);
bool lte_tcp_disconnect(void);

#endif
