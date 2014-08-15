#include <rtthread.h>
#include <lwip/netdb.h> 
#include <lwip/sockets.h>


#define LEN     (256)

#define TEST_IP     ("192.168.1.123")
#define TEST_PORT   (10000)

#define CLIENT_PORT     (10030)

enum VAL {
 KEEP_ALIVE = 1,
 TRUE = 0,
 FALSE = -1,
 DATA_ERR = -100,
 FALSE_REUSEADDR = -101,
 FALSE_KEEPALIVE = -102,
};

unsigned char send_data[] = "Hello,I/m clinet, just test !";
char recv_buff[LEN];

int client_sock = -1;
int recv_bytes = -1;




/*deal with data which recvived from remote tcp server*/
int deal_data(const char *s, int bytes)
{
  char buff[LEN];
  const char *p = s;
  int i = 0;
  if(bytes <= 0 || !s){
    MY_DEBUG("%s, %d: can't deal with data !\n\r",__func__,__LINE__);
    return FALSE;
  }
  rt_memset(buff, 0, LEN);
  for(i = 0; i < bytes; i++) {
    buff[i] = *p++ ;
  }
  
  //check
  if(buff[0] != 0xfe || buff[15] != 0x23) {
    MY_DEBUG("%s, %d: data error..\n\r",__func__,__LINE__);
    return (int)DATA_ERR;
  }
  //if read
  if(buff[10] == 0xF1) {
    MY_DEBUG("%s, %d: Now read data from file.xml..\n\r",__func__,__LINE__);
    //read data from file.xml
    //TODO...
  }else if(buff[10] == 0xF0) {
    MY_DEBUG("%s, %d: Now call uart write function...\n\r",__func__,__LINE__);
    //write data to uart 1
    //TODO...
  }
  
  return (int)TRUE;
}


int set_socket(int sockfd, int flag)
{
  int sock_reuse = 1;
  int err = -1;
  MY_DEBUG("%s, %d\n\r",__func__,__LINE__);
  
  if(sockfd < 0) {
    MY_DEBUG("%s, %d: sockfd error!\n\r",__func__,__LINE__);
    return (int)DATA_ERR;
  }
  /*sock_reuse*/
   err = setsockopt(client_sock, SOL_SOCKET, 
                   SO_REUSEADDR, 
                   (const char*)&sock_reuse, sizeof(sock_reuse));
  if(err < 0) {
    MY_DEBUG("setsockopt faild!\n\r");
    return FALSE_REUSEADDR;    
  }
  /*keepalive*/
  if(flag == KEEP_ALIVE) {
      int keepalive = 1; // 开启keepalive属性
    //  int keepidle = 60; // 如该连接在60秒内没有任何数据往来,则进行探测
    //  int keepinterval = 5; // 探测时发包的时间间隔为5 秒
    //  int keepcount = 3; // 探测尝试的次数。如果第1次探测包就收到响应了,则后2次的不再发。
      err = setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive , sizeof(keepalive ));
      if(err < 0) {
        MY_DEBUG("%s, %d: Keep alive faild !\n\r",__func__,__LINE__);
        return FALSE_KEEPALIVE;
      }
  }

  return TRUE;
//  setsockopt(sockfd, SOL_TCP, TCP_KEEPIDLE, (void*)&keepidle , sizeof(keepidle ));
//  setsockopt(sockfd, SOL_TCP, TCP_KEEPINTVL, (void *)&keepinterval , sizeof(keepinterval ));
//  setsockopt(sockfd, SOL_TCP, TCP_KEEPCNT, (void *)&keepcount , sizeof(keepcount ));  
  
}

int init_socket(void)
{
  int err = -1;
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
  
  MY_DEBUG("%s, %d: init socket..\n\r",__func__,__LINE__);
  
  if(client_sock >= 0) {
//    lwip_close(client_sock);
    client_sock = -1;
  }
  /*socket*/
  client_sock = socket(AF_INET, SOCK_STREAM, 0);
  if(client_sock < 0) {
    MY_DEBUG("%s, %d: create socket faild !\n\r",__func__,__LINE__);
    return FALSE;
  }
  /*setsockopt*/
  err = set_socket(client_sock, KEEP_ALIVE);
  if(err < 0) {
    MY_DEBUG("setsockopt faild!\n\r");
    lwip_close(client_sock);
    return FALSE;
  }
  /*bind client*/
  client_addr.sin_family = AF_INET;
  client_addr.sin_port = htons(CLIENT_PORT);
  client_addr.sin_addr.s_addr = INADDR_ANY;
  rt_memset(&(client_addr.sin_zero), 0, sizeof(client_addr.sin_zero));  
  if(lwip_bind(client_sock, (struct sockaddr *)&client_addr, sizeof(struct sockaddr)) < 0) {
    MY_DEBUG("bind client faild\n\r");
    lwip_close(client_sock);
    return FALSE;
   }  
 /*connect server*/
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(TEST_PORT);
  server_addr.sin_addr.s_addr=inet_addr(TEST_IP);
  rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));
  
  if(connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0) {
      MY_DEBUG("can't connect..\n\r");
      lwip_close(client_sock);
      return FALSE;
  }  
  
  return TRUE;
}


