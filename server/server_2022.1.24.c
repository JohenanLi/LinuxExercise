<<<<<<< HEAD
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

#include "monitor.h"

#define MYPORT 50007 // the port users will be connecting to

#define BACKLOG 5 // how many pending connections queue will hold

#define BUF_SIZE 200

int fd_A[BACKLOG]; // accepted connection fd
int conn_amount;   // current connection amount

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

int main(void)
{
    int sock_fd, new_fd;            // listen on sock_fd, new connection on new_fd
    struct sockaddr_in server_addr; // server address information
    struct sockaddr_in client_addr; // connector's address information
    socklen_t sin_size;
    int yes = 1;
    char buf[BUF_SIZE];
    int ret;
    int i;
    //文件名字
    char *fname = "./monitor.txt";
    //文件长度
    int length;
    char *p;

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

    printf("listen port %d\n", MYPORT);

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
                        send(fd_A[i], result, sizeof(result), 0);
                        pclose(command_fp);
                    }
                    else if (!strcmp(title, "getInfo:"))
                    {
                        monitor();
                        sleep(2);
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
                        send(fd_A[i], buf, sizeof(buf), 0);
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
                send(new_fd, "bye", 4, 0);
                close(new_fd);
                break;
            }
        }
        showclient();
    }

    // close other connections
    for (i = 0; i < BACKLOG; i++)
    {
        if (fd_A[i] != 0)
        {
            close(fd_A[i]);
        }
    }

    exit(0);
=======
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

#include "monitor.h"

#define MYPORT 50007 // the port users will be connecting to

#define BACKLOG 5 // how many pending connections queue will hold

#define BUF_SIZE 200

int fd_A[BACKLOG]; // accepted connection fd
int conn_amount;   // current connection amount

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

int main(void)
{
    int sock_fd, new_fd;            // listen on sock_fd, new connection on new_fd
    struct sockaddr_in server_addr; // server address information
    struct sockaddr_in client_addr; // connector's address information
    socklen_t sin_size;
    int yes = 1;
    char buf[BUF_SIZE];
    int ret;
    int i;
    //文件名字
    char *fname = "./monitor.txt";
    //文件长度
    int length;
    char *p;

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

    printf("listen port %d\n", MYPORT);

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
                        send(fd_A[i], result, sizeof(result), 0);
                        pclose(command_fp);
                    }
                    else if (!strcmp(title, "getInfo:"))
                    {
                        monitor();
                        sleep(2);
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
                        send(fd_A[i], buf, sizeof(buf), 0);
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
                send(new_fd, "bye", 4, 0);
                close(new_fd);
                break;
            }
        }
        showclient();
    }

    // close other connections
    for (i = 0; i < BACKLOG; i++)
    {
        if (fd_A[i] != 0)
        {
            close(fd_A[i]);
        }
    }

    exit(0);
>>>>>>> 33a1bbb41f799208d0f46f921d9dd0eae3fb6d4a
}