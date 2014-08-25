#include <stdio.h>
#include <rtthread.h>
#include <lwip/netdb.h> 
#include <sqlite3.h>


char *db_name = "/test.db";
char *sql = "create table MyTable_1( ID integer primary key autoincrement, name nvarchar(32) )";

//sqlite callback function
static int callback(void *NotUsed, int argc, char **argv, char **azColName){
//  int i;
//  MY_DEBUG("%s, %d: Now in here..\n\r",__func__,__LINE__);
//  for(i=0; i<argc; i++){
//    MY_DEBUG("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
//  }
//  MY_DEBUG("\n");
  MY_DEBUG("%s, %d: Hello world!\n\r",__func__,__LINE__);
  return 0;
}


void test_db(void)
{
    sqlite3 *db;
    int ret = -1;
    char *zErrMsg = 0;
    

    ret = sqlite3_open(db_name, &db);
    MY_DEBUG("%s, %d:  ret = %d\n\r",__func__,__LINE__,ret);
    if(ret != SQLITE_OK) {
      MY_DEBUG("%s, %d: open db file %s faild : %s ...\n\r",__func__,__LINE__, db_name,sqlite3_errmsg(db));
      return ;
    }else {
      MY_DEBUG("%s, %d: open db file %s success ...\n\r",__func__,__LINE__, db_name);
    }
    ret = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if(ret != SQLITE_OK) {
      MY_DEBUG("%s, %d: exec error: %s..\n\r",__func__,__LINE__,sqlite3_errmsg(db));
      sqlite3_free(zErrMsg);
      sqlite3_close(db);
      return ;
    }else {
      MY_DEBUG("%s, %d: exec success..\n\r",__func__,__LINE__);
    }
     sqlite3_free(zErrMsg);
    sqlite3_close(db);
    return ;
}