/*
 * @Author: RoxyKko
 * @Date: 2023-04-11 21:15:13
 * @LastEditors: RoxyKko
 * @LastEditTime: 2023-04-11 22:53:48
 * @Description: 服务器端
 */

#include "iot_main.h"

#define MAX_EVENTS      512
#define DATABASE_NAME   "sht20"                 // 数据库名
#define TABLE_NAME      "RPI4B"                 // 数据库表名
#define Vision          1.5                     // 版本号
#define lastEdit        "2023-04-06 17:57:49"   // 最后编辑时间

static inline void print_usage(char *progname);

int main(int argc, char **argv)
{
    struct epoll_event          event;                          // epoll事件结构体
	struct epoll_event          event_array[MAX_EVENTS];        // epoll事件数组
    packinfo_t                  pack_info;                      // 数据结构体
    int					        listenfd;                       // server监听套接字
    int                         connfd;                         // client连接套接字
    char                       *progname        =       NULL;   // 程序名
    int                         daemon_run      =       0;      // 守护进程
    int                         opt;                            // getopt_long返回值
    int                         rv;                             // 返回值
    int                         i, j;
    int                         serv_port       =       0;      // 服务器端口
    char                        buf[1024];                      // 数据缓冲区
    int                         epollfd         =       -1;     // epoll句柄
    int                         events          =       -1;     // epoll_wait返回值
    sqlite3 			       *db;                             // 数据库句柄

    struct option long_option[] =
		{
			{"daemon", no_argument, NULL, 'b'},
			{"port", required_argument, NULL, 'p'},
			{"help", no_argument, NULL, 'h'},
			{0, 0, 0, 0}};

    progname = argv[0];
    while ((opt = getopt_long(argc, argv, "bp:h", long_option, NULL)) != -1)
    {
        switch (opt)
		{
		case 'b':
			daemon_run = 1;
			break;
		case 'p':
			serv_port = atoi(optarg);
			break;
		case 'h':
			print_usage(progname);
			return EXIT_SUCCESS;
		default:
			break;
		}
    }

    if( logger_init("./logger/running.log", LOG_LEVEL_INFO) < 0)
    {
        fprintf(stderr, "initial logger system failure\n");
        return -1;
    }

    log_info("\n\n\n");
    log_info("============================================================\n");
    log_info("=                                                          =\n");
    log_info("=                      IOT Project                         =\n");
    log_info("=                                                          =\n");
    log_info("=                      Version: %.2f                        =\n", Vision);
    log_info("=                                                          =\n");
    log_info("=                      LastEdit: %s                       =\n", lastEdit);
    log_info("=                                                          =\n");
    log_info("============================================================\n");

    if (!serv_port)
	{
		print_usage(progname);
		return -2;
	}

    if (daemon_run)
	{
		if (daemon(0, 0) < 0)
		{
			log_error("Runing daemon failure:%s\n", strerror(errno));
			return -3;
		}
		log_debug("Runing daemon successfully!\n");
	}

    set_socket_rlimit();
    if ((listenfd = socket_server_init(NULL, serv_port)) < 0)
	{
		log_error("ERROR: %s server listen on port %d failure\n", argv[0], serv_port);
		return -2;
	}
    log_debug("%s server start to listen on port %d\n", argv[0],serv_port);

    if( (epollfd=epoll_create(MAX_EVENTS)) < 0 )
    {
		log_error("Create epoll failure:%s\n", strerror(errno));
		return -4;
	}

    event.events = EPOLLIN;
	event.data.fd = listenfd;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &event) < 0)
	{
		log_error("Execute epoll_ctl failure:%s\n", strerror(errno));
		return -5;
	}

    // 初始化数据库
    if (database_init(DATABASE_NAME, &db) < 0)
    {
        log_error("database init failed!\n");
        printf("database init failed!\n");
        return -6;
    }

    // 若数据库中不存在表，则创建表
    if (database_create_table(TABLE_NAME, &db) < 0)
    {
        log_error("database create table failed!\n");
        database_close(DATABASE_NAME, &db);
        return -7;
    }

    while(1)
    {
        // 没接收到数据时，阻塞等待
        events = epoll_wait(epollfd, event_array, MAX_EVENTS, -1);
        if (events < 0)
		{
			log_error("epoll failure: %s\n", strerror(errno));
			break;
		}
		else if (events == 0)
		{
			log_error("epoll get timeout\n");
			continue;
		}

        // events > 0 是活动事件计数
        for(i = 0; i < events; i++)
        {
            if ((event_array[i].events & EPOLLERR) || (event_array[i].events & EPOLLHUP))
			{
				log_error("epoll_wait get error on fd[%d]: %s\n", event_array[i].data.fd, strerror(errno));
				epoll_ctl(epollfd, EPOLL_CTL_DEL, event_array[i].data.fd, NULL);
				close(event_array[i].data.fd);
			}

            // 监听socket得到event意味着有新的客户端链接
            if (event_array[i].data.fd == listenfd)
			{
				if ((connfd = accept(listenfd, (struct sockaddr *)NULL, NULL)) < 0)
				{
					log_error("accept() failure: %s \n", strerror(errno));
                    close(connfd);
					continue;
				}

				event.data.fd = connfd;
				event.events = EPOLLIN;
				if (epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &event) < 0)
				{
					log_error("epoll add client socket failure: %s \n", strerror(errno));
					close(event_array[i].data.fd);
					continue;
				}

				log_info("epoll add new client socket[%d] ok.\n", connfd);

			}
            else    // 客户端套接字已连接并获取数据
            {
                // 清空数据缓存区
                memset(buf, 0, sizeof(buf));

                // 读取客户端数据
                if ((rv = read(event_array[i].data.fd, buf, sizeof(buf))) <= 0)
				{
					log_error("socket[%d] read failure or get disconnect and will be removed. \n", event_array[i].data.fd);

					if(epoll_ctl(epollfd, EPOLL_CTL_DEL, event_array[i].data.fd, NULL) < 0)
                    {
                        log_error("epoll_ctl del socket[%d] failure: %s \n", event_array[i].data.fd, strerror(errno));
                    }

					close(event_array[i].data.fd);
					continue;
				}
                
                // 若rv>0读取到数据
                data_segmentation(buf, &pack_info);

                // 数据库插入数据
                if (database_insert_data(TABLE_NAME, &db, &pack_info) < 0)
                {
                    log_error("database insert data failed!\n");
                    database_close(DATABASE_NAME, &db);
                    return -8;
                }


            }

        }   // end for

    }   // end while
    database_close(DATABASE_NAME, &db);
    return 0;

}

static inline void print_usage(char *progname)
{
	printf("Usage: %s [OPTION] ...\n", progname);

	printf(" %s is a socket server program, which used toverify client and echo back string from it \n", progname);
	printf("\nMandatory arguments to long options are mandatory for short option too:\n");

	printf(" -b[daemon ] set program running on background\n");
	printf(" -p[port   ] Socket server port address\n");
	printf(" -h[help   ] Display this help information\n");

	printf("\nExample: %s -b -p 8900\n", progname);
	return;
}