#include <rtthread.h>
#include <string.h>
#include <dfs_posix.h>

#include "dev_opt.h"
rt_int8_t eof = -1;
rt_uint8_t add_dev(dev_t *dev)
{
    rt_uint8_t fd;
    rt_uint8_t i;

    struct stat buf;
    memset(&buf,0,sizeof(buf));
    dev_t *tmpbuff = RT_NULL;
    if((fd=open(DEV,DFS_O_CREAT|DFS_O_RDWR,0))<0)
    {
        rt_kprintf("%s:%d Open %s failed\n",__func__,__LINE__,DEV);
        return 0;
    }
    stat(DEV, &buf);
    if(buf.st_size>0)
    {
        tmpbuff = (dev_t*)rt_malloc(buf.st_size);
        if(0==tmpbuff)
        {
            rt_kprintf("%s:%s Memory allocation failure\n",__func__,__LINE__);
            close(fd);
            return 0;
        }
        if(read(fd,tmpbuff,buf.st_size)<buf.st_size)
        {
            rt_kprintf("%s:%d Read %s error\n",__func__,__LINE__,DEV);
            rt_free(tmpbuff);
            close(fd);
            return 0;
        }
        for(i=0;i<(buf.st_size/sizeof(dev_t));i++)
        {
            if(rt_memcmp((tmpbuff+i)->mac,dev->mac,MAC_LEN)==0)
            {
                rt_kprintf("%s:%d Device existed\n",__func__,__LINE__);
                rt_free(tmpbuff);
                close(fd);
                return 0;
            }
        }
    }
    if(buf.st_size>0)
    {
        lseek(fd,0,SEEK_END);
    }
    
    if(write(fd,dev,sizeof(dev_t))<sizeof(dev_t))
    {
        rt_kprintf("%s:%d Write %s error\n",__func__,__LINE__,DEV);
        if(!tmpbuff)
            rt_free(tmpbuff);
        close(fd);
        return 0;
    }
    if(!tmpbuff)
        rt_free(tmpbuff);
    close(fd);
    return 1;
}

rt_uint8_t del_dev(dev_t *dev)
{
    rt_uint8_t fd;
    rt_uint8_t i;
    struct stat buf;
    dev_t *tmpbuff;
    dev_t tmp;
    rt_memset(&tmp,0,sizeof(dev_t));
    rt_memset(&buf,0,sizeof(buf));
    if((fd = open(DEV, DFS_O_CREAT|DFS_O_RDWR,0)<0))
    {
        rt_kprintf("%s:%d Open %s failed\n",__func__,__LINE__,DEV);
        return 0;
    }
    stat(DEV, &buf);
    if(buf.st_size>0)
    {
        tmpbuff = (dev_t*)rt_malloc(buf.st_size);
        rt_memset(tmpbuff,0,buf.st_size);
        if(0==tmpbuff)
        {
            rt_kprintf("%s:%d Memory allocation failure\n",__func__,__LINE__);
            close(fd);
            return 0;
        }
        if(read(fd,tmpbuff,buf.st_size)<buf.st_size)
        {
            rt_kprintf("%s:%d Read %s error\n",__func__,__LINE__,DEV);
            rt_free(tmpbuff);
            close(fd);
            return 0;
        }
        
        close(fd);
                
        if((fd = open(DEV, DFS_O_CREAT|DFS_O_RDWR|DFS_O_TRUNC,0)<0))
        {
            rt_kprintf("%s:%d Open %s failed\n",__func__,__LINE__,DEV);
            return 0;
        }
        
        for(i=0;i<(buf.st_size/sizeof(dev_t));i++)
        {
            if(rt_memcmp((tmpbuff+i)->mac,dev->mac,MAC_LEN)==0)
            {
                continue;
            }
            write(fd,tmpbuff+i,sizeof(dev_t));
        }
        
        rt_free(tmpbuff);
        close(fd);
        return 1;
    }
    close(fd);
    return 0;
}



rt_uint8_t exist_dev(dev_t *dev)
{
    rt_uint8_t fd;
    rt_uint8_t i;
    struct stat buf;
    dev_t *tmpbuff;
    if((fd = open(DEV, DFS_O_CREAT|DFS_O_RDWR,0)<0))
    {
        rt_kprintf("%s:%d Open %s failed\n",__func__,__LINE__,DEV);
        return 0;
    }
    stat(DEV, &buf);
    if(buf.st_size>0)
    {
        tmpbuff = (dev_t*)rt_malloc(buf.st_size);
        if(0==tmpbuff)
        {
            rt_kprintf("%s:%d Memory allocation failure\n",__func__,__LINE__);
            close(fd);
            return 0;
        }
        if(read(fd,tmpbuff,buf.st_size)<buf.st_size)
        {
            rt_kprintf("%s:%d Read %s error\n",__func__,__LINE__,DEV);
            rt_free(tmpbuff);
            close(fd);
            return 0;
        }
        for(i=0;i<(buf.st_size/sizeof(dev_t));i++)
        {
            if(rt_memcmp((tmpbuff+i)->mac,dev->mac,MAC_LEN)==0)
            {
                rt_kprintf("%s:%d device existed\n",__func__,__LINE__);
                dev->stat = (tmpbuff+i)->stat;
                rt_free(tmpbuff);
                close(fd);
                return 1;
            }
        }
    }
    close(fd);
    return 0;
}

