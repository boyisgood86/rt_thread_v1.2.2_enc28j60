#include <rtthread.h>
#include <lwip/netdb.h> 
#include <lwip/sockets.h>
#include "netif/etharp.h"
#include "netif/ethernetif.h"
#include "cJSON.h"
#include "ym_config.h"


#define LEN     (256)

#define ASK_UDP     (10001)

typedef struct ip_mac {
    char mac[20];
    char ip[20];
}IP_MAC;



enum {
    port = 4977,
    back_data_size = 512,
};

/** The list of network interfaces. */
//extern struct netif *netif_list;
/*udp server*/


extern ip_addr_t dns_getserver(u8_t numdns);


/* find tcp server ip \ mac \ port */
IP_MAC* find_ip(void)
{
    rt_ubase_t index;
    IP_MAC *mac_ip;
    
    char buff[4];
 //   char mac_temp[20];
    char i = 0;
    struct netif * netif;

    mac_ip = (IP_MAC*)rt_malloc(sizeof(IP_MAC));
    if(!mac_ip) { MY_DEBUG("%s,%d: malloc mac ip faild !\n\r",__func__,__LINE__); return 0;}
    rt_memset(mac_ip, 0, sizeof(IP_MAC));    
    rt_enter_critical();

    netif = netif_list;
    
    
    while( netif != RT_NULL )
    {
        /*MAC address*/
//        rt_memset(mac_buff, 0, sizeof(mac_buff));
        for (index = 0; index < netif->hwaddr_len; index ++) {
//          MY_DEBUG("%s,%d: mac addrsss is :%02X\n\r",__func__,__LINE__,netif->hwaddr[index]);
          rt_memset(buff, 0, sizeof(buff));
          sprintf(buff, "%02X", netif->hwaddr[index]);
           strcat(mac_ip->mac, buff);
           strcat(mac_ip->mac, ":");
        }
        mac_ip->mac[17] = 0;;
        sprintf(mac_ip->ip, "%s", ipaddr_ntoa(&(netif->ip_addr)));
        /* ip add*/
       rt_kprintf("ip address: %s\n\r", ipaddr_ntoa(&(netif->ip_addr)));
       MY_DEBUG("%s,%d: mac add is %s\n\r",__func__,__LINE__,mac_ip->mac);

        netif = netif->next;
    }

#if LWIP_DNS
    {
        struct ip_addr ip_addr;

        for(index=0; index<DNS_MAX_SERVERS; index++)
        {
            ip_addr = dns_getserver(index);
//            rt_kprintf("dns server #%d: %s\n", index, ipaddr_ntoa(&(ip_addr)));
        }
    }
#endif /**< #if LWIP_DNS */

    rt_exit_critical();
    
    return mac_ip;
}

static int creat_js(const char *ip,const char *mac,int port,char* const back_data)
{
     cJSON *root,*js_body,*js_other,*js_head;
     
     const char *const body = "body";
     const char *const other = "other";
     const char *const head = "head";
     /* create root */
      root = cJSON_CreateObject();
      if(!root) { MY_DEBUG("%s,%d: get root faild !\n\r",__func__,__LINE__); goto EXIT; }
      /* body */
      cJSON_AddItemToObject(root,body,js_body=cJSON_CreateObject());
      cJSON_AddItemToObject(js_body,other,js_other=cJSON_CreateObject());
      cJSON_AddStringToObject(js_other, "gatewayId", mac);
      cJSON_AddStringToObject(js_other, "gatewayIp", ip);
      cJSON_AddStringToObject(js_other, "gatewayName", "家庭网关");
      cJSON_AddNumberToObject(js_other,"gatewayPort",port);
      /* head */
      cJSON_AddItemToObject(root,head,js_head = cJSON_CreateObject());
      cJSON_AddStringToObject(js_head, "message", "通信成功");
      cJSON_AddNumberToObject(js_head,"method",10001);
      cJSON_AddStringToObject(js_head, "mobileid", "353714986786243");
      cJSON_AddNumberToObject(js_head,"status",0);
      cJSON_AddStringToObject(js_head, "boxid", "827312897");
      
      /* js to char* */
      {
        char *s = cJSON_PrintUnformatted(root);
        if(s) {
            strcpy(back_data, s);
            rt_free(s);            
        }
        cJSON_Delete(root);
      }
  return 0;
EXIT:
  return -1;
}


