#include "mqtt_app.h"
#include "malloc.h"

#include <stdlib.h>
#include <string.h>

uint8_t Debugbuffer[256];
        
static uint8_t jsonbuf[256];
static uint16_t jsonbuf_len;

//mqtt_app_t  mqttAppObj;

void mqtt_app_json(char *buf)
{
    
    cJSON* obj1 = cJSON_CreateObject();
    cJSON* obj2 = cJSON_CreateArray();
    
    cJSON* obj3 = cJSON_CreateObject();
    cJSON_AddNumberToObject(obj3, "sensorsId", 200601242);
    cJSON_AddStringToObject(obj3, "switcher", "%d");
    cJSON_AddItemToArray(obj2, obj3);

    cJSON* obj4 = cJSON_CreateObject();
    cJSON_AddNumberToObject(obj4, "sensorsId", 200601243);
    cJSON_AddStringToObject(obj4, "switcher", "%d");
    cJSON_AddItemToArray(obj2, obj4);

    cJSON* obj5 = cJSON_CreateObject();
    cJSON_AddNumberToObject(obj5, "sensorsId", 200601244);
    cJSON_AddStringToObject(obj5, "value", "%2.1f");
    cJSON_AddItemToArray(obj2, obj5);

    cJSON_AddItemToObject(obj1, "sensorDatas", obj2);
    
    char *data = cJSON_PrintUnformatted(obj1);
    printf("json format:%s",data);
    
    strcpy(buf,data);
    
    cJSON_Delete(obj1);
    cJSON_Delete(obj2);
    cJSON_Delete(obj3);
    cJSON_Delete(obj4);
    cJSON_Delete(obj5);
      
}

void mqtt_user_client_init(MQTTPacket_connectData *client)
{
    client->clientID.cstring=MQTT_CLIENT_ID;
    client->username.cstring=MQTT_CLIENT_USER_NAME;
    client->password.cstring=MQTT_CLIENT_USER_PWD;
    client->cleansession=1;
    client->keepAliveInterval=60;
    client->MQTTVersion=4;
    client->willFlag=0;
    
}

bool mqtt_client_connect(mqtt_app_t *obj,char *ip,uint16_t port)
{
    uint16_t len,packetid;
    uint8_t retry=0;
    uint8_t packettype,dup,sp,rc;
    
    uint32_t    time1,time2,tcnt=0;
    
    rc=obj->mqtt_trans.mqtt_trans_status();
    if(rc == 4)
    {
        obj->mqtt_trans.mqtt_trans_disconn();
        
    }
    
    if(!obj->mqtt_trans.mqtt_trans_conn(ip,port))
    {
        obj->MQStatus=MQTT_STATUS_NET_CONNECT_ERR;
        return false;
    }
    
    obj->MQStatus=MQTT_STATUS_NET_CONNECT_OK;
    
    len=MQTTSerialize_connect(Debugbuffer,256,obj->pClient);
    if(obj->mqtt_trans.mqtt_trans_send(Debugbuffer,len) !=1)
    {
        
        rc=obj->mqtt_trans.mqtt_trans_status();
        if(rc == 4)
        {
            obj->mqtt_trans.mqtt_trans_disconn();
            
        }
        if(!obj->mqtt_trans.mqtt_trans_conn(ip,port))
            return false;
    }
    
    time1=HAL_GetTick();
    while(retry<10)
    {
        
        len=obj->mqtt_trans.mqtt_trans_recv(Debugbuffer,256);
        if(len<=0)
        {
            time2=HAL_GetTick();
            if(time2>=time1)
                tcnt=time2-time1;
            else
                tcnt=UINT32_MAX-time1+time2;
            
            if((tcnt>0) && (tcnt/1000))
            {
                tcnt=0;
                time1=HAL_GetTick();
                retry++;
            }
            continue;
        }
        printf("rec cnt:%d ",len);
        printbuf(Debugbuffer,len);
        
        if(MQTTDeserialize_ack(&packettype,&dup,&packetid,Debugbuffer,len) &&
            (packettype == CONNACK))
        {
            MQTTDeserialize_connack(&sp,&rc,Debugbuffer,len);
            if(rc == 0)
            {
                printf("connect ok!");
                return true;
            }
        }
    }
    
    return false;
}

