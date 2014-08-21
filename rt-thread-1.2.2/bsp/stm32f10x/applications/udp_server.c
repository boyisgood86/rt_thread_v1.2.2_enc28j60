#include <rtthread.h>
#include <lwip/netdb.h> 
#include <lwip/sockets.h>
//#include "netif/etharp.h"
//#include "netif/ethernetif.h"


#define LEN     (256)

//#define TEST_IP     ("192.168.1.125")
#define UDP_PORT   (10010)



/** The list of network interfaces. */
extern struct netif *netif_list;
/*udp server*/
void udp_server(void)
{
  int err = -1, sock_reuse = 1;
  int udp_sock = -1;
  int recv_bytes = -1;
  rt_uint32_t addr_len;
  socklen_t addrlen;
  
  char udp_recv_buff[LEN];
  
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
//  struct netif * netif;
//  struct ifconf ifconf;
  
  MY_DEBUG("Now in  %s, %d\n\r",__func__,__LINE__);
  
  udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(udp_sock < 0) {
    MY_DEBUG("create udp socket faild !\n\r");
    goto OUT;
  }

//   /*get ip add */
//  addrlen = sizeof(server_addr.sin_zero);
//  if(getsockname(udp_sock, (struct sockaddr *)&server_addr,&addrlen) < 0){
//    MY_DEBUG("%s, %d: get ip add faild !\n\r",__func__,__LINE__);
//  }else {
//    MY_DEBUG("%s, %d: my host ip is : %s\n\r",__func__,__LINE__, inet_ntoa( server_addr.sin_addr.s_addr));
//    MY_DEBUG("%s, %d: my host port is : %d\n\r",__func__,__LINE__, (int) ntohs(server_addr.sin_port));
//  }
  
    list_if();
  /*setsockopt*/
  err = setsockopt(udp_sock, SOL_SOCKET, 
                   SO_REUSEADDR, 
                   (const char*)&sock_reuse, sizeof(sock_reuse));
  
  if(err < 0) {
    MY_DEBUG("setsockopt faild!\n\r");
//    lwip_close(client_sock);
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
  
  
  /*get ip add */
  addrlen = sizeof(server_addr);
  if(getsockname(udp_sock, (struct sockaddr *)&server_addr,&addrlen) < 0){
    MY_DEBUG("%s, %d: get ip add faild !\n\r",__func__,__LINE__);
  }else {
    MY_DEBUG("%s, %d: my host ip is : %s\n\r",__func__,__LINE__, inet_ntoa( server_addr.sin_addr));
    MY_DEBUG("%s, %d: my host port is : %d\n\r",__func__,__LINE__, (int) ntohs(server_addr.sin_port));
  }
  
  MY_DEBUG("Wate dta,udp:  %s, %d\n\r",__func__,__LINE__);
  /*recvfrom*/
  while(1) {
    rt_memset(udp_recv_buff, 0, LEN);
    rt_memset(&(client_addr.sin_zero), 0, sizeof(client_addr.sin_zero));
    recv_bytes = recvfrom(udp_sock, udp_recv_buff, LEN, 0, (struct sockaddr*)&client_addr, &addr_len);
    
    if(recv_bytes < 0) {
      MY_DEBUG("udp server recv error..\n\r");
      goto OUT;
    }
    MY_DEBUG("\n(%s , %d) said : ",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
    MY_DEBUG("%s\n\r", udp_recv_buff);

    recv_bytes =  sendto(udp_sock, "tcp server port is 10020 ", 32, 0,
              (struct sockaddr*)&client_addr, sizeof(struct sockaddr));    
   
    if(recv_bytes < 0) {
      MY_DEBUG("udp server send faild...\n\r");
      goto OUT;
    }
    rt_thread_delay(1000);
  }
  
OUT:
    lwip_close(udp_sock);
    udp_sock = -1;
    return ;
}

