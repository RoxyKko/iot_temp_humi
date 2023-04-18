/*** 
 * @Author: RoxyKko
 * @Date: 2023-04-04 17:37:02
 * @LastEditors: RoxyKko
 * @LastEditTime: 2023-04-10 16:50:09
 * @Description: socket client 端代码
 */

#ifndef __SOCKET_CLIENT_H__
#define __SOCKET_CLIENT_H__

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

// #include <linux/tcp.h>
#include <netinet/tcp.h>
#include "iot_main.h"
#include "packinfo.h"
#include "logger.h"



int socket_client_init(char *serv_ip, int port);

int sendata(int sockfd, packinfo_t pack_info);

int get_sock_status(int sockfd);

#endif
