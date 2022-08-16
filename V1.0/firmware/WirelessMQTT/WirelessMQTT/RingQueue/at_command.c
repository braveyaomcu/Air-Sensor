#include "at_command.h"


#define AT_AGENT_BUFFER_SIZE    256
#define ESP8266_RX_BUFFER_SIZE  1024

uint8_t atAgentBuffer[AT_AGENT_BUFFER_SIZE];    //AT指令发送数据时的缓存
uint8_t Rxbuffer[ESP8266_RX_BUFFER_SIZE];       //AT指令接收数据时的缓存
chr_fifo_t wifiChrFifo;
esp8266_conf_t esp8266Conf;

bool at_agent_send_raw(uint8_t *data,size_t size)
{
    if(size <= AT_AGENT_BUFFER_SIZE)
    {
        memcpy(atAgentBuffer,data,size);
        if( HAL_UART_Transmit(&huart2,data,size,1000) == HAL_OK)
            return true;
        else
            return false;
    }
    else
        return false;
}

bool at_agent_send_string(char *str)
{
    return at_agent_send_raw((uint8_t *)str,strlen(str));
}

bool at_agent_send_string_and_wait(char *str,uint32_t ms)
{
    if( at_agent_send_string(str) == false)
        return false;
    
    delay_ms(ms);
    return true;
}

bool at_agent_wait_for_string(uint32_t ms,uint8_t *result,uint8_t numOfPara,...)
{
    if(!result)
        return false;
    if(numOfPara == 0)
        return false;
    
    *result=0;
    
    va_list va_ap;
    va_start(va_ap,numOfPara);
    char * arg[numOfPara];
    for(int i=0;i<numOfPara;i++)
        arg[i]=va_arg(va_ap,char *);
    
    va_end(va_ap);
    
    for(int i=0;i<=ms;i+=50)
    {
        delay_ms(50);
        for(int j=0;j<numOfPara;j++)
        {
            if(strstr((char *)(wifiChrFifo.pool),arg[j]) != NULL)
            {
                *result=j+1;
                return true;
            }
        }
    }
    
    return false;                       
}

bool at_agent_return_string(char *result,uint8_t select,char *splitter)
{
    
    if(!result)
        return false;
    if(select==0)
        return false;
    
    char *str=(char *)(wifiChrFifo.pool);
    
    str=strtok(str,splitter);
    if(str == NULL)
    {
        strcpy(result,"");
        return false;
    }
    
    while(str)
    {
        select--;
        if(select == 0)
        {
            strcpy(result,str);
            return true;
        }
        str=strtok(NULL,splitter);
        
    }
    
    strcpy(result,"");
    return false;
}

bool at_agent_return_integer(int *result,uint8_t select,char *splitter)
{
    
    if(!result)
        return false;
    if(select==0)
        return false;
    
    if(at_agent_return_string((char *)(wifiChrFifo.pool),select,splitter) == true)
    {
        *result=atoi((char *)(wifiChrFifo.pool));
        return true;
    }
    return false;
}

bool at_agent_reset(void)
{
    uint8_t res;
    
    sprintf((char *)atAgentBuffer,"AT+RST\r\n");
    chr_fifo_flush(&wifiChrFifo);
    memset(wifiChrFifo.pool,0,wifiChrFifo.itemCount);
    if( at_agent_send_string((char *)atAgentBuffer) == false)
        return false;
    else
    {
        if(at_agent_wait_for_string(1000,&res,2,"OK","ERROR") == false)
            return false;
        else if(res == 2)
            return false;
    }
    
    return true;
}

bool at_agent_restore(void)
{
    uint8_t res;
    
    sprintf((char *)atAgentBuffer,"AT+RESTORE\r\n");
    chr_fifo_flush(&wifiChrFifo);
    memset(wifiChrFifo.pool,0,wifiChrFifo.itemCount);
    if( at_agent_send_string((char *)atAgentBuffer) == false)
        return false;
    else
    {
        if(at_agent_wait_for_string(1000,&res,2,"OK","ERROR") == false)
            return false;
        else if(res == 2)
            return false;
    }
    
    return true;
}

