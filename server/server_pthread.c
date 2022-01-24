#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <asm/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sched.h>
#include <pthread.h>
#include <bits/cpu-set.h>
#include <arpa/inet.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "monitor.h"

#define MYPORT 50007 // the port users will be connecting to

#define BACKLOG 5 // how many pending connections queue will hold

#define BUF_SIZE 200

int fd_A[BACKLOG]; // accepted connection fd
int conn_amount;   // current connection amount

void init_daemon(void)
{
    int pid;
    int i;
    int fd0;
    //重设文件创建掩模
    umask(0);

    if (pid = fork())
        //是父进程，结束父进程
        exit(0);
    else if (pid < 0)
        //fork失败，退出
        exit(1);

    //是第一子进程，后台继续执行
    //第一子进程成为新的会话组长和进程组长
    setsid();

    //并与控制终端分离
    if (pid = fork())
        //是第一子进程，结束第一子进程
        exit(0);
    else if (pid < 0)
        //fork失败，退出
        exit(1);

    //是第二子进程，继续
    //第二子进程不再是会话组长

    //关闭打开的文件描述符
    for (i = 0; i < NOFILE; ++i)
        close(i);

    //改变工作目录到根目录
    if (chdir("/") < 0)
    {
        printf("dir error\n");
        return;
    }

    //输出重定向
    fd0 = open("/dev/null", O_RDWR);
    dup2(fd0, 0);
    dup2(fd0, 1);
    dup2(fd0, 2);

    return;
}
void showclient()
{
    int i;
    printf("client amount: %d\n", conn_amount);
    for (i = 0; i < BACKLOG; i++)
    {
        printf("[%d]:%d  ", i, fd_A[i]);
    }
    printf("\n\n");
}
void *server_listen(void *arg)
{

    int sock_fd = (int)arg;
    struct sockaddr_in client_addr;
    char buf[BUF_SIZE];
    int new_fd;
    socklen_t sin_size;

    int ret;
    int i;
    //文件名字
    char *fname = "./monitor.txt";
    //文件长度
    int length;
    char *p;

    fd_set fdsr;
    int maxsock;
    char title[9];
    memset(title, '\0', sizeof(title));
    char *token0;
    char *token1;
    struct timeval tv;

    conn_amount = 0;
    sin_size = sizeof(client_addr);
    maxsock = sock_fd;

    while (1)
    {
        // initialize file descriptor set
        FD_ZERO(&fdsr);
        FD_SET(sock_fd, &fdsr);

        // 设置超时时限
        tv.tv_sec = 30;
        tv.tv_usec = 0;

        // add active connection to fd set
        for (i = 0; i < BACKLOG; i++)
        {
            if (fd_A[i] != 0)
            {
                FD_SET(fd_A[i], &fdsr);
            }
        }

        ret = select(maxsock + 1, &fdsr, NULL, NULL, &tv);
        if (ret < 0)
        {
            perror("select");
            break;
        }
        else if (ret == 0)
        {
            printf("timeout\n");
            continue;
        }

        // check every fd in the set
        for (i = 0; i < conn_amount; i++)
        {
            if (FD_ISSET(fd_A[i], &fdsr))
            {
                ret = recv(fd_A[i], buf, sizeof(buf), 0);
                if (ret <= 0)
                { // client close
                    printf("client[%d] close\n", i);
                    close(fd_A[i]);
                    FD_CLR(fd_A[i], &fdsr);
                    fd_A[i] = 0;
                }
                else
                { // receive data
                    if (ret < BUF_SIZE)
                        memset(&buf[ret], '\0', 1);
                    printf("client[%d] send:%s\n", i, buf);
                    //回复
                    strncpy(title, buf, 8);
                    printf("com %d", strcmp(title, "command:"));
                    if (!strcmp(title, "command:"))
                    {
                        printf("command!\n");
                        FILE *command_fp;

                        char buffer[512];
                        char result[4096];
                        memset(buffer, 0, sizeof(buffer));
                        memset(result, 0, sizeof(result));
                        //截取命令
                        token0 = strtok(buf, ":");
                        token1 = strtok(NULL, ":");
                        command_fp = popen(token1, "r");
                        if (fgets(buffer, sizeof(buffer), command_fp) != NULL)
                        {
                            strcpy(result, buffer);
                            while (fgets(buffer, sizeof(buffer), command_fp) != NULL)
                            {
                                strcat(result, buffer);
                            }
                        }
                        else
                        {
                            char erro[] = "command wrong!";
                            strcpy(result, erro);
                        }
                        printf("result %s\n", result);
                        if (!fork()) //向客户端发送回应数据
                        {
                            if (send(fd_A[i], result, sizeof(result), 0) < 0)
                            {
                                printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
                            }
                            exit(0);
                        }
                        pclose(command_fp);
                    }
                    else if (!strcmp(title, "getInfo:"))
                    {
                        p = getFileAll(fname, &length);
                        if (p == NULL)
                        {
                            printf("open file error!");
                            exit(0);
                        }
                        if (!fork()) //向客户端发送回应数据
                        {
                            if (send(fd_A[i], p, length, 0) < 0)
                            {
                                printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
                            }
                            exit(0);
                        }
                    }
                    else
                    {
                        printf("title %s\ntitlenum %d\n", title, strlen(title));
                        strcat(buf, "+ACK");
                        
                        if (!fork()) //向客户端发送回应数据
                        {
                            if (send(fd_A[i], buf, sizeof(buf), 0) < 0)
                            {
                                printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
                            }
                            exit(0);
                        }
                        
                    }
                }
            }
        }

        // check whether a new connection comes
        if (FD_ISSET(sock_fd, &fdsr))
        {
            new_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &sin_size);
            if (new_fd <= 0)
            {
                perror("accept");
                continue;
            }

            // add to fd queue
            if (conn_amount < BACKLOG)
            {
                fd_A[conn_amount++] = new_fd;
                printf("new connection client[%d] %s:%d\n", conn_amount,
                       inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                if (new_fd > maxsock)
                    maxsock = new_fd;
            }
            else
            {
                printf("max connections arrive, exit\n");
                if (!fork()) //向客户端发送回应数据
                        {
                            if ( send(new_fd, "bye", 4, 0) < 0)
                            {
                                printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
                            }
                            exit(0);
                        }
                close(new_fd);
                break;
            }
        }
        showclient();
    }
    pthread_exit(0);
}

