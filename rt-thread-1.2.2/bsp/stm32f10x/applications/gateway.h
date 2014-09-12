#ifndef __GATEWAY_H__
#define __GATEWAY_H__
#include <rtthread.h>

typedef struct user{
    rt_uint8_t  user[16];
    rt_uint8_t  psd[8];
}user;

typedef struct device{
    rt_uint8_t  mac[8];
    rt_uint8_t  stat;
}device;


#endif