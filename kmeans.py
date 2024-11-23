import sys

def kmeans(k, points):

    iter1 = 300
    sig = 1e-4**2
    
    n = len(points)
    d = len(points[0]) if points else 0

    if k <= 1 or k >= n:
        print("Invalid number of clusters!")
        sys.exit()


    clusters = [points[i] for i in range(k)]
    sz = [1 for _ in range(k)]
    visit = [-1 for _ in range(n)]

    for i in range(k):
        visit[i] = i

    while iter1 != 0:
        iter1 -= 1
        nc = [-1 for _ in range(n)]

        for t in range(n):
            p = points[t]
            mind = float('inf')
            mine = 0
            for j in range(k):
                curd = sum((clusters[j][i] - p[i]) ** 2 for i in range(d))
                if curd < mind:
                    mind = curd
                    mine = j
            nc[t] = mine

        new_clusters = [[0.0] * d for _ in range(k)]
        counts = [0] * k
        for t in range(n):
            mine = nc[t]
            for i in range(d):
                new_clusters[mine][i] += points[t][i]
            counts[mine] += 1

        for j in range(k):
            if counts[j] > 0:
                for i in range(d):
                    new_clusters[j][i] /= counts[j]

        mcha = max(
            sum((new_clusters[j][i] - clusters[j][i]) ** 2 for i in range(d))
            for j in range(k)
        )

        clusters = new_clusters
        if mcha < sig:
            break
    return nc