/*Hearbet function*/
int client_hearbet(void)
{
  int retval = -1, maxfdp = -1, heart_count = 0;
  int recv_bytes = -1;
  char heart_buff[LEN];
  struct timeval timeout;
  fd_set readset;
  //TODO...
  if(client_sock == -1) {
//    if(init_socket() < 0)
    MY_DEBUG("%s, %d: Can't send heart beat data to romet server\n\r",__func__,__LINE__);
    return FALSE;
  }
  MY_DEBUG("%s, %d: Now send heartbeat data to tcp server...\n\r",__func__,__LINE__);
 /*send*/
  maxfdp = client_sock + 1;
  FD_ZERO(&readset);
  FD_SET(client_sock, &readset);
  while(1) {
      if( send(client_sock, send_data, sizeof(send_data), 0) < 0) {
          MY_DEBUG("send data to server faild !\n\r");
          lwip_close(client_sock);
          return FALSE;
      }
      timeout.tv_sec = 20;
      timeout.tv_usec = 0;
      maxfdp = client_sock + 1;
      FD_ZERO(&readset);
      FD_SET(client_sock, &readset);      
      retval = select(maxfdp, &readset, NULL, NULL, &timeout);
      if(retval < 0) {
        MY_DEBUG("%s, %d: select faild !\n\r",__func__,__LINE__);
        lwip_close(client_sock);
        return FALSE;
      }
      else if( !retval ) {
        MY_DEBUG("%s, %d: select timeout...\n\r",__func__,__LINE__);
        heart_count++;
        if(heart_count == 10) {
          lwip_close(client_sock);
          return FALSE;
        }
        FD_CLR(client_sock,&readset);
        continue ;
      }
      else {
        if(FD_ISSET(client_sock, &readset)) {
           /*recv*/ 
            rt_memset(heart_buff, 0, LEN);
            recv_bytes = recv(client_sock, heart_buff, LEN, 0);
            if(recv_bytes <= 0) {
              lwip_close(client_sock);
              client_sock = -1;
              return FALSE;              
            }else {
              if(strncmp("Hello client", heart_buff, 13) == 0) {
                MY_DEBUG("%s, %d: recv data from server : %s\n\r",__func__,__LINE__,heart_buff);
                FD_CLR(client_sock,&readset);
                return TRUE;
              }
            }
        }
        MY_DEBUG("%s, %d: select timeout...\n\r",__func__,__LINE__);
        FD_CLR(client_sock,&readset);
        heart_count++;
        if(heart_count == 20) {
          lwip_close(client_sock);
          return FALSE;
        }
      }
  }
}



