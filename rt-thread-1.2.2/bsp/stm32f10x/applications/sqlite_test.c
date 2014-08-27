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
    int result = -1;
    char *errmsg = NULL;
    

    result = sqlite3_open(db_name, &db);
    MY_DEBUG("%s, %d:  ret = %d\n\r",__func__,__LINE__,result);
    if(result != SQLITE_OK) {
      MY_DEBUG("%s, %d: open db file %s faild : %s ...\n\r",__func__,__LINE__, db_name,sqlite3_errmsg(db));
      return ;
    }else {
      MY_DEBUG("%s, %d: open db file %s success ...\n\r",__func__,__LINE__, db_name);
    }

    result = sqlite3_exec(db, "create table tb(ID integer primary key autoincrement, name nvarchar(32))", NULL, NULL, NULL);
    if(result != SQLITE_OK) {
      MY_DEBUG("%s, %d: create table faild: %d, %s\n\r",__func__,__LINE__, result,sqlite3_errmsg(db));
      MY_DEBUG("flow memory use : \n\r");
      list_mem();
      sqlite3_close( db );
      return ;
    }else {
      MY_DEBUG("%s, %d: create success !\n\r",__func__,__LINE__);
    }
    sqlite3_close( db );
    return ;
}