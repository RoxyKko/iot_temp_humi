/*** 
 * @Author: RoxyKko
 * @Date: 2023-04-04 17:06:27
 * @LastEditors: RoxyKko
 * @LastEditTime: 2023-04-10 18:26:32
 * @Description: 
 */

#ifndef __IOT_MAIN_H__
#define __IOT_MAIN_H__


#include <stdio.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <gpiod.h>
#include <stdint.h>
#include <ctype.h>
#include <syslog.h>
#include <signal.h>

#include "i2c_sht20.h"
#include "socket_client.h"
#include "sqlite3.h"
#include "get_time.h"
#include "logger.h"
#include "database.h"
#include "packinfo.h"

# endif