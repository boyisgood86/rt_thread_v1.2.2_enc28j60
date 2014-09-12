#include <rtthread.h>
#include <string.h>
#include "gateway.h"
#include "login.h"
#include "ym_config.h"
#include "white_list.h"

user admin={"admin","admin"};

extern rt_uint8_t   white_list_enable;

/*recover administrator to default account*/
void recover_admin(void)
{
    memset(admin.user,0,sizeof(admin.user));
    memset(admin.psd,0,sizeof(admin.psd));
    rt_strncpy(admin.user,ADMIN_NAME,strlen(ADMIN_NAME));
    rt_strncpy(admin.psd,ADMIN_PSD,strlen(ADMIN_PSD));
    return;
}

/*modify administrator account*/
rt_uint8_t change_admin(rt_uint8_t *name,rt_uint8_t *psd)
{
    rt_uint8_t   name_len,psd_len;
    
    if((RT_NULL==name)||(RT_NULL==psd))
    {
        return ERR;
    }
    name_len = rt_strlen(name);
    psd_len  = rt_strlen(psd);
    if(name_len>16||psd_len>8)
    {
        return ERR;
    }
    if(rt_strcmp(name, admin.user))
    {
        memset(admin.user,0,sizeof(admin.user));
        rt_strncpy(admin.user,name,name_len);
    }
    if(rt_strcmp(psd, admin.psd))
    {
        memset(admin.psd,0,sizeof(admin.psd));
        rt_strncpy(admin.psd,psd,psd_len);
    }
    return OK;
}

/*verify the user is or not administrator*/

rt_uint8_t login_authentication(const JSON *arg)
{
    if((RT_NULL==arg->name)||(RT_NULL==arg->passwd))
    {
        return LOGIN_ERR;
    }
    if(rt_strcmp(arg->name, admin.user)||rt_strcmp(arg->passwd, admin.psd))
    {
        if(white_list_enable && user_exist(arg))
        {
            return LOGIN_OK;
        }
        return LOGIN_ERR;
    }
    return LOGIN_OK|IS_ADMIN;
}

