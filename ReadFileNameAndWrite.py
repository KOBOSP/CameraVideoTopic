import os


# readInfo函数，根据文件夹路径读取文件夹下所有文件名
def readInfo():
    filePath = '/home/kobosp/R1015/mav0/cam0/data'
    name = os.listdir(filePath)  # os.listdir方法返回一个列表对象
    return name


# 程序入口
if __name__ == "__main__":
    fileList = readInfo()  # 读取文件夹下所有的文件名，返回一个列表
    rowInfo=[]
    file = open('TSR1015mav0.txt', 'w')  # 创建文件，权限为写入
    for i in fileList:
        i = i.split('.')[0]
        rowInfo.append(i)
    rowInfo.sort()
    for i in rowInfo:
        TS = i + '\n'
        file.write(TS)
    file.close()
