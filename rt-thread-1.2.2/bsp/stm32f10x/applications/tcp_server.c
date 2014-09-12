#include <rtthread.h>
#include <lwip/netdb.h> 
#include <lwip/sockets.h>
#include "cJSON.h"
#include "ym_config.h"
#include "login.h"
#include "white_list.h"
#include "dev_opt.h"


#define LEN     (256)
#define BACKLOG     (10)

enum {
  disable = 0,
  enable = 1,  
  buff_size = 1024,
};

extern rt_uint8_t white_list_enable;

/* do mac */
static unsigned char
a2x(const char c)
{
  if(c >= '0' && c <= '9')
    return (unsigned char)atoi(&c);
  else if(c >= 'a' && c <= 'f')
    return 0xa + (c-'a');
  else if(c >= 'A' && c <= 'F')
    return 0xa + (c-'A');
  else
    return 0;
//    switch(c) {
//    case '0'...'9':
//
//        return (unsigned char)atoi(&c);
//    case 'a'...'f':
//        return 0xa + (c-'a');
//    case 'A'...'F':  
//        return 0xa + (c-'A');
//    default:  
//        goto error;  
//    }   
//error:  
//    return 0;  
} 

/*
** Mac Format Like BC:52:C6:18:ED:FD:12:23
*/
#define MAC_LEN_IN_BYTE 8   

void copy_str2mac(char *mac_buff, char* str)
{
        for(int i = 0 ; i < MAC_LEN_IN_BYTE; i++){
            mac_buff[i] = (a2x(str[i*3]) << 4) + a2x(str[i*3 + 1]);
        }
}


/* on or off whitelist */
static int do_whitelist_msg(char *const out, WhiteList *wl, char value)
{
/* enable or disable whitelist */
  cJSON *root, *js_body, *js_list,*js_other, *js_user,*js_head;
  white_list_t * whitelist= RT_NULL;
  char count = 0;
  char i = 0;
  
  /**/
  const char * const head = "head";
  const char * const body = "body";
  const char * const list = "list";
  const char * const other = "other";
  const char * const user = "user";   
  
  /*check out*/
  if(!out) { MY_DEBUG("%s,%d: out is NULL..\n\r",__func__,__LINE__); goto EXIT; }
  /* get whitelist */
  is_enable_list();
  if(white_list_enable == ENABLE) 
    white_list((void*)&whitelist, &count); 
  
  /*root*/
   root = cJSON_CreateObject();
   if(!root) { MY_DEBUG("%s,%d: get root faild !\n\r",__func__,__LINE__); goto EXIT; }
   /* body */
   cJSON_AddItemToObject(root, body, js_body = cJSON_CreateObject()); 
    /*list*/
   if(whitelist != RT_NULL) {
      cJSON_AddItemToObject(js_body, list, js_list = cJSON_CreateArray());
      for(i = 0; i < count; i++) cJSON_AddItemToArray(js_list, cJSON_CreateString( (whitelist+i)->name) );
   }
    /* other */
    cJSON_AddItemToObject(js_body, other, js_other = cJSON_CreateObject());
    cJSON_AddNumberToObject(js_other,"isAuthority",white_list_enable);   
       /* user */
    cJSON_AddItemToObject(js_body, user, js_user = cJSON_CreateObject());
    cJSON_AddStringToObject(js_user,"name",wl->js.name);
    cJSON_AddStringToObject(js_user,"password",wl->js.passwd);   
       /* head */
    cJSON_AddItemToObject(root, head, js_head = cJSON_CreateObject());
    if(white_list_enable == ENABLE) 
      cJSON_AddStringToObject(js_head,"message","白名单开启");
    if(white_list_enable != ENABLE) 
      cJSON_AddStringToObject(js_head,"message","白名单关闭");
    
    cJSON_AddNumberToObject(js_head,"method",wl->js.method);
    cJSON_AddStringToObject(js_head,"mobileid",wl->js.mobileid);
    
    if(wl->isAuthority == enable) {
      if(value == 1) {
        cJSON_AddNumberToObject(js_head,"status",1);
      }else if(value < 0){
        cJSON_AddNumberToObject(js_head,"status",2);
      }
    }else if(wl->isAuthority == disable) {
      if(value == 0) {
        cJSON_AddNumberToObject(js_head,"status",1);
      }else if(value < 0) {
        cJSON_AddNumberToObject(js_head,"status",2);
      }
    }else
      cJSON_AddNumberToObject(js_head,"status",3);
    
//    cJSON_AddNumberToObject(js_head,"status",1);
    cJSON_AddStringToObject(js_head,"boxid","827312897");
       
    {
          char *s = cJSON_PrintUnformatted(root);
          if(s) {
              strcpy(out, s);
              rt_free(s);
          }
     }    
    
  /*free*/
  cJSON_Delete(root);
  rt_free(whitelist);
  
  return ym_ok;
EXIT:
  if(root)  cJSON_Delete(root);
  if(whitelist) rt_free(whitelist);
  return ym_err;
}


/* do login error msg */
static int do_error_msg(char * const out)
{
  cJSON *root, *js_head;
  const char * const head = "head";
  const char * const status = "status";
  const char * const message = "message";
  // {"head":{"message":ERR_MSG,"status":0}}
  
  if(!out) { MY_DEBUG("%s,%d: out is NULL...\n\r",__func__,__LINE__); goto EXIT; }
    
  /* create json string root */
  root = cJSON_CreateObject();
  if(!root) { MY_DEBUG("%s,%d:  get root is faild !\n\r",__func__,__LINE__); cJSON_GetErrorPtr(); goto EXIT; }
  
  cJSON_AddItemToObject(root,head,js_head=cJSON_CreateObject());
  cJSON_AddNumberToObject(js_head,status,0);
  cJSON_AddStringToObject(js_head,message,"登陆失败");
  
  {
    char *s = cJSON_PrintUnformatted(root);
    if(!s) { MY_DEBUG("%s,%d: js print to string faild!\n\r",__func__,__LINE__); goto EXIT; }
    rt_memset(out, 0, buff_size);
    strcpy(out, s);
    rt_free(s);
  }
  
  /* delet root node */
  cJSON_Delete(root);
  
  /**/
  MY_DEBUG("%s,%d: out string is %s\n\r",__func__,__LINE__,out);
  return ym_ok;
EXIT:
    if(root)    cJSON_Delete(root);
    return ym_err;
}

/* Normal user login msg packing */
static int do_normal_msg(char* const out)
{
//TODO...
  cJSON *root, *js_head;
  const char * const head = "head";
  const char * const status = "status";
  const char * const message = "message";

  if(!out) { MY_DEBUG("%s,%d: out is NULL...\n\r",__func__,__LINE__); goto EXIT; }
    /* create json string root */
  root = cJSON_CreateObject();
  if(!root) { MY_DEBUG("%s,%d: get js root faild !\n\r",__func__,__LINE__); goto EXIT; }
  cJSON_AddItemToObject(root,head,js_head=cJSON_CreateObject());
  cJSON_AddNumberToObject(js_head,status,1);
  cJSON_AddStringToObject(js_head,message,"登陆成功");
  
  {
    char *s = cJSON_PrintUnformatted(root);
    if(!s) { MY_DEBUG("%s,%d: js print to string faild!\n\r",__func__,__LINE__); goto EXIT; }
    rt_memset(out, 0, buff_size);
    strcpy(out, s);
    rt_free(s);
  }
  
  /* delet root node */
  cJSON_Delete(root);  
  return ym_ok;
  
EXIT:
  if(root) cJSON_Delete(root);
  return ym_err;
}

