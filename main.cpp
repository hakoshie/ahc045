#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <tuple> // Include tuple header

#include <chrono>

using namespace std;
// timer start
auto start_time = chrono::high_resolution_clock::now();
const int time_limit = 1900;
bool local = false;
using ll = long long;
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

    vector<pair<int,int>> G(M);
    for (int i = 0; i < M; ++i) {
        cin >> G[i].first;
        G[i].second = i;
    }
    sort(G.begin(), G.end(), greater<pair<int,int>>());
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
    vector<double>group_var(M, 0);
    double tmp_var = 0;
    for (int i = 0; i < M; ++i) {
        // Select a random point from points
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> distrib(0, (int)remaining_points.size() - 1); // cast size() to int
        int idx = distrib(gen);
        int group_id =G[i].second;
        groups[group_id].push_back(get<2>(remaining_points[idx]));

        // Remove the selected point from points
        remaining_points.erase(remaining_points.begin() + idx);

        while (groups[group_id].size() < (size_t)G[i].first) { // cast G[i] to size_t for comparison
            double min_dist = 1e9;
            int min_idx = -1;

            for (size_t j = 0; j < remaining_points.size(); ++j) {
                for (int idx_g : groups[group_id]) {
                    double dist = sqrt(pow(get<0>(remaining_points[j]) - get<0>(points_xy[idx_g]), 2) +
                                       pow(get<1>(remaining_points[j]) - get<1>(points_xy[idx_g]), 2));
                    if (dist < min_dist) {
                        min_dist = dist;
                        min_idx = (int)j; // cast j to int
                    }
                }
            }
            groups[group_id].push_back(get<2>(remaining_points[min_idx]));
            remaining_points.erase(remaining_points.begin() + min_idx);
        }
        // Calculate the variance of the group
        double x_sum = 0, y_sum = 0;
        for (int idx_g : groups[group_id]) {
            x_sum += get<0>(points_xy[idx_g]);
            y_sum += get<1>(points_xy[idx_g]);
        }
        x_sum /= groups[group_id].size();
        y_sum /= groups[group_id].size();
        double var = 0;
        for (int idx_g : groups[group_id]) {
            var += pow(get<0>(points_xy[idx_g]) - x_sum, 2) + pow(get<1>(points_xy[idx_g]) - y_sum, 2);
        }
        var /= groups[group_id].size();
        // groupを平均からの距離でsortする
        // sort(groups[group_id].begin(), groups[group_id].end(), [&](int a, int b) {
        //     double dist_a = sqrt(pow(get<0>(points_xy[a]) - x_sum, 2) + pow(get<1>(points_xy[a]) - y_sum, 2));
        //     double dist_b = sqrt(pow(get<0>(points_xy[b]) - x_sum, 2) + pow(get<1>(points_xy[b]) - y_sum, 2));
        //     return dist_a < dist_b;
        // });
        // グループをx,y座標でsortする
        // sort(groups[group_id].begin(), groups[group_id].end(), [&](int a, int b) {
        //     if (x[a] != x[b]) {
        //         return x[a] < x[b];
        //     }
        //     return y[a] < y[b];
        // });
        group_var[group_id] = var;
        tmp_var += var;
    }
    random_device rd;
    mt19937 gen(rd());

    // Simulated annealing
    // 温度
    double start_temperature = 1000000;
    double end_temperature = 0.1;
    double current_temperature = start_temperature;
    
    while(1) {
        // check time
        auto current_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(current_time - start_time);
        if (duration.count() > time_limit) {
            break;
        }
        int g1 = uniform_int_distribution<>(0, M - 1)(gen);
        int g2 = uniform_int_distribution<>(0, M - 1)(gen);
        if (g1 == g2 || groups[g1].empty() || groups[g2].empty()) continue;

        int i1 = uniform_int_distribution<>(0, (int)groups[g1].size() - 1)(gen);
        int i2 = uniform_int_distribution<>(0, (int)groups[g2].size() - 1)(gen);

        int p1 = groups[g1][i1];
        int p2 = groups[g2][i2];
        // distanceが2500\sqrt(2)以上ならcontinue
        double dist = sqrt(pow(get<0>(points_xy[p1]) - get<0>(points_xy[p2]), 2) +
                           pow(get<1>(points_xy[p1]) - get<1>(points_xy[p2]), 2));
        if (dist > 2*W * sqrt(2)) continue;
        // p1, p2をswapして分散を計算する
        swap(groups[g1][i1], groups[g2][i2]);
        double x_sum1 = 0, y_sum1 = 0;
        for (int idx_g : groups[g1]) {
            x_sum1 += get<0>(points_xy[idx_g]);
            y_sum1 += get<1>(points_xy[idx_g]);
        }
        x_sum1 /= groups[g1].size();
        y_sum1 /= groups[g1].size();
        double var1 = 0;
        for (int idx_g : groups[g1]) {
            var1 += pow(get<0>(points_xy[idx_g]) - x_sum1, 2) + pow(get<1>(points_xy[idx_g]) - y_sum1, 2);
        }
        var1 /= groups[g1].size();
        double x_sum2 = 0, y_sum2 = 0;
        for (int idx_g : groups[g2]) {
            x_sum2 += get<0>(points_xy[idx_g]);
            y_sum2 += get<1>(points_xy[idx_g]);
        }
        x_sum2 /= groups[g2].size();
        y_sum2 /= groups[g2].size();
        double var2 = 0;
        for (int idx_g : groups[g2]) {
            var2 += pow(get<0>(points_xy[idx_g]) - x_sum2, 2) + pow(get<1>(points_xy[idx_g]) - y_sum2, 2);
        }
        var2 /= groups[g2].size();
        // 分散が小さくなったらswapを維持
        // simulate annealing
        double delta= (var1 + var2) - (group_var[g1] + group_var[g2]);
        
        if (delta < 0 || exp(-delta / current_temperature) > uniform_real_distribution<>(0, 1)(gen)) {
            group_var[g1] = var1;
            group_var[g2] = var2;
            // グルーブを平均からの距離でsortする
            // sort(groups[g1].begin(), groups[g1].end(), [&](int a, int b) {
            //     double dist_a = sqrt(pow(get<0>(points_xy[a]) - x_sum1, 2) + pow(get<1>(points_xy[a]) - y_sum1, 2));
            //     double dist_b = sqrt(pow(get<0>(points_xy[b]) - x_sum1, 2) + pow(get<1>(points_xy[b]) - y_sum1, 2));
            //     return dist_a < dist_b;
            // });
            // sort(groups[g2].begin(), groups[g2].end(), [&](int a, int b) {
            //     double dist_a = sqrt(pow(get<0>(points_xy[a]) - x_sum2, 2) + pow(get<1>(points_xy[a]) - y_sum2, 2));
            //     double dist_b = sqrt(pow(get<0>(points_xy[b]) - x_sum2, 2) + pow(get<1>(points_xy[b]) - y_sum2, 2));
            //     return dist_a < dist_b;
            // });
            // グループをx,y座標でsortする
            // sort(groups[g1].begin(), groups[g1].end(), [&](int a, int b) {
            //     if (x[a] != x[b]) {
            //         return x[a] < x[b];
            //     }
            //     return y[a] < y[b];
            // });
            // sort(groups[g2].begin(), groups[g2].end(), [&](int a, int b) {
            //     if (x[a] != x[b]) {
            //         return x[a] < x[b];
            //     }
            //     return y[a] < y[b];
            // });
        } else {
            // 分散が大きくなったら元に戻す
            swap(groups[g1][i1], groups[g2][i2]);
        }
        double progress = (double) std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count() / (double) time_limit;
        current_temperature =std::pow(start_temperature, 1.0 - progress) * std::pow(end_temperature, progress);



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
        // L=min(L,15); 
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
        else if ((int)group.size() -idx ==2){
           edges[k].emplace_back(group[idx],group.back());
        }
        
    }
    answer(groups, edges);

    return 0;
}