bool at_agent_set_mode(wifi_mode_t mode)
{
    uint8_t res;
    
    sprintf((char *)atAgentBuffer,"AT+CWMODE=%d\r\n",mode);
    chr_fifo_flush(&wifiChrFifo);
    memset(wifiChrFifo.pool,0,wifiChrFifo.itemCount);
    if( at_agent_send_string((char *)atAgentBuffer) == false)
        return false;
    else
    {
        if(at_agent_wait_for_string(1000,&res,2,"OK","ERROR") == false)
            return false;
        else if(res == 2)
            return false;
        
        esp8266Conf.netMode=mode;
    }
    
    return true;
}

bool at_agent_get_mode(void)
{
    uint8_t res;
    sprintf((char *)atAgentBuffer,"AT+CWMODE?\r\n");
    chr_fifo_flush(&wifiChrFifo);
    memset(wifiChrFifo.pool,0,wifiChrFifo.itemCount);
    if( at_agent_send_string((char *)atAgentBuffer) == false)
        return false;
    else
    {
        if(at_agent_wait_for_string(1000,&res,2,"OK","ERROR") == false)
            return false;
        else if(res == 2)
            return false;
        
        if(at_agent_return_integer((int *)&res,2,":"))
            esp8266Conf.netMode=(wifi_mode_t)res;
        else
            esp8266Conf.netMode=WIFI_MODE_ERROR;
    }
    
    return true;
}

bool at_agent_get_ip(void)
{
    uint8_t res;
    char *p;
    
    sprintf((char *)atAgentBuffer,"AT+CIFSR\r\n");
    chr_fifo_flush(&wifiChrFifo);
    memset(wifiChrFifo.pool,0,wifiChrFifo.itemCount);
    if( at_agent_send_string((char *)atAgentBuffer) == false)
        return false;
    else
    {
        if(at_agent_wait_for_string(1000,&res,2,"OK","ERROR") == false)
            return false;
        else if(res == 2)
            return false;
        p=strstr((char *)wifiChrFifo.pool,"+CIFSR:APIP");
        sscanf(p,"+CIFSR:APIP,\"%[^\"]",esp8266Conf.apIP);
        p=strstr((char *)wifiChrFifo.pool,"+CIFSR:APMAC");
        sscanf(p,"+CIFSR:APMAC,\"%[^\"]",esp8266Conf.apMAC);
    }
    
    sprintf((char *)atAgentBuffer,"AT+CIPSTA?\r\n");
    chr_fifo_flush(&wifiChrFifo);
    if( at_agent_send_string((char *)atAgentBuffer) == false)
        return false;
    else
    {
        if(at_agent_wait_for_string(1000,&res,2,"OK","ERROR") == false)
            return false;
        else if(res == 2)
            return false;
        
        p=strstr((char *)wifiChrFifo.pool,"+CIPSTA:ip");
        sscanf(p,"+CIPSTA:ip:\"%[^\"]",esp8266Conf.stationIp);
        p=strstr((char *)wifiChrFifo.pool,"+CIPSTA:gateway");
        sscanf(p,"+CIPSTA:gateway:\"%[^\"]",esp8266Conf.stationGateway);
    }
    
    return true;
}

bool at_agent_joint_ap(char *ssid,char *pwd)
{
    uint8_t res;
    
    sprintf((char *)atAgentBuffer,"AT+CWJAP=\"%s\",\"%s\"\r\n",ssid,pwd);
    chr_fifo_flush(&wifiChrFifo);
    memset(wifiChrFifo.pool,0,wifiChrFifo.itemCount);
    if( at_agent_send_string((char *)atAgentBuffer) == false)
        return false;
    else
    {
        if(at_agent_wait_for_string(10000,&res,3,"OK","ERROR","FAIL") == false)
            return false;
        
        if(res == 1)
            return true;
        
    }    
    
    return false;
}

