#include <rtthread.h>
#include <string.h>
#include <dfs_posix.h>
#include "white_list.h"

rt_uint8_t   white_list_enable = ENABLE;

rt_uint8_t white_list(void **user,rt_uint8_t *cnt)
{
    rt_uint8_t fd;
    rt_uint8_t i;
    struct stat buf;
    rt_memset(&buf,0,sizeof(buf));
    white_list_t *tmpbuff;
    if((fd = open(WHITE_LIST,DFS_O_CREAT|DFS_O_RDWR,0)<0))
    {
        rt_kprintf("Open white list failed\n");
        return FAIL;
    }
    stat(WHITE_LIST, &buf);
    if(buf.st_size>0)
    {
        tmpbuff = (white_list_t*)rt_malloc(buf.st_size);
        if(0==tmpbuff)
        {
            rt_kprintf("Memory allocation failure\n");
            close(fd);
            return FAIL;
        }
        if(read(fd,tmpbuff,buf.st_size)<buf.st_size)
        {
            rt_kprintf("Read white list error\n");
            close(fd);
            return FAIL;
        }
        *cnt = buf.st_size/sizeof(white_list_t);
        *user = tmpbuff;
        close(fd);
        return SUCCESS;
    }
    *cnt = 0;
    *user = 0;
    close(fd);
    return FAIL;
}

rt_int8_t enable_white_list(void)
{
    white_list_enable = ENABLE;
    int fd;
    if((fd = open(WHITE_LIST_ENABLE, DFS_O_CREAT|DFS_O_RDWR,0)<0))
    {
        rt_kprintf("Open file failed\n");
        return fd;
    }
    write(fd,&white_list_enable,sizeof(white_list_enable));
    close(fd);
    return is_enable_list();
}

rt_int8_t disable_white_list(void)
{
    int fd;
    white_list_enable = DISABLE;
    if((fd = open(WHITE_LIST_ENABLE, DFS_O_CREAT|DFS_O_RDWR,0)<0))
    {
        rt_kprintf("Open file failed\n");
        return fd;
    }
    write(fd,&white_list_enable,sizeof(white_list_enable));
    close(fd);
    return is_enable_list();
}

rt_int8_t is_enable_list(void)
{
    int fd;
    if((fd = open(WHITE_LIST_ENABLE, DFS_O_CREAT|DFS_O_RDWR|DFS_O_APPEND,0)<0))
    {
        rt_kprintf("Open file failed\n");
        return fd;
    }
    read(fd,&white_list_enable,sizeof(white_list_enable));
    close(fd);
    return white_list_enable;
}

rt_uint8_t user_exist(const JSON *user)
{
    rt_uint8_t fd;
    rt_uint8_t i;
    struct stat buf;
    rt_memset(&buf,0,sizeof(buf));
    
    white_list_t *tmpbuff;
    
    if((fd = open(WHITE_LIST, DFS_O_CREAT|DFS_O_RDWR|DFS_O_APPEND,0)<0))
    {
        rt_kprintf("Open white list failed\n");
        return FAIL;
    }
    stat(WHITE_LIST, &buf);
    if(buf.st_size>0)
    {
        tmpbuff = (white_list_t*)rt_malloc(buf.st_size);
        if(0==tmpbuff)
        {
            rt_kprintf("Memory allocation failure\n");
            close(fd);
            return FAIL;
        }
        if(read(fd,tmpbuff,buf.st_size)<buf.st_size)
        {
            rt_kprintf("Read white list error\n");
            rt_free(tmpbuff);
            close(fd);
            return FAIL;
        }
        for(i=0;i<(buf.st_size/sizeof(white_list_t));i++)
        {
            if(rt_strcmp((tmpbuff+i)->name,user->name)==0)
            {
                rt_kprintf("user existed\n");
                rt_free(tmpbuff);
                close(fd);
                return SUCCESS;
            }
        }
    }
    return FAIL;
}

rt_uint8_t add_white_list(white_list_t *user)
{
    rt_uint8_t fd;
    rt_uint8_t i;
    int cnt = 0;
    struct stat buf;
    rt_memset(&buf,0,sizeof(buf));
    white_list_t *tmpbuff = RT_NULL;
    
    if((fd = open(WHITE_LIST, DFS_O_CREAT|DFS_O_RDWR,0)<0))
    {
        rt_kprintf("Open %s failed\n",WHITE_LIST);
        return FAIL;
    }
    stat(WHITE_LIST, &buf);
    if(buf.st_size>0)
    {
        tmpbuff = (white_list_t*)rt_malloc(buf.st_size);
        rt_memset(tmpbuff,0,buf.st_size);
        if(0==tmpbuff)
        {
            rt_kprintf("Memory allocation failure\n");
            close(fd);
            return FAIL;
        }
        if((cnt=read(fd,tmpbuff,buf.st_size))<buf.st_size)
        {
            rt_kprintf("Read %s error\n",WHITE_LIST);
            rt_free(tmpbuff);
            close(fd);
            return FAIL;
        }
        for(i=0;i<(buf.st_size/sizeof(white_list_t));i++)
        {
            if((rt_strncmp((tmpbuff+i)->name,user->name,16))==0)
            {
                rt_kprintf("user existed\n");
                rt_free(tmpbuff);
                close(fd);
                return EXIST;
            }
        }
    }
    if(buf.st_size>0)
    {
        lseek(fd,0,SEEK_END);
    }
    if(write(fd,user,sizeof(white_list_t))<sizeof(white_list_t))
    {
        rt_kprintf("Write %s error\n",WHITE_LIST);
        rt_free(tmpbuff);
        close(fd);
        return FAIL;
    }
    close(fd);
    return SUCCESS;
}

rt_uint8_t del_white_list(white_list_t *user)
{
    rt_uint8_t fd;
    rt_uint8_t i;
    struct stat buf;
    rt_memset(&buf,0,sizeof(buf));
    
    white_list_t *tmpbuff;
    white_list_t tmp;
    memset(&tmp,0,sizeof(white_list_t));
    if((fd = open(WHITE_LIST, DFS_O_CREAT|DFS_O_RDWR,0)<0))
    {
        rt_kprintf("Open white list failed\n");
        return FAIL;
    }
    stat(WHITE_LIST, &buf);
    if(buf.st_size>0)
    {
        tmpbuff = (white_list_t*)rt_malloc(buf.st_size);
        if(0==tmpbuff)
        {
            rt_kprintf("Memory allocation failure\n");
            close(fd);
            return FAIL;
        }
        if(read(fd,tmpbuff,buf.st_size)<buf.st_size)
        {
            rt_kprintf("Read white list error\n");
            rt_free(tmpbuff);
            close(fd);
            return FAIL;
        }
        close(fd);
                
        if((fd = open(WHITE_LIST, DFS_O_CREAT|DFS_O_RDWR|DFS_O_TRUNC,0)<0))
        {
            rt_kprintf("%s:%d Open %s failed\n",__func__,__LINE__,WHITE_LIST);
            return FAIL;
        }
        for(i=0;i<(buf.st_size/sizeof(white_list_t));i++)
        {
            if(rt_strcmp((tmpbuff+i)->name,user->name)==0)
            {
                continue;
            }
            write(fd,tmpbuff+i,sizeof(white_list_t));
        }
        rt_free(tmpbuff);
        close(fd);
        return SUCCESS;
    }
    return FAIL;
}

