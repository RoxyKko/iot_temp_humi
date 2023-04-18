/*** 
 * @Author: RoxyKko
 * @Date: 2023-04-05 19:24:03
 * @LastEditors: RoxyKko
 * @LastEditTime: 2023-04-10 18:18:29
 * @Description: sqlite的使用
 */

#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "iot_main.h"
#include "packinfo.h"

int database_init(char *dbname, sqlite3 **db);

int database_close(char *dbname, sqlite3 **db);

int database_create_table(char *dbname, sqlite3 **db);

int database_insert_data(char *dbname, sqlite3 **db, packinfo_t *pack_info);

int database_select_data(char *dbname, sqlite3 **db, packinfo_t *pack_info);

int database_delete_data(char *dbname, sqlite3 **db);

int database_check_data(char *dbname, sqlite3 **db);




# endif
