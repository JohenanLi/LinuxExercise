//1． init.c清单
#define _GNU_SOURCE
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
#include <asm/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sched.h>
#include <pthread.h>

#define buffer_size 1024

void init_daemon(void)
{
    int pid;
    int i;
    int fd0;
    //重设文件创建掩模
    umask(0);           

    if(pid=fork())
        //是父进程，结束父进程
        exit(0);
    else if(pid< 0)
         //fork失败，退出
        exit(1);       

    //是第一子进程，后台继续执行
    //第一子进程成为新的会话组长和进程组长
    setsid();           
    
    //并与控制终端分离
    if(pid=fork())
        //是第一子进程，结束第一子进程
        exit(0);        
    else if(pid< 0)
        //fork失败，退出
        exit(1);        
    
    //是第二子进程，继续
    //第二子进程不再是会话组长

    //关闭打开的文件描述符
    for(i=0;i<NOFILE;++i)  
        close(i);
    
    //改变工作目录到根目录
    if(chdir("/")<0){
        printf("dir error\n");
        return;
    }

    //输出重定向
    fd0 = open("/dev/null", O_RDWR);
    dup2(fd0,0);
    dup2(fd0,1);
    dup2(fd0,2);

    return;
}

// void monitor(){  
//     getmem();
//     getcpuinfo();
//     checkcore();
//     loadmonitor();
//     memmonitor();
//     iodevicemonitor();
//     filesysmonitor();
//     netmonitor();
// }

// char* two_one(char a[][100]){

//     int row = sizeof(a) / sizeof(a[0]);//行数
//     int col = sizeof(a[0]) / sizeof(a[0][0]);//列数

//     int b[row*col];
//     int cnt=0;
//     for(int i = 0; i < row; i++)
//     {
//         for(int j = 0; j<col; j++)
//         {
//             b[cnt]=a[i][j];
//             cnt++;
//         }
//     }
//     return b;

// }

int main(){
    
    //初始化Daemon进程
    // init_daemon();

    //创建文件
    int open_fd;
    if(open_fd != -1)
        close(open_fd);
    open_fd = open("/tmp/server.log",O_RDWR|O_APPEND|O_CREAT, 0644);
    dup2(open_fd, STDIN_FILENO);  
    dup2(open_fd, STDOUT_FILENO);  
    dup2(open_fd, STDERR_FILENO); 

    //创建套接字
	int serv_sock = socket(AF_INET, SOCK_STREAM,IPPROTO_TCP);
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

    
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size = sizeof(clnt_addr);

    //提示服务器端启动
    write(open_fd,"\nServer work!\n",sizeof("\nServer work!\n"));

    //阻塞直到有客户端连接
    while(1){
        //接受客户端请求
	    //定义客户端的套接字，这里返回一个新的套接字，后面通信时，就用这个clnt_sock进行通信
        if((clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size))==-1){
            continue;
        };

        char str[buffer_size];
        char title[300];
        char *token0;
        char *token1;
        // char *catCommand= "/bin/sh ";
        int time;
        // //getmem()
        // char mem[30]="Memory:";
        // //getcpuinfo()
        // char core[30]="Cores in total:";
        // char cpu[100]="CPU info:";
        // //checkcore()
        // //loadmonitor()
        // char load1[30]="loadavg in 1 minute:";
        // char load5[30]="loadavg in 5 minute:";
        // char load15[30]="loadavg in 15 minute:";

        while(1){

            //接收客户端数据
            memset(str,0,sizeof(buffer_size));
            recv(clnt_sock, str, sizeof(str),0);
            token0 = strtok(str,":");
            token1 = strtok( NULL,":");

            if(!strcmp(token0,"time")){
                time = atoi(token1);
                write(open_fd,"时间",sizeof("时间"));
                write(open_fd,token1,sizeof(token1));
            }else if(!strcmp(token0,"command")){
                write(open_fd,"命令",sizeof("命令"));
                write(open_fd,token1,sizeof(token1));
                
                FILE *command_fp;
                char buffer[200];
                char result[2048];
                // strcat(catCommand,token1);
                printf("token1:%s",token1);
                command_fp = popen(token1,"r");
                if(fgets(buffer,sizeof(buffer),command_fp) != NULL){
                    strcpy(result,buffer);
                    while(fgets(buffer,sizeof(buffer),command_fp) != NULL){
                    strcat(result,buffer);
                }
                }else{
                    char erro[]="command wrong!";
                    strcpy(result,erro);
                }
                send(clnt_sock,result,sizeof(result),0);
                pclose(command_fp);
            }else{
                printf("无法识别");
            }
            
            strcpy(title, "client send:");
            strcat(title,str);
            write(open_fd,title,sizeof(title));



        }
        
        // strcat(str, "+ACK");
        // send(clnt_sock, str, sizeof(str),0);
        close(clnt_sock);
    }

    //关闭文件流
    close(open_fd);
	//关闭套接字
	close(serv_sock);

	return 0;
}
