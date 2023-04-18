/*
 * @Author: RoxyKko
 * @Date: 2023-04-05 20:54:52
 * @LastEditors: RoxyKko
 * @LastEditTime: 2023-04-10 23:57:16
 * @Description: 数据库sqlite的使用
 */

#include "database.h"

/**
 * @name: database_init(char *dbname)
 * @description: 数据库sqlite初始化
 * @param {char} *dbname database文件名
 * @param {sqlite3} *db 数据库指针
 * @return {int} 0为正常执行，非0则出现错误
 */
int database_init(char *dbname, sqlite3 **db)
{
    
    int     rv          = -1;
    char    dbname_buf[128] = {0};

    if ((dbname == NULL) || (db == NULL))
    {
        log_error("The sqlite_init() argument incorrect!\n");
        return -1;
    }

    memset(dbname_buf, 0, sizeof(dbname_buf));
    sprintf(dbname_buf, "%s.db", dbname);
    rv = sqlite3_open(dbname_buf, db); // 打开数据表文件
    if (rv)
    {
        log_error("Can't open database: %s\n", sqlite3_errmsg(*db));
        return -2;
    }
    else
    {
        log_info("Opened database successfully!\n");
        return 0;
    }
}

/**
 * @name: int database_close(char *dbname, sqlite3 *db)
 * @description: 关闭database文件
 * @param {char} *dbname database文件名
 * @param {sqlite3} *db 数据库指针
 * @return {int}
 */
int database_close(char *dbname, sqlite3 **db)
{
    int     rv          = -1;

    if ((dbname == NULL) || (db == NULL))
    {
        log_error("The sqlite_close() argument incorrect!\n");
        return -1;
    }

    rv = sqlite3_close(*db);
    while (rv == SQLITE_BUSY)
    {
        // sqlite3_next_stmt() 返回一个指向当前连接中的下一个预处理语句的指针。
        rv = SQLITE_OK;
        sqlite3_stmt *pStmt;
        pStmt = sqlite3_next_stmt(*db, 0);

        if (pStmt != NULL)
        {
            rv = sqlite3_finalize(pStmt);
            if (rv == SQLITE_OK)
            {
                rv = sqlite3_close(*db);
            }
        }
    }

    log_info("database_close: %s.db closed!\n", dbname);
    return 0;
}

/**
 * @name: database_create_table(char *dbname, sqlite3 *db)
 * @description: 创建数据库表
 * @param {char} *dbname database文件名
 * @param {sqlite3} *db  数据库指针
 * @return {*}
 */
