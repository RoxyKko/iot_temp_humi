/*** 
 * @Author: RoxyKko
 * @Date: 2023-04-11 21:14:55
 * @LastEditors: RoxyKko
 * @LastEditTime: 2023-04-11 22:38:36
 * @Description: 服务器端
 */


#ifndef  _IOT_MAIN_H_
#define _IOT_MAIN_H_

#include <stdio.h>
#include <sys/types.h>	// for socket
#include <sys/socket.h> // for socket
#include <string.h>		// fpr strerror and memset
#include <errno.h>		// for errno
#include <netinet/in.h> // for sockaddr_in
#include <arpa/inet.h>	// for inet_aton
#include <fcntl.h>		// for open
#include <unistd.h>		// for read/write/close
#include <getopt.h>		// for getopt_long
#include <stdlib.h>		// for atoi
#include <sys/epoll.h>  // for epoll
#include <sys/time.h>
#include <sys/resource.h>

#include "database.h"
#include "logger.h"
#include "packinfo.h"
#include "sqlite3.h"
#include "socket_server.h"

#endif  