bool at_agent_quit_ap(void)
{
    uint8_t res;
    
    sprintf((char *)atAgentBuffer,"AT+CWQAP\r\n");
    chr_fifo_flush(&wifiChrFifo);
    memset(wifiChrFifo.pool,0,wifiChrFifo.itemCount);
    if( at_agent_send_string((char *)atAgentBuffer) == false)
        return false;
    else
    {
        if(at_agent_wait_for_string(1000,&res,2,"OK","ERROR") == false)
            return false;
        
        if(res == 2)
            return false;
        
    }    
    
    return true;
}

bool at_agent_tcp_set_multi(bool multi)
{
    uint8_t res;
    
    sprintf((char *)atAgentBuffer,"AT+CIPMUX=%d\r\n",multi);
    chr_fifo_flush(&wifiChrFifo);
    memset(wifiChrFifo.pool,0,wifiChrFifo.itemCount);
    if( at_agent_send_string((char *)atAgentBuffer) == false)
        return false;
    else
    {
        if(at_agent_wait_for_string(1000,&res,2,"OK","ERROR") == false)
            return false;
        else if(res == 2)
            return false;
        
        esp8266Conf.netConnect.multiMode=multi;
    }
    
    return true;
}

bool at_agent_tcp_connect(char *ip,uint16_t port)
{
    uint8_t res;
    
    sprintf((char *)atAgentBuffer,"AT+CIPSTART=\"TCP\",\"%s\",%d\r\n",ip,port);
    chr_fifo_flush(&wifiChrFifo);
    memset(wifiChrFifo.pool,0,wifiChrFifo.itemCount);
    if( at_agent_send_string((char *)atAgentBuffer) == false)
        return false;
    else
    {
        if(at_agent_wait_for_string(1000,&res,2,"OK","ERROR") == false)
            return false;
        else if(res == 2)
            return false;
        
        //esp8266Conf.netConnect.multiMode=multi;
    }
    
    return true;
}

bool at_agent_tcp_disconnect(void)
{
    uint8_t res;
    
    sprintf((char *)atAgentBuffer,"AT+CIPCLOSE\r\n");
    chr_fifo_flush(&wifiChrFifo);
    memset(wifiChrFifo.pool,0,wifiChrFifo.itemCount);
    if( at_agent_send_string((char *)atAgentBuffer) == false)
        return false;
    else
    {
        if(at_agent_wait_for_string(1000,&res,2,"OK","ERROR") == false)
            return false;
        else if(res == 2)
            return false;
        
        //esp8266Conf.netConnect.multiMode=multi;
    }
    
    return true;
}

bool at_agent_get_connect_status(void)
{
    uint8_t res,id,te;
    char *p;
    
    sprintf((char *)atAgentBuffer,"AT+CIPSTATUS\r\n");
    chr_fifo_flush(&wifiChrFifo);
    memset(wifiChrFifo.pool,0,wifiChrFifo.itemCount);
    if( at_agent_send_string((char *)atAgentBuffer) == false)
        return false;
    else
    {
        if(at_agent_wait_for_string(1000,&res,2,"OK","ERROR") == false)
            return false;
        
        if(res == 2)
            return false;
        
        
        p=strstr((char *)wifiChrFifo.pool,"STATUS:");
        sscanf(p,"STATUS:%1d",(int *)&res);
        esp8266Conf.netConnect.linkStatus=res;
        if(res == 3)
        {
            p=strstr(p,"+CIPSTATUS:");
            sscanf(p,"+CIPSTATUS:%1d%*[,\"]%[^\"]%*[,\"]%[^\"]%*2c%d,%d,%d",(int *)&id,esp8266Conf.netConnect.linkType,esp8266Conf.netConnect.remoteIp,(int *)&esp8266Conf.netConnect.remotePort,(int *)&esp8266Conf.netConnect.localPort,(int *)&te);
            esp8266Conf.netConnect.linkId=id;
            esp8266Conf.netConnect.teType=te;
        }
        
        
    }    
    
    return true;
}