rt_uint8_t change_dev(dev_t *dev)
{
    rt_uint8_t fd;
    rt_uint8_t i;
    struct stat buf;
    dev_t *tmpbuff;
    if((fd = open(DEV, O_RDWR|O_CREAT,0)<0))
    {
        rt_kprintf("%s:%d Open %s failed\n",__func__,__LINE__,DEV);
        return 0;
    }
    stat(DEV, &buf);
    if(buf.st_size>0)
    {
        tmpbuff = (dev_t*)rt_malloc(buf.st_size);
        if(0==tmpbuff)
        {
            rt_kprintf("%s:%d Memory allocation failure\n",__func__,__LINE__);
            close(fd);
            return 0;
        }
        if(read(fd,tmpbuff,buf.st_size)<buf.st_size)
        {
            rt_kprintf("%s:%d Read %s error\n",__func__,__LINE__,DEV);
            rt_free(tmpbuff);
            close(fd);
            return 0;
        }
        for(i=0;i<(buf.st_size/sizeof(dev_t));i++)
        {
            if(rt_memcmp((tmpbuff+i)->mac,dev->mac,MAC_LEN)==0 || rt_memcmp((tmpbuff+i)->shortadd,dev->shortadd,SHORT_LEN)==0)
            {
                (tmpbuff+i)->stat = dev->stat;
                
                lseek(fd,0,DFS_SEEK_SET);
                write(fd,tmpbuff,buf.st_size);
                
                close(fd);
                return 1;
            }
        }
    }
    
    return 0;
}

rt_uint8_t change_shortadd_dev(dev_t *dev)
{
    rt_uint8_t fd;
    rt_uint8_t i;
    struct stat buf;
    dev_t *tmpbuff;
    if((fd = open(DEV, O_RDWR|O_CREAT,0)<0))
    {
        rt_kprintf("%s:%d Open %s failed\n",__func__,__LINE__,DEV);
        return 0;
    }
    stat(DEV, &buf);
    if(buf.st_size>0)
    {
        tmpbuff = (dev_t*)rt_malloc(buf.st_size);
        if(0==tmpbuff)
        {
            rt_kprintf("%s:%d Memory allocation failure\n",__func__,__LINE__);
            close(fd);
            return 0;
        }
        if(read(fd,tmpbuff,buf.st_size)<buf.st_size)
        {
            rt_kprintf("%s:%d Read %s error\n",__func__,__LINE__,DEV);
            rt_free(tmpbuff);
            close(fd);
            return 0;
        }
        for(i=0;i<(buf.st_size/sizeof(dev_t));i++)
        {
            if(rt_memcmp((tmpbuff+i)->mac,dev->mac,MAC_LEN)==0)
            {
                rt_strcpy((tmpbuff+i)->shortadd,dev->shortadd,SHORT_LEN);
                lseek(fd,0,DFS_SEEK_SET);
                write(fd,tmpbuff,buf.st_size);
                
                close(fd);
                return 1;
            }
        }
    }
    
    return 0;
}



rt_uint8_t dev_list(void **dev,rt_uint8_t *cnt)
{
    rt_uint8_t fd;
    rt_uint8_t i;
    struct stat buf;
    dev_t *tmpbuff;
    if((fd = open(DEV, DFS_O_CREAT|DFS_O_RDWR,0)<0))
    {
        rt_kprintf("%s:%d Open %s failed\n",__func__,__LINE__,DEV);
        return 0;
    }
    stat(DEV, &buf);
    if(buf.st_size>0)
    {
        tmpbuff = (dev_t*)rt_malloc(buf.st_size);
        if(0==tmpbuff)
        {
            rt_kprintf("%s:%d Memory allocation failure\n",__func__,__LINE__);
            close(fd);
            return 0;
        }
        if(read(fd,tmpbuff,buf.st_size)<buf.st_size)
        {
            rt_kprintf("%s:%d Read %s error\n",__func__,__LINE__,DEV);
            close(fd);
            return 0;
        }
        *cnt = buf.st_size/sizeof(dev_t);
        *dev = tmpbuff;
        close(fd);
        return 1;
    }
    *cnt = 0;
    *dev = 0;
    close(fd);
    return 0;
}