int main(void)
{
    //初始化Daemon进程
    init_daemon();
    signal(SIGCHLD,SIG_IGN);

    //创建文件
    int open_fd;
    if (open_fd != -1)
        close(open_fd);
    open_fd = open("/tmp/server.log", O_RDWR | O_APPEND | O_CREAT, 0644);
    dup2(open_fd, STDIN_FILENO);
    dup2(open_fd, STDOUT_FILENO);
    dup2(open_fd, STDERR_FILENO);

    int sock_fd, new_fd;            // listen on sock_fd, new connection on new_fd
    struct sockaddr_in server_addr; // server address information
    struct sockaddr_in client_addr; // connector's address information
    int yes = 1;

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    //closesocket后想继续重用该socket
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        perror("setsockopt");
        exit(1);
    }

    //设置socket参数
    server_addr.sin_family = AF_INET;         // host byte order
    server_addr.sin_port = htons(MYPORT);     // short, network byte order
    server_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
    memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));

    //绑定文件描述符和服务器的ip和端口号
    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind");
        exit(1);
    }

    //进入监听状态，等待用户发起请求
    if (listen(sock_fd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }
    //提示服务器端启动
    write(open_fd, "\nServer work!\n", sizeof("\nServer work!\n"));

    pthread_t tid1, tid2;
    if (pthread_create(&tid1, NULL, monitor, NULL) != 0)
    {
        printf("pthread_create monitor error.");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&tid2, NULL, server_listen, (void *)sock_fd) != 0)
    {
        printf("pthread_create listen error.");
        exit(EXIT_FAILURE);
    }
    void *p = NULL;
    pthread_join(tid1, &p);
    pthread_join(tid2, &p);
    //关闭文件流
    close(open_fd);
    // close other connections
    for (int i = 0; i < BACKLOG; i++)
    {
        if (fd_A[i] != 0)
        {
            close(fd_A[i]);
        }
    }

    exit(0);
}