/* pack json string for return to ask [eg. admin]*/
static int do_admin_msg(char * const out, white_list_t * whitelist, char count, JSON *js)
{
    cJSON *root, *js_body, *js_list,*js_other, *js_user,*js_head;
    char i = 0;
    const char * const head = "head";
    const char * const body = "body";
    const char * const list = "list";
    const char * const other = "other";
    const char * const user = "user";
    const char * const isAuthority = "isAuthority";
    if(!out) {
      MY_DEBUG("%s,%d: out is NULL!\n\r",__func__,__LINE__);
      goto EXIT;
    }
    /* create json string root */
    root = cJSON_CreateObject();
    if(!root) { MY_DEBUG("%s,%d: get root faild\n\r",__func__,__LINE__); goto EXIT; }
    
    /* packing a json string now... */     

//{"body":{"list":["admin","admin11","admin22"],"other":{"isAuthority":true},"user":{"name":"admin","password":"admin"}},"head":{"method":1000,"mobileid":"353714986786243","status":0}}        
        
        /* body */
      cJSON_AddItemToObject(root, body, js_body = cJSON_CreateObject());
        /*list*/
      if(whitelist != RT_NULL) {
        cJSON_AddItemToObject(js_body, list, js_list = cJSON_CreateArray());
        for(i = 0; i < count; i++) cJSON_AddItemToArray(js_list, cJSON_CreateString( (whitelist+i)->name) );
      }       
       /* other */
       cJSON_AddItemToObject(js_body, other, js_other = cJSON_CreateObject());
       cJSON_AddNumberToObject(js_other,isAuthority,white_list_enable);
//       cJSON_AddNumberToObject(js_other,"isAuthority",1);
       
       /* user */
       cJSON_AddItemToObject(js_body, user, js_user = cJSON_CreateObject());
       cJSON_AddStringToObject(js_user,"name",js->name);
       cJSON_AddStringToObject(js_user,"password",js->passwd);
       
       /* head */
       cJSON_AddItemToObject(root, head, js_head = cJSON_CreateObject());
       cJSON_AddStringToObject(js_head,"message","登陆成功");
       cJSON_AddNumberToObject(js_head,"method",js->method);
       cJSON_AddStringToObject(js_head,"mobileid",js->mobileid);
       cJSON_AddNumberToObject(js_head,"status",1);
       cJSON_AddStringToObject(js_head,"boxid","827312897");
       
       {
            char *s = cJSON_PrintUnformatted(root);
            if(s) {
                strcpy(out, s);
                rt_free(s);
            }
       }
       MY_DEBUG("%s,%d: do admin data string is :\n%s\n\r",__func__,__LINE__, out);
       cJSON_Delete(root);
       return ym_ok;
EXIT:
  if(root)    cJSON_Delete(root);
  return ym_err;
}

/* add user msg packing */
static int do_sign_msg(char *const out, int value,white_list_t * whitelist, char count, JSON * js)
{
    cJSON *root, *js_body, *js_list,*js_other, *js_user,*js_head;
    char i = 0;
    const char * const head = "head";
    const char * const body = "body";
    const char * const list = "list";
    const char * const other = "other";
    const char * const user = "user";
    const char * const isAuthority = "isAuthority";
    
    if(!out) {
      MY_DEBUG("%s,%d: out is NULL!\n\r",__func__,__LINE__);
      goto EXIT;
    }
    /* create json string root */
    root = cJSON_CreateObject();
    if(!root) { MY_DEBUG("%s,%d: get root faild\n\r",__func__,__LINE__); goto EXIT; }
    
    /* packing a json string now... */     

//{"body":{"list":["admin","admin11","admin22"],"other":{"isAuthority":true},"user":{"name":"admin","password":"admin"}},"head":{"method":1000,"mobileid":"353714986786243","status":0}}        
        
        /* body */
      cJSON_AddItemToObject(root, body, js_body = cJSON_CreateObject());
        /*list*/
      if(whitelist != RT_NULL) {
        cJSON_AddItemToObject(js_body, list, js_list = cJSON_CreateArray());
        for(i = 0; i < count; i++) cJSON_AddItemToArray(js_list, cJSON_CreateString( (whitelist+i)->name) );
      }       
       /* other */
       cJSON_AddItemToObject(js_body, other, js_other = cJSON_CreateObject());     
       cJSON_AddNumberToObject(js_other,"isAuthority",white_list_enable);
       
       
       /* user */
       cJSON_AddItemToObject(js_body, user, js_user = cJSON_CreateObject());
       cJSON_AddStringToObject(js_user,"name",js->name);
       cJSON_AddStringToObject(js_user,"password",js->passwd);     
       
       /* head */
       cJSON_AddItemToObject(root, head, js_head = cJSON_CreateObject());
       if(value == FAIL)
        cJSON_AddStringToObject(js_head,"message","注册失败");
       else if(value == SUCCESS)
         cJSON_AddStringToObject(js_head,"message","注册成功");
       else if(value == EXIST)
         cJSON_AddStringToObject(js_head,"message","用户已存在");
       else
         cJSON_AddStringToObject(js_head,"message","未知错误");
       
       cJSON_AddNumberToObject(js_head,"method",js->method);
       cJSON_AddStringToObject(js_head,"mobileid",js->mobileid);
       cJSON_AddNumberToObject(js_head,"status",1);
       cJSON_AddStringToObject(js_head,"boxid","827312897");
   
       
       {
            char *s = cJSON_PrintUnformatted(root);
            if(s) {
                strcpy(out, s);
                rt_free(s);
            }
       }
       MY_DEBUG("%s,%d: do admin data string is :\n%s\n\r",__func__,__LINE__, out);
       cJSON_Delete(root);
       return ym_ok;
EXIT:
  if(root)    cJSON_Delete(root);
  return ym_err;
}


/* back to tcp client */
static int Send(char *out, int conn)
{

  if(send(conn, out, buff_size, 0) < 0) { 
    MY_DEBUG("%s,%d: send faild !\n\r",__func__,__LINE__); 
    return ym_err; 
  }
  return ym_ok;
}

/* parsel json string */
static JSON js_handel(const char * const string)
{
//TODO...
  const char *s = string;
  JSON cjson;
  cJSON * js_root, *js_body, *js_head, *js_user, *js_other,*js;  

  rt_memset(&cjson, 0, sizeof(JSON));
  
  if(!s) {
    MY_DEBUG("%s,%d: tcp string is NULL...\n\r",__func__,__LINE__);
    goto EXIT;
  }
    /* create json root */
    js_root = cJSON_Parse(string);
    if(!js_root) {
     MY_DEBUG("%s,%d: get string root faild !\n\r",__func__,__LINE__);
     goto EXIT;
   }
    /*  get body */
    js_body = cJSON_GetObjectItem(js_root, "body") ;
    if(!js_body) { 
      MY_DEBUG("%s,%d: no body !\n\r",__func__,__LINE__); 
      goto EXIT;
    }
    /*get user*/
    js_user = cJSON_GetObjectItem(js_body, "user");
    if(!js_user) { 
      MY_DEBUG("%s,%d: no user !\n\r",__func__,__LINE__); 
      goto EXIT;
    }
    
    /* get name and passwd */
    js = cJSON_GetObjectItem(js_user, "name");
    if(!js) { 
      MY_DEBUG("%s,%d: no name..!\n\r",__func__,__LINE__); 
      goto EXIT; 
    }
    if(js->type == cJSON_String) 
      strcpy(cjson.name , js->valuestring);
    
    /* get passwd */
    js = cJSON_GetObjectItem(js_user, "password");
    if(js) { 
      MY_DEBUG("%s,%d: no passwd\n\r",__func__,__LINE__); 
      if(js->type == cJSON_String) 
        strcpy(cjson.passwd, js->valuestring);
    }
    /* parsel head */
    js_head = cJSON_GetObjectItem(js_root, "head") ;
    if(!js_head) { 
      MY_DEBUG("%s,%d: no head..\n\r",__func__,__LINE__); 
      goto EXIT; 
    };
    /* get method */
    js = cJSON_GetObjectItem(js_head, "method");
    if(!js) { 
      MY_DEBUG("%s,%d: no method\n\r",__func__,__LINE__); 
      goto EXIT; 
    }
    if(js->type == cJSON_Number)  
      cjson.method = js->valueint;
    /* get mobile ID */
    js = cJSON_GetObjectItem(js_head, "mobileid");
    if(!js) { 
      MY_DEBUG("%s,%d: no mobileid\n\r",__func__,__LINE__); 
      goto EXIT; 
    }
    if(js->type == cJSON_String)  
      strcpy(cjson.mobileid , js->valuestring); 
    
    cJSON_Delete(js_root);
  
  cjson.flag = 1;
  return cjson;
EXIT:
  cjson.flag = 0;
  if(js_root)  cJSON_Delete(js_root);
  return cjson;
}