/*************************************************************************************
**************************************************************************************
**************************************************************************************/
bool esp8266_check(void)
{
    uint8_t res;
    chr_fifo_create(&wifiChrFifo,Rxbuffer,ESP8266_RX_BUFFER_SIZE);
    sprintf((char *)atAgentBuffer,"AT\r\n");
    chr_fifo_flush(&wifiChrFifo);
    memset(wifiChrFifo.pool,0,wifiChrFifo.itemCount);
    if( at_agent_send_string((char *)atAgentBuffer) == false)
        return false;
    else
    {
        if(at_agent_wait_for_string(1000,&res,2,"OK","ERROR") == false)
            return false;       
        
    }
    
    return true;
}

bool esp8266_tcp_init(void)
{
    uint8_t retry=0,sta=0;
    MX_USART2_UART_Init();
    
    chr_fifo_create(&wifiChrFifo,Rxbuffer,ESP8266_RX_BUFFER_SIZE);
    
    do{
        switch(sta)
        {
            case 0:
                printf("try reset\r\n");
                if(at_agent_reset())
                {
                    sta++;
                    retry=0;
                    printf("reset ok!\r\n");
                }
                else
                    retry++;
                delay_ms(1000);
                break;
            case 1:
                printf("try restore\r\n");
                if(at_agent_restore())
                {
                    sta++;
                    retry=0;
                    printf("restore ok!\r\n");
                }
                else
                    retry++;
                break;
            case 2:
                printf("try mode set\r\n");
                if(at_agent_set_mode(WIFI_MODE_ALL))
                {
                    sta++;
                    retry=0;
                    printf("mode set ok!\r\n");
                }
                else
                    retry++;
                delay_ms(1000);
                break;
            case 3:
                printf("try connect wifi\r\n");
                if(at_agent_joint_ap("TP-LINK_1D47","yy215478"))
                {
                    sta=10;
                    retry=0;
                    printf("wifi connected!\r\n");
                }
                else
                    retry++;
                delay_ms(1000);
                break;
            case 10:
                printf("wifi connect ok\r\n");
                at_agent_get_ip();
                return true;
                
        }
        
    }while(retry<5);
    
    //if(retry >=5)
    {
        printf("wifi init error\r\n");
        //while(1);
        return false;
    }
        
}

bool esp8266_tcp_connect(char *ip,uint16_t port)
{
    uint8_t retry=0;
    printf("try connect server\r\n");
    
    do{
        if(at_agent_tcp_connect(ip,port))
        {
            
            printf("server connected!\r\n");
            break;
        }
        else
            retry++;
    }while(retry<5);
    
    if(retry>=5)
        return false;
    at_agent_get_connect_status();
    
    return true;
}

int esp8266_tcp_send(uint8_t *buf,size_t len)
{
    uint8_t res;
    
    sprintf((char *)atAgentBuffer,"AT+CIPSEND=%d\r\n",len);
    chr_fifo_flush(&wifiChrFifo);
    memset(wifiChrFifo.pool,0,wifiChrFifo.itemCount);
    if( at_agent_send_string((char *)atAgentBuffer) == false)
        return 0;
    else
    {
        if(at_agent_wait_for_string(50,&res,2,"OK","ERROR") == false)
            return 0;
        else if(res == 2)
            return 0;
        
        if(at_agent_send_raw(buf,len) == false)
            return 0;
        else
        {
            
        }
        
    }
    
    return 1;
}

