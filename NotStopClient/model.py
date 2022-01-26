import socket
from PySide6.QtCore import QTimer,QObject,Signal,Slot
from PySide6.QtQml import QmlElement
from time import strftime,localtime,sleep
import json
from LinuxTest import randomcolor
import re
send_cmd = "在这里输入命令"
class Backend(QObject):

    #设置各类信号量
    updated = Signal(str,arguments=['time'])
    memoryInfo = Signal(str)
    cpuLoadInfo = Signal(str)
    diskInfo = Signal(str)
    kernelModuleInfo = Signal(str)
    systemInfo = Signal(str)
    networkInfo = Signal(str)
    cpuCount = Signal(int)
    cmdResult = Signal(str)
    #计时器设置
    def __init__(self) -> None:
        super().__init__()
        self.client = socket.socket()

        #链接地址和端口,元组(本地，端口)
        self.client.connect(('47.118.34.75',50007))
        self.execsocket = socket.socket()
        
        self.timer = QTimer()
        self.timer.setInterval(1000)
        self.timer.timeout.connect(self.update_time)
        self.timer.timeout.connect(self.getAllInfo)
        self.timer.timeout.connect(self.exec)
        self.timer.start()
    def update_time(self):
        curr_time = strftime("%H:%M:%S",localtime())
        self.updated.emit(curr_time)                                                                 
    def exec(self):
        global send_cmd                                                
        if send_cmd != "在这里输入命令":
            self.timer.stop()
            print(self.client.detach())
            self.execsocket.connect(('47.118.34.75',50007))
            cmd = "command:"+send_cmd

            #发送数据 b将字符串转为bys类型
            self.execsocket.send(cmd.encode("utf-8")) #send只能发送bytes格式数据
            #接收服务器端的返回(长度和详细内容)，需要声明收多少，默认1024字节
             
            # cmd_res = self.client.recv(102400)
            new_res = self.execsocket.recv(4096)
            received_data = new_res.decode("utf-8")
            print(received_data)
            self.cmdResult.emit(received_data)
            self.timer.start()
            
        # else:
        #     if self.timer.isActive():
        #         pass
        #     else:
        #         self.timer.timeout.connect(self.getAllInfo)
    def fileSocket(self):
        self.client.send("getInfo:".encode("utf-8")) #send只能发送bytes格式数据
        received_data = self.client.recv(102400)
        with open("textrecv/monitor.txt","w") as f:
            f.write(received_data.decode('utf-8',errors="ignore"))
            f.close()
        
        print("file ok")
    #获取各类信息并且形成json格式
    def getAllInfo(self):
        # self.fileSocket()
        
        allInfoFileDes = open("./textrecv/monitor.txt","r")
        allInfoStr = allInfoFileDes.read(102400)
        allInfoLines = allInfoStr.split("------------------------------\n")

        for i in range(len(allInfoLines)):
            try:
                if i==0:
                    staticInfoList = allInfoLines[i].split("\n")
                    staticInfoDict = {}
                    k = 0
                    for j in {0,1,2,3,4,5,9,10,11}:
                        tempDict = {}
                        tempList = staticInfoList[j].split(":")
                        tempDict['name']=tempList[0]
                        tempDict["value"] = tempList[1]
                        tempDict['colorCode'] = randomcolor()
                        staticInfoDict["ListElement"+str(k)] = tempDict
                        k+= 1
                    staticInfoDict["ListElement"+str(k)] = {'name':"数据获取时间","value":staticInfoList[8],"colorCode":randomcolor()}
                    # print(json.dumps(staticInfoDict))
                    self.cpuLoadInfo.emit(json.dumps(staticInfoDict))
                elif i== 1:
                    memoryInfoList = allInfoLines[i].split("\n")
                    memoryDict = {}
                    k=0
                    for j in range(1,len(memoryInfoList)-1):
                        tempDict = {}
                        tempList = memoryInfoList[j].split(":")
                        tempDict['name']=tempList[0]
                        tempDict["value"] = tempList[1]
                        tempDict['colorCode'] = randomcolor()
                        memoryDict["ListElement"+str(k)] = tempDict
                        k+= 1
                    self.memoryInfo.emit(json.dumps(memoryDict))
                elif i== 2:
                    pass
                elif i== 3:
                    diskInfoList = allInfoLines[i].split("\n")
                    diskInfoDict = {}
                    k=0
                    for j in range(1,len(diskInfoList)-1):
                        tempDict = {}
                        tempDict['name']=""
                        tempDict["value"] = diskInfoList[j]
                        tempDict['colorCode'] = randomcolor()
                        diskInfoDict["ListElement"+str(k)] = tempDict
                        k+= 1
                    self.diskInfo.emit(json.dumps(diskInfoDict))

                elif i== 4:
                    networkiostDict = {}
                    networkList = allInfoLines[i].split("\n")
                    networkDict = {"name":networkList[0],"value":"\n".join(networkList[1:]),"colorCode":randomcolor()}
                    k=0
                    networkiostDict["ListElement"+str(k)] = networkDict
                    k+= 1
                    iostList = allInfoLines[2].split("\n")
                    iostDict = {"name":iostList[0],"value":"\n".join(iostList[1:]),"colorCode":randomcolor()}
                    networkiostDict["ListElement"+str(k)] = iostDict
                    self.networkInfo.emit(json.dumps(networkiostDict))
                elif i== 5:
                    kernelModuleList = allInfoLines[i].split("\n")
                    kernelModuleDict = {}
                    k=0
                    for j in range(1,len(kernelModuleList)-1):
                        tempDict = {}
                        tempDict['name']=""
                        tempDict["value"] = kernelModuleList[j]
                        tempDict['colorCode'] = randomcolor()
                        kernelModuleDict["ListElement"+str(k)] = tempDict
                        k+= 1
                    self.kernelModuleInfo.emit(json.dumps(kernelModuleDict))
                elif i== 6:
                    systemInfoList = allInfoLines[i].split("\n")
                    systemInfoDict = {}
                    k=0
                    tempList = systemInfoList[1].split(":")
                    tempDict = {}
                    tempDict['name']=tempList[0]
                    tempDict["value"] = tempList[1]
                    tempDict['colorCode'] = randomcolor()
                    systemInfoDict["ListElement"+str(k)] = tempDict
                    
                    k+= 1
                    tempDict = {}
                    tempDict['name']=systemInfoList[2]
                    tempDict["value"] = "\n".join(systemInfoList[3:12])
                    tempDict['colorCode'] = randomcolor()
                    systemInfoDict["ListElement"+str(k)] = tempDict
                    k+= 1
                    tempDict = {}
                    tempDict['name']=systemInfoList[13]
                    tempDict["value"] = "\n".join(systemInfoList[14:])
                    tempDict['colorCode'] = randomcolor()
                    systemInfoDict["ListElement"+str(k)] = tempDict
                    self.systemInfo.emit(json.dumps(systemInfoDict))
                elif i== 7:
                    cpuAvailCount  = re.findall("AVAILABLE",allInfoLines[i])
                    self.cpuCount.emit(cpuAvailCount)
            except Exception as e:
                print("数据获取异常")
                # exit(-1)
        self.fileSocket()

                    

QML_IMPORT_NAME = "examples.signals.qmltopy1"
QML_IMPORT_MAJOR_VERSION = 1


@QmlElement
class Console(QObject):
    """Output stuff on the console."""

    @Slot(str)
    def output(self, s):
        global send_cmd
        send_cmd = s

