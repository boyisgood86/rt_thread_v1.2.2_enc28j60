#ifndef __YM_CONFIG_H__
#define __YM_CONFIG_H__


/* tcp server port */
#define TCP_PORT   (4977)
/* udp server port */
#define UDP_PORT   (4937)

/* define support device count */
#define DEV_COUNT   (5)

/* -1, 0, 1 */
#define  YM_OK      (0)
#define  YM_ERR     (-1)
/* tcp json struct */

typedef struct cjson
{
    char flag;          /* check this struct write or not, write is 1, othr is 0 */
    char name[16];      /* user name */
    char passwd[16];    /* user passwd */
    char mobileid[16];  /* user mobileid */
//    char isAuthority;   /* enable or disable whitelist, 0 is off and 1 is on */
    int method;         /* operation cmd */
    int status;         /* device status */
    
}JSON;

typedef struct ym_device
{
    char devcice_progress;   /* device statu */
    char device_mac[8];    /*mac string 00:12:4B:00:04:B1:98:C1 */
}YM_DEVICE;

/* login */
typedef struct clogin
{
  JSON js;  
}CLOGIN;

/* sing */
typedef struct csign_in
{
    JSON js;  
}CSIGN_IN;

/* change user passwd */
typedef struct passwd
{
    JSON js;
    char new_passwd[16];
}Passwd_t;

/* rm user */
typedef struct deltuser
{
    JSON js;
}deltuser_t;

/* enable or disable whitelist */
typedef struct WhiteList
{
     JSON js;
     char isAuthority; /* enable or disable whitelist, 0 is off and 1 is on */
}WhiteList;

/* device control*/
typedef struct ctr_device
{
  JSON js;
/*   device id   */
  /* device status */

}CTR_DEV;

/* read device status */
typedef struct read_device
{
    int count;
    int op;
    JSON js;
    YM_DEVICE dev[DEV_COUNT];
    
}ReadDev;

/* packing json string */
//typedef struct jstring{
//	char body[5];       /* json string body, value is "body" */
//	char list[5];       /* "list" */
//	char user[5];       /* "user" */
//	char head[5];      /* json string head, value is "head" */
//	char other[6];     /* "other" */
//	JSON *js;
//}JSTRING;

/* method code */
enum method {
  login         = 1000,     /* method is login */
  ctr_device       = 1002,     /* ctrol LED */
  sign_in       = 1004,     /* user sign in */ 
  re_device     = 1007,     /* read device status */
  search        = 10001,    /* udp  broadcast ip and port*/  
  ch_passwd     = 10004,    /* change passwd */
  off_whitelist = 10005,    /* enable or disable whitelist */
  rm_user       = 10007,    /* delete user */
  add_device    = 11001,    /* add device*/
  rm_device     = 11002,    /* rm device */
  
 };

/* return value */
enum err{
  ym_err  = -1,
  ym_ok   =  0,
  ym_en   =  1,
};
/* TODO....*/


#endif /* __YM_CONFIG_H__ */
