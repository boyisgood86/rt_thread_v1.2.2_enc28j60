/*
*
* ֻ��һ��ʹ��uart1 �豸��demo �ļ�
*
*/

#include <board.h>
#include <rtthread.h>

#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif  /* RT_USING_COMPONENTS_INIT */

#ifdef RT_USING_LWIP
#include <lwip/sys.h>
#include <lwip/api.h>
#include <netif/ethernetif.h>
#include <lwip/sockets.h>
#include <lwip/dns.h>
#endif




#define     USART        ("uart1") 
#define     NAME_NULL    ("name_null")
#define     NO_UART      ("no_uart")
#define     UART_OK      ("uart_ok")

#define     LEN     (1024)




rt_device_t usart_device = RT_NULL;





unsigned char tx_buff[] = "Hello, I/m usart 1 ,just demo !\n\r";
char uart_buff[LEN];
//char rx_buff[LEN];

extern struct rt_mutex socket_mutex;
extern struct rt_mutex uart_mutex;
extern int client_sock;
//rt_mutex_take(&socket_mutex,RT_WAITING_FOREVER);
//rt_mutex_release(&socket_mutex);



char * find_uart(rt_device_t device, const char *name)
{
  if(!name) {
    MY_DEBUG("%s, %d: Name NULL..\n\r",__func__,__LINE__);
    return NAME_NULL;
  }
  /*find uart*/
  usart_device = rt_device_find(name);
  if(usart_device == RT_NULL) {
    MY_DEBUG("%s, %d: find %s faild..\n\r",__func__,__LINE__,name);
    return NO_UART;
  }
  MY_DEBUG("%s, %d: find %s success..\n\r",__func__,__LINE__,name);
  return UART_OK ;
}


/*uart to tcp */
void uart_tcp(void)
{
  struct rt_spi_device * rt_spi_device;
  int err  =-1;
  
  int read_size = -1;
  if (client_sock < 0) {
    return ;
  }
  
//  /*just test spi20*/
//  rt_spi_device = (struct rt_spi_device *)rt_device_find("spi21");
//  if(rt_spi_device != RT_NULL) {
//    MY_DEBUG("%s, %d: ------------> find spi20!\n\r",__func__,__LINE__);
//  }else MY_DEBUG("%s, %d:----------->can't find spi21 !\n\r",__func__,__LINE__);
//  
  //TODO..
  if(!usart_device) {
    if(find_uart(usart_device, USART) != UART_OK) {
        return ;
    }
  }
  
  rt_memset(uart_buff, 0, sizeof(uart_buff));
  
  rt_mutex_take(&uart_mutex,RT_WAITING_FOREVER);
  read_size = rt_device_read(usart_device, 0, uart_buff, LEN);
  rt_mutex_release(&uart_mutex);
  
  if(read_size > 0) {
       err = rt_mutex_take(&socket_mutex,1000);
      if(err < 0) {
        MY_DEBUG("%s, %d: No socket mutex..\n\r",__func__,__LINE__);
        return ;
      }
      err = send(client_sock, uart_buff, read_size, 0);
      rt_mutex_release(&socket_mutex);
      if(err < 0) {
        MY_DEBUG("%s, %d: send faild !\n\r",__func__,__LINE__);
        lwip_close(client_sock);
        client_sock = -1;
        return ;
      } 
  } 
  
//  err = rt_mutex_take(&socket_mutex,1000);
//  if(err < 0) {
//    MY_DEBUG("%s, %d: No socket mutex..\n\r",__func__,__LINE__);
//    return ;
//  }
//  err = send(client_sock, tx_buff, sizeof(tx_buff), 0);
//  rt_mutex_release(&socket_mutex);
//  if(err < 0) {
//    MY_DEBUG("%s, %d: send faild !\n\r",__func__,__LINE__);
//    lwip_close(client_sock);
//    client_sock = -1;
//    return ;
//  }
  //TODO..
}