static int parse_recv( const char * udp_recv_buff)
{
    cJSON *root, *dir1;
    cJSON *json, *json_value, *json_timestamp, *json_child;
    struct cJSON *child;
    char *string = NULL;
    const char *ro = "head";
    int number = 0,i = 0, size ;

    // parsel
    MY_DEBUG("%s: %s\n\r",__func__, udp_recv_buff);
    json = cJSON_Parse(udp_recv_buff);
    if(!json) {
      MY_DEBUG("%s,%d: get jason faild !\n\r",__func__,__LINE__);
//        cJSON_GetErrorPtr();
        goto EXIT;
    }
    
    size = cJSON_GetArraySize(json);
    MY_DEBUG("%s,%d: size is %d\n\r",__func__,__LINE__,size);
    
    for(i = 0; i < size; i++) {
        json_child = cJSON_GetArrayItem(json,i);
        if(json_child) {
            string = cJSON_Print(json_child);
            MY_DEBUG("%s,%d: string is %s\n\r",__func__,__LINE__,string);
            json_child = cJSON_Parse(string);
            if(json_child) {
                json_value = cJSON_GetObjectItem(json_child, "method");
                MY_DEBUG("%s: %d: json value type is %d\n\r",__func__,__LINE__,json_value->type);
                if(json_value->type == cJSON_Number) {
                    number = json_value->valueint;
                    break;
                }
            }
        }
    }
    
    cJSON_Delete(json);
    cJSON_Delete(json_child);
    if(ASK_UDP != number){
      MY_DEBUG("%s: ask port is error : %d\n\r",__func__,number);
      goto EXIT;
    } 
    
    return number;
EXIT:
  if(json)  cJSON_Delete(json);
//  if(mac_temp)  rt_free(mac_temp);
    return -1;
  
}

void udp_server(void)
{
  int err = -1, sock_reuse = 1;
  int udp_sock = -1;
  int recv_bytes = -1;
  int number = -1;
  IP_MAC * gateway;
  
  rt_uint32_t addr_len;
  socklen_t addrlen;
  
  
  
  char udp_recv_buff[LEN];
  char *ip = NULL;
  char *mac = NULL;
  char *mac_temp = NULL;
  char *back_data = NULL;
  
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
  
  udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(udp_sock < 0) {
    MY_DEBUG("create udp socket faild !\n\r");
    goto OUT;
  }


//    list_if();
  /*setsockopt*/
  err = setsockopt(udp_sock, SOL_SOCKET, 
                   SO_REUSEADDR, 
                   (const char*)&sock_reuse, sizeof(sock_reuse));
  
  if(err < 0) {
    MY_DEBUG("setsockopt faild!\n\r");
    goto OUT;
  }  
  
  /*bind server*/
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(UDP_PORT);
  server_addr.sin_addr.s_addr= INADDR_ANY;
  rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));
  
  if(lwip_bind(udp_sock, 
              (struct sockaddr *)&server_addr, 
               sizeof(struct sockaddr)) < 0) {
                MY_DEBUG("bind client faild\n\r");
                goto OUT;
              }
 
  gateway = find_ip();
  if(!gateway) { MY_DEBUG("%s,%d: find ip faild !\n\r",__func__,__LINE__); }
  
  /* do mac :: */
  
  MY_DEBUG("%s, ip is %s\n\r",__func__, gateway->ip);
  MY_DEBUG("%s, mac is %s\n\r",__func__,gateway->mac);
  MY_DEBUG("%s: Wate data...\n\r",__func__);
    back_data = (char*)malloc(sizeof(char)*back_data_size);
    if(!back_data) goto OUT;
//    rt_free(gateway);
  /*recvfrom*/
  while(1) {
     number = -1;
    rt_memset(udp_recv_buff, 0, LEN);
    rt_memset(&(client_addr.sin_zero), 0, sizeof(client_addr.sin_zero));
    recv_bytes = recvfrom(udp_sock, udp_recv_buff, LEN, 0, (struct sockaddr*)&client_addr, &addr_len);
    
    if(recv_bytes < 0) {
      MY_DEBUG("udp server recv error..\n\r");
      goto OUT;
    }
    MY_DEBUG("\n(%s , %d) said : ",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
    MY_DEBUG("%s\n\r", udp_recv_buff);
    
    /* do data */
    rt_memset(back_data, 0, back_data_size);
    if(parse_recv(udp_recv_buff) != ASK_UDP) {
      MY_DEBUG("%s: do data faild !\n\r",__func__);
      continue ;
    }
      
    /* back to client */
    if( creat_js(gateway->ip,gateway->mac, port,back_data) != 0) {
      MY_DEBUG("%s,%d: create js faild !\n\r",__func__,__LINE__); continue ;
    }
    MY_DEBUG("%s,%d: back_data string is :\n%s\n\r",__func__,__LINE__,back_data);
    
    recv_bytes =  sendto(udp_sock, back_data, back_data_size, 0,
              (struct sockaddr*)&client_addr, sizeof(struct sockaddr));    
   
    if(recv_bytes < 0) {
      MY_DEBUG("udp server send faild...\n\r");
      goto OUT;
    }
    rt_thread_delay(1000);
  }
  
OUT:
    if(gateway) rt_free(gateway);
    if(back_data) rt_free(back_data);
    if(udp_sock >= 0) lwip_close(udp_sock);
    udp_sock = -1;
    return ;
}

