import sys
import math
from itertools import accumulate

def main():
    input = sys.stdin.read().split()
    idx = 0
    N = int(input[idx]); idx += 1
    M = int(input[idx]); idx += 1
    Q = int(input[idx]); idx += 1
    L = int(input[idx]); idx += 1
    W = int(input[idx]); idx += 1
    G = list(map(int, input[idx:idx+M]))
    idx += M
    lx, rx, ly, ry = [], [], [], []
    for _ in range(N):
        lx.append(int(input[idx])); idx +=1
        rx.append(int(input[idx])); idx +=1
        ly.append(int(input[idx])); idx +=1
        ry.append(int(input[idx])); idx +=1

    # Calculate center coordinates
    centers = []
    for i in range(N):
        cx = (lx[i] + rx[i]) // 2
        cy = (ly[i] + ry[i]) // 2
        centers.append((cx, cy, i))
    
    # Sort cities based on Hilbert curve (simulate using x+y for simplicity)
    sorted_cities = sorted(centers, key=lambda x: (x[0] + x[1], x[0], x[1]))
    order = [c[2] for c in sorted_cities]
    
    # Split into groups according to G
    ptr = 0
    groups = []
    for g in G:
        groups.append(order[ptr:ptr+g])
        ptr += g
    
    queries = []
    # Query each group if possible
    for group in groups:
        if len(group) <= L:
            queries.append(group)
        else:
            # Split into chunks of L
            n = len(group)
            for i in range(0, n, L):
                end = min(i+L, n)
                queries.append(group[i:end])
    
    # Now execute queries (up to Q times)
    edges = []
    for q in queries[:Q]:
        print(f"? {len(q)} {' '.join(map(str, q))}")
        sys.stdout.flush()
        for _ in range(len(q)-1):
            a, b = map(int, input().split())
            if a > b:
                a, b = b, a
            edges.append((a, b))
    
    # Output phase
    print("!")
    sys.stdout.flush()
    
    # Assign each group and output their edges (just the collected edges within group)
    # Note: This is a simplified approach. In practice, need to track which edges belong to which group.
    # Here, we assume edges are already collected and group is connected.
    for group in groups:
        members = set(group)
        group_edges = []
        for a, b in edges:
            if a in members and b in members:
                group_edges.append((a, b))
        # Output the group and edges (assuming MST)
        # For the purpose of this example, output first Gk-1 edges
        print(' '.join(map(str, group)))
        for e in group_edges[:len(group)-1]:
            print(f"{e[0]} {e[1]}")

if __name__ == '__main__':
    main()