/* do login */
static int do_login(int conn,char * const string)
{
//TODO...
  int value,ret;
  char *out = NULL;
  char count = 0;
  white_list_t * whitelist= RT_NULL;
  CLOGIN js_logig;
  rt_memset(&js_logig, 0, sizeof(js_logig));
  
  /* paresel js string data */
  js_logig.js = js_handel(string);
  
  /* malloc 1024 kbyte */ 
  out = (char*)rt_malloc(buff_size);
  if(!out)  { 
    MY_DEBUG("%s,%d: malloc out faild !\n\r",__func__,__LINE__); 
    goto EXIT; 
  }
  rt_memset(out, 0, buff_size);
  
  /* login */
  value = login_authentication(&js_logig.js);
  
  if(value == (IS_ADMIN|LOGIN_OK)) {
    MY_DEBUG("%s,%d: admin login..\n\r",__func__,__LINE__);
    /* read whitelist */
    white_list((void*)&whitelist, &count);
  /* packing admin json string */
    if( do_admin_msg(out, whitelist, count, &js_logig.js) < 0) { 
      MY_DEBUG("%s,%d: do admin jsson string faild !\n\r",__func__,__LINE__);
      goto EXIT; 
    }
  }
  else if(value == LOGIN_OK) {  //Normal user login
    if( do_normal_msg(out) < 0) { 
      MY_DEBUG("%s,%d: do mormal msg faild !\n\r",__func__,__LINE__); 
      goto EXIT; 
    }
  }else if (value == LOGIN_ERR) { // login faild
    //TODO ...
    if( do_error_msg(out) < 0) { 
      MY_DEBUG("%s,%d: packing err msg faild !\n\r",__func__,__LINE__); 
      goto EXIT; 
    }
  }
  
  /*send to client */
  MY_DEBUG("%d: now send..\n\r",__LINE__);
  if( Send(out, conn) < 0) { 
    MY_DEBUG("%s,%d: send faild !\n\r",__func__,__LINE__); 
    goto EXIT; 
  }

/*free malloc*/  
  rt_free(out);
  rt_free(whitelist);
  return ym_ok;
EXIT:
  if(out)   rt_free(out);
  if(whitelist) rt_free(whitelist);
  return ym_err;  
  
}

/* do sign */
static int do_sign(int conn, char * const string)
{
//TODO...
  CSIGN_IN js_sign;
  char ret;
  white_list_t user ;
  white_list_t * whitelist= RT_NULL;
  char count = 0;
  
  rt_memset(&js_sign, 0, sizeof(js_sign));
  /* parsel tcp data*/
  js_sign.js = js_handel(string);
//TODO...
  char *out = (char*)rt_malloc(sizeof(char) * buff_size);
  if(!out)  { 
    MY_DEBUG("%s,%d: malloc out faild !\n\r",__func__,__LINE__); 
    goto EXIT; 
  }
  rt_memset(out, 0, buff_size);
  rt_memset(&user, 0, sizeof(white_list_t));
  
  strcpy(user.name, js_sign.js.name);
  strcpy(user.psd, js_sign.js.passwd);
  /* add user and passwd to whitelist */
  is_enable_list();
  ret = add_white_list(&user);
  /* read whitelist */
   white_list((void*)&whitelist, &count);
  /* pack sign msg */
  if(do_sign_msg(out, ret,whitelist,count, &js_sign.js) < 0) { 
    MY_DEBUG("%s,%d: do sign msg faild !\n\r",__func__,__LINE__); 
    goto EXIT; 
  }
  
  /* send to client */
  if( Send(out, conn) < 0) { 
    MY_DEBUG("%s,%d: send faild !\n\r",__func__,__LINE__); 
    goto EXIT; 
  } 
  MY_DEBUG("%s,%d:<----------->\n\r",__func__,__LINE__);
  
  /* free */
  rt_free(out);
  rt_free(whitelist);
  
  return ym_ok;
EXIT:
  if(out) rt_free(out);
  if(whitelist) rt_free(whitelist);
  return ym_err;  
}

/* whitelist tcp data */
static WhiteList white_js_handel(const char * const string)
{
   WhiteList wl;
   const char *s = string;
   cJSON * js_root, *js_body, *js_head, *js_user, *js_other,*js;    
   rt_memset(&wl, 0, sizeof(WhiteList));
   wl.js = js_handel(string);
   if(!wl.js.flag) {
     MY_DEBUG("%s,%d: do tcp data faild !\n\r",__func__,__LINE__);
     goto EXIT;
   }
   
   js_root = cJSON_Parse(string);
   if(!js_root) {
     MY_DEBUG("%s,%d: get string root faild !\n\r",__func__,__LINE__);
     goto EXIT;
   }
    /*  get body */
    js_body = cJSON_GetObjectItem(js_root, "body") ;
    if(!js_body) { 
      MY_DEBUG("%s,%d: no body !\n\r",__func__,__LINE__); 
      goto EXIT;
    }
    /* get other */
    js_other = cJSON_GetObjectItem(js_body, "other");
      if( !js_other) { 
          MY_DEBUG("%s,%d: no isAuthority\n\r",__func__,__LINE__);
          goto EXIT;
      }else {
            js = cJSON_GetObjectItem(js_other, "isAuthority");
            if(!js) { 
              MY_DEBUG("%s,%d: isAuthority error..\n\r",__func__,__LINE__);
            }else{
              if(js->type == cJSON_Number) {
                wl.isAuthority = js->valueint;
              }
            }
        }
        cJSON_Delete(js_root);
        return wl;
EXIT:
   if(js_root) cJSON_Delete(js_root);
   wl.js.flag = 0;
   return wl;
}

/* do_whitelist */
static int do_whitelist(int conn, char * const string)
{
  WhiteList wl;
  char value;
  char *out;
  rt_memset(&wl, 0, sizeof(wl));
  /* paresel tcp data */
  wl = white_js_handel(string);
  if( !wl.js.flag) {
    MY_DEBUG("%s,%d: do tcp data faild !\n\r",__func__,__LINE__);
    goto EXIT;
  }
   
  if(wl.isAuthority == disable)
   value =  disable_white_list();
  else if(wl.isAuthority == enable)
   value =  enable_white_list();
 /*malloc*/
  out = (char*)rt_malloc(sizeof(char) * buff_size);
  if(!out) { 
    MY_DEBUG("%s,%d: malloc out faild !\n\r",__func__,__LINE__); 
    goto EXIT; 
  }
  rt_memset(out, 0, buff_size);
  /* packing whitelist json string */
  if(do_whitelist_msg(out, &wl, value) < 0) { 
    MY_DEBUG("%s,%d: do whitelist msg faild !\n\r",__func__,__LINE__); 
    goto EXIT; 
  }
  
  /*send to ask*/
  if( Send(out, conn) < 0) { 
    MY_DEBUG("%s,%d: send faild !\n\r",__func__,__LINE__); 
    goto EXIT; 
  } 
  MY_DEBUG("%s,%d:<+++++++>\n\r",__func__,__LINE__); 
  
  /* free */
   rt_free(out);
   
  return ym_ok;
EXIT:
  if(out)   rt_free(out);
  return ym_err;    
    
}