static uint8_t m;
static uint8_t i=0;
static uint8_t count=0;
static char tmp[5];
int esp8266_tcp_recv(uint8_t *buf,size_t len)
{
    uint8_t res;

    //未建立连接
    if(esp8266Conf.netConnect.linkStatus != 3)
        return 0;

    //未接收数据
    if((chr_fifo_pop(&wifiChrFifo,&res) != ERR_RING_Q_NONE) )
        return 0;
    
    switch(m)
    {
        case 0:
            if(res == '+')
                m++;
            break;
        case 1:
            if(res == 'I')
                m++;
            else
                m=0;
            break;
        case 2:
            if(res == 'P')
                m++;
            else
                m=0;
            break;
        case 3:
            if(res == 'D')
                m++;
            else
                m=0;
            break;
        case 4:
            if(res == ',')
            {
                m++;
                memset(tmp,0,sizeof(tmp));
                i=0;
            }
            else
                m=0;
            break; 
        case 5:
            if(isdigit(res))
            {
                
                tmp[i]=res;
                i++;
            }
            else
            {
                count=atoi(tmp);    //接收数据个数
                count=count<len?count:len;
                i=0;
                m=10;
            }
            break; 
        case 6:
            if(res == ':')
            {
                m=10;

            }
            else
                m=0;
            break;
        case 10:
            if(i<count)
            {
                buf[i]=res;
                i++;
            }
            
            if(i == count)
            {
                m=0;
                return count;
            }
    }
    
    return 0;
}

bool esp8266_tcp_disconnect(void)
{
    uint8_t retry=0;
    printf("try disconnect server\r\n");
    
    do{
        if(at_agent_tcp_disconnect())
        {
            
            printf("server disconnected!\r\n");
            break;
        }
        else
            retry++;
    }while(retry<5);
    
    if(retry>=5)
        return false;
    
    return true;
}


int esp8266_tcp_status(void)
{
    uint16_t offset=0;
    if(wifiChrFifo.head <6)
        offset=6;
    else
        offset=wifiChrFifo.head-6;
    if(strstr((char *)wifiChrFifo.pool+offset,"CLOSED") == NULL)
        return 0;
    else
        return 1;
}

/************************************************************************
*************************************************************************
*************************************************************************/


bool at_lte_send_raw(uint8_t *data,size_t size)
{
    if(size <= AT_AGENT_BUFFER_SIZE)
    {
        memcpy(atAgentBuffer,data,size);
        if( HAL_UART_Transmit(&huart3,data,size,1000) == HAL_OK)
            return true;
        else
            return false;
    }
    else
        return false;
}

bool at_lte_send_string(char *str)
{
    return at_lte_send_raw((uint8_t *)str,strlen(str));
}




bool at_lte_cfg_scene(void)
{
    uint8_t res;
    sprintf((char *)atAgentBuffer,"AT+QICSGP=1,1,\"CMNET\",\"\",\"\",1\r\n");
    chr_fifo_flush(&wifiChrFifo);
    memset(wifiChrFifo.pool,0,wifiChrFifo.itemCount);
    if( at_lte_send_string((char *)atAgentBuffer) == false)
        return false;
    else
    {
        if(at_agent_wait_for_string(1000,&res,2,"OK","ERROR") == false)
            return false;
        else if(res == 2)
            return false;
        
        
    }
    
    return true;
}



bool at_lte_act_scene(void)
{
    uint8_t res;
    sprintf((char *)atAgentBuffer,"AT+QIACT=1\r\n");
    chr_fifo_flush(&wifiChrFifo);
    memset(wifiChrFifo.pool,0,wifiChrFifo.itemCount);
    if( at_lte_send_string((char *)atAgentBuffer) == false)
        return false;
    else
    {
        if(at_agent_wait_for_string(1000,&res,2,"OK","ERROR") == false)
            return false;
        else if(res == 2)
            return false;
        
        
    }
    
    return true;
}

bool at_lte_deact_scene(void)
{
    uint8_t res;
    sprintf((char *)atAgentBuffer,"AT+QIDEACT=1\r\n");
    chr_fifo_flush(&wifiChrFifo);
    memset(wifiChrFifo.pool,0,wifiChrFifo.itemCount);
    if( at_lte_send_string((char *)atAgentBuffer) == false)
        return false;
    else
    {
        if(at_agent_wait_for_string(1000,&res,2,"OK","ERROR") == false)
            return false;
        else if(res == 2)
            return false;
        
        
    }
    
    return true;
}

