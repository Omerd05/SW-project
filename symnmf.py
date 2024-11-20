import numpy as np
import matplotlib.pyplot as plt
import symnmf as snmf
import math
import sys
np.random.seed(1234)
max_iter=300
eps=1e-4
def symnmf(H,W):
    n=len(H)
    k=len(H[0])
    new_H = [[0 for j in range(k)] for i in range(n)]
    wh = [[0 for j in range(k)] for i in range(n)]
    hht=[[0 for j in range(n)] for i in range(n)]
    hhth=[[0 for j in range(k)] for i in range(n)]
    iter=0
    diff=1
    while iter<max_iter and diff>=eps:
        wh=np.dot(W,H)
        hht=np.dot(H,np.transpose(H))
        hhth=np.dot(hht,H)
        for i in range(n):
            for j in range(k):
                if hhth[i][j]>0:
                    new_H[i][j]=H[i][j]*(0.5+0.5*wh[i][j]/hhth[i][j])
        diff=0
        for i in range(n):
            for j in range(k):
                diff+=(new_H[i][j]-H[i][j])**2
        diff=math.sqrt(diff)
        H=new_H
        iter+=1
    return H

k=int(sys.argv[1])
goal=sys.argv[2]
file_path=sys.argv[3]

with open(file_path, 'r') as file:
    content = file.read()

lines = content.strip().split("\n")
points = [list(map(float, x.split(','))) for x in lines if x]

n = len(points)
d = len(points[0]) if points else 0

H=[[]]
if goal=='sym':
    res=snmf.sym(points)
elif goal=='ddg':
    res=snmf.ddg(points)
    #res=snmf.ddg(similar)
elif goal=='norm':
    #similar=snmf.sym(points)
    #diagonal = snmf.ddg(similar)
    res=snmf.norm(points)
elif goal=='symnmf':
    w=snmf.norm(points)
    mid=0
    for i in range(n):
        for j in range(n):
            mid+=w[i][j]
    mid/=n*n
    lower_bound = 0
    upper_bound = 2 * math.sqrt(mid / k)
    h = np.random.uniform(lower_bound, upper_bound, (n, k)).tolist()
    #print(h)
    res=snmf.symnmf(h,w)

for row in res:
    print(','.join(f'{val:.4f}' for val in row))
