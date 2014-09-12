#ifndef __DEV_OPT_H__
#define __DEV_OPT_H__

#define DEV     "/dev"

#define MAC_LEN     8
#define SHORT_LEN   2
typedef struct dev_t
{
    rt_uint8_t  mac[MAC_LEN];
    rt_uint8_t  stat;
    rt_uint8_t  shortadd[SHORT_LEN];
}dev_t;

rt_uint8_t add_dev(dev_t *dev);     //添加设备
rt_uint8_t del_dev(dev_t *dev);     //删除设备
rt_uint8_t exist_dev(dev_t *dev);   //查找设备
rt_uint8_t change_dev(dev_t *dev);  //修改设备
rt_uint8_t dev_list(void **dev,rt_uint8_t *cnt);    //获取设备列表
rt_uint8_t change_shortadd_dev(dev_t *dev);         //修改设备短地址

#endif