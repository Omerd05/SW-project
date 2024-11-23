import numpy as np
import symnmf as snmf
import math
import sys
np.random.seed(1234)
max_iter = 300
eps = 1e-4

def Symnmf(points_in,k):
    n = len(points_in)
    d = len(points_in[0]) if points_in else 0
    w = snmf.norm(points_in)
    mid = 0  # Average value of the matrix
    for i in range(n):
        for j in range(n):
            mid += w[i][j]
    mid /= n * n

    lower_bound = 0
    upper_bound = 2 * math.sqrt(mid / k)
    h = np.random.uniform(lower_bound, upper_bound, (n, k)).tolist()  # Sampling points
    res = snmf.symnmf(h, w)
    return res

if __name__ == "__main__":
    # Inputs
    k = int(sys.argv[1])
    goal = sys.argv[2]
    file_path = sys.argv[3]

    with open(file_path, 'r') as file:
        content = file.read()

    lines = content.strip().split("\n")
    points = [list(map(float, x.split(','))) for x in lines if x]

    if goal == 'sym':
        res = snmf.sym(points)
    elif goal == 'ddg':
        res = snmf.ddg(points)
    elif goal == 'norm':
        res = snmf.norm(points)
    elif goal == 'symnmf':
        res = Symnmf(points,k)

    for row in res:
        print(','.join(f'{val:.4f}' for val in row))
