#define _GNU_SOURCE
#include <asm/types.h>
#include<stdio.h> 
#include<stdlib.h>
#include<unistd.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <string.h>
#include <sched.h>
#include <pthread.h>

#define  CTEST_CORE_TRG_MS  1000
#define  CTEST_BUSY_CYCLES  (10 * 1000 * 1000)

int corecount; //cpu总核数

float totalmem;  //总内存大小

void checkcore();
void getmem();   //获取内存大小
void getcpuinfo();    //获取cpu总核数、cpu类型
void loadmonitor(); //负载
void memmonitor();   //内存监控
void iodevicemonitor(); //io设备监测
void filesysmonitor();  //文件系统监测
void netmonitor();    //网卡eth0

int stopflag = 1;
char cpuinfo[20][100];   //存放getcpu（）读到信息
char meminfo[60][100];   //存放getmem（）读到信息
char filesys[20][100];  //存放filesysmonitor（）读到信息
char io[8][100];        //存放iodevicemonitor（）读到信息
char net[30][80];       //存放netmonitor（）读到信息
static __u32  measure_preemption(__u32  target_ms) ;


  int main() 
 {  
    getmem();
    printf("-----------------------------");
    getcpuinfo();
    int coreavail =  sysconf(_SC_NPROCESSORS_ONLN);  //读取当前可用核数
    printf("Cores available:%d\n",coreavail);
  
    while(stopflag){
    checkcore();
    printf("-----------------------------");
    loadmonitor();
    printf("-----------------------------");
    memmonitor();
    printf("-----------------------------");
    iodevicemonitor();
    printf("-----------------------------");
    filesysmonitor();
    printf("-----------------------------");
    netmonitor();
    printf("-----------------------------");
    printf("\n");
    sleep(10);
       
    }
 }

void getcpuinfo(){
    FILE * fp; 
   
    int i = 0;
    fp=popen("cat /proc/cpuinfo| grep 'physical id'| sort| uniq| wc -l","r");  //读取物理cpu个数
    fgets(cpuinfo[i],sizeof(cpuinfo[i]),fp);
    int phycpu = atoi(cpuinfo[i]);
    i++;
    fp=popen("cat /proc/cpuinfo| grep 'cpu cores'| uniq|awk '{print $4}'","r");  //读取每个物理cpu的核数
    fgets(cpuinfo[i],sizeof(cpuinfo[i]),fp);
    int perphycore = atoi(cpuinfo[i]);
    i++;
    corecount = phycpu * perphycore;  //总核数
    fp=popen("cat /proc/cpuinfo | grep name | cut -f2 -d: | uniq -c","r");  //cpu型号等信息读取
    fgets(cpuinfo[i],sizeof(cpuinfo[i]),fp);
    
    printf("Cores in total: %d\n",corecount);
    printf("CPU info:%s\n",cpuinfo[i]);
    pclose(fp); 

}
 
void getmem(){
     
     FILE * fp; 
    char buffer[80]; 
    fp=popen("free|grep Mem|awk '{print $2}'","r");   //总内存大小
    fgets(buffer,sizeof(buffer),fp);
    totalmem = atof(buffer); 
    printf("Memory:%.0fkb\n",totalmem);
    pclose(fp); 
}


void loadmonitor(){
    FILE * fp; 
    char buffer[80]; 
    float loadavg1,loadavg5,loadavg15;
    fp=popen("cat /proc/loadavg|awk '{print $1}'","r");   //1分钟负载
    fgets(buffer,sizeof(buffer),fp);
    loadavg1=atof(buffer);
    printf("loadavg in 1 minute:%.2f.\n",loadavg1);
    fp=popen("cat /proc/loadavg|awk '{print $2}'","r");  //5分钟负载
    fgets(buffer,sizeof(buffer),fp);
    loadavg5=atof(buffer);
    printf("loadavg in 5 minutes:%.2f.\n",loadavg5); 
    fp=popen("cat /proc/loadavg|awk '{print $3}'","r"); //15分钟负载
    fgets(buffer,sizeof(buffer),fp);
    loadavg15=atof(buffer);
    printf("loadavg in 15 minutes:%.2f.\n",loadavg15);
        pclose(fp); 

}

