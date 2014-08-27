#include <board.h>
#include <rtthread.h>
#include "filesystem.h"
#include "cJSON.h"

#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif  /* RT_USING_COMPONENTS_INIT */


char *file_name = "/test.txt";


void file_op(void)
{
 //   char *out;
    char text[] = "{\"timestamp\":\"2013-11-19T08:50:11\",\"value\":1}";
    cJSON *json, *json_value, *json_timestamp ;
    
    MY_DEBUG("%s:%d: <------------>\n\r",__func__,__LINE__);
    list_mem();
    json=cJSON_Parse(text);
    if(!json) {
      MY_DEBUG("%s:%d:  Error before: [%s]\n\r",__func__,__LINE__,cJSON_GetErrorPtr());
    }else {
//        out=cJSON_Print(json);
        
        json_timestamp = cJSON_GetObjectItem( json , "timestamp");
        if( json_timestamp->type == cJSON_String ) {
          MY_DEBUG("%s, %d: timestamp is %s\n\r",__func__,__LINE__,json_timestamp->valuestring);
        }
        
        json_value = cJSON_GetObjectItem(json, "value");
        if(json_value->type == cJSON_Number) {
          MY_DEBUG("%s, %d: value is %d\n\r",__func__,__LINE__, json_value->valueint);
        }
        cJSON_Delete(json);
 //       MY_DEBUG("%s\n",out);
//        free(out);
    }
    MY_DEBUG("%s:%d <<----------------->>\n\r",__func__,__LINE__);
    return ;
}



//
///*  test file system */
//void file_op(void)
//{
//  int fd = -1;
//  MY_DEBUG("%s, %d: Now test file system ..\n\r",__func__,__LINE__);
//  
//   fd = open(file_name,DFS_O_CREAT|DFS_O_RDWR,0);
//   if(fd >= 0) {
//     MY_DEBUG("%s, %d: open fils text.txt success !\n\r",__func__,__LINE__);
//   }else {
//     MY_DEBUG("%s, %d: open file text.txt faild !\n\r",__func__,__LINE__);
//     return ;
//   }
//   
//   int count = write(fd,"Hello , my name is jason!",32);
//   if(count < 0) {
//     MY_DEBUG("%s, %d: write faild !\n\r",__func__,__LINE__);
//     close(fd);
//     return ;
//   }
//  
//   close(fd);
//   
//   {
//        char buff[256];
//        rt_memset(buff, 0, sizeof(buff));
//        fd = open(file_name, DFS_O_RDWR, 0);
//        if(fd < 0) {
//          MY_DEBUG("%s, %d: open file faild..\n\r",__func__,__LINE__);
//          return ;
//        }
//        
//        if(read(fd, buff, 32)) {
//          MY_DEBUG("%s, %d: read data from flash : %s\n\r",__func__,__LINE__,buff);
//          close(fd);
//          return ;
//        }else {
//          MY_DEBUG("%s, %d: read data faild !\n\r",__func__,__LINE__);
//          close(fd);
//          return ;
//        }
//   }
//}
