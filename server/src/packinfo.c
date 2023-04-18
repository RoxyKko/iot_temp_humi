/*
 * @Author: RoxyKko
 * @Date: 2023-04-11 17:14:15
 * @LastEditors: RoxyKko
 * @LastEditTime: 2023-04-11 21:05:42
 * @Description: 服务器接收数据包
 */

#include "packinfo.h"

/**
 * @name: int data_segmentation(char *buf, packinfo_t *pack_info)
 * @description: 将数据从接收缓冲区中提取至结构体中
 * @param {char} *buf 接收缓冲区
 * @param {packinfo_t} *pack_info 数据结构体
 * @return {*} 0为正常执行，非0则出现错误
 */
int data_segmentation(char *buf, packinfo_t *pack_info)
{
    char        *buf_ptr[4];
    char        *p = NULL;
    int          j = 0;

    if( !buf || !pack_info )
    {
        log_error("The data_segmentation() argument incorrect!\n");
        return -1;
    }

    log_debug("Read data from client:%s\n", buf);

    p = strtok(buf, "/");
    while(p)
    {
        buf_ptr[j++] = p;
        p = strtok(NULL, "/");
    }

    strcpy(pack_info->devid, buf_ptr[0]);
    strcpy(pack_info->time, buf_ptr[1]);
    pack_info->temp = atof(buf_ptr[2]);
    pack_info->humi = atof(buf_ptr[3]);

    return 0;
}
