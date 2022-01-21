from subprocess import getoutput,Popen
def getDiskInfo():
    a = getoutput("df -h")
    return a.split("\n")[2:]

def getAllInfo():
    allInfo = Popen(["/bin/sh","/home/minipython/桌面/LinuxExercise/server/testcpu"],)
    print(allInfo)

getAllInfo()