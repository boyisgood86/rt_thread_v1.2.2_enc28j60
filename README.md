rt_thread_v1.2.2_enc28j60
=========================
最新的稳定版本  rt_thread v1.2.2 <2014-08-01 14:16发布>


tcp/ip 协议栈： lwip - 1.4-1

硬件平台   stm32f103RDT6


MAC 芯片   spi-enc28j60

static IP  192.168.1.210   <在rtconfig.h里面进行修改>


DHCP: <在rtconfig.h里面进行修改>


2014-8-15修改如下：

1  在 opt.h 里面打开  以下这几个宏：

----------------

SO_REUSE   /*端口复用*/


LWIP_SO_RCVTIMEO  /*接收超时*/


LWIP_SO_SNDTIMEO  /*发送超时*/


LWIP_TCP_KEEPALIVE  /*保活*/


-----------------

2 在 tcp_impl.h 里面， 调整了以下三个宏的值：

A  	#define  TCP_KEEPIDLE_DEFAULT     3000UL  /*7200000 ms ---> 3000ms*/

B  	#define  TCP_KEEPINTVL_DEFAULT    1000UL	/*75000  ms ---> 1000ms*/

C   #define  TCP_KEEPCNT_DEFAULT      3U      /*9次 --> 3次*/


------------------------

2014-8-21 增加内容如下：

1  SPI 2 的驱动注册进入了rtt驱动框架 rt_hw_SPI2_init()

2  添加 W25X80 spi flash 驱动 <来自于 https://github.com/boyisgood86/realtouch-stm32f4 中的w25qxx 驱动，仅仅是修改了一下扇区、页、容量>

3  挂载文件系统 ELMFAT 成功 dfs_init() elm_init() mount...

4  文件读写测试通过   在rtconfig.h 文件中打开 FILE_TEST 宏即可测试。在 init 主线程中测试。