/*******************************************************************************************/

static int do_passwd_msg(char *out,Passwd_t *ps, int value)
{
   cJSON *root, *head;
   /* create root */
   root = cJSON_CreateObject(); 
   if(!root) {
     MY_DEBUG("%s,%d: create root faild !\n\r",__func__,__LINE__);
     goto EXIT;
   }
   cJSON_AddItemToObject(root, "head", head=cJSON_CreateObject());
   
   if(value == 1)
    cJSON_AddStringToObject(head,"message","修改密码成功");
   else if(value == 0)
    cJSON_AddStringToObject(head,"message","修改密码失败");
   
   cJSON_AddNumberToObject(head,"method",ps->js.method);
   cJSON_AddStringToObject(head,"mobileid",ps->js.mobileid);
   
   if(value == 1) 
     cJSON_AddNumberToObject(head,"status",1);
   if(value == 0)
     cJSON_AddNumberToObject(head,"status",2);
   
   cJSON_AddStringToObject(head,"boxid",ps->js.mobileid);
   {
        char *s = cJSON_PrintUnformatted(root);
        if(s) {
            strcpy(out, s);
            rt_free(s);
        }
        
   }
   if(root)
    cJSON_Delete(root);
  return ym_ok;
EXIT:
  if(root)
    cJSON_Delete(root);
  return ym_err;
}




static int passwd_js_handle(Passwd_t *ps, const char * const string)
{
  cJSON * root, *js_body,*js_user,*js_passwd;
  
  if(!ps || !string) {
    MY_DEBUG("%s,%d: string or ps is NULL..\n\r",__func__,__LINE__);
    goto EXIT;
  }
  /* get basic json */
  ps->js = js_handel(string);
  if(!ps->js.flag) {
    MY_DEBUG("%s,%d: get basic json faild !\n\r",__func__,__LINE__);
    goto EXIT;
  }
  
     /* create json root */
    root = cJSON_Parse(string);
    if(!root) {
     MY_DEBUG("%s,%d: get string root faild !\n\r",__func__,__LINE__);
     goto EXIT;
   }   
   /* get body */
    /*  get body */
    js_body = cJSON_GetObjectItem(root, "body") ;
    if(!js_body) { 
      MY_DEBUG("%s,%d: no body !\n\r",__func__,__LINE__); 
      goto EXIT;
    }
    /* get user */
    js_user = cJSON_GetObjectItem(js_body, "user") ;
    if(!js_user) { 
      MY_DEBUG("%s,%d: no body !\n\r",__func__,__LINE__); 
      goto EXIT;
    } 
   /* get new passwd */
   js_passwd =  cJSON_GetObjectItem(js_user,"newPassword");
   if(!js_passwd) {
     MY_DEBUG("%s,%d: no new passwd !\n\r",__func__,__LINE__);
     goto EXIT;
   }
   strcpy(ps->new_passwd, js_passwd->valuestring);
   
   /* free space */
   cJSON_Delete(root);
  return ym_ok;
EXIT:
  return ym_err;
}


/* change passwd */
static int do_passwd(int conn, const char * const string)
{
//TODO...
  
  Passwd_t * ym_passwd = RT_NULL;
  
  ym_passwd = (Passwd_t *)rt_malloc(sizeof(Passwd_t));
  if(!ym_passwd) {
    MY_DEBUG("%s,%d: malloc ym_passwd faild !\n\r",__func__,__LINE__);
    goto EXIT;
  }
  rt_memset(ym_passwd, 0, sizeof(Passwd_t));
  
  /* get js  */
  if(passwd_js_handle(ym_passwd, string) < 0) {
    MY_DEBUG("%s,%d:get js faild !\n\r",__func__,__LINE__);
    goto EXIT;
  }
  /* debug */
  MY_DEBUG("%s,%d: for debug: new passwd  is %s\n",__func__,__LINE__, ym_passwd->new_passwd);
  /* change passwd */
  int value = change_admin(ym_passwd->js.name, ym_passwd->new_passwd);
  /* malloc 1024 bytes*/
  char *out = (char*)rt_malloc(buff_size);
  if(!out) {
    MY_DEBUG("%s,%d: malloc out is faild !\n\r",__func__,__LINE__);
    goto EXIT;
  }
  rt_memset(out, 0, buff_size);
  /* packing send data */
  if(do_passwd_msg(out,ym_passwd, value) < 0) {
    MY_DEBUG("%s,%d: do passwd msg faild !\n\r",__func__,__LINE__);
    goto EXIT;
  }
  
  /* send data to ask */
  if( Send(out, conn) < 0) { 
    MY_DEBUG("%s,%d: send faild !\n\r",__func__,__LINE__); 
    goto EXIT; 
  }  
  
  /* for debug */
  MY_DEBUG("%s,%d: out string is %s\n@@@@@@@@@@@@@@@@@@@@@\n\r",__func__,__LINE__,out);
    
  if(out)
    rt_free(out);
  
  if(ym_passwd)
    rt_free(ym_passwd);
  

  return ym_ok;
EXIT:
  if(ym_passwd)
    rt_free(ym_passwd);
  if(out)
    rt_free(out);
      
  return ym_err;
}

/************************************************/
/* rm user msg */
static int do_rm_user_msg(deltuser_t user, char *out)
{
    cJSON *root, *js_body, *js_list,*js_other, *js_user,*js_head;
    char count = 0;
    white_list_t * whitelist= RT_NULL;
    char i = 0;
    if(!out) {
      MY_DEBUG("%s,%d: out is NULL!\n\r",__func__,__LINE__);
      goto EXIT;
    }
    /* create json string root */
    root = cJSON_CreateObject();
    if(!root) { 
      MY_DEBUG("%s,%d: get root faild\n\r",__func__,__LINE__); 
      goto EXIT; 
    }
    
    /* get whitelist */
    white_list((void*)&whitelist, &count);
    
    /* packing a json string now... */     

//{"body":{"list":["admin","admin11","admin22"],"other":{"isAuthority":true},"user":{"name":"admin","password":"admin"}},"head":{"method":1000,"mobileid":"353714986786243","status":0}}        
        
        /* body */
      cJSON_AddItemToObject(root, "body", js_body = cJSON_CreateObject());
        /*list*/
      if(whitelist != RT_NULL) {
        cJSON_AddItemToObject(js_body, "list", js_list = cJSON_CreateArray());
        for(i = 0; i < count; i++) cJSON_AddItemToArray(js_list, cJSON_CreateString( (whitelist+i)->name) );
      }       
       /* other */
       cJSON_AddItemToObject(js_body, "other", js_other = cJSON_CreateObject());
       cJSON_AddNumberToObject(js_other,"isAuthority",white_list_enable);
//       cJSON_AddNumberToObject(js_other,"isAuthority",1);
       
       /* user */
       cJSON_AddItemToObject(js_body, "user", js_user = cJSON_CreateObject());
       cJSON_AddStringToObject(js_user,"name",user.js.name);
       cJSON_AddStringToObject(js_user,"password",user.js.passwd);
       
       /* head */
       cJSON_AddItemToObject(root, "head", js_head = cJSON_CreateObject());
       cJSON_AddStringToObject(js_head,"message","登陆成功");
       cJSON_AddNumberToObject(js_head,"method",user.js.method);
       cJSON_AddStringToObject(js_head,"mobileid",user.js.mobileid);
       cJSON_AddNumberToObject(js_head,"status",1);
       cJSON_AddStringToObject(js_head,"boxid","827312897");
       
       {
            char *s = cJSON_PrintUnformatted(root);
            if(s) {
                strcpy(out, s);
                rt_free(s);
            }
       }
       MY_DEBUG("%s,%d: do admin data string is :\n%s\n\r",__func__,__LINE__, out);
       cJSON_Delete(root);
       return ym_ok;
EXIT:
  if(root)    cJSON_Delete(root);
  return ym_err;    
}

