/*
 * @Author: RoxyKko
 * @Date: 2023-04-04 17:04:15
 * @LastEditors: RoxyKko
 * @LastEditTime: 2023-04-04 17:26:45
 * @Description: 获取时间
 */

#include <get_time.h>

#define dateFormat "%04d-%02d-%02d %02d:%02d:%02d"

double get_time(char *datime)
{
    struct          timeval       tv;
    struct          tm           *st;
    static double   last_time  =  0;

    // 获取时间
    gettimeofday(&tv, NULL);
    // 报错获取的秒数
    last_time = tv.tv_sec;
    // 将获取到的秒数转换为本地时间
    st        = localtime(&tv.tv_sec);

    if( datime != NULL )
    {
        memset(datime, 0, sizeof(datime));
        sprintf(datime, dateFormat, st->tm_year + 1900, st->tm_mon + 1, st->tm_mday, st->tm_hour, st->tm_min, st->tm_sec);
    }

    // 返回时间
    return last_time;

}