bool mqtt_app_init(mqtt_app_t *obj,eMQTrans_t trans)
{
    obj->MQStatus=MQTT_STATUS_INIT;
    obj->MQTrans=trans;
    
    if(trans == MQTT_TRANS_WIFI)
    {
        obj->mqtt_trans.mqtt_trans_init=esp8266_tcp_init;
        obj->mqtt_trans.mqtt_trans_conn=esp8266_tcp_connect;
        obj->mqtt_trans.mqtt_trans_send=esp8266_tcp_send;
        obj->mqtt_trans.mqtt_trans_recv=esp8266_tcp_recv;
        obj->mqtt_trans.mqtt_trans_disconn=esp8266_tcp_disconnect;
        obj->mqtt_trans.mqtt_trans_status=esp8266_tcp_status;
    }
    else if(trans == MQTT_TRANS_LTE)
    {
        obj->mqtt_trans.mqtt_trans_init=lte_tcp_init;
        obj->mqtt_trans.mqtt_trans_conn=lte_tcp_connect;
        obj->mqtt_trans.mqtt_trans_send=lte_tcp_send;
        obj->mqtt_trans.mqtt_trans_recv=lte_tcp_recv;
        obj->mqtt_trans.mqtt_trans_disconn=lte_tcp_disconnect;
        obj->mqtt_trans.mqtt_trans_status=at_lte_get_status;
    }
    else if(trans == MQTT_TRANS_LAN)
    {
        
    }
    else
    {
        
        obj->MQStatus=MQTT_STATUS_TRANS_TYPE_ERR;
    }
    
    //create json format
    obj->pJsonCreate=mqtt_app_json;
    obj->pJsonCreate(obj->pJsonFormat);
    
    //net init
    if(!obj->mqtt_trans.mqtt_trans_init())
    {
        obj->MQStatus=MQTT_STATUS_NET_INIT_ERR;
        return false;
    }
    else
        obj->MQStatus=MQTT_STATUS_NET_INIT_OK;
    
    obj->pClient=(MQTTPacket_connectData *)my_malloc(sizeof(MQTTPacket_connectData));
    mqtt_user_client_init(obj->pClient);
    
    if(!mqtt_client_connect(obj,MQTT_SERVER_ADDRESS,MQTT_SERVER_PORT))
    {
        obj->MQStatus=MQTT_STATUS_SERVER_CONNECT_ERR;
        return false;
    }
    else
        obj->MQStatus=MQTT_STATUS_SERVER_CONNECT_OK;
        
    //sub handler
    
    
    return true;
}



void mqtt_client_init(MQTTPacket_connectData *client)
{
    client->clientID.cstring=MQTT_CLIENT_ID;
    client->username.cstring=MQTT_CLIENT_USER_NAME;
    client->password.cstring=MQTT_CLIENT_USER_PWD;
    client->cleansession=1;
    client->keepAliveInterval=60;
    client->MQTTVersion=4;
    client->willFlag=0;
    
}


void mqtt_subscribe_create(void)
{
    uint16_t len;
    MQTTString topic[1];
    int requestedQoSs[1];
    topic[0].cstring=MQTT_CLIENT_USER_SUB1;
    topic[0].lenstring.len=0;
    requestedQoSs[0]=0;
    len=MQTTSerialize_subscribe(Debugbuffer,256,0,1,1,topic,requestedQoSs);
    esp8266_tcp_send(Debugbuffer,len);
}


void mqtt_app_publish(mqtt_app_t *obj,char * tpc)
{
    uint16_t len;
    MQTTString topic;
    
    uint8_t x,y;
    float z;
    
    topic.cstring=tpc;
    topic.lenstring.len=0;
    
    x=rand()%2;
    y=rand()%2;
    z=rand()%100;
    
    sprintf((char *)jsonbuf, obj->pJsonFormat, x,y,z);
    jsonbuf_len=strlen((char *)jsonbuf);
    printf("$$ %d: %d,%d,%2.1f\r\n",obj->transCnt++,x,y,z);
    
    len=MQTTSerialize_publish(Debugbuffer,256,0,0,0,1,topic,jsonbuf,jsonbuf_len);
    obj->mqtt_trans.mqtt_trans_send(Debugbuffer,len);
}



void mqtt_client_pool(mqtt_app_t *obj)
{
    int len,qos;
    unsigned short packetid;
    uint8_t rc,dup,retained,packettype;
    
    MQTTString topic;
    uint8_t *payload;
    int payloadlen;
    
    obj->tick++;
    if(obj->tick >= obj->pClient->keepAliveInterval*1000)
    {
        obj->tick=0;
        len=MQTTSerialize_pingreq(Debugbuffer,256);
        obj->mqtt_trans.mqtt_trans_send(Debugbuffer,len);
    }
    
    len=obj->mqtt_trans.mqtt_trans_recv(Debugbuffer,256);
    if(len<=0)
        return;
    printf("rec cnt:%d ",len);
    printbuf(Debugbuffer,len);
    
    MQTTDeserialize_ack(&packettype,&dup,&packetid,Debugbuffer,len);
            
    switch(packettype)
    {
        
        case PINGRESP:
            rc=Debugbuffer[1];
            if(rc == 0)
            {
                printf("ping ok!");
            }
            break;
        case PUBLISH:
            MQTTDeserialize_publish(&dup,&qos,&retained,&packetid,&topic,
                                    &payload,&payloadlen,Debugbuffer,len);
            
            //if(strcmp(topic.cstring,
            break; 
            
    }

}