/* rm user */
static int do_rm_user(int conn, const char * const string)
{
    deltuser_t user;
    rt_memset(&user, 0, sizeof(user));
    
    user.js = js_handel(string);
    if(!user.js.flag) {
      MY_DEBUG("%s,%d: get user json string faild !\n\r",__func__,__LINE__);
      goto EXIT;
    }
    /* delete user */
    
    //TODO...
    
    /* malloc 1024 bytes */
    char *out = (char*)rt_malloc(buff_size);
    if(!out) {
      MY_DEBUG("%s,%d: malloc out faild !\n\r",__func__,__LINE__);
      goto EXIT;
    }
    rt_memset(out, 0, buff_size);
    
    /* packing data */
    if(do_rm_user_msg(user, out) < 0) {
      MY_DEBUG("%s,%d: packing rm user msg faild !\n\r",__func__,__LINE__);
      goto EXIT;
    }
    
    /* send data */
    if(Send(out, conn) < 0) {
      MY_DEBUG("%s,%d: send data faild !\n\r",__func__,__LINE__);
      goto EXIT;
    }
    /* debug */
    MY_DEBUG("%s,%d: debug rm back msg is :\n%s\n^^^^^^^^^^^^^^^^^^^^^\n",__func__,__LINE__, out);
    
    if(out)
      rt_free(out);
    return ym_ok;
EXIT:
  if(out)
    rt_free(out);
  return ym_err;
}

/*  mac to string */
static char * mac2str(char * mac_buff, char count)
{
//  char mac[32] = {0};
  char temp_buff[3] = {0};
  char i = 0;
  
  char *mac = (char*)rt_malloc(25);
  if(!mac) {
    MY_DEBUG("%s,%d: malloc faild !\n\r",__func__,__LINE__);
    goto EXIT;
  }
  
  rt_memset(mac, 0, 25);
  if(!mac_buff) {
    MY_DEBUG("%s,%d: mac is NULL", __func__,__LINE__);
    goto EXIT;
  }
  
  for(i = 0; i < count;i++) {
    rt_memset(temp_buff, 0, sizeof(temp_buff));
    sprintf(temp_buff, "%02X", mac_buff[i]);
    strcat(mac, temp_buff);
    if(i < (count-1))
        strcat(mac, ":");
  }
  
//  MY_DEBUG("%s,%d: mac string is %s\n\r",__func__,__LINE__, mac);
  return mac;
EXIT:
  return RT_NULL;
}


/* do read device msg */
static int do_ReDevice_msg(char * out, ReadDev read_dev)
{
  //TODO....  
  cJSON *root, *js_head, *js_list, *js_user,*js_arry,*js_body;
  int i  = 0, j = 0;
  char *mac = RT_NULL;
  
  root = cJSON_CreateObject();
  if(!root) {
    MY_DEBUG("%s,%d: get root faild !\n\r",__func__,__LINE__);
    goto EXIT;
  }
   cJSON_AddItemToObject(root, "body", js_body = cJSON_CreateObject());
   cJSON_AddItemToObject(js_body, "list", js_list = cJSON_CreateArray());
   for(i = 0; i< read_dev.count; i++) {
        cJSON_AddItemToArray(js_list, js_arry = cJSON_CreateObject());
        mac = mac2str(read_dev.dev[i].device_mac, 8);
//        MY_DEBUG("%s,%d: mac string is %s\n\r",__func__,__LINE__, mac);
        cJSON_AddStringToObject(js_arry, "device_mac",mac); //TODO...
        cJSON_AddNumberToObject(js_arry, "devcice_progress", read_dev.dev[i].devcice_progress);
        rt_free(mac);
        mac = RT_NULL;
        js_arry = RT_NULL;
   }
   cJSON_AddItemToObject(js_body, "user", js_user = cJSON_CreateObject());
   cJSON_AddStringToObject(js_user, "name", read_dev.js.name);
   cJSON_AddStringToObject(js_user, "password", read_dev.js.passwd);

   cJSON_AddItemToObject(root, "head", js_head = cJSON_CreateObject());
   if(read_dev.op) {
     cJSON_AddStringToObject(js_head, "message", "读取状态成功"); 
   }else
     cJSON_AddStringToObject(js_head, "message", "读取状态失败");
   
   cJSON_AddNumberToObject(js_head, "method", read_dev.js.method);
   cJSON_AddStringToObject(js_head, "mobileid", read_dev.js.mobileid);
   
   if(read_dev.op) {
       cJSON_AddNumberToObject(js_head, "status", 1); 
   }else
     cJSON_AddNumberToObject(js_head, "status", 0); 
   
   {
        char *s =  cJSON_PrintUnformatted(root);
        if(s) {
            strcpy(out, s);
            rt_free(s);
        }
   }
   
   
  if(root) 
    cJSON_Delete(root);
  if(js_arry)    
    cJSON_Delete(js_arry);
  if(mac)
    rt_free(mac);
    
  return ym_ok;
EXIT:
  if(root)
    cJSON_Delete(root);
  if(mac)
    rt_free(mac);
  if(js_arry)
     cJSON_Delete(js_arry);
  return ym_err;
}


