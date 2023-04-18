/*
 * @Author: RoxyKko
 * @Date: 2023-04-04 17:53:48
 * @LastEditors: RoxyKko
 * @LastEditTime: 2023-04-11 16:40:43
 * @Description: 日志系统
 */

#include "logger.h"

// 获取程序名
extern const char* __progname;
#define PROGRAM_NAME __progname

static const char* _LOG_LEVELS[] = { 
    LOG_STRING_DEBUG,
    LOG_STRING_INFO,
    LOG_STRING_WARN,
    LOG_STRING_ERROR,
    LOG_STRING_FATAL
};


static struct logger g_logger;

/*** 
 * @name: int logger_init(char *filename, int loglevel);
 * @description: 日志系统初始化
 * @param {char} *filename  日志文件名
 * @param {int} loglevel    日志等级
 * @return {int}  0:成功 -1:失败
 */
int logger_init(char *filename, int loglevel)
{
    // 若日志等级大于最大日志等级，则将日志等级设置为最大日志等级，否则传参给结构体
    g_logger.loglevel = loglevel > LOG_LEVEL_MAX ? LOG_LEVEL_MAX : loglevel;

    // 若日志文件名为空或者为stdout，则使用标准输出，否则打开日志文件
    if( !filename || !strcasecmp(filename, "stdout") )
    {
        g_logger.use_stdout = 1;
        g_logger.fp         = stderr;
    }
    else
    {
        g_logger.use_stdout = 0;
        g_logger.fp         = fopen(filename, "a+");
        if( !g_logger.fp )
        {
            g_logger.fp         = fopen(filename, "w+");
            if( !g_logger.fp )
            {
                fprintf(stderr, "open log file %s failed: %s\n", filename, strerror(errno));
                return -1;
            }
        }
    }

    return 0;
}

/**
 * @name: 
 * @description: log_generic(const int level, const char *format, va_list args)
 * @param {int} level       日志级别，表示该日志消息的重要程度
 * @param {char} *format    日志消息的格式字符串，可以包含可变参数占位符
 * @param {va_list} args    可变参数列表，包含实际的日志消息参数
 * @return {*}
 */
static void log_generic(const int level, const char *format, va_list args)
{
    char        buf[256];
    struct tm  *tm;
    time_t      time_now;

    vsprintf(buf, format, args);
    time(&time_now);
    tm = localtime(&time_now);

    int res = fprintf(g_logger.fp, 
    "%s : %02d-%02d-%02d %02d:%02d:%02d [%s]: %s\n"
    , PROGRAM_NAME
    , tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday
    , tm->tm_hour, tm->tm_min, tm->tm_sec
    , _LOG_LEVELS[level]
    , buf
    );

    fflush(g_logger.fp);
}

/* 
 * @description: 根据日志等级，输出日志信息
 * @param {char} *fmt  格式化字符串
 * @param {va_list} ap       可变参数列表
 * @return {void}
 */ 
void log_error(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log_generic(LOG_LEVEL_ERROR, fmt, args);
    va_end(args);
}

void log_warn(char *fmt, ...)
{
    if( g_logger.loglevel > LOG_LEVEL_WARN )
        return;
    
    va_list args;
    va_start(args, fmt);
    log_generic(LOG_LEVEL_WARN, fmt, args);
    va_end(args);
}

void log_info(char *fmt, ...)
{
    if( g_logger.loglevel > LOG_LEVEL_INFO )
        return;
    
    va_list args;
    va_start(args, fmt);
    log_generic(LOG_LEVEL_INFO, fmt, args);
    va_end(args);
}

void log_debug(char *fmt, ...)
{
    if( g_logger.loglevel > LOG_LEVEL_DEBUG )
        return;
    
    va_list args;
    va_start(args, fmt);
    log_generic(LOG_LEVEL_DEBUG, fmt, args);
    va_end(args);
}

void log_fatal(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log_generic(LOG_LEVEL_FATAL, fmt, args);
    va_end(args);
}