void tcp_client(void)
{
  //TODO..
  int retval = -1, maxfdp = -1, heart_count = 0;
  int recv_bytes = -1;
  char client_buff[LEN];
  char err_back[] = "Hi, data string error!";
  struct timeval timeout;
  fd_set readset;
  
  /*init socket*/
  if(init_socket() == FALSE) {
    MY_DEBUG("%s, %d: init socket faild !\n\r",__func__,__LINE__);
    return ;
  }
  /*Hearbeat*/
    if(client_hearbet() == FALSE) {
      MY_DEBUG("%s, %d: heart beat data faild !\n\r",__func__,__LINE__);
      return ;
    }
  
  while(1) {
    timeout.tv_sec = 20;
    timeout.tv_usec = 0;
    maxfdp = client_sock + 1;
    FD_ZERO(&readset);
    FD_SET(client_sock, &readset);    
    /*select*/
    retval = select(maxfdp, &readset, NULL, NULL, &timeout);
    if(retval < 0) {
      MY_DEBUG("%s, %d : select error..\n\r",__func__,__LINE__);
      lwip_close(client_sock);
      client_sock = -1;
      return ;
    }
    else if(retval == 0) {
      heart_count++;
      if(heart_count == 100) {
        lwip_close(client_sock);
        client_sock = -1;
        return ;
      }
      MY_DEBUG("%s, %d: select timeout..\n\r",__func__, __LINE__);
      FD_CLR(client_sock,&readset);
      continue ;
    }
    else {
      if(FD_ISSET(client_sock, &readset)) {
        rt_memset(client_buff, 0, LEN);
        recv_bytes = recv(client_sock, client_buff, LEN, 0);
        if(recv_bytes <= 0) {
          MY_DEBUG("tcp client restart..\n\r");
          return ;
        }
        //do data
        MY_DEBUG("%s, %d: recv data from remote server : %s\n\r",__func__,__LINE__,client_buff);
        retval = deal_data(client_buff, recv_bytes);
        if(retval == TRUE) {
          MY_DEBUG("%s, %d: OK! \n\r",__func__,__LINE__);
          continue ;
        }else if(retval == DATA_ERR) {
          if(send(client_sock, err_back, sizeof(err_back), 0) < 0) {
            lwip_close(client_sock);
            client_sock = -1;
            return ;
          }
        }
      }
      //must ?
      heart_count++;
      FD_CLR(client_sock,&readset);
      if(heart_count == 1000) {
         lwip_close(client_sock);
         client_sock = -1;
         return ;
      }
    }
  }
}

///*tcp client demo*/
//void tcp_client(void)
//{
//  int err = -1, sock_reuse = 1;
//  struct sockaddr_in server_addr;
//  struct sockaddr_in client_addr;
//  
//  MY_DEBUG("Now in  %s, %d\n\r",__func__,__LINE__);
//  /*socket*/
//  client_sock = socket(AF_INET, SOCK_STREAM, 0);
//  if(client_sock < 0){
//    MY_DEBUG("create socket faild!\n\r");
//    goto OUT;
//  }
//  
//  /*setsockopt*/
//  err = setsockopt(client_sock, SOL_SOCKET, 
//                   SO_REUSEADDR, 
//                   (const char*)&sock_reuse, sizeof(sock_reuse));
//  
//  if(err < 0) {
//    MY_DEBUG("setsockopt faild!\n\r");
////    lwip_close(client_sock);
//    goto OUT;
//  }
//  
////  err = setsockopt(client_sock, SOL_SOCKET, 
////                   SO_REUSEPORT, 
////                   (const char*)&sock_reuse, sizeof(sock_reuse));
////  
////  if(err < 0) {
////    MY_DEBUG("setsockopt faild!\n\r");
//////    lwip_close(client_sock);
////    goto OUT;
////  }  
// 
//  /* bind client*/
//  client_addr.sin_family = AF_INET;
//  client_addr.sin_port = htons(CLIENT_PORT);
//  client_addr.sin_addr.s_addr = INADDR_ANY;
//  rt_memset(&(client_addr.sin_zero), 0, sizeof(client_addr.sin_zero));  
//  if(lwip_bind(client_sock, 
//              (struct sockaddr *)&client_addr, 
//               sizeof(struct sockaddr)) < 0) {
//                MY_DEBUG("bind client faild\n\r");
//                goto OUT;
//              }
//  
///*connect server*/
//  server_addr.sin_family = AF_INET;
//  server_addr.sin_port = htons(TEST_PORT);
//  server_addr.sin_addr.s_addr=inet_addr(TEST_IP);
//  rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));
//  
//  if(connect(client_sock, 
//            (struct sockaddr *)&server_addr, 
//             sizeof(struct sockaddr)) < 0) {
//                MY_DEBUG("can't connect..\n\r");
//                goto OUT;
//            }
//  MY_DEBUG("connect success: %s, %d\n", __func__,__LINE__);
// /*send*/ 
//  if( send(client_sock, send_data, sizeof(send_data), 0) < 0) {
//      MY_DEBUG("send data to server faild !\n\r");
//      goto OUT;
//  }
//  
// /*recv*/ 
//  rt_memset(recv_buff, 0, LEN);
//  recv_bytes = recv(client_sock, recv_buff, LEN, 0);
//  if(recv_bytes < 0) {
//    MY_DEBUG("recv error...\n\r");
//    goto OUT;
//  }else if(recv_bytes == 0) {
//    MY_DEBUG("server down...\n\r");
//  }else {
//    MY_DEBUG("recv data from server is %s\n",recv_buff );
//  }
//    
//    lwip_close(client_sock);
//    client_sock = -1;
//    return ;    
//  
//  
//OUT:
//    lwip_close(client_sock);
//    client_sock = -1;
//    return ;  
//}
//