static  ReadDev  read_js_handel(const char * const string)
{
//   ReadDev * read_dev = RT_NULL;
   const char *s = string;
   char mac_buff[MAC_LEN_IN_BYTE];
   char * p = NULL;
   int size = -1, i = 0;
    ReadDev  read_dev ;
      
   cJSON * js_root, *js_body, *js_head, *js_list,*js_item, *item,*dev_mac, *dev_progress, *js;
   rt_memset(&read_dev, 0, sizeof(read_dev));
   
   if(!s) {
     MY_DEBUG("%s,%d: tcp string is NULL..\n\r",__func__,__LINE__);
     goto EXIT;
   }
   /* deal with baisc string */
   read_dev.js = js_handel(s);
   if( !read_dev.js.flag) {
     MY_DEBUG("%s,%d: deal basic string is faild !\n\r",__func__,__LINE__);
     goto EXIT;
   }
    /* create json root */
    js_root = cJSON_Parse(s);
    if(!js_root) {
     MY_DEBUG("%s,%d: get string root faild !\n\r",__func__,__LINE__);
     goto EXIT;
   }   
   /* get body */
    /*  get body */
    js_body = cJSON_GetObjectItem(js_root, "body") ;
    if(!js_body) { 
      MY_DEBUG("%s,%d: no body !\n\r",__func__,__LINE__); 
      goto EXIT;
    }
    /* get list */
    js_list = cJSON_GetObjectItem(js_body, "list") ;
    if(!js_body) { 
      MY_DEBUG("%s,%d: no body !\n\r",__func__,__LINE__); 
      goto EXIT;
    }    

    
   size = cJSON_GetArraySize(js_list);
   MY_DEBUG("%s,%d: size is %d\n\r",__func__,__LINE__, size);
   

   int j  = 0;

   /* device count */
   read_dev.count = 0;
   read_dev.op = 100;  
   /* get mac and progress */
       for(i = 0; i < size; i++) {
           js_item = cJSON_GetArrayItem(js_list,i);
           if(js_item) {
                p = cJSON_PrintUnformatted(js_item);
    //            MY_DEBUG("%s,%d: p is %s\n",__func__,__LINE__, p);
                item = cJSON_Parse(p);
                if(!item) 
                  continue ;
                dev_mac = cJSON_GetObjectItem(item, "device_mac");            
                if(dev_mac) {
                    /* copy str to mac */
                    rt_memset(mac_buff, 0, sizeof(mac_buff));
                    copy_str2mac(mac_buff, dev_mac->valuestring);
                    
                    /* for test */
                    for(j = 0; j < 8; j++) {
                        read_dev.dev[i].device_mac[j] = mac_buff[j];
                    }
                    (read_dev.count)++;
    //                read_dev.dev++;
                }
                dev_progress =  cJSON_GetObjectItem(item, "dev_progress");
                if(dev_progress) {
                    read_dev.dev[i].devcice_progress = dev_progress->valueint;
                }
    //            cJSON_Delete(item);
           }
       }    
   if(item) 
     cJSON_Delete(item);
   cJSON_Delete(js_root);
   read_dev.js.flag = 1;
   return read_dev;
EXIT:
  if(js_root) 
    cJSON_Delete(js_root);
  read_dev.js.flag = 0;
  return read_dev;
}
/************************************** opreation device *******************************************/
/* read device data */
ReadDev  read_dev;
static int do_ReDevice(int conn, const char * const string)
{
    
    dev_t  dev;
    int i = 0, j = 0;
    char *out = NULL;
    rt_memset(&read_dev, 0, sizeof(ReadDev));
    rt_memset(&dev, 0, sizeof(dev));
    /* parsel tcp data */
   read_dev =  read_js_handel(string);
   if(!read_dev.js.flag) {
     MY_DEBUG("%s,%d: parsel tcp data faild !\n\r",__func__,__LINE__);
     goto EXIT;
   }
   /* for tes */
   /* op */
   read_dev.op = 1;
   
   for(i = 0; i < read_dev.count; i++) {
     memcpy(dev.mac, read_dev.dev[i].device_mac, 8);
     dev.stat = read_dev.dev[i].devcice_progress;
     
     if(!exist_dev(&dev)){ 
       read_dev.op = 0;
       continue ;
     }
     read_dev.dev[i].devcice_progress = dev.stat;
   }

    out = (char*)rt_malloc(buff_size);
    if(!out) {
      MY_DEBUG("%s,%d: malloc faild !\n\r",__func__,__LINE__);
      goto EXIT;
    }
    rt_memset(out, 0, buff_size);
   
 /* packing send data */
    if(do_ReDevice_msg(out, read_dev) < 0) {
      MY_DEBUG("%s,%d: do read device msg faild !\n\r",__func__,__LINE__);
      goto EXIT;
    }
    
    MY_DEBUG("%s,%d: out string is %s\n\r",__func__,__LINE__,out);
      
  if( Send(out, conn) < 0) { 
    MY_DEBUG("%s,%d: send faild !\n\r",__func__,__LINE__); 
    goto EXIT; 
  } 
  MY_DEBUG("%s,%d:<-------->\n\r",__func__,__LINE__);  
    
    if(out) 
      rt_free(out);
    return ym_ok;
EXIT:
  if(out) rt_free(out);
  return ym_err;
}

/******************************************************************************************/
/* do add device msg */
static int do_add_device_msg(char * out ,ReadDev op)
{

  cJSON *root, *js_head;
  
  /* root */
  root = cJSON_CreateObject();
  if(!root) {
    MY_DEBUG("%s,%d: get root faild !\n\r",__func__,__LINE__);
    goto EXIT;
  }
  /* head */
  cJSON_AddItemToObject(root, "head", js_head=cJSON_CreateObject());
  if(op.op)
    cJSON_AddStringToObject(js_head,"message","添加设备成功");
  else
    cJSON_AddStringToObject(js_head,"message","添加设备失败");
  
  cJSON_AddNumberToObject(js_head,"method",op.js.method);
  cJSON_AddStringToObject(js_head,"mobileid",op.js.mobileid);
  
  if(op.op)
     cJSON_AddNumberToObject(js_head,"status",1);
  else
    cJSON_AddNumberToObject(js_head,"status",2);
  
  {
     char *s = cJSON_PrintUnformatted(root);
     if(s) {
        strcpy(out, s);
        rt_free(s);
     }
  }
  
  if(root)
    cJSON_Delete(root);
  
  return ym_ok;
EXIT:
  if(root)
    cJSON_Delete(root);  
  return ym_err;
}

/* add device */
static int do_add_device(int conn, const char * const string)
{
//TODO...
    dev_t  dev;
    int i = 0, j = 0;
    char *out = NULL;
    rt_memset(&read_dev, 0, sizeof(ReadDev));
    rt_memset(&dev, 0, sizeof(dev));
    /* parsel tcp data */
   read_dev =  read_js_handel(string);
   if(!read_dev.js.flag) {
     MY_DEBUG("%s,%d: parsel tcp data faild !\n\r",__func__,__LINE__);
     goto EXIT;
   }
   /*op*/
   read_dev.op = 1;
   
   /* add device */
   for(i = 0; i < read_dev.count; i++) {
     memcpy(dev.mac, read_dev.dev[i].device_mac, 8);
     dev.stat = read_dev.dev[i].devcice_progress; 
     if(!add_dev(&dev)){ 
       read_dev.op = 0;
     }
   }
    /* malloc 1024 bytes */ 
     out = (char*)rt_malloc(buff_size);
     if(!out) {
       MY_DEBUG("%s,%d: malloc faild !\n\r",__func__,__LINE__);
       goto EXIT;
     }
     rt_memset(out, 0, buff_size);
     /* packing data */
     if(do_add_device_msg(out, read_dev) < 0) {
       MY_DEBUG("%s,%d: do add device msg faild !\n\r",__func__,__LINE__);
       goto EXIT;
     }
     MY_DEBUG("%s,%d: send data is :\n%s\n\r",__func__,__LINE__,out);
     /* send data */
      if( Send(out, conn) < 0) { 
        MY_DEBUG("%s,%d: send faild !\n\r",__func__,__LINE__); 
        goto EXIT; 
      }
   
   if(out)   
     rt_free(out);
   MY_DEBUG("%s,%d: add success------------>\n\r",__func__,__LINE__);
   return ym_ok;
EXIT:
  if(out)   
    rt_free(out);
  return ym_err;
}

/************* rm device *************************/
static int do_rm_device_msg(char *out, ReadDev op)
{
  cJSON *root, *js_head;
  
  /* root */
  root = cJSON_CreateObject();
  if(!root) {
    MY_DEBUG("%s,%d: get root faild !\n\r",__func__,__LINE__);
    goto EXIT;
  }
  /* head */
  cJSON_AddItemToObject(root, "head", js_head=cJSON_CreateObject());
  if(op.op)
    cJSON_AddStringToObject(js_head,"message","删除设备成功");
  else
    cJSON_AddStringToObject(js_head,"message","删除设备失败");
  
  cJSON_AddNumberToObject(js_head,"method",op.js.method);
  cJSON_AddStringToObject(js_head,"mobileid",op.js.mobileid);
  
  if(op.op)
     cJSON_AddNumberToObject(js_head,"status",1);
  else
    cJSON_AddNumberToObject(js_head,"status",2);
  
  {
     char *s = cJSON_PrintUnformatted(root);
     if(s) {
        strcpy(out, s);
        rt_free(s);
     }
  }
  
  if(root)
    cJSON_Delete(root);  
  return ym_ok;
EXIT:
  return ym_err;
}

