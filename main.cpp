#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <tuple> // Include tuple header

using namespace std;

bool local = true;

vector<pair<int, int>> query(const vector<int>& c) {
    if (local) {
        vector<pair<int, int>> result;
        for (size_t i = 0; i < c.size() - 1; ++i) {
            result.emplace_back(c[i], c[i + 1]);
        }
        return result;
    }

    cout << "? " << c.size();
    for (int val : c) {
        cout << " " << val;
    }
    cout << endl;

    vector<pair<int, int>> result;
    for (size_t i = 0; i < c.size() - 1; ++i) {
        int a, b;
        cin >> a >> b;
        result.emplace_back(a, b);
    }
    return result;
}

void answer(const vector<vector<int>>& groups, const vector<vector<pair<int, int>>>& edges) {
    cout << "!" << endl;
    for (size_t i = 0; i < groups.size(); ++i) {
        for (int val : groups[i]) {
            cout << val << " ";
        }
        cout << endl;

        for (const auto& edge : edges[i]) {
            cout << edge.first << " " << edge.second << endl;
        }
    }
}

int main() {
    int N, M, Q, L, W;
    cin >> N >> M >> Q >> L >> W;

    vector<int> G(M);
    for (int i = 0; i < M; ++i) {
        cin >> G[i];
    }

    vector<int> lx(N), rx(N), ly(N), ry(N);
    for (int i = 0; i < N; ++i) {
        cin >> lx[i] >> rx[i] >> ly[i] >> ry[i];
    }

    // Use center of rectangle
    vector<int> x(N), y(N);
    for (int i = 0; i < N; ++i) {
        x[i] = (lx[i] + rx[i]) / 2;
        y[i] = (ly[i] + ry[i]) / 2;
    }

    vector<int> id(N);
    for (int i = 0; i < N; ++i) {
        id[i] = i;
    }

    vector<tuple<int, int, int>> points;
    for (int i = 0; i < N; ++i) {
        points.emplace_back(x[i], y[i], id[i]);
    }

    vector<vector<int>> groups(M);
    vector<tuple<int, int>> points_xy(N); // use tuple<int,int> instead of tuple<int, int, int>
    for(int i =0; i<N; i++){
        points_xy[i] = make_tuple(x[i],y[i]);
    }


    
    vector<tuple<int, int, int>> remaining_points = points;

    for (int i = 0; i < M; ++i) {
        // Select a random point from points
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> distrib(0, (int)remaining_points.size() - 1); // cast size() to int
        int idx = distrib(gen);
        
        groups[i].push_back(get<2>(remaining_points[idx]));

        // Remove the selected point from points
        remaining_points.erase(remaining_points.begin() + idx);

        while (groups[i].size() < (size_t)G[i]) { // cast G[i] to size_t for comparison
            double min_dist = 1e9;
            int min_idx = -1;

            for (size_t j = 0; j < remaining_points.size(); ++j) {
                for (int gid : groups[i]) {
                    double dist = sqrt(pow(get<0>(remaining_points[j]) - get<0>(points_xy[gid]), 2) +
                                       pow(get<1>(remaining_points[j]) - get<1>(points_xy[gid]), 2));
                    if (dist < min_dist) {
                        min_dist = dist;
                        min_idx = (int)j; // cast j to int
                    }
                }
            }

            groups[i].push_back(get<2>(remaining_points[min_idx]));
            remaining_points.erase(remaining_points.begin() + min_idx);
        }
    }

    // Get edges from queries
    vector<vector<pair<int, int>>> edges(M);
    for (int k = 0; k < M; ++k) {
        vector<int> group = groups[k];
        sort(group.begin(), group.end(), [&](int a, int b) {
            if (x[a] != x[b]) {
                return x[a] < x[b];
            }
            return y[a] < y[b];
        });
        
        int idx = 0;
        for (int i = 0; i <= (int)group.size() - L; i += L - 1) {
            vector<int> sub_group(group.begin() + i, group.begin() + min((int)group.size(), i + L));
            vector<pair<int, int>> ret = query(sub_group);
            edges[k].insert(edges[k].end(), ret.begin(), ret.end());
            idx = i + L - 1;
        }
        // 最後の余りの部分
        if ((int)group.size()-idx >2)
        {
            vector<int> sub_group(group.begin() + idx, group.end());
            vector<pair<int, int>> ret = query(sub_group);
            edges[k].insert(edges[k].end(), ret.begin(), ret.end());
        }
        else if ((int)group.size() -idx !=1){
           edges[k].emplace_back(group[idx],group.back());
        }
        
    }
    answer(groups, edges);

    return 0;
}