int database_create_table(char *dbname, sqlite3 **db)
{
    char    sql[128]    = {0};
    int     rv          = -1;
    char   *zErrMsg     = 0;

    if ((dbname == NULL) || (db == NULL))
    {
        log_error("The sqlite_create_table() argument incorrect!\n");
        return -1;
    }

    memset(sql, 0, sizeof(sql));
    sprintf(sql, "CREATE TABLE if not exists %s(SN CHAR(10),DATIME CHAR(50),TEMP CHAR(15), HUMI CHAR(15));",
            dbname);

    rv = sqlite3_exec(*db, sql, 0, 0, &zErrMsg);
    if (rv != SQLITE_OK)
    {
        log_error("Sqlite_create_table error:%s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -2;
    }

    log_info("database_create_table: %s.db created!\n", dbname);
    return 0;
}

/**
 * @name: database_insert_data(char *dbname, sqlite3 *db, packinfo_t pack_info)
 * @description: 向数据库中插入数据
 * @param {char} *dbname database文件名
 * @param {sqlite3} *db 数据库指针
 * @param {packinfo_t} pack_info    数据结构体
 * @return {int} 0为正常执行，非0则出现错误
 */
int database_insert_data(char *dbname, sqlite3 **db, packinfo_t *pack_info)
{
    char    sql[512]    = {0};
    int     rv          = -1;
    char   *zErrMsg     = 0;

    if ((dbname == NULL) || (db == NULL) || (pack_info == NULL))
    {
        log_error("The sqlite_insert_data() argument incorrect!\n");
        return -1;
    }

    memset(sql, 0, sizeof(sql));
    sprintf(sql, "INSERT INTO %s VALUES ('%s', '%s', '%f', '%f');",
            dbname, pack_info->devid, pack_info->time, pack_info->temp, pack_info->humi);
    log_debug("data insert sql: %s\n", sql);

    rv = sqlite3_exec(*db, sql, 0, 0, &zErrMsg);
    if (rv != SQLITE_OK)
    {
        log_error("Sqlite_insert_data error:%s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -2;
    }

    log_info("Last data insert table successfully: %s, %s, %f, %f\n",
             pack_info->devid, pack_info->time, pack_info->temp, pack_info->humi);
    return 0;
}

/**
 * @name: database_select_data(char *dbname, sqlite3 *db, packinfo_t pack_info)
 * @description: 选择数据库文件并返回第一条数据到pack_info
 * @param {char} *dbname database文件名
 * @param {sqlite3} *db 数据库指针
 * @param {packinfo_t} pack_info 数据结构体
 * @return {int} 返回剩余行数，负数则出现错误
 */
int database_select_data(char *dbname, sqlite3 **db, packinfo_t *pack_info)
{
    char    sql[128]    = {0};
    int     rv          = -1;
    char   *zErrMsg     = 0;  
    char  **dbResult;                   // 二维数组，存放结果
    int     nRow=0, nColumn=0;          // 行数和列数

    if((dbname == NULL) || (db == NULL) || (pack_info == NULL))
    {
        log_error("The sqlite_select_data() argument incorrect!\n");
        return -1;
    }

    memset(sql, 0, sizeof(sql));
    sprintf(sql, "SELECT * FROM %s LIMIT 1;", dbname);          // 选择第一条数据

    rv = sqlite3_get_table(*db, sql, &dbResult, &nRow, &nColumn, &zErrMsg);
    if (rv != SQLITE_OK)
    {
        log_error("Sqlite_select_data error:%s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -2;
    }

    memset(pack_info, 0, sizeof(packinfo_t));
    // 0-3 四个存第一行的标签，4-7四个存第二行的数据
    strcpy(pack_info->devid, dbResult[4]);
    strcpy(pack_info->time, dbResult[5]);
    pack_info->temp = atof(dbResult[6]);
    pack_info->humi = atof(dbResult[7]);
    log_info("Last data select table successfully: %s, %s, %f, %f\n",
             pack_info->devid, pack_info->time, pack_info->temp, pack_info->humi);

    return nRow;
}

/**
 * @name: database_delete_data(char *dbname, sqlite3 *db, packinfo_t pack_info)
 * @description:  删除数据库中第一条数据
 * @param {char} *dbname database文件名
 * @param {sqlite3} *db 数据库指针
 * @return {int } 0为正常执行，非0则出现错误
 */
int database_delete_data(char *dbname, sqlite3 **db)
{
    char    sql[128]    = {0};
    int     rv          = -1;
    char   *zErrMsg     = 0;


    if((dbname == NULL) || (db == NULL))
    {
        log_error("The sqlite_delete_data() argument incorrect!\n");
        return -1;
    }

    memset(sql, 0, sizeof(sql));
    sprintf(sql, "DELETE FROM %s WHERE rowid IN (SELECT rowid FROM %s LIMIT 1);", dbname ,dbname);          // 删除第一条数据
    rv = sqlite3_exec(*db, sql, 0, 0, &zErrMsg);

    if(rv != SQLITE_OK)
    {
        log_error("Sqlite_delete_data error:%s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -2;
    }

    log_info("Delete first data successfully!\n");
    return 0;
}

int database_check_data(char *dbname, sqlite3 **db)
{
    char    sql[128];
    int     rv;
    char   *zErrMsg = 0;
    char  **dbResult;
    int     nRow=0, nColumn=0;

    if((dbname == NULL) || (db == NULL))
    {
        log_error("The sqlite_check_data() argument incorrect!\n");
        return -1;
    }

    memset(sql, 0, sizeof(sql));
    sprintf(sql, "SELECT * FROM %s LIMIT 1;", dbname);          // 选择第一条数据

    rv = sqlite3_get_table(*db, sql, &dbResult, &nRow, &nColumn, &zErrMsg);
    if (rv != SQLITE_OK)
    {
        log_error("Sqlite_check_data error:%s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -2;
    }

    return nRow;
}