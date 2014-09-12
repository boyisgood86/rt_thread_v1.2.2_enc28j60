#ifndef __LOGIN_H__
#define __LOGIN_H__

#include "ym_config.h"

#define OK    1
#define ERR   0
#define LOGIN_ERR   0x0
#define LOGIN_OK    0x1
#define IS_ADMIN    0x2  


#define OK_MSG  "Login Success"
#define ERR_MSG "Login Failure"
#define ADMIN_NAME  "admin"
#define ADMIN_PSD   "admin"

rt_uint8_t login_authentication(const JSON *user);

rt_uint8_t change_admin(rt_uint8_t *name,rt_uint8_t *psd);

void recover_admin(void);

#endif