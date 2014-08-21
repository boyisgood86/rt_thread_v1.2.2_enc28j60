/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 * 2013-07-12     aozima       update for auto initial.
 */

/**
 * @addtogroup STM32
 */
/*@{*/

#include <board.h>
#include <rtthread.h>

#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif  /* RT_USING_COMPONENTS_INIT */

#ifdef RT_USING_DFS
/* dfs filesystem:ELM filesystem init */
#include <dfs_elm.h>
/* dfs Filesystem APIs */
#include <dfs_fs.h>
#endif

#ifdef RT_USING_RTGUI
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_server.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/driver.h>
#include <rtgui/calibration.h>
#endif

#include "led.h"

ALIGN(RT_ALIGN_SIZE)
//static rt_uint8_t led_stack[ 512 ];
//static struct rt_thread led_thread;
//static void led_thread_entry(void* parameter)
//{
//    unsigned int count=0;
//
//    rt_hw_led_init();
//
//    while (1)
//    {
//        /* led1 on */
//      MY_DEBUG("led on....\n\r");
//      rt_thread_delay( RT_TICK_PER_SECOND/2 ); /* sleep 0.5 second and switch to other thread */
//      MY_DEBUG("led off...\n\r");
//      rt_thread_delay( RT_TICK_PER_SECOND/2 ); /* sleep 0.5 second and switch to other thread */
////#ifndef RT_USING_FINSH
////        rt_kprintf("led on, count : %d\r\n",count);
////#endif
////        count++;
////        rt_hw_led_on(0);
////        rt_thread_delay( RT_TICK_PER_SECOND/2 ); /* sleep 0.5 second and switch to other thread */
////
////        /* led1 off */
////#ifndef RT_USING_FINSH
////        rt_kprintf("led off\r\n");
////#endif
////        rt_hw_led_off(0);
////        rt_thread_delay( RT_TICK_PER_SECOND/2 );
//    }
//}

/*socket mutex*/
struct rt_mutex socket_mutex;
/*uart mutex*/
struct rt_mutex uart_mutex;

extern void tcp_client(void);
static void tcp_client_thread(void *arg)
{
  
  while(1) {
    rt_thread_delay(500);
    tcp_client();
    rt_thread_delay(30000);
  }
}

extern void uart_tcp(void);
static void uart_tcp_thread(void *arg)
{
  while(1) {
    rt_thread_delay(500);
    uart_tcp();
  }
}

extern void file_op(void);
static void file_test_thread(void *arg)
{
  while(1) {
    rt_thread_delay(3000);
    file_op();
  }
}

extern void udp_server(void);
static void udp_server_thread(void *arg)
{
  while(1) {
    rt_thread_delay(1000);
    udp_server();
  }
}

extern void tcp_server(void);
static void tcp_server_thread(void *arg)
{
  while(1) {
     rt_thread_delay(500);
     tcp_server();
  }
}

#ifdef RT_USING_RTGUI
rt_bool_t cali_setup(void)
{
    rt_kprintf("cali setup entered\n");
    return RT_FALSE;
}

void cali_store(struct calibration_data *data)
{
    rt_kprintf("cali finished (%d, %d), (%d, %d)\n",
               data->min_x,
               data->max_x,
               data->min_y,
               data->max_y);
}
#endif /* RT_USING_RTGUI */

