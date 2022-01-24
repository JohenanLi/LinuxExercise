from PySide6.QtCore import QObject,Signal
from PySide6.QtCore import QTimer,QObject,Signal
from time import strftime,localtime
import json
from LinuxTest import randomcolor,fileSocket
import re
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
    #计时器设置
    def __init__(self) -> None:
        super().__init__()
        self.getAllInfo()
        self.timer = QTimer()
        self.timer.setInterval(1000)
        self.timer.timeout.connect(self.update_time)
        self.timer.timeout.connect(self.getAllInfo)
        self.timer.start()
    def update_time(self):
        curr_time = strftime("%H:%M:%S",localtime())
        self.updated.emit(curr_time)
    #获取各类信息并且形成json格式
    def getAllInfo(self):
        fileSocket()
        allInfoFileDes = open("./textrecv/monitor.txt","r")
        allInfoStr = allInfoFileDes.read(102400)
        allInfoLines = allInfoStr.split("------------------------------\n")

        for i in range(len(allInfoLines)):
            match i:
                case 0:
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
                case 1:
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
                case 2:
                    pass
                case 3:
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

                case 4:
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
                case 5:
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
                case 6:
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
                    tempDict['name']=systemInfoList[3]
                    tempDict["value"] = "\n".join(systemInfoList[4:13])
                    tempDict['colorCode'] = randomcolor()
                    systemInfoDict["ListElement"+str(k)] = tempDict
                    k+= 1
                    tempDict = {}
                    tempDict['name']=systemInfoList[14]
                    tempDict["value"] = "\n".join(systemInfoList[15:])
                    tempDict['colorCode'] = randomcolor()
                    systemInfoDict["ListElement"+str(k)] = tempDict
                    self.systemInfo.emit(json.dumps(systemInfoDict))
                case 7:
                    cpuAvailCount  = re.findall("AVAILABLE",allInfoLines[i])
                    self.cpuCount.emit(cpuAvailCount)

                    

