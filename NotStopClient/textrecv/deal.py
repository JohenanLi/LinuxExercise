f = open("colornew.txt","r")
a = []
for i in f.readlines():
    a.append(i.rstrip())
print(a)