//void usart1_tcpip(void)
//{
//   rt_device_t usart_device;
//   rt_err_t result = RT_EOK;  
//   int read_size = -1;
//
//#ifdef RT_USING_UART1   
//   /*find uart1*/
//   usart_device = rt_device_find(USART);
//   if (usart_device == RT_NULL) {
//      MY_DEBUG("Can't find device %s \n\r", USART);
//      goto OUT;
//   } 
////      rt_device_open(usart_device, RT_DEVICE_OFLAG_RDWR);  
//    while(1) {
//      rt_memset(rx_buff, 0, LEN);
//      read_size = rt_device_read(usart_device, 0, rx_buff, LEN);
//      if(read_size > 0) {
//        rt_device_write(usart_device, 0, rx_buff, read_size);
//        MY_DEBUG("Now tcp client fd is %d\n\r", client_sock);
//        if(client_sock >= 0) {
//          if(lwip_send(client_sock, rx_buff, read_size,0) < 0) {
//            MY_DEBUG("%s , %d : send faild!\n\r",__func__,__LINE__);
//          }
//          lwip_send(client_sock, "\n\r", 4, 0);
//        }
//        
//        MY_DEBUG("\n\r");
//      }else {
//        rt_device_write(usart_device, 0, tx_buff, sizeof(tx_buff));
//      }
//      rt_thread_delay(1000);
//    }
//#endif
//   return ;
//OUT:  
//  return ;
//}



//struct rx_msg
//{
//    rt_device_t dev;
//    rt_size_t   size;
//};
// 
//static struct rt_messagequeue  rx_mq;
//static char uart_rx_buffer[64];
//static char msg_pool[2048];
// 
//// ���������ص�����
//rt_err_t uart_input(rt_device_t dev, rt_size_t size)
//{
//    struct rx_msg msg;
//    msg.dev = dev;
//    msg.size = size;
//   
//        // ���������ݷ�����Ϣ����
//    rt_mq_send(&rx_mq, &msg, sizeof(struct rx_msg));
//   
//    return RT_EOK;
//}
// 
//// ������ں���
//void usr_echo_thread_entry(void* parameter)
//{
//    struct rx_msg msg;
//   
//    rt_device_t device;
//    rt_err_t result = RT_EOK;
//   
//    
//    // ��RTϵͳ�л�ȡ����1�豸
//    device = rt_device_find("uart1");
//    if (device != RT_NULL)
//    {
//                           // ָ�����մ������ݵĻص�����
//        rt_device_set_rx_indicate(device, uart_input);
//                           // �Զ�д��ʽ���豸
//        rt_device_open(device, RT_DEVICE_OFLAG_RDWR);
//    }
//    MY_DEBUG("Now : %s, %d \n\r",__func__,__LINE__);
//   
//    while(1)
//    {
//                           // ����Ϣ�����л�ȡ���ص�����������Ϣ�����е�����
//        result = rt_mq_recv(&rx_mq, &msg, sizeof(struct rx_msg), 50);
//        if (result == -RT_ETIMEOUT)
//        {
//            // timeout, do nothing
//        }
//       
//        if (result == RT_EOK)
//        {
//            rt_uint32_t rx_length;
//           
//            rx_length = (sizeof(uart_rx_buffer) - 1) > msg.size ?
//                msg.size : sizeof(uart_rx_buffer) - 1;
//           
//            rx_length = rt_device_read(msg.dev, 0, &uart_rx_buffer[0], rx_length);
//            uart_rx_buffer[rx_length] = '\0';
//            // ������д�ص�����1
//            rt_device_write(device, 0, &uart_rx_buffer[0], rx_length);
//        }
//    }
//}
//// �������̳�ʼ������
//void usr_echo_init(void)
//{
//    rt_thread_t thread ;
//   
//    rt_err_t result; 
//      // ������Ϣ���У�������д洢�ռ�
//    result = rt_mq_init(&rx_mq, "mqt", &msg_pool[0], 128 - sizeof(void*), sizeof(msg_pool), RT_IPC_FLAG_FIFO);
//   
//    if (result != RT_EOK) 
//    { 
//        rt_kprintf("init message queue failed.\n"); 
//        return; 
//    } 
//    // ���������߳�
//    thread = rt_thread_create("devt",
//        usr_echo_thread_entry, RT_NULL,
//        1024, 25, 7);
//    // ���������߳�
//    if (thread != RT_NULL) {
//        MY_DEBUG("Now start usart 1 thread!  :%s, %d\n",__func__,__LINE__);
//        rt_thread_startup(thread);
//    }
//}
 







