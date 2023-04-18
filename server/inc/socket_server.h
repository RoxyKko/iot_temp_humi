/*** 
 * @Author: RoxyKko
 * @Date: 2023-04-11 21:16:03
 * @LastEditors: RoxyKko
 * @LastEditTime: 2023-04-11 22:02:22
 * @Description: 服务器端socket
 */

#ifndef _SOCKET_SERVER_H_
#define _SOCKET_SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


int socket_server_init(char *listen_ip, int listen_port);

void set_socket_rlimit();

#endif // _SOCKET_SERVER_H_