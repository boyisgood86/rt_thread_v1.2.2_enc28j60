#ifndef __WHITE_LIST_H__
#define __WHITE_LIST_H__

#include "ym_config.h"

#define DISABLE 0
#define ENABLE  1

#define FAIL    0
#define SUCCESS 1
#define EXIST   2

#define WHITE_LIST  "/wlist"
#define WHITE_LIST_ENABLE   "/wlenable"


typedef unsigned char uint8;

typedef struct white_list_t
{
    rt_uint8_t  name[16];
    rt_uint8_t  psd[8];
}white_list_t;


rt_int8_t is_enable_list(void);      //check white list is enabled or disabled 
rt_int8_t enable_white_list(void);   //enable white list 
rt_int8_t disable_white_list(void);  //disable white list

rt_uint8_t add_white_list(white_list_t *user);   //add user to white list
rt_uint8_t del_white_list(white_list_t *user);      //delete user from white list

rt_uint8_t user_exist(const JSON *user);            //check user exist
rt_uint8_t white_list(void **user,rt_uint8_t *cnt);      
#endif

