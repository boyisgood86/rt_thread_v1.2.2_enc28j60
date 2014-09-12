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

rt_uint8_t add_dev(dev_t *dev);     //����豸
rt_uint8_t del_dev(dev_t *dev);     //ɾ���豸
rt_uint8_t exist_dev(dev_t *dev);   //�����豸
rt_uint8_t change_dev(dev_t *dev);  //�޸��豸
rt_uint8_t dev_list(void **dev,rt_uint8_t *cnt);    //��ȡ�豸�б�
rt_uint8_t change_shortadd_dev(dev_t *dev);         //�޸��豸�̵�ַ

#endif