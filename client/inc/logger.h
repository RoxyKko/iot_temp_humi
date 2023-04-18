/*** 
 * @Author: RoxyKko
 * @Date: 2023-04-04 17:44:09
 * @LastEditors: RoxyKko
 * @LastEditTime: 2023-04-04 18:34:36
 * @Description: 日志系统
 */

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "logger.h"

/*** 
 * @name: LOG_LEVEL
 * @description: 日志等级枚举
 */
enum LOG_LEVEL
{
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
    LOG_LEVEL_MAX
};

/*** 
 * @name: logger
 * @description: 日志记录器结构体
 */
typedef struct logger
{
    FILE *fp;
    int loglevel;
    int use_stdout;
} logger_t;

/*** 
 * @description: 不同日志记录级别的记录器前缀字符串
 */
#define     LOG_STRING_DEBUG    "DEBUG"
#define     LOG_STRING_INFO     "INFO"
#define     LOG_STRING_WARN     "WARN"
#define     LOG_STRING_ERROR    "ERROR"
#define     LOG_STRING_FATAL    "FATAL"


/*** 
 * @name: int logger_init(char *filename, int loglevel);
 * @description: 日志系统初始化
 * @param {char} *filename  日志文件名
 * @param {int} loglevel    日志等级
 * @return {int}  0:成功 -1:失败
 */
int logger_init(char *filename, int loglevel);



/* 
 * @description: 根据日志等级，输出日志信息
 * @param {char} *fmt  格式化字符串
 * @param {va_list} ap       可变参数列表
 * @return {void}
 */ 
void log_error(char *fmt, ...);
void log_fatal(char *fmt, ...);
void log_warn(char *fmt, ...);
void log_info(char *fmt, ...);
void log_debug(char *fmt, ...);


# endif
