from random import randint
from subprocess import getoutput,Popen
from tkinter import S

from simplejson import load
import re
import json
import socket
from time import sleep
def randomcolor():
    colorlist = ['aliceblue', 'antiquewhite', 'aqua', 'aquamarine', 'azure', 
    'beige', 'bisque', 'black', 'blanchedalmond', 'blue', 'blueviolet', 'brown', 
    'burlywood', 'cadetblue', 'chartreuse', 'chocolate', 'coral', 
    'cornflowerblue', 'cornsilk', 'crimson', 'cyan', 'darkblue',
     'darkcyan', 'darkgoldenrod', 'darkgray', 'darkgreen', 'darkgrey', 
     'darkkhaki', 'darkmagenta', 'darkolivegreen', 'darkorange', 'darkorchid', 
     'darkred', 'darksalmon', 'darkseagreen', 'darkslateblue', 'darkslategray', 
     'darkslategrey', 'darkturquoise', 'darkviolet', 'deeppink', 'deepskyblue', 
     'dimgray', 'dimgrey', 'dodgerblue', 'firebrick', 'floralwhite', 'forestgreen', 
     'fuchsia', 'gainsboro', 'ghostwhite', 'gold', 'goldenrod', 'gray', 'grey', 'green', 
     'greenyellow', 'honeydew', 'hotpink', 'indianred', 'indigo', 'ivory', 'khaki', 'lavender', 
     'lavenderblush', 'lawngreen', 'lemonchiffon', 'lightblue', 'lightcoral', 'lightcyan', 
     'lightgoldenrodyellow', 'lightgray', 'lightgreen', 'lightgrey', 'lightpink', 'lightsalmon', 
     'lightseagreen', 'lightskyblue', 'lightslategray', 'lightslategrey', 'lightsteelblue',
      'lightyellow', 'lime', 'limegreen', 'linen', 'magenta', 'maroon', 'mediumaquamarine', 
      'mediumblue', 'mediumorchid', 'mediumpurple', 'mediumseagreen', 'mediumslateblue', 
      'mediumspringgreen', 'mediumturquoise', 'mediumvioletred', 'midnightblue', 'mintcream', 
      'mistyrose', 'moccasin', 'navajowhite', 'navy', 'oldlace', 'olive', 'olivedrab', 'orange', 
      'orangered', 'orchid', 'palegoldenrod', 'palegreen', 'paleturquoise', 'palevioletred', 
      'papayawhip', 'peachpuff', 'peru', 'pink', 'plum', 'powderblue', 'purple', 'red', 'rosybrown', 
      'royalblue', 'saddlebrown', 'salmon', 'sandybrown', 'seagreen', 'seashell', 'sienna', 'silver', 
      'skyblue', 'slateblue', 'slategray', 'slategrey', 'snow', 'springgreen', 'steelblue', 'tan', 'teal', 
      'thistle', 'tomato', 'turquoise', 'violet', 'wheat', 'white', 'whitesmoke', 'yellow', 'yellowgreen']
    return colorlist[randint(0,len(colorlist)-1)]