bool at_lte_get_csq(void)
{
    uint8_t res;
    sprintf((char *)atAgentBuffer,"AT+CSQ\r\n");
    chr_fifo_flush(&wifiChrFifo);
    memset(wifiChrFifo.pool,0,wifiChrFifo.itemCount);
    if( at_lte_send_string((char *)atAgentBuffer) == false)
        return false;
    else
    {
        if(at_agent_wait_for_string(1000,&res,2,"OK","ERROR") == false)
            return false;
        else if(res == 2)
            return false;
        
        
    }
    
    return true;
}

bool at_lte_reset(void)
{
    HAL_GPIO_WritePin(GPIOB, PWK_Pin, GPIO_PIN_RESET);
    delay_ms(5000);
    HAL_GPIO_WritePin(GPIOB, PWK_Pin, GPIO_PIN_SET);
    delay_ms(5000);
    
    
    
    return true;
}

/*
+QISTATE: 
0,"TCP","xxx.xxx.xxx.xxx",xxx
,0,2,1,0,1,"uart1"  
 
OK 
*/
int at_lte_get_status(void)
{
    uint8_t res,offset=0;
    int index=0;
    char *p,*str;
    
    
    sprintf((char *)atAgentBuffer,"AT+QISTATE?\r\n");
    chr_fifo_flush(&wifiChrFifo);
    memset(wifiChrFifo.pool,0,wifiChrFifo.itemCount);
    if( at_lte_send_string((char *)atAgentBuffer) == false)
        return 0;
    else
    {
        if(at_agent_wait_for_string(1000,&res,2,"OK","ERROR") == false)
            return 0;
        else if(res == 2)
            return 0;
        
        p=strstr((char *)wifiChrFifo.pool,"+QISTATE:");
        if(p!=NULL)
            offset=p-(char *)wifiChrFifo.pool;
        else
            offset=0;
        str=(char *)wifiChrFifo.pool+offset;
        
        p=strtok(str,",");
        while(p)
        {
            
            index++;
            if(index>=6)
                break;
            p=strtok(NULL,",");
        }
        res=atoi(p);
        //sscanf(p,"%*[^,]%*2c%[^\"]%*3c%[^\"]%*2c%4d,%d,%d",type,ip,(int *)&port,&res1,(int *)&res);
        return res;
    }
    
    
}

bool lte_check(void)
{
    uint8_t res;
    chr_fifo_create(&wifiChrFifo,Rxbuffer,ESP8266_RX_BUFFER_SIZE);
    sprintf((char *)atAgentBuffer,"AT\r\n");
    chr_fifo_flush(&wifiChrFifo);
    memset(wifiChrFifo.pool,0,wifiChrFifo.itemCount);
    if( at_lte_send_string((char *)atAgentBuffer) == false)
        return false;
    else
    {
        if(at_agent_wait_for_string(1000,&res,2,"OK","ERROR") == false)
            return false; 
        
        
    }
    
    return true;
}

bool lte_tcp_init(void)
{
    uint8_t retry=0;
    uint8_t status=0;
    chr_fifo_create(&wifiChrFifo,Rxbuffer,ESP8266_RX_BUFFER_SIZE);
    at_lte_reset();
    
    do
    {
        switch(status)
        {
            case 0:
                if(!at_lte_get_csq())
                {
                    retry++;
                    delay_ms(2000);
                    printf("get csq error\r\n");
                }
                else
                {
                    status++;
                    retry=0;
                    printf("get csq ok\r\n");
                }
                break;
            case 1:
                if(!at_lte_cfg_scene())
                {
                    retry++;
                    delay_ms(2000);
                    printf("cfg scene err\r\n");
                }
                else
                {
                    printf("cfg scene ok\r\n");
                    status++;
                    retry=0;
                }
                break;
            case 2:
                if(!at_lte_act_scene())
                {
                    retry++;
                    at_lte_deact_scene();
                    delay_ms(2000);
                     printf("act scene err\r\n");
                }
                else
                {
                    printf("act scene ok\r\n");
                    status++;
                    retry=0;
                }
                break;
            case 3:
                return true;
        }
                    
    }while(retry<5);
    
    return false;
    
}

