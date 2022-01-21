#include<stdio.h> 
#include<stdlib.h>
#include<unistd.h>

int corecount; //cpu�ܺ���

float totalmem;  //���ڴ��С

void getmem();   //��ȡ�ڴ��С
void getcpu();    //��ȡcpu�ܺ�����cpu����
void loadmonitor(); //����
void memmonitor();   //�ڴ���
void iodevicemonitor(); //io�豸���
void filesysmonitor();  //�ļ�ϵͳ���
void netmonitor();    //����eth0

int stopflag = 1;
char cpuinfo[20][100];   //���getcpu����������Ϣ
char meminfo[60][100];   //���getmem����������Ϣ
char filesys[20][100];  //���filesysmonitor����������Ϣ
char io[8][100];        //���iodevicemonitor����������Ϣ
char net[30][80];       //���netmonitor����������Ϣ


int main()
{
	getmem();
	getcpu();
	// int coreavail = sysconf(_SC_NPROCESSORS_ONLN);  //��ȡ��ǰ���ú���
	// printf("Cores available:%d\n", coreavail);
	// while (stopflag) {
	// 	loadmonitor();
	// 	memmonitor();
	// 	iodevicemonitor();
	// 	filesysmonitor();
	// 	netmonitor();
	// 	printf("\n");
	// 	sleep(3);

	// }
}

void getcpu() {
	FILE * fp;

	int i = 0;
	fp = popen("cat /proc/cpuinfo| grep 'physical id'| sort| uniq| wc -l", "r");  //��ȡ����cpu����
	fgets(cpuinfo[i], sizeof(cpuinfo[i]), fp);
	int phycpu = atoi(cpuinfo[i]);
	i++;
	fp = popen("cat /proc/cpuinfo| grep 'cpu cores'| uniq|awk '{print $4}'", "r");  //��ȡÿ������cpu�ĺ���
	fgets(cpuinfo[i], sizeof(cpuinfo[i]), fp);
	int perphycore = atoi(cpuinfo[i]);
	i++;
	corecount = phycpu * perphycore;  //�ܺ���
	fp = popen("cat /proc/cpuinfo | grep name | cut -f2 -d: | uniq -c", "r");  //cpu�ͺŵ���Ϣ��ȡ
	fgets(cpuinfo[i], sizeof(cpuinfo[i]), fp);

	printf("Cores in total: %d\n", corecount);
	printf("CPU info:%s\n", cpuinfo[i]);
	pclose(fp);

}

void getmem() {

	FILE * fp;
	char buffer[80];
	fp = popen("free|grep Mem|awk '{print $2}'", "r");   //���ڴ��С
	fgets(buffer, sizeof(buffer), fp);
	totalmem = atof(buffer);
	printf("Memory:%.0fkb\n", totalmem);
	pclose(fp);
}


void loadmonitor() {
	FILE * fp;
	char buffer[80];
	float loadavg1, loadavg5, loadavg15;
	fp = popen("cat /proc/loadavg|awk '{print $1}'", "r");   //1���Ӹ���
	fgets(buffer, sizeof(buffer), fp);
	loadavg1 = atof(buffer);
	printf("loadavg in 1 minute:%.2f.\n", loadavg1);
	fp = popen("cat /proc/loadavg|awk '{print $2}'", "r");  //5���Ӹ���
	fgets(buffer, sizeof(buffer), fp);
	loadavg5 = atof(buffer);
	printf("loadavg in 5 minutes:%.2f.\n", loadavg5);
	fp = popen("cat /proc/loadavg|awk '{print $3}'", "r"); //15���Ӹ���
	fgets(buffer, sizeof(buffer), fp);
	loadavg15 = atof(buffer);
	printf("loadavg in 15 minutes:%.2f.\n", loadavg15);
	pclose(fp);

}

void memmonitor() {
	FILE * fp;

	int i = 0;
	fp = popen("cat /proc/meminfo", "r");    //�ڴ���Ϣ һ��һ�ж�����ά�ַ�����
	while (fgets(meminfo[i], sizeof(meminfo[i]), fp) != NULL) {
		printf("%s", meminfo[i]);
		i++;

	}
	pclose(fp);
}

void filesysmonitor() {

	FILE * fp;

	int i = 0;
	fp = popen("df -h", "r");      //�ļ�ϵͳ��Ϣ һ��һ�ж�����ά�ַ�����
	while (fgets(filesys[i], sizeof(filesys[i]), fp) != NULL) {
		printf("%s", filesys[i]);
		i++;
	}
	pclose(fp);
}

void iodevicemonitor()
{
	FILE * fp;

	int i = 0;
	fp = popen("iostat -x|grep -A 5 'Device'", "r");   //��������豸��Ϣ һ��һ�ж�����ά�ַ����� 
	while (fgets(io[i], sizeof(io[i]), fp) != NULL) {
		printf("%s", io[i]);
		i++;
	}


	pclose(fp);
}

void netmonitor() {

	FILE * fp;
	int i = 0;
	fp = popen("ifconfig eth0", "r");    //eth0��Ϣ һ��һ�ж�����ά�ַ�����
	while (fgets(net[i], sizeof(net[i]), fp) != NULL) {
		printf("%s", net[i]);
		i++;
	}
	pclose(fp);
}

