from sklearn.cluster import KMeans
import numpy as np
local=True
def query(c):
    if local:
        return [(c[i], c[i + 1]) for i in range(len(c) - 1)]
    print("?", len(c), *c, flush=True)
    return [tuple(map(int, input().split())) for _ in range(len(c) - 1)]

def answer(groups, edges):
    print("!")
    for i in range(len(groups)):
        print(*groups[i])
        for e in edges[i]:
            print(*e)

# read input
N, M, Q, L, W = map(int, input().split())
G = list(map(int, input().split()))
lx, rx, ly, ry = [], [], [], []
for _ in range(N):
    a, b, c, d = map(int, input().split())
    lx.append(a)
    rx.append(b)
    ly.append(c)
    ry.append(d)

# use center of rectangle
x = [(l + r) // 2 for l, r in zip(lx, rx)]
y = [(l + r) // 2 for l, r in zip(ly, ry)]
points = np.array(list(zip(x, y)))

# Perform initial clustering
kmeans = KMeans(n_clusters=M, random_state=42, n_init="auto").fit(points)
labels = kmeans.labels_

# Assign points to clusters
clusters = [[] for _ in range(M)]
for i, label in enumerate(labels):
    clusters[label].append(i)

# Ensure each cluster matches the exact size of G
sorted_clusters = sorted(clusters, key=lambda c: len(c), reverse=True)
groups = []
remaining_cities = [city for cluster in sorted_clusters for city in cluster]
start_idx = 0
for g_size in G:
    groups.append(remaining_cities[start_idx:start_idx + g_size])
    start_idx += g_size

# Get edges from queries
edges = [[] for _ in range(M)]
for k in range(M):
    group = sorted(groups[k], key=lambda i: (x[i], y[i]))
    for i in range(0, len(group) - 1, 2):
        if i < len(group) - 2:
            ret = query(group[i : i + 3])
            edges[k].extend(ret)
        else:
            edges[k].append(group[i : i + 2])

# Output answer
answer(groups, edges)
