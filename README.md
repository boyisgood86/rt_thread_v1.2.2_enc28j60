rt_thread_v1.2.2_enc28j60

@author： jason.shi
@Emali: newleaves@126.com

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


2014-08-25 增加内容：

将sqlite-3.8加入了编译，并准备测试。
测试结果： 能创建数据库，但是无法操作sql语句，因为操作sql语句的时候，需要较大ram空间。而我板子上的内存空间仅仅64KB，不够用，于是无法测试。
听rtt groups.google.com 上的朋友建议，如果要使用sqlite，ram空间的硬性指标应该在 100K+，而按照测试来看，不跑数据库，在使用lwip +ELMFAT的时候，内存消耗应该在30-40KB之间
这个数值可能因每个人的程序不同而不同，具体的，可以在程序中调用  list_mem() API  ，然后通过串口打印观察。


2014-08-27 增加内容如下：

将 网络开源的cjson库移植到rtt上，并使用如下程序进行测试：

http://sourceforge.net/projects/cjson/ 
------------------
    char *out;
    char text[] = "{\"timestamp\":\"2013-11-19T08:50:11\",\"value\":1}";
    cJSON *json;
    
    MY_DEBUG("%s:%d: <------------>\n\r",__func__,__LINE__);
    list_mem();
    json=cJSON_Parse(text);
    if(!json) {
      MY_DEBUG("%s:%d:  Error before: [%s]\n\r",__func__,__LINE__,cJSON_GetErrorPtr());
    }else {
        out=cJSON_Print(json);
        list_mem();
        cJSON_Delete(json);
        MY_DEBUG("%s\n",out);
        free(out);
    }
    MY_DEBUG("%s:%d <<----------------->>\n\r",__func__,__LINE__);
	
-------------------

在cjson.c中修改的地方为： 将malloc 替换成 rt_malloc   将free 替换成 rt_free.  将 cJSON.c 文件中的 size_t 修改为 rt_size_t，并相应的调整了 cJSON.h文件中相对应的地方
bug:  尚未严格测试，尚不明白。
具体使用方法，可以参考：
http://blog.csdn.net/xukai871105/article/details/17094113
-------------------