/*
**
    AT+QIOPEN=1,0,"TCP","mq.tlink.io",1883,0,1

    OK

    +QIOPEN: 0,0
*/
bool lte_tcp_connect(char *ip,uint16_t port)
{
    uint8_t res;
    uint8_t idle=0;
    char *p;
    
    sprintf((char *)atAgentBuffer,"AT+QIOPEN=1,0,\"TCP\",\"%s\",%d,0,1\r\n",ip,port);
    chr_fifo_flush(&wifiChrFifo);
    memset(wifiChrFifo.pool,0,wifiChrFifo.itemCount);
    if( at_lte_send_string((char *)atAgentBuffer) == false)
        return false;
    else
    {
        if(at_agent_wait_for_string(1000,&res,2,"OK","ERROR") == false)
            return false;
        else if(res == 2)
            return false;
        
        delay_ms(100);
        while(idle<5)
        {
            p=strstr((char *)wifiChrFifo.pool,"+QIOPEN:");
            if(p==NULL)
            {
                idle++;
                delay_ms(100);
                continue;
            }
            else
            {
                sscanf(p,"%*[^,]%*c%d",(int *)&res);
                return res==0?true:false;
            }
        }
        //esp8266Conf.netConnect.multiMode=multi;
    }
    
    return false;;
}

bool lte_tcp_disconnect(void)
{
    uint8_t res;
    
    sprintf((char *)atAgentBuffer,"AT+QICLOSE=0\r\n");
    chr_fifo_flush(&wifiChrFifo);
    memset(wifiChrFifo.pool,0,wifiChrFifo.itemCount);
    if( at_lte_send_string((char *)atAgentBuffer) == false)
        return false;
    else
    {
        if(at_agent_wait_for_string(1000,&res,2,"OK","ERROR") == false)
            return false;
        else if(res == 2)
            return false;
        
        //esp8266Conf.netConnect.multiMode=multi;
    }
    
    return true;
}

int lte_tcp_send(uint8_t *buf,size_t len)
{
    uint8_t res;
    
    sprintf((char *)atAgentBuffer,"AT+QISEND=0,%d\r\n",len);
    chr_fifo_flush(&wifiChrFifo);
    memset(wifiChrFifo.pool,0,wifiChrFifo.itemCount);
    if( at_lte_send_string((char *)atAgentBuffer) == false)
        return 0;
    else
    {
        if(at_agent_wait_for_string(5000,&res,2,"> ","ERROR") == false)
            return 0;
        else if(res == 2)
            return 0;
        
        if(at_lte_send_raw(buf,len) == false)
            return 0;
        else
        {
            
        }
        
    }
    
    return 1;
}

/*
***     +QIURC: "recv",0,4
***     data
*/

static uint8_t  offset1;
static uint8_t  datalen;
static uint8_t  status=0;
int lte_tcp_recv(uint8_t *buf,size_t len)
{
    uint8_t id,offset=0;
    char *p;
    
    len=0;
    switch(status)
    {
        case 0:
            if(wifiChrFifo.tail>12)
                offset=wifiChrFifo.tail-12;
            p=strstr((char *)wifiChrFifo.pool+offset,"\"recv\",");
            if(p != NULL)
            {
                offset1=p-(char *)wifiChrFifo.pool;
                status++;
            }
            break;
        case 1:
            sscanf((char *)wifiChrFifo.pool+offset1, "%*[^,]%*c%d%*c%d",(int *) &id,(int *) &len);
            if(len>0)
            {
                status++;
                offset1=wifiChrFifo.tail;
                datalen=len;
            }
            break;
        case 2:
            if(wifiChrFifo.tail >= offset1+2+datalen)
            {
                
                memcpy(buf,wifiChrFifo.pool+offset1+2,datalen);
                status=0;
                return datalen;
            }
    }
    
    return 0;
}
