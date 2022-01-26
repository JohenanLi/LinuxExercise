/* File Name:  monitor.h*/
#define CTEST_CORE_TRG_MS 1000
#define CTEST_BUSY_CYCLES (10 * 1000 * 1000)

int corecount; //cpu总核数

float totalmem; //总内存大小
void getTime();
void checkCore();
void getMem();          //获取内存大小
void getCpuinfo();      //获取cpu总核数、cpu类型
void loadMonitor();     //负载
void Memmonitor();      //内存监控
void Iodevicemonitor(); //io设备监测
void Filesysmonitor();  //文件系统监测
void Netmonitor();      //网卡eth0
void getModule();
void getSysinfo();

int stopflag = 1;

char date[100];
char cpuinfo[20][100]; //存放getcpuinfo（）读到信息
char meminfo[80][100]; //存放getmem（）读到信息
char filesys[30][100]; //存放filesysmonitor（）读到信息
char io[20][100];      //存放iodevicemonitor（）读到信息
char net[30][80];      //存放netmonitor（）读到信息
char mod[500][150];
char sysinfo[80][100];
char temp[30];
char divide[80] = "------------------------------\n";
static __u32 measure_preemption(__u32 target_ms);
FILE *fpmonitor = NULL;
/*
FunName:getFileAll
   Desc:get the file content
   Para:[fname] filename pointer
 Return:1.[*pBuf] file content pointer
        2.[*length] file length
*/
char *getFileAll(char *fname, int *length)
{
  int fileLight = 0;
  char *pBuf; //定义文件指针
  FILE *pFile;

  pFile = fopen(fname, "r"); //获取文件的指针
  if (pFile == NULL)
  {
    printf("\nOpen file %s fail\n", pFile);
    return NULL;
  }

  fseek(pFile, 0, SEEK_END); //把指针移动到文件的结尾 ，获取文件长度
  fileLight = ftell(pFile);  //获取文件长度
  pBuf = (char *)malloc(fileLight);
  rewind(pFile);                    //把指针移动到文件开头 因为我们一开始把指针移动到结尾，如果不移动回来 会出错
  fread(pBuf, 1, fileLight, pFile); //读文件
  pBuf[fileLight] = 0;              //把读到的文件最后一位 写为0 要不然系统会一直寻找到0后才结束
  fclose(pFile);                    // 关闭文件
  *length = fileLight;
  return pBuf;
}

void *monitor()
{
  while (stopflag)
  {
    getMem();
    getCpuinfo();
    // printf("\n");
    getTime();
    // printf("\n");
    loadMonitor();
    // printf("------------------------------\n");
    Memmonitor();
    // printf("------------------------------\n");
    Iodevicemonitor();
    // printf("------------------------------\n");
    Filesysmonitor();
    // printf("------------------------------\n");
    Netmonitor();
    // printf("------------------------------\n");
    getModule();
    // printf("------------------------------\n");
    getSysinfo();
    // printf("------------------------------\n");
    checkCore();
    sleep(3);
  }
  pthread_exit(0);
}

void getTime()
{
  fpmonitor = fopen("monitor.txt", "a+");
  fputs("\n\n", fpmonitor);
  FILE *fp;
  int i = 0;
  fp = popen("date", "r");
  fgets(date, sizeof(date), fp);
  fputs(date, fpmonitor);
  pclose(fp);
  fclose(fpmonitor);
}

void getCpuinfo()
{

  fpmonitor = fopen("monitor.txt", "a+");
  FILE *fp;

  int i = 0;
  fp = popen("cat /proc/cpuinfo| grep 'physical id'| sort| uniq| wc -l", "r"); //读取物理cpu个数
  fgets(cpuinfo[i], sizeof(cpuinfo[i]), fp);
  char phynum[30] = "物理cpu个数:";
  strcat(phynum, cpuinfo[i]);
  fputs(phynum, fpmonitor);
  int phycpu = atoi(cpuinfo[i]);
  i++;
  fp = popen("cat /proc/cpuinfo| grep 'cpu cores'| uniq|awk '{print $4}'", "r"); //读取每个物理cpu的核数
  fgets(cpuinfo[i], sizeof(cpuinfo[i]), fp);
  char percore[30] = "每个物理cpu的核数:";
  strcat(percore, cpuinfo[i]);
  fputs(percore, fpmonitor);
  int perphycore = atoi(cpuinfo[i]);
  i++;
  corecount = phycpu * perphycore; //总核数
  char str[50];
  sprintf(str, "%d", corecount);
  char cputotal[30] = "cpu总核数:";
  strcat(cputotal, str);
  fputs(cputotal, fpmonitor);
  fputs("\n", fpmonitor);
  int coreavail = sysconf(_SC_NPROCESSORS_ONLN); //读取当前可用核数
  sprintf(str, "%d", coreavail);
  char cpuavail[30] = "当前可用核数:";
  strcat(cpuavail, str);
  fputs(cpuavail, fpmonitor);
  fputs("\n", fpmonitor);
  i++;
  fp = popen("cat /proc/cpuinfo | grep name | cut -f2 -d: | uniq -c", "r"); //cpu型号等信息读取
  fgets(cpuinfo[i], sizeof(cpuinfo[i]), fp);
  char cputype[30] = "cpu型号信息:";
  strcat(cputype, cpuinfo[i]);
  fputs(cputype, fpmonitor);
  // printf("Cores in total: %d\n", corecount);
  // printf("Cores available:%d\n", coreavail);
  // printf("CPU info:%s\n", cpuinfo[i]);
  pclose(fp);
  fclose(fpmonitor);
}