extern void rt_hw_spi2_init(void);
void rt_init_thread_entry(void* parameter)
{
#ifdef RT_USING_COMPONENTS_INIT
    /* initialization RT-Thread Components */
    rt_components_init();
#endif

#ifdef  RT_USING_FINSH
    finsh_set_device(RT_CONSOLE_DEVICE_NAME);
#endif  /* RT_USING_FINSH */
       
      rt_hw_spi2_init();  /*Using SPI 2*/

    /* Filesystem Initialization */
#if defined(RT_USING_DFS) && defined(RT_USING_DFS_ELMFAT)
     
     dfs_init();
     elm_init();
     
     if(w25x80_init("flash0", "spi20") != RT_EOK) {
          MY_DEBUG("%s, %d: w25x80 init faild !\n\r",__func__,__LINE__);
      }     
    /* mount sd card fat partition 1 as root directory */
    if (dfs_mount("flash0", "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("File System initialized!\n");
    }
    else {
        rt_kprintf("File System initialzation failed!\n");
        mkfs("elm", "flash0");  /* 如果是spi flash 第一次使用，则打开这里，进行格式化 */
        MY_DEBUG("mkfs File System\n\r");
    }
#endif  /* RT_USING_DFS */

//#ifdef RT_USING_RTGUI
//    {
//        extern void rt_hw_lcd_init();
//        extern void rtgui_touch_hw_init(void);
//
//        rt_device_t lcd;
//
//        /* init lcd */
//        rt_hw_lcd_init();
//
//        /* init touch panel */
//        rtgui_touch_hw_init();
//
//        /* find lcd device */
//        lcd = rt_device_find("lcd");
//
//        /* set lcd device as rtgui graphic driver */
//        rtgui_graphic_set_device(lcd);
//
//#ifndef RT_USING_COMPONENTS_INIT
//        /* init rtgui system server */
//        rtgui_system_server_init();
//#endif
//
//        calibration_set_restore(cali_setup);
//        calibration_set_after(cali_store);
//        calibration_init();
//    }
//#endif /* #ifdef RT_USING_RTGUI */
    
    rt_mutex_init(&socket_mutex,"musb",RT_IPC_FLAG_FIFO); 
    rt_mutex_init(&uart_mutex,"uart",RT_IPC_FLAG_FIFO); //uart_mutex

#ifdef RT_USING_LWIP
	#ifdef ENC28J60
		rt_hw_enc28j60_init();
	#endif /*ENC28J60*/
        
    #ifdef  TCP_CLIENT
          sys_thread_new("tcp_client", tcp_client_thread, NULL, TCPIP_THREAD_STACKSIZE, TCPIP_THREAD_PRIO);
    #endif   /*TCP_CLIENT*/
          
    #ifdef UDP_SERVER
          sys_thread_new("udp_server", udp_server_thread, NULL, TCPIP_THREAD_STACKSIZE, TCPIP_THREAD_PRIO);    
    #endif /* UDP_SERVER */
          
#ifdef TCP_SERVER
          sys_thread_new("tcp_server", tcp_server_thread, NULL, TCPIP_THREAD_STACKSIZE, TCPIP_THREAD_PRIO);
#endif /* TCP_SERVER */
          
    #ifdef UART_TO_TCP
          sys_thread_new("tcp_uart", uart_tcp_thread, NULL, TCPIP_THREAD_STACKSIZE, TCPIP_THREAD_PRIO);    
    #endif /*UART_TO_TCP*/
          
    #ifdef FILE_TEST
          sys_thread_new("file_test", file_test_thread, NULL, TCPIP_THREAD_STACKSIZE, TCPIP_THREAD_PRIO);    
    #endif /* FILE_TEST */

#endif /*RT_USING_LWIP*/
}

int rt_application_init(void)
{
    rt_thread_t init_thread;

//    rt_err_t result;

//    /* init led thread */
//    result = rt_thread_init(&led_thread,
//                            "led",
//                            led_thread_entry,
//                            RT_NULL,
//                            (rt_uint8_t*)&led_stack[0],
//                            sizeof(led_stack),
//                            20,
//                            5);
//    if (result == RT_EOK)
//    {
//        rt_thread_startup(&led_thread);
//    }
    

#if (RT_THREAD_PRIORITY_MAX == 32)
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 8, 20);
#else
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 80, 20);
#endif

    if (init_thread != RT_NULL)
        rt_thread_startup(init_thread);

    return 0;
}

/*@}*/