static int do_rm_device(int conn, const char * const string)
{
    dev_t  dev;
    int i = 0, j = 0;
    char *out = NULL;
    rt_memset(&read_dev, 0, sizeof(ReadDev));
    rt_memset(&dev, 0, sizeof(dev));
    /* parsel tcp data */
   read_dev =  read_js_handel(string);
   if(!read_dev.js.flag) {
     MY_DEBUG("%s,%d: parsel tcp data faild !\n\r",__func__,__LINE__);
     goto EXIT;
   }
   /*op*/
   read_dev.op = 1;
   /* rm device */
    for(i = 0; i < read_dev.count; i++) {
     memcpy(dev.mac, read_dev.dev[i].device_mac, 8);
     dev.stat = read_dev.dev[i].devcice_progress; 
     if(!del_dev(&dev)){ 
       read_dev.op = 0;
     }
   }  
  /* malloc 1024 bytes */
     out = (char*)rt_malloc(buff_size);
     if(!out) {
       MY_DEBUG("%s,%d: malloc faild !\n\r",__func__,__LINE__);
       goto EXIT;
     }
     rt_memset(out, 0, buff_size);
 /* packing */
     if(do_rm_device_msg(out, read_dev) < 0) {
       MY_DEBUG("%s,%d: do rm device msg faild !\n\r",__func__,__LINE__);
       goto EXIT;
     }
   /* send data */  
     if( Send(out, conn) < 0) { 
        MY_DEBUG("%s,%d: send faild !\n\r",__func__,__LINE__); 
        goto EXIT; 
     }   
   
  if(out)
    rt_free(out);
  return ym_ok;
EXIT:
  if(out)
    rt_free(out);
  return ym_err;
}


/**************************************** ctrol LED  **************************************************/

static int do_ctr_led_msg(char *out, ReadDev op)
{
  cJSON *root, *js_head;
  
  /* root */
  root = cJSON_CreateObject();
  if(!root) {
    MY_DEBUG("%s,%d: get root faild !\n\r",__func__,__LINE__);
    goto EXIT;
  }
  /* head */
  cJSON_AddItemToObject(root, "head", js_head=cJSON_CreateObject());
  if(op.op)
    cJSON_AddStringToObject(js_head,"message","删除设备成功");
  else
    cJSON_AddStringToObject(js_head,"message","删除设备失败");
  
  cJSON_AddNumberToObject(js_head,"method",op.js.method);
  cJSON_AddStringToObject(js_head,"mobileid",op.js.mobileid);
  
  if(op.op)
     cJSON_AddNumberToObject(js_head,"status",1);
  else
    cJSON_AddNumberToObject(js_head,"status",2);
  
  {
     char *s = cJSON_PrintUnformatted(root);
     if(s) {
        strcpy(out, s);
        rt_free(s);
     }
  }
  
  if(root)
    cJSON_Delete(root);  
  return ym_ok;
EXIT:
  return ym_err;
}


static int do_ctr_device(int conn, const char * const string)
{
//    dev_t  dev;
    int i = 0, j = 0;
    char *out = NULL;
    rt_memset(&read_dev, 0, sizeof(ReadDev));
//    rt_memset(&dev, 0, sizeof(dev));
    /* parsel tcp data */
   read_dev =  read_js_handel(string);
   if(!read_dev.js.flag) {
     MY_DEBUG("%s,%d: parsel tcp data faild !\n\r",__func__,__LINE__);
     goto EXIT;
   }
   /*op*/
   read_dev.op = 1;
   /* contrl device */
    for(i = 0; i < read_dev.count; i++) {
//     memcpy(dev.mac, read_dev.dev[i].device_mac, 8);
//     dev.stat = read_dev.dev[i].devcice_progress; 
//     if(!del_dev(&dev)){ 
//       read_dev.op = 0;
//     }
   }  
  /* malloc 1024 bytes */
     out = (char*)rt_malloc(buff_size);
     if(!out) {
       MY_DEBUG("%s,%d: malloc faild !\n\r",__func__,__LINE__);
       goto EXIT;
     }
     rt_memset(out, 0, buff_size);
 /* packing */
//     if(do_rm_device_msg(out, read_dev) < 0) {
//       MY_DEBUG("%s,%d: do rm device msg faild !\n\r",__func__,__LINE__);
//       goto EXIT;
//     }
   /* send data */  
     if( Send(out, conn) < 0) { 
        MY_DEBUG("%s,%d: send faild !\n\r",__func__,__LINE__); 
        goto EXIT; 
     }   
   
  if(out)
    rt_free(out);
  return ym_ok;
EXIT:
  if(out)
    rt_free(out);
  return ym_err;
}


/********************************************************************************************/

/* get method from js string */
static int get_method(char * const string)
{
   cJSON * js_root,*js_head,*js;
    int method;
    if(!string) {
      MY_DEBUG("%s,%d: string is NULL..\n\r",__func__,__LINE__);
      goto EXIT;
    }
    
    /* create root */
    js_root = cJSON_Parse(string);
    if(!js_root) {
      MY_DEBUG("%s,%d: get string root faild !\n\r",__func__,__LINE__);
      goto EXIT;
    }
    /* get head */
    js_head = cJSON_GetObjectItem(js_root, "head");
    if(!js_head) {
      MY_DEBUG("%s,%d: The string no have \"head\" key word !\n\r",__func__,__LINE__);
      goto EXIT;
    }
    /* get method */
    js = cJSON_GetObjectItem(js_head, "method");
    if(!js) {
      MY_DEBUG("%s,%d: The string no have \"method\" key word !\n\r",__func__,__LINE__);
      goto EXIT;
    }
    if(js->type == cJSON_Number) {
       method = js->valueint;
    }else {
      MY_DEBUG("%s,%d: method value type is error !\n\r",__func__,__LINE__);
      goto EXIT;
    } 
    
    if(js_root) 
      cJSON_Delete(js_root);
    return method;
EXIT:
    if(js_root) 
      cJSON_Delete(js_root);  
    return -1;
}


/* recv data from connection fd */
static int do_recv(int conn)
{
  int value = 0;
  int method = -1;
  char *buff = NULL;
  JSON  cjson;

   /* list mem*/
//  list_mem();
  /* check connect descript file */
  if(conn < 0) { 
    MY_DEBUG("%s,%d: con fd is %d\n\r",__func__,__LINE__,conn); 
    goto EXIT; 
  }
  
  /* malloc 1024 bytes */
  buff = (char*)rt_malloc(buff_size);
  if(!buff) { 
    MY_DEBUG("%s,%d: malloc buff faild !\n\r",__func__,__LINE__); 
    goto EXIT; 
  }
  rt_memset(buff, 0, buff_size);  /* rt_memset() */
  
  /* recive data from remote perr */
  value = recv(conn, buff, buff_size, 0);
  if(value < 0) { 
    MY_DEBUG("%s,%d: recv error\n\r",__func__,__LINE__); 
    goto EXIT; 
    
  }else if( !value ) { 
    MY_DEBUG("%s, %d: tcp client quit..\n\r",__func__,__LINE__); 
    goto EXIT;
  }
  
  /* get method */
  method = get_method(buff);

  switch(method) {
    /*登陆*/
        case login:         
          do_login(conn,buff);        
          break;
    /*注册 or 添加用户*/
        case sign_in:       
          do_sign(conn, buff);        
          break;  
    /*白名单操作 */     
        case off_whitelist: 
          do_whitelist(conn, buff);   
          break; //TODO...
    /*密码修改*/      
        case ch_passwd:     
          do_passwd(conn, buff);      
          break; //TODO...
    /*删除用户*/      
        case rm_user:       
          do_rm_user(conn, buff);     
          break; //TODO...
    /*读取设备状态*/      
        case re_device:     
          do_ReDevice(conn, buff);    
          break; 
    /*添加用户*/      
        case add_device:    
          do_add_device(conn, buff);  
          break;
    /*删除设备*/      
        case rm_device:     
          do_rm_device(conn, buff);   
          break;
    /*控制设备*/      
        case ctr_device:    
          do_ctr_device(conn, buff);  
          break;
    /*其他*/      
        default :
            goto EXIT;
  };
  
  /* free buff */
    if(buff)
        rt_free(buff);
   return value;
EXIT:
  if(buff)   
    rt_free(buff);
  return ym_err;
}