void getMem()
{
  fpmonitor = fopen("monitor.txt", "w+");
  FILE *fp;
  char buffer[80];
  fp = popen("free|grep Mem|awk '{print $2}'", "r"); //总内存大小
  fgets(buffer, sizeof(buffer), fp);
  char memtotal[30] = "总内存大小:";
  strcat(memtotal, buffer);
  fputs(memtotal, fpmonitor);
  totalmem = atof(buffer);
  // printf("Memory:%.0fkb\n", totalmem);
  pclose(fp);
  fclose(fpmonitor);
}

void loadMonitor()
{
  fpmonitor = fopen("monitor.txt", "a+");
  FILE *fp;
  char buffer[80];
  float loadavg1, loadavg5, loadavg15;
  fp = popen("cat /proc/loadavg|awk '{print $1}'", "r"); //1分钟负载
  fgets(buffer, sizeof(buffer), fp);
  char loadone[30] = "1分钟负载:";
  strcat(loadone, buffer);
  fputs(loadone, fpmonitor);
  loadavg1 = atof(buffer);
  // printf("loadavg in 1 minute:%.2f.\n", loadavg1);
  fp = popen("cat /proc/loadavg|awk '{print $2}'", "r"); //5分钟负载
  char loadfive[30] = "5分钟负载:";
  strcat(loadfive, buffer);
  fputs(loadfive, fpmonitor);
  fgets(buffer, sizeof(buffer), fp);
  loadavg5 = atof(buffer);
  // printf("loadavg in 5 minutes:%.2f.\n", loadavg5);
  fp = popen("cat /proc/loadavg|awk '{print $3}'", "r"); //15分钟负载
  char loadfif[30] = "15分钟负载:";
  strcat(loadfif, buffer);
  fputs(loadfif, fpmonitor);
  fgets(buffer, sizeof(buffer), fp);
  loadavg15 = atof(buffer);
  // printf("loadavg in 15 minutes:%.2f.\n", loadavg15);
  pclose(fp);
  fputs(divide, fpmonitor);
  fclose(fpmonitor);
}

void Memmonitor()
{
  fpmonitor = fopen("monitor.txt", "a+");
  fputs("内存信息:\n", fpmonitor);
  FILE *fp;
  int i = 0;
  fp = popen("cat /proc/meminfo", "r"); //内存信息 一行一行读进二维字符数组
  while (fgets(meminfo[i], sizeof(meminfo[i]), fp) != NULL)
  {
    // printf("%s", meminfo[i]);
    fputs(meminfo[i], fpmonitor);
    i++;
  }
  pclose(fp);
  fputs(divide, fpmonitor);
  fclose(fpmonitor);
}

void Filesysmonitor()
{
  fpmonitor = fopen("monitor.txt", "a+");
  fputs("文件系统信息:\n", fpmonitor);
  FILE *fp;
  int i = 0;
  fp = popen("df -h", "r"); //文件系统信息 一行一行读进二维字符数组
  while (fgets(filesys[i], sizeof(filesys[i]), fp) != NULL)
  {
    // printf("%s", filesys[i]);
    fputs(filesys[i], fpmonitor);
    i++;
  }
  pclose(fp);
  fputs(divide, fpmonitor);
  fclose(fpmonitor);
}

void Iodevicemonitor()
{
  fpmonitor = fopen("monitor.txt", "a+");
  fputs("输入输出设备信息:\n", fpmonitor);
  FILE *fp;
  int i = 0;
  fp = popen("iostat -x|grep -A 5 'Device'", "r"); //输入输出设备信息 一行一行读进二维字符数组
  while (fgets(io[i], sizeof(io[i]), fp) != NULL)
  {
    // printf("%s", io[i]);
    fputs(io[i], fpmonitor);
    i++;
  }
  pclose(fp);
  fputs(divide, fpmonitor);
  fclose(fpmonitor);
}

void Netmonitor()
{
  fpmonitor = fopen("monitor.txt", "a+");
  fputs("eth0信息:\n", fpmonitor);
  FILE *fp;
  int i = 0;
  fp = popen("ifconfig eth0", "r"); //eth0信息 一行一行读进二维字符数组
  while (fgets(net[i], sizeof(net[i]), fp) != NULL)
  {
    // printf("%s", net[i]);
    fputs(net[i], fpmonitor);
    i++;
  }
  pclose(fp);
  fputs(divide, fpmonitor);
  fclose(fpmonitor);
}

static __u32 get_cur_time_us(void)
{

  struct timeval tv;
  struct timezone tz;

  gettimeofday(&tv, &tz);

  return (tv.tv_sec * 1000000ULL) + tv.tv_usec;
}

