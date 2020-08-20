import numpy as np
import math as m
import random
import matplotlib.pyplot as plt
import evaluate as eva

dataPath = "./data/dataset.txt"


# 导入数据
def loadData():
    points = np.loadtxt(dataPath, delimiter='\t')
    return points


def calculateDistance(data, clu, k):
    """
    计算质点与数据点的距离
    :param data: 样本点
    :param clu:  质点集合
    :param k: 类别个数
    :return: 质心与样本点距离矩阵
    """
    dis = []
    for i in range(len(data)):
        dis.append([])
        for j in range(k):
            dis[i].append(m.sqrt((data[i, 0] - clu[j, 0])**2 + (data[i, 1]-clu[j, 1])**2))
    return np.asarray(dis)


def divide(data, dis):
    """
    对数据点分组
    :param data: 样本集合
    :param dis: 质心与所有样本的距离
    :param k: 类别个数
    :return: 分割后样本
    """
    clusterRes = [0] * len(data)
    for i in range(len(data)):
        seq = np.argsort(dis[i])
        clusterRes[i] = seq[0]

    return np.asarray(clusterRes)


def getCenter(data, clusterRes, k):
    """
    计算质心
    :param group: 分组后样本
    :param k: 类别个数
    :return: 计算得到的质心
    """
    clunew = []
    for i in range(k):
        # 计算每个组的新质心
        idx = np.where(clusterRes == i)
        sum = data[idx].sum(axis=0)
        avg_sum = sum/len(data[idx])
        clunew.append(avg_sum)
    clunew = np.asarray(clunew)
    return clunew[:, 0: 2]


def kmeans(data, clu, k):
    """
    迭代收敛更新质心
    :param data: 样本集合
    :param clu: 质心集合
    :param k: 类别个数
    :return: 误差， 新质心
    """
    clulist = calculateDistance(data, clu, k)
    clusterRes = divide(data, clulist)
    clunew = getCenter(data, clusterRes, k)
    err = clunew - clu
    return err, clunew, k, clusterRes


def drawPic(data, clusterRes, clusterNum):
    """
    结果可视化
    :param data:样本集
    :param clusterRes:聚类结果
    :param clusterNum: 类个数
    :return:
    """
    nPoints = len(data)
    scatterColors = ['black', 'blue', 'green', 'yellow', 'red', 'purple', 'orange', 'brown']
    for i in range(clusterNum):
        color = scatterColors[i % len(scatterColors)]
        x1 = [];  y1 = []
        for j in range(nPoints):
            if clusterRes[j] == i:
                x1.append(data[j, 0])
                y1.append(data[j, 1])
        plt.scatter(x1, y1, c=color, alpha=1, marker='+')
    plt.show()


if __name__ == '__main__':
    k = 10                                          # 类别个数
    data = loadData()
    clu = random.sample(data[:, 0:2].tolist(), k)  # 随机取质心
    clu = np.asarray(clu)
    err, clunew,  k, clusterRes = kmeans(data, clu, k)
    while np.any(abs(err) > 0):
        print(clunew)
        err, clunew,  k, clusterRes = kmeans(data, clunew, k)

    clulist = calculateDistance(data, clunew, k)
    clusterResult = divide(data, clulist)

    nmi, acc, purity = eva.eva(clusterResult, np.asarray(data[:, 2]))
    print(nmi, acc, purity)
    drawPic(data, clusterResult, k)