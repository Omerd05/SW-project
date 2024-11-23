from kmeans import *
from symnmf_clone import Symnmf
import sys
import math

#Inputs
k=int(sys.argv[1])
file_path=sys.argv[2]

with open(file_path, 'r') as file:
    content = file.read()

lines = content.strip().split("\n")
points = [list(map(float, x.split(','))) for x in lines if x]

n=len(points)
d=len(points[0]) if points else 0

def calc_Shil(clusters):
    sz_clusters=[0 for i in range(k)]
    for i in range(n):
        sz_clusters[clusters[i]]+=1
    shil=0
    for i in range(n):
        if(sz_clusters[clusters[i]]==1):
            continue
        else:
            dist_clusters=[0 for i in range (k)]
            for j in range(n):
                if (i!=j):
                    dist_clusters[clusters[j]]+=math.sqrt(sum((points[i][l]-points[j][l])**2 for l in range(d)))
            a=dist_clusters[clusters[i]]/(sz_clusters[clusters[i]]-1) if sz_clusters[clusters[i]]>1 else 0      
            b=float('inf')
            for j in range(k):
                if (j!=clusters[i] and sz_clusters[j]>0):
                    if((b*sz_clusters[j])>dist_clusters[j]):
                        b=(dist_clusters[j]/sz_clusters[j])
            shil+= ((b-a)/max(a,b))
    shil=shil/n
    return shil

H_symnmf=Symnmf(points,k)
clusters_kmeans=kmeans(k,points)

#clustering symnmf
clusters_symnmf=[0 for i in range(n)]
for i in range(n):
    max_val=H_symnmf[i][0]
    max_index=0
    for j in range(1,k):
        if (H_symnmf[i][j]>max_val):
            max_val=H_symnmf[i][j]
            max_index=j
    clusters_symnmf[i]=max_index


shil_symnmf=calc_Shil(clusters_symnmf)
shil_kmeans=calc_Shil(clusters_kmeans)

print(f"nmf: {shil_symnmf:.4f}")
print(f"kmeans: {shil_kmeans:.4f}")