static __u64 get_cpu_usage_us(void)
{

  struct rusage u;

  getrusage(RUSAGE_SELF, &u);

  return (u.ru_utime.tv_sec * 1000000ULL) + u.ru_utime.tv_usec +
         (u.ru_stime.tv_sec * 1000000ULL) + u.ru_stime.tv_usec;
}
static __u32 measure_preemption(__u32 target_ms)
{

  static volatile __u32 v1, v2;

  __u64 st_t, en_t, st_c, en_c, real_delta, slice_delta;
  __s32 loop_repeats = 0;

  st_t = get_cur_time_us();
  st_c = get_cpu_usage_us();

repeat_loop:

  v1 = CTEST_BUSY_CYCLES;

  while (v1--)
    v2++;
  sched_yield();

  en_t = get_cur_time_us();

  if (en_t - st_t < target_ms * 1000)
  {
    loop_repeats++;
    goto repeat_loop;
  }

  /* Let's see what percentage of this time we actually had a chance to
     run, and how much time was spent in the penalty box. */

  en_c = get_cpu_usage_us();

  real_delta = (en_t - st_t) / 1000;
  slice_delta = (en_c - st_c) / 1000;

  return real_delta * 100 / slice_delta;
}

void checkCore()
{
  fpmonitor = fopen("monitor.txt", "a");

  __u32 cpu_cnt = sysconf(_SC_NPROCESSORS_ONLN),
        idle_cpus = 0, maybe_cpus = 0, i;

  for (i = 0; i < cpu_cnt; i++)
  {

    __s32 fr = fork();

    if (fr < 0)
      printf("monitor thread checkCore fork failed\n");

    if (!fr)
    {

      cpu_set_t c;
      __u32 util_perc;

      CPU_ZERO(&c);
      CPU_SET(i, &c);

      if (sched_setaffinity(0, sizeof(c), &c))
        printf("sched_setaffinity failed for cpu %d\n", i);

      util_perc = measure_preemption(CTEST_CORE_TRG_MS);

      if (util_perc < 110)
      {

        // printf("    Core #%u:  AVAILABLE     (%u%%)\n", i, util_perc);
        fprintf(fpmonitor, "%s%u%s%u%%%s\n", "    Core #", i, ":  AVAILABLE     (", util_perc, ")");
        exit(0);
      }
      else if (util_perc < 250)
      {

        // printf("    Core #%u:  CAUTION    (%u%%)\n", i, util_perc);
        fprintf(fpmonitor, "%s%u%s%u%%%s\n", "    Core #", i, ":  CAUTION     (", util_perc, ")");
        exit(1);
      }

      // printf("    Core #%u:  OVERBOOKED    (%u%%)\n", i, util_perc);
      fprintf(fpmonitor, "%s%u%s%u%%%s\n", "    Core #", i, ":  OVERBOOKED     (", util_perc, ")");
      exit(2);
    }
  }

  fclose(fpmonitor);
}

void getModule()
{
  fpmonitor = fopen("monitor.txt", "a+");
  fputs(" 已加载模块列表:\n", fpmonitor);
  FILE *fp;
  int i = 0;
  fp = popen("lsmod", "r");
  while (fgets(mod[i], sizeof(mod[i]), fp) != NULL)
  {
    // printf("%s", mod[i]);
    fputs(mod[i], fpmonitor);
    i++;
  }
  pclose(fp);
  fputs(divide, fpmonitor);
  fclose(fpmonitor);
}

void getSysinfo()
{
  fpmonitor = fopen("monitor.txt", "a+");
  fputs("系统信息:\n", fpmonitor);
  FILE *fp;
  int i = 0;
  fp = popen("uname -a ", "r");
  fgets(sysinfo[i], sizeof(sysinfo[i]), fp);
  char kernel[150] = "内核版本:";
  strcat(kernel, sysinfo[i]);
  fputs(kernel, fpmonitor);
  // printf("%s", sysinfo[i]);
  i++;
  fp = popen("last|head -n 10 ", "r");
  char login[100] = "\n最近5次登录用户、登录时间:\n";
  fputs(login, fpmonitor);
  while (fgets(sysinfo[i], sizeof(sysinfo[i]), fp) != NULL)
  {
    // printf("%s", sysinfo[i]);
    fputs(sysinfo[i], fpmonitor);
    i++;
  }
  char history[50] = "最近5条被使用过的命令:\n";
  fputs(history, fpmonitor);
  fp = popen("cat  ~/.bash_history|tail -n 6 ", "r");
  fgets(temp, sizeof(temp), fp);
  if (fgets(sysinfo[i], sizeof(sysinfo[i]), fp) != NULL)
  {
    // printf("%s", sysinfo[i]);
    fputs(sysinfo[i], fpmonitor);
  }
  else
  {
    fp = popen("cat ~/.zsh_history|tail -n 5 ", "r");
  }

  while (fgets(sysinfo[i], sizeof(sysinfo[i]), fp) != NULL)
  {
    // printf("%s", sysinfo[i]);
    fputs(sysinfo[i], fpmonitor);
  }
  fputs(divide, fpmonitor);
  fclose(fpmonitor);
}