def getAllInfo():
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
                    tempDict['color'] = randomcolor()
                    staticInfoDict["ListElement"+str(k)] = tempDict
                    k+= 1
                staticInfoDict["ListElement"+str(k+1)] = {'name':"数据获取时间","value":staticInfoList[8],"color":randomcolor()}

            case 1:
                memoryInfoList = allInfoLines[i].split("\n")
                memoryDict = {}
                k=0
                for j in range(1,len(memoryInfoList)-1):
                    tempDict = {}
                    tempList = memoryInfoList[j].split(":")
                    tempDict['name']=tempList[0]
                    tempDict["value"] = tempList[1]
                    tempDict['color'] = randomcolor()
                    memoryDict["ListElement"+str(k)] = tempDict
                    k+= 1
            case 2:
                iostList = allInfoLines[i].split("\n")
                iostDict = {"name":iostList[0],"value":"\n".join(iostList[1:]),"color":randomcolor()}
            
            case 3:
                diskInfoList = allInfoLines[i].split("\n")
                diskInfoDict = {}
                k=0
                for j in range(1,len(diskInfoList)-1):
                    tempDict = {}
                    tempDict['name']=""
                    tempDict["value"] = diskInfoList[j]
                    tempDict['color'] = randomcolor()
                    diskInfoDict["ListElement"+str(k)] = tempDict
                    k+= 1

            case 4:
                networkList = allInfoLines[i].split("\n")
                networkDict = {"name":networkList[0],"value":"\n".join(networkList[1:]),"color":randomcolor()}
            
            case 5:
                kernelModuleList = allInfoLines[i].split("\n")
                kernelModuleDict = {}
                k=0
                for j in range(1,len(kernelModuleList)-1):
                    tempDict = {}
                    tempDict['name']=""
                    tempDict["value"] = kernelModuleList[j]
                    tempDict['color'] = randomcolor()
                    kernelModuleDict["ListElement"+str(k)] = tempDict
                    k+= 1

            case 6:
                systemInfoList = allInfoLines[i].split("\n")
                systemInfoDict = {}
                k=0
                tempList = systemInfoList[1].split(":")
                tempDict = {}
                tempDict['name']=tempList[0]
                tempDict["value"] = tempList[1]
                tempDict['color'] = randomcolor()
                systemInfoDict["ListElement"+str(k)] = tempDict
                k+= 1
                tempDict = {}
                tempDict['name']=systemInfoList[3]
                tempDict["value"] = "\n".join(systemInfoList[4:13])
                tempDict['color'] = randomcolor()
                systemInfoDict["ListElement"+str(k)] = tempDict
                k+= 1
                tempDict = {}
                tempDict['name']=systemInfoList[14]
                tempDict["value"] = "\n".join(systemInfoList[15:])
                tempDict['color'] = randomcolor()
                systemInfoDict["ListElement"+str(k)] = tempDict
                # print(systemInfoDict)
            case 7:
                cpuAvailCount  = re.findall("AVAILABLE",allInfoLines[i])
                print(len(cpuAvailCount))


def getLoadAvg():
    b = getoutput("cat /proc/loadavg")
    pattern = re.compile('\d\.\d*[1-9]')
    newB = pattern.findall(b)
    return newB

def getMemoryInfo():
    cmdInfo = getoutput("cat /proc/meminfo")
    cmdSplitInfo = cmdInfo.splitlines()
    memoryDict = {}
    i = 0
    for one in cmdSplitInfo[0:8]:
        tempDict = {}
        tempList = one.split(":")
        tempDict['name']=tempList[0]
        tempDict["value"] = tempList[1]
        memoryDict["ListElement"+str(i)] = tempDict
        i+= 1
    
    # print(memoryDict)
    return json.dumps(memoryDict)

def socketConnection():
    HOST = "127.0.0.1"
    PORT = 50007
    while True:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((HOST, PORT))
            print('connect ok')
            cmd = input()
            bytcmd = 'command:%s'%cmd
            buffer = ""
            s.send(bytes(bytcmd,encoding="utf-8"))
            # while True:
            #     data= s.recv(1024)
            #     if data:
            #         buffer += data.decode("utf-8",errors='ignore')
            #         print(buffer)
            #     else:
            #         break
                
            s.close()
            
            print(buffer)
            
def lastSocketConnection():
    client = socket.socket()

    #链接地址和端口,元组(本地，端口)
    client.connect(('127.0.0.1',50007))

    #使用input循环向服务端发送请求
    while True:
        cmd = "command:"+input(">>:").strip()
        if len(cmd) == 0:
            continue

        #发送数据 b将字符串转为bys类型
        client.send(cmd.encode("utf-8")) #send只能发送bytes格式数据
        #接收服务器端的返回(长度和详细内容)，需要声明收多少，默认1024字节
        received_data = b''
        cmd_res = client.recv(4096)
        received_data += cmd_res
        print("recv:",received_data.decode('utf-8'))
        sleep(0.3)

def fileSocket():
    client = socket.socket()

    #链接地址和端口,元组(本地，端口)
    client.connect(('127.0.0.1',50007))
    client.send("getInfo:".encode("utf-8")) #send只能发送bytes格式数据
    received_data = client.recv(102400)
    with open("textrecv/monitor.txt","w") as f:
        f.write(received_data.decode('utf-8',errors="ignore"))
        f.close()
    client.close()
if __name__=="__main__":
    # print(getLoadAvg())
    # getMemoryInfo()
    # socketConnection()
    # lastSocketConnection()
    getAllInfo()
    # fileSocket()