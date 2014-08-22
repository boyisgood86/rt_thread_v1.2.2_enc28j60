#include <stdio.h>
#include <rtthread.h>
#include <lwip/netdb.h> 
#include <sqlite3.h>


char *db_name = "/test.db";



void test_db(void)
{
    sqlite3 *db;
    int ret = -1;

    ret = sqlite3_open(db_name, &db);
    MY_DEBUG("%s, %d:  ret = %d\n\r",__func__,__LINE__,ret);
    if(ret) {
        MY_DEBUG("%s, %d: close sqlite..\n\r",__func__,__LINE__);
        sqlite3_close(db);
    }else {
      MY_DEBUG("%s, %d: open or cread db \"%s\" faild..\n\r",__func__,__LINE__,db_name);
    }
    return ;
}