#include<stdio.h> 
#include<stdlib.h>
#include<unistd.h>

int corecount = 2; //get from system !!!!!!
int cputhreshold1 = 50;    //attention
int cputhreshold2 = 80;    //warning
int cputhreshold3 = 90;    //alarm

float memthreshold1 = 0.5;    //attention
float memthreshold2 = 0.8;    //warning
float memthreshold3 = 0.9;    //alarm
float totalmem;

int iothreshold = 80;

void getmem();
void cpumonitor();
void loadmonitor();
void memmonitor();
void iodevicemonitor(int devicenum);
void filesysmonitor();
void netmonitor(int interfacenum);

int devicenum = 1;   //eth0 1;......
int iodevice = 1;//sda 1; sr0 2;......
int interfacenum=1;

int stopflag = 1;

  int main() 
 {  
    getmem();
    int coreavail =  sysconf(_SC_NPROCESSORS_ONLN);
    printf("Cores available:%d\n",coreavail);
    while(stopflag){
     cpumonitor();
     loadmonitor();
     memmonitor();
     iodevicemonitor(devicenum);
     filesysmonitor();
     netmonitor(interfacenum);
      printf("\n");
      sleep(3);
       
    }
 }

void getmem(){
     
     FILE * fp; 
    char buffer[80]; 
    fp=popen("free|grep Mem|awk '{print $2}'","r"); 
    fgets(buffer,sizeof(buffer),fp);
    totalmem = atof(buffer); 
    printf("Memory:%.0fkb\n",totalmem);
    pclose(fp); 
}

void cpumonitor(){
FILE * fp; 
    char buffer[80]; 
    fp=popen("top 1 -n 1|grep Cpu|awk '{print $9}'","r"); 
    int count = 0;
    float cpu,cpuused;
    while(fgets(buffer,sizeof(buffer),fp) != NULL){
     cpu = atof(buffer);
    if(!cpu)
        cpu=100;
     cpuused = 100 - cpu;
    //cpu threshold
    if( cpuused <= cputhreshold1){  //well
         printf("CPU%d works well.",count);
                                
    }
    else if(cpuused <= cputhreshold2){ //attention
           printf("Attention.");
        //email?
}
    else if(cpuused <= cputhreshold3){
        printf("Warning!");
        //email?
}
    else{
        printf("Alarm!");
        //email?
      }
   
    printf("CPU%d is %.1f%% used.\n",count,cpuused);
    count++;
  }
    pclose(fp); 
}

void loadmonitor(){
    FILE * fp; 
    char buffer[80]; 
    float loadavg1,loadavg5,loadavg15;
    fp=popen("cat /proc/loadavg|awk '{print $1}'","r"); 
    fgets(buffer,sizeof(buffer),fp);
    loadavg1=atof(buffer);
    printf("loadavg in 1 minute:%.2f.",loadavg1);
    if(loadavg1<=corecount*0.7)
       printf("Works well.\n");
    else{

       printf("High load!!!\n");
   }
    fp=popen("cat /proc/loadavg|awk '{print $2}'","r"); 
    fgets(buffer,sizeof(buffer),fp);
    loadavg5=atof(buffer);
    printf("loadavg in 5 minutes:%.2f.",loadavg5); 
    if(loadavg5<=corecount*0.7)
       printf("Works well.\n");
    else{

       printf("High load!!!\n");
   }
    fp=popen("cat /proc/loadavg|awk '{print $3}'","r"); 
    fgets(buffer,sizeof(buffer),fp);
    loadavg15=atof(buffer);
    printf("loadavg in 15 minutes:%.2f.",loadavg15);
    if(loadavg15<=corecount*0.7)
       printf("Works well.\n");
    else{

       printf("High load!!!\n");
   }
        pclose(fp); 

}

void memmonitor(){
   FILE * fp; 
    char buffer[80]; 
   //total is fixed,so just get the used part
    fp=popen("free|grep Mem|awk '{print $3}'", "r"); 
    fgets(buffer,sizeof(buffer),fp); 
    int memused = atoi(buffer);
    float mem = memused / totalmem;
    //mem threshold
    if( mem <= memthreshold1){  //well
         printf("Memory status: well.");
                                
    }
    else if( mem <= memthreshold2){ //attention
        printf("Attention.");
        //email?
}
    else if( mem <= memthreshold3){
        printf("Warning!");
        //email?
}
    else{
        printf("Alarm!");
        //email?
}
      float memratio = mem*100;
      printf("Memory is %.2f%% used.\n",memratio);
     
    pclose(fp); 
}

void filesysmonitor(){

    FILE * fp;
    char buffer[10]; 
    fp=popen("df -h|awk '{ if(NR>1){print $1,$4}}'","r");
     printf("File System Space available:\n");
     while(fgets(buffer,sizeof(buffer),fp) != NULL){
	printf("%s",buffer);
}
     pclose(fp);
}

void iodevicemonitor(int devicenum)
 {  
    FILE * fp; 
    char buffer[10]; 
    switch(devicenum){
     case 1: fp=popen("iostat -x|grep sda|awk '{print $23}'","r");  break;
     case 2: fp=popen("iostat -x|grep sr0|awk '{print $23}'","r");  break;
    }
    fgets(buffer,sizeof(buffer),fp); 
    float util = atof(buffer);
    if(util<=80){
    printf("The device works well!");
  }
    else
    {
    printf("The device is busy with I/O operations!");
     }
    printf("util:%.2f%%\n",util);
     
    pclose(fp); 
  }

void netmonitor(int interfacenum){

     FILE * fp; 
    char TX[80],RX[80]; 
    fp=popen("ifconfig eth0|grep dropped|tail -n 1|awk '{print $5}'","r");   // change the interface  
    fgets(TX,sizeof(TX),fp); 
    fp=popen("ifconfig eth0|grep dropped|head -n 1|awk '{print $5}'","r"); 
    fgets(RX,sizeof(RX),fp); 
    int TXdrop = atoi(TX);
    int RXdrop = atoi(RX);
    // printf("%d",TXdrop);
     //printf("%d",RXdrop);
   
    if(TXdrop){
    printf("eth0 has dropped packets (TX)!");
  }
    if(RXdrop){
    printf("eth0 has dropped packets (RX)!");
}
    
    if(!TXdrop&&!RXdrop){
    printf("eth0 works well.\n");
   }
    pclose(fp); 
}

