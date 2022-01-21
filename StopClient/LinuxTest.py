from subprocess import getoutput
def getDiskInfo():
    a = getoutput("df -h")
    # print(a.split("\n")[2])
    return a.split("\n")[2]