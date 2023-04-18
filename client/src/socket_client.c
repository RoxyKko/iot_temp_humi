/*
 * @Author: RoxyKko
 * @Date: 2023-04-04 18:38:48
 * @LastEditors: RoxyKko
 * @LastEditTime: 2023-04-11 23:06:14
 * @Description: socket相关函数
 */

#include "socket_client.h"

int 	connect_flag = 0;


/**
 * @name: socket_client_init(char *serv_ip, int port)
 * @description: 建立TCP socket连接
 * @param {char} *serv_ip 服务器IP地址
 * @param {int} port 服务器端口号
 * @return {*}若成功返回socket描述符，否则返回<0
 */
int socket_client_init(char *serv_ip, int port)
{
    int                 socket_fd;
    struct sockaddr_in  servaddr;
    int                 rv = -1;

    // 创建socket并获得socket描述符
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    // 创建socket失败
    if (socket_fd < 0)
    {
        printf("Create socket failure: %s\n", strerror(errno));
        return -1;
    }

    // 创建socket成功
    printf("Create socket[%d] successfully\n", socket_fd);

    // 初始化结构体，将空余的8位字节填充为0
    // 设置参数，connect连接服务器
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;                 // IPV4
    servaddr.sin_port        = htons(port);             // port
    inet_aton(serv_ip, &servaddr.sin_addr);             // 将点分十进制IP转换为网络字节序
    rv = connect(socket_fd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    // 连接服务器失败
    if (rv < 0)
    {
        printf("Connect to server[%s:%d] failure : %s\n",
               serv_ip, port, strerror(errno));
        return -2;
    }
    // 连接服务器成功,打印服务器IP地址和端口号
    printf("Connect to server[%s:%d] successfully!\n", serv_ip, port);

    // 若链接成功则返回socket描述符
    return socket_fd;
}

/**
 * @name: sendata(int sockfd, packinfo_t pack_info)
 * @description: 
 * @param {int} sockfd
 * @param {packinfo_t} pack_info
 * @return {*} 成功返回0，否则返回<0
 */
int sendata(int sockfd, packinfo_t pack_info)
{
    int         rv            = -1;
    char        send_buf[128] = {0};
    int         send_len      = 0;
    int         send_count    = 0;

    if(sockfd < 0)
    {
        if(!connect_flag)
        {
            printf("The sendata() argument connfd incorrect!\n");
        }
		return -1;
    }

    memset(send_buf, 0, sizeof(send_buf));
    sprintf(send_buf, "%s/%s/%f/%f", pack_info.devid, pack_info.time, pack_info.temp, pack_info.humi);

    send_len = strlen(send_buf);
    log_debug("Sendata: %s\n", send_buf);
    while(send_count < send_len)
    {
        rv = write(sockfd, send_buf + send_count, send_len - send_count);
        if(rv < 0)
        {
            log_error("Sendata error: %s\n", strerror(errno));
            return -2;
        }
        
        send_count += rv;
        
    }

    log_info("Send data to sever successfully:%s\n", send_buf);
	return 0;
}

/**
 * @name: get_sock_status(int sockfd)
 * @description: 获取socket状态
 * @param {int} sockfd socket描述符
 * @return {*} 1:连接成功，0:连接失败，-1:参数错误，-2:获取状态失败
 */
int get_sock_status(int sockfd)
{
    struct tcp_info     info;
    int                 len = sizeof(info);
    int                 rv  = -1;

    if(sockfd < 0)
    {
        log_error("The get_sock_status() argument sockfd incorrect!\n");
        return -1;
    }

    memset(&info, 0, sizeof(info));

    rv = getsockopt(sockfd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
    if(rv < 0)
    {
        log_error("Get socket status error: %s\n", strerror(errno));
        return -2;
    }

    if(info.tcpi_state == TCP_ESTABLISHED) // TCP_ESTABLISHED在#include <netinet/tcp.h>中定义
    {
        if(connect_flag == 0)
        {
            log_info("*************************************************************************\n\n");
            log_info("Socket[%d] is connected\n\n", sockfd);
            log_info("*************************************************************************\n\n");
        }
        connect_flag = 1;
        return 1;
    }
    else
    {
        if(connect_flag == 1)
        {
            log_info("*************************************************************************\n\n");
            log_info("Socket[%d] is disconnected\n\n", sockfd);
            log_info("*************************************************************************\n\n");
        }
        connect_flag = 0;
        return 0;
    }
}