/* Accept */
static int Accept(int listenfd)
{
    MY_DEBUG("%s,%d\n\r",__func__,__LINE__);
    
    int confd = -1, i = 0, maxfd;
    int sock_reuse = 1; // overload address and port number
    int keepalive = 1; // 开启keepalive属性
    int conn_cout, ret;
    int client[BACKLOG];
    
    socklen_t sin_size;
    
    struct timeval timeout;
    struct sockaddr_in client_addr;
    
    fd_set rset;
    fd_set allset;
    
    /* is listenfd < 0, return -1 */
    if(listenfd < 0) {  
      MY_DEBUG("%s,%d: listenfd is error!\n\r",__func__,__LINE__); 
      goto EXIT; 
    }
    
    /* initation connection client[] */
    for(i = 0; i < BACKLOG; i++) { 
      client[i] = -1; 
    }
    
    /* set select maxfd */
    maxfd =  listenfd;
    /* set connect count */
    conn_cout = 0;
    
    /* set sin_size */
    sin_size = sizeof(client_addr); 
    
    while(1) 
    {
        /* init file describle */
        FD_ZERO(&rset);
        FD_SET(listenfd, &rset);
        
        /* set timeout */
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        
        /* add active connection to client[] */
        for(i = 0; i < BACKLOG; i++) { 
          if(client[i]) 
            FD_SET(client[i], &rset); 
        }
        
        /*  select */
        ret = select(maxfd +1, &rset, NULL, NULL, NULL);
        if(ret < 0) { 
          MY_DEBUG("%s,%d: select error\n\r",__func__,__LINE__);
          goto EXIT; 
        }else if(!ret){ 
          MY_DEBUG("%s,%d: select...\n\r",__func__,__LINE__); 
          continue; 
        }
        
        /* check every fd in client[] by set */
//        for(i = 0; i < conn_cout; i++) {
        for(i = 0; i < BACKLOG; i++) {
          /* if client[i] < 0 ,don't check */
          if(client[i] >= 0) {
              /* check client[]*/
              if(FD_ISSET(client[i], &rset)) { 
                ret = do_recv(client[i]); 
                if(ret < 0) { 
                  MY_DEBUG("%s,%d: do recv faild !\n\r",__func__,__LINE__); 
                  lwip_close(client[i]); 
                  FD_CLR(client[i], &rset);
//                  client[i] = 0; 
                  client[i] = -1; 
                  conn_cout--;          //TODO...
                }
              }
          }  
           
        } /* for()  check client[] */
        
        /* check listen fd by set or not */
        if(FD_ISSET(listenfd, &rset)) {
            confd = accept(listenfd, (struct sockaddr *)&client_addr, &sin_size);   /* accept */
            if(confd < 0)  { 
              MY_DEBUG("%s,%d: No new connect..\n\r",__func__,__LINE__);
              continue; 
            }
            /* add new connection to client[]  */
            if(conn_cout < BACKLOG) { 
              MY_DEBUG("%s,%d: add new connection to client[]..\n\r",__func__,__LINE__);
             /* set sockfd*/      
              setsockopt(confd, SOL_SOCKET, SO_REUSEADDR,(const char*)&sock_reuse, sizeof(sock_reuse));
              setsockopt(confd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive , sizeof(keepalive ));
//TODO...
              for(i = 0; i < BACKLOG; i++) {
                if(client[i] < 0) {
                   client[i] = confd;
                   break ;
                }
              }
              conn_cout++;
//              client[conn_cout++] = confd;
              /* replace select maxfd by confd */
              if(confd > maxfd)   
                maxfd = confd;              
            }else { 
//              MY_DEBUG("%s,%d: more user connect.. server will rester..\n\r",__func__,__LINE__); 
                MY_DEBUG("%s,%d: more user connect.. zero all connect..\n\r",__func__,__LINE__); 

                    for(i = 0; i < BACKLOG; i++) { 
                      if(client[i] >= 0) 
                        lwip_close(client[i]); 
                     // goto EXIT;
                   }
                   conn_cout = 0;
                   continue ;
//              lwip_close(confd);
//              break;
            }

        }
    }
    /* close all connection and quit select */
//    for(i = 0; i < BACKLOG; i++) { 
//      if(client[i] != 0) 
//        lwip_close(client[i]); 
//      goto EXIT;
//    }
    
    return ym_ok;
    
EXIT:
    return ym_err;
}



/* initation tcp socket */
static int init_socket(void)
{
  int err = -1, sock_reuse = 1;
  int sockfd;
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
  
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) {
    MY_DEBUG("%s,%d: create sockfd faild !\n\r",__func__,__LINE__);
    goto EXIT;
  }
  
  /*setsockopt*/
  err = setsockopt(sockfd, SOL_SOCKET, 
                   SO_REUSEADDR, 
                   (const char*)&sock_reuse, sizeof(sock_reuse));
  if(err < 0) {
    MY_DEBUG("%s,%d: setsockopt faild!\n\r",__func__,__LINE__);
    goto EXIT;
  }
  
    /*bind*/
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(TCP_PORT); 
   server_addr.sin_addr.s_addr = INADDR_ANY;
   rt_memset(&(server_addr.sin_zero),8, sizeof(server_addr.sin_zero));

   if( lwip_bind(sockfd, (struct sockaddr *)&server_addr, 
                 sizeof(struct sockaddr)) < 0) {
                   MY_DEBUG("%s, %d: bind faild !\n\r",__func__,__LINE__);
                   goto EXIT;
                 }
   else MY_DEBUG("%s:%d: sockfd init success !\n\r",__func__,__LINE__);
    return sockfd;

EXIT:
  if(sockfd >= 0)  
    lwip_close(sockfd); 
  return ym_err;
}


void tcp_server(void)
{
  int err = -1, sock_reuse = 1;
  int tcp_sock = -1 , do_sock = -1;
  int recv_bytes = -1;
  JSON js;
  rt_uint32_t addr_len;
  
  char tcp_recv_buff[LEN];
  
  struct sockaddr_in client_addr; 
  
  MY_DEBUG("Now in  %s, %d\n\r",__func__,__LINE__);
  
   tcp_sock = init_socket();
   if(tcp_sock < 0) {
     MY_DEBUG("%s,%d: init sockfd faild !\n\r",__func__,__LINE__);
     return ;
   }
   if(lwip_listen(tcp_sock, 5) < 0) {
     MY_DEBUG("%s, %d: listen faild!\n\r",__func__,__LINE__);
     goto EXIT;
   }
  /*accept*/
   if(Accept(tcp_sock) < 0) {
        MY_DEBUG("%s,%d: tcp server quit..\n\r",__func__,__LINE__);
        goto EXIT;
   }
    
   
EXIT:
  lwip_close(tcp_sock);
  tcp_sock = -1;
  return ;
}