void memmonitor(){
   FILE * fp; 
    
    int i = 0; 
    fp=popen("cat /proc/meminfo", "r");    //内存信息 一行一行读进二维字符数组
    while(fgets(meminfo[i],sizeof(meminfo[i]),fp) != NULL){
     printf("%s",meminfo[i]);
      i++;
      
}  
    pclose(fp); 
}

void filesysmonitor(){

    FILE * fp;
    
    int i = 0;
    fp=popen("df -h","r");      //文件系统信息 一行一行读进二维字符数组
     while(fgets(filesys[i],sizeof(filesys[i]),fp) != NULL){
	printf("%s",filesys[i]);
        i++;
}
     pclose(fp);
}

void iodevicemonitor()
 {  
    FILE * fp; 
  
    int i = 0; 
    fp = popen("iostat -x|grep -A 5 'Device'","r");   //输入输出设备信息 一行一行读进二维字符数组 
    while(fgets(io[i],sizeof(io[i]),fp) != NULL){
      printf("%s",io[i]);
           i++;
} 
    

    pclose(fp); 
  }

void netmonitor(){

     FILE * fp; 
    int i = 0;
    fp=popen("ifconfig eth0","r");    //eth0信息 一行一行读进二维字符数组
    while(fgets(net[i],sizeof(net[i]),fp) != NULL){
     printf("%s",net[i]);
       i++;
   }
    pclose(fp); 
}



static __u32  get_cur_time_us(void) {

  struct timeval tv;
  struct timezone tz;

  gettimeofday(&tv, &tz);

  return (tv.tv_sec * 1000000ULL) + tv.tv_usec;

}


static __u64  get_cpu_usage_us(void) {

  struct rusage u;

  getrusage(RUSAGE_SELF, &u);

  return (u.ru_utime.tv_sec * 1000000ULL) + u.ru_utime.tv_usec +
         (u.ru_stime.tv_sec * 1000000ULL) + u.ru_stime.tv_usec;

}
static __u32  measure_preemption(__u32  target_ms) {

  static volatile __u32  v1, v2;

  __u64 st_t, en_t, st_c, en_c, real_delta, slice_delta;
  __s32 loop_repeats = 0;

  st_t = get_cur_time_us();
  st_c = get_cpu_usage_us();

repeat_loop:

  v1 = CTEST_BUSY_CYCLES ;

  while (v1--) v2++;
  sched_yield();

  en_t = get_cur_time_us();

  if (en_t - st_t < target_ms * 1000) {
    loop_repeats++;
    goto repeat_loop;
  }

  /* Let's see what percentage of this time we actually had a chance to
     run, and how much time was spent in the penalty box. */

  en_c = get_cpu_usage_us();

  real_delta  = (en_t - st_t) / 1000;
  slice_delta = (en_c - st_c) / 1000;

  return real_delta * 100 / slice_delta;

}

void checkcore(){
    
     __u32  cpu_cnt = sysconf(_SC_NPROCESSORS_ONLN),
      idle_cpus = 0, maybe_cpus = 0, i;
   
  for (i = 0; i < cpu_cnt; i++) {

    __s32 fr = fork();

    if (fr < 0) printf("fork failed");

    if (!fr) {

      cpu_set_t c;
      __u32  util_perc;

      CPU_ZERO(&c);
      CPU_SET(i, &c);

      if (sched_setaffinity(0, sizeof(c), &c))
        printf("sched_setaffinity failed for cpu %d", i);

      util_perc = measure_preemption(CTEST_CORE_TRG_MS);

      if (util_perc < 110) {

        printf("    Core #%u:  AVAILABLE     (%u%%)\n", i, util_perc);
        exit(0);

      } else if (util_perc < 250) {

        printf("    Core #%u:  CAUTION    (%u%%)\n", i, util_perc); 
        exit(1);

      }

      printf("    Core #%u:  OVERBOOKED    (%u%%)\n", i,util_perc);
      exit(2);

    }

  }





}
