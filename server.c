//1． init.c清单
#include <unistd.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
 
void init_daemon(void)
{
    int pid;
    int i;
    int fd0;

    umask(0);           //重设文件创建掩模

    if(pid=fork())
        exit(0);        //是父进程，结束父进程
    else if(pid< 0)
        exit(1);        //fork失败，退出

    //是第一子进程，后台继续执行
    setsid();           //第一子进程成为新的会话组长和进程组长
    
    //并与控制终端分离
    if(pid=fork())
        exit(0);        //是第一子进程，结束第一子进程
    else if(pid< 0)
        exit(1);        //fork失败，退出
    
    //是第二子进程，继续
    //第二子进程不再是会话组长
    for(i=0;i<NOFILE;++i)  //关闭打开的文件描述符
        close(i);
 
    if(chdir("/")<0){
        printf("dir error\n");
        return;
    }    //改变工作目录到根目录

    fd0 = open("/dev/null", O_RDWR);//输出重定向
    dup2(fd0,0);
    dup2(fd0,1);
    dup2(fd0,2);

    return;
}

int main(){
    
    //初始化Daemon进程
    init_daemon();
    //创建文件
    int open_fd;
    if(open_fd != -1)
        close(open_fd);
    open_fd = open("/tmp/server.log",O_RDWR|O_APPEND|O_CREAT, 0644);
    dup2(open_fd, STDIN_FILENO);  
    dup2(open_fd, STDOUT_FILENO);  
    dup2(open_fd, STDERR_FILENO); 

    //创建套接字
	int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int clnt_sock;
	//初始化socket元素
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serv_addr.sin_port = htons(50007);

	//绑定文件描述符和服务器的ip和端口号
	bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    //进入监听状态，等待用户发起请求
	listen(serv_sock, 20);
    //接受客户端请求
	//定义客户端的套接字，这里返回一个新的套接字，后面通信时，就用这个clnt_sock进行通信
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size = sizeof(clnt_addr);
    //
    // fprintf(stdout,"Server work!");
    write(open_fd,"\nServer work!\n",sizeof("\nServer work!\n"));
    while(1){
        //阻塞直到有客户端连接
        if((clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size))==-1){
            continue;
        };
        //接收客户端数据，并相应
	char str[256];
    char title[300];
	read(clnt_sock, str, sizeof(str));
	// printf("client send: %s\n",str);
    // fprintf(stdout,"client send: %s\n",str);
    strcpy(title, "client send:");
    strcat(title,str);
    write(open_fd,title,sizeof(title));

	strcat(str, "+ACK");
	write(clnt_sock, str, sizeof(str));
    close(clnt_sock);
    }
    //关闭文件流
    close(open_fd);
	//关闭套接字
	close(serv_sock);

	return 0;
}
