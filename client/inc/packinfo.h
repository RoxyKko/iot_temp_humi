/*** 
 * @Author: RoxyKko
 * @Date: 2023-04-05 22:32:51
 * @LastEditors: RoxyKko
 * @LastEditTime: 2023-04-05 22:34:21
 * @Description: 传输数据结构体
 */

#ifndef __PACKINFO_H__
#define __PACKINFO_H__

#define DEVID_LEN   16
#define TIME_LEN    32


typedef struct packinfo_st
{
    char devid[DEVID_LEN];
    char time[TIME_LEN];
    float temp;
    float humi;
} packinfo_t;

#endif