#ifndef __METHOD_H_
#define __METHOD_H_

int ym_user_login(int conn,char * const string);      //登陆
int ym_user_sign(int conn,char * const string);       //注册添加
int ym_user_list(int conn,char * const string);  //用户列表 白名单
int ym_new_passwd(int conn,char * const string);     // 修改密码
int ym_user_delet(int conn,char * const string);    //删除用户
int ym_device_query(int conn,char * const string);   //查询设备状态
int ym_device_add(int conn,char * const string); // 添加设备
int ym_device_delet(int conn,char * const string);  //删除设备
int ym_device_contr(int conn,char * const string); //控制设备 

#endif /* __METHOD_H_*/



