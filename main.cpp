#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <iomanip>
#include <deque>
#include <tuple>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <unordered_map>
#include <sstream>
#include <stdexcept>
#include <random>
#include <chrono>
#include <cassert>
#include <atcoder/dsu>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <unordered_set>
#define rep(i, x) for (int i = 0; i < (int)(x); i++)
#define FOR(i, a, done) for (int i = (a); i < (done); ++i)
#define all(a) (a).begin(), (a).end()
#define x first
#define y second
#define debug(a) std::cout << (a) << std::endl

namespace mp = boost::multiprecision;
using cpp_int = mp::cpp_int;
using ll = long long;
using pii = std::pair<int, int>;
using namespace std;

template <class T> bool chmax(T &a, const T &b) { if (a < b) { a = b; return 1; } return 0; }
template <class T> bool chmin(T &a, const T &b) { if (a > b) { a = b; return 1; } return 0; }

// Timer
auto start_time = std::chrono::high_resolution_clock::now();
const int time_limit_init = 500;
const int time_limit = 1700;
const int time_limit_final =1900;
bool LOCAL = false;

using ll = long long;
const int Nconst=800;
int W;

vector<int>width(Nconst);
vector<int>height(Nconst);
vector<pair<int, int>> points_xy(Nconst);
// Function to perform a query
std::set<pii> query(const std::vector<int>& c) {
    if (LOCAL) {
        std::set<pii> result;
        for (size_t i = 0; i < c.size() - 1; ++i) {
            result.insert({c[i], c[i + 1]});
        }
        return result;
    }

    std::cout << "? " << c.size();
    for (int val : c) {
        std::cout << " " << val;
    }
    std::cout << std::endl;

    std::set<pii> result;
    for (size_t i = 0; i < c.size() - 1; ++i) {
        int a, b;
        std::cin >> a >> b;
        result.insert({a, b});
    }
    return result;
}

// Function to print the answer
void answer(const std::vector<std::vector<int>>& groups, const std::vector<std::set<pii>>& edges) {
    
    std::cout << "!" << std::endl;
    atcoder::dsu uf(800);
    for (size_t i = 0; i < groups.size(); ++i) {
        for (int val : groups[i]) {
            std::cout << val << " ";
        }
        std::cout << std::endl;

        for (const auto& edge : edges[i]) {
            if(uf.same(edge.first, edge.second)) continue;           
            std::cout << edge.first << " " << edge.second << std::endl;
            uf.merge(edge.first, edge.second);
            
        }
    }
}
// 木の直径を求める
int tree_diameter(const vector<int>& group, const set<pair<int, int>>& edges) {
    unordered_map<int, vector<int>> graph;
    unordered_set<int> group_set(group.begin(), group.end());

    // グループ内の頂点のみで隣接リストを作成
    for (auto [u, v] : edges) {
        if (group_set.count(u) && group_set.count(v)) {
            graph[u].push_back(v);
            graph[v].push_back(u);
        }
    }

    auto bfs_farthest = [&](int start) -> pair<int, int> {
        queue<int> q;
        unordered_map<int, int> dist;
        int farthest_node = start, max_dist = 0;

        q.push(start);
        dist[start] = 0;

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (int v : graph[u]) {
                if (!dist.count(v)) {
                    dist[v] = dist[u] + 1;
                    q.push(v);
                    if (dist[v] > max_dist) {
                        max_dist = dist[v];
                        farthest_node = v;
                    }
                }
            }
        }
        return {farthest_node, max_dist};
    };

    // グループの最初の頂点を始点とする
    if (group.empty()) return 0;
    int start = group[0];
    int farthest = bfs_farthest(start).first;
    return bfs_farthest(farthest).second;
}
// ランダムな長さ L のパスを生成
pair<vector<int>, vector<pair<int, int>>> generate_random_path(
    const vector<int>& group, const set<pair<int, int>>& edges, int pathLen) {
    // cerr<<"generate_random_path"<<endl;
    int diameter = tree_diameter(group, edges);
    pathLen = min(pathLen, diameter);
    // if(pathLen==diameter){
    //     if(rand() % 2 == 0){
    //         pathLen = max(2, pathLen -1);
    //         if(rand() % 2 == 0){
    //             pathLen = max(2, pathLen -1);
    //         }
    //     }
    // }
    random_device rd;
    mt19937 gen(rd());

    // group に含まれる頂点のみの隣接リストを作る
    unordered_map<int, vector<int>> graph;
    unordered_set<int> group_set(group.begin(), group.end());

    for (auto [u, v] : edges) {
        if (group_set.count(u) && group_set.count(v)) {
            graph[u].push_back(v);
            graph[v].push_back(u);
        }
    }
    // cerr<<"while"<<endl;
    // timer start
    auto start_time = std::chrono::high_resolution_clock::now();
    while (true) {
        // check time
  
        // ランダムな開始点
        uniform_int_distribution<int> dist(0, group.size() - 1);
        int start = group[dist(gen)];
        vector<int> path = {start};
        vector<pair<int, int>> used_edges;
        unordered_set<int> visited = {start};

        // ランダムウォーク
        while ((int)path.size() < pathLen + 1) {
            // cerr<<"path.size(): " << path.size() << endl;
            // auto current_time = std::chrono::high_resolution_clock::now();
            // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time);
            // if (duration.count() > 30) {
            //     pathLen =max(2, pathLen/2);
            //     start_time = std::chrono::high_resolution_clock::now();
            // }
            int current = path.back();
            vector<int> neighbors;

            for (int neighbor : graph[current]) {
                if (visited.find(neighbor) == visited.end()) {
                    neighbors.push_back(neighbor);
                }
            }

            if (neighbors.empty()) break; // 行き止まりならリトライ

            uniform_int_distribution<int> dist2(0, neighbors.size() - 1);
            int next_node = neighbors[dist2(gen)];
            path.push_back(next_node);
            used_edges.emplace_back(min(current, next_node), max(current, next_node));
            visited.insert(next_node);
        }

        if ((int)path.size() >= pathLen + 1) {
            return {path, used_edges}; // 成功したら返す
        }
    }
}
// Function to compute the MST length
double compute_mst_length(const std::vector<int>& group) {
    int n = group.size();
    std::vector<std::tuple<double, int, int>> edges;

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            int u = group[i], v = group[j];
            double dist = std::sqrt(std::pow(points_xy[u].first - points_xy[v].first, 2) +
                                     std::pow(points_xy[u].second - points_xy[v].second, 2));
            edges.emplace_back(dist, u, v);
        }
    }

    std::sort(edges.begin(), edges.end());
    atcoder::dsu uf(points_xy.size());
    double total_length = 0;
    for (auto [cost, u, v] : edges) {
        if (!uf.same(u, v)) {
            uf.merge(u, v);
            total_length += cost;
        }
    }
    return total_length;
}

// Function to compute the MST order
std::vector<int> compute_mst_with_order(const std::vector<int>& group) {
    int n = group.size();
    std::vector<std::tuple<double, int, int>> edges;

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            int u = group[i], v = group[j];
            double dist = std::sqrt(std::pow(points_xy[u].first - points_xy[v].first, 2) +
                                     std::pow(points_xy[u].second - points_xy[v].second, 2));
            edges.emplace_back(dist, u, v);
        }
    }

    std::sort(edges.begin(), edges.end());
    atcoder::dsu uf(points_xy.size());
    std::vector<std::vector<int>> adj(points_xy.size());
    for (auto [cost, u, v] : edges) {
        if (!uf.same(u, v)) {
            uf.merge(u, v);
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
    }

    std::vector<int> order;
    std::vector<bool> visited(points_xy.size(), false);
    std::function<void(int)> dfs = [&](int v) {
        visited[v] = true;
        order.push_back(v);
        for (int u : adj[v]) {
            if (!visited[u]) dfs(u);
        }
    };

    dfs(group[0]);
    return order;
}
std::set<pair<int,int>> compute_mst_edges(const std::vector<int>& group) {
    int n = group.size();
    std::vector<std::tuple<double, int, int>> edges;

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            int u = group[i], v = group[j];
            double dist = std::sqrt(std::pow(points_xy[u].first - points_xy[v].first, 2) +
                                     std::pow(points_xy[u].second - points_xy[v].second, 2));
            edges.emplace_back(dist, u, v);
        }
    }

    std::sort(edges.begin(), edges.end());
    atcoder::dsu uf(points_xy.size());
    std::vector<std::vector<int>> adj(points_xy.size());
    std::set<pair<int,int>> result;
    for (auto [cost, u, v] : edges) {
        if (!uf.same(u, v)) {
            uf.merge(u, v);
            if(u>v) swap(u,v);
            result.insert({u, v});
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
    }
    return result;


}
double calculate_variance(const std::vector<int>& group) {
    double x_sum = 0, y_sum = 0;
    for (int idx_g : group) {
        x_sum += points_xy[idx_g].first;
        y_sum += points_xy[idx_g].second;
    }
    x_sum /= group.size();
    y_sum /= group.size();

    double variance = 0;
    for (int idx_g : group) {
        variance += std::pow(points_xy[idx_g].first - x_sum, 2) + std::pow(points_xy[idx_g].second - y_sum, 2);
        // variance += std::pow(width[idx_g], .5) + std::pow(height[idx_g], .5);
    }
    return variance / group.size();
}
double compute_all_path_length(const std::vector<int>& group) {
    double total_length = 0;
    for (size_t i = 0; i < group.size(); ++i) {
        for (size_t j = i + 1; j < group.size(); ++j) {
            int u = group[i], v = group[j];
            double dist = std::sqrt(std::pow(points_xy[u].first - points_xy[v].first, 2) +
                                     std::pow(points_xy[u].second - points_xy[v].second, 2));
            total_length += dist;
        }
    }
    return total_length;
}
int main() {
    int N, M, Q, L;
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
        width[i] = rx[i] - lx[i];
        height[i] = ry[i] - ly[i];
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

    for(int i =0; i<N; i++){
        points_xy[i] = make_pair(x[i],y[i]);
    }


    
    
    vector<double>group_var(M, 0);
    vector<double>group_dist(M, 0);
    vector<double>group_mst_dist(M, 0);
    vector<double>group_all_path_length(M, 0);
    double tmp_var = 1e15;
    double tmp_dist =1e15;
    double tmp_mst_dist =1e15;
    double tmp_all_path_length = 1e15;
    double start_temperature = 1e20;
    double end_temperature = 1e0;
    double current_temperature = start_temperature;
    random_device rd;
    mt19937 gen(rd());
    // 以下を10回繰り返して最もvarianceが小さくなるようにする
    for(int trial=0;trial<100;trial++){
        // check time
        auto current_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(current_time - start_time);
        if (duration.count() > time_limit_init) {
            break;
        }
        double var_t=0;
        double dist_t=0;
        double mst_dist_t=0;
        double all_path_length_t=0;
        
        vector<vector<int>> groups_t(M);
        vector<double> group_var_t(M, 0);
        vector<double> group_dist_t(M, 0);
        vector<double> group_mst_dist_t(M, 0);
        vector<double> group_all_path_length_t(M, 0);
        vector<tuple<int, int, int>> remaining_points = points;
        for (int i = 0; i < M; ++i) {
            // Select a random point from points
         
            uniform_int_distribution<> distrib(0, (int)remaining_points.size() - 1); // cast size() to int
            int idx = distrib(gen);
            int group_id =G[i].second;
            groups_t[group_id].push_back(get<2>(remaining_points[idx]));

            // Remove the selected point from points
            remaining_points.erase(remaining_points.begin() + idx);

            while (groups_t[group_id].size() < (size_t)G[i].first) { // cast G[i] to size_t for comparison
                double min_dist = 1e9;
                int min_idx = -1;

                for (size_t j = 0; j < remaining_points.size(); ++j) {
                    for (int idx_g : groups_t[group_id]) {
                        double dist = sqrt(pow(get<0>(remaining_points[j]) -points_xy[idx_g].first, 2) +
                                        pow(get<1>(remaining_points[j]) -points_xy[idx_g].second, 2));
                        auto delta= min_dist - dist;
                        // if (dist < min_dist or (delta<2*W*sqrt(2) and .35> uniform_real_distribution<>(0, 1)(gen))) {
                        if (dist < min_dist) {
                            min_dist = dist;
                            min_idx = (int)j; // cast j to int
                        }
                    }
                }
                dist_t += min_dist;
                groups_t[group_id].push_back(get<2>(remaining_points[min_idx]));
                remaining_points.erase(remaining_points.begin() + min_idx);
            }
            double var=calculate_variance(groups_t[group_id]);
            group_var_t[group_id] = var;
            var_t += var;
            group_dist_t[group_id] = dist_t;
            // group_all_path_length_t[group_id] = compute_all_path_length(groups_t[group_id]);
            // all_path_length_t += group_all_path_length_t[group_id];
            // group_mst_dist_t[group_id] = compute_mst_length(groups_t[group_id]);
            mst_dist_t += group_mst_dist_t[group_id];
            double progress = (double) std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count() / (double) time_limit_init;
            current_temperature =std::pow(start_temperature, 1.0 - progress) * std::pow(end_temperature, progress);
        }
        // variance
        if(tmp_var > var_t){
            tmp_var = var_t;
            groups = groups_t;
            group_var = group_var_t;
        }
        // dist*variance
        // if(tmp_dist * tmp_var > dist_t*var_t){
        //     tmp_dist = dist_t;
        //     tmp_var = var_t;
        //     groups = groups_t;
        //     group_var = group_var_t;
        // }
        // all_path_length
        // if(tmp_all_path_length > all_path_length_t){
        //     tmp_all_path_length = all_path_length_t;
        //     tmp_var = var_t;
        //     groups = groups_t;
        //     group_var = group_var_t;
        //     group_dist = group_dist_t;
        //     group_mst_dist = group_mst_dist_t;
        //     group_all_path_length = group_all_path_length_t;
        // }
        // mst_dist
        // if(tmp_mst_dist> mst_dist_t){
        //     tmp_mst_dist = mst_dist_t;
        //     tmp_var = var_t;
        //     groups = groups_t;
        //     group_var = group_var_t;
        //     group_dist = group_dist_t;
        //     group_mst_dist = group_mst_dist_t;
        // }

    }
    cerr<<"tmp_var: " << tmp_var << endl;
    // random_device rd;
    // mt19937 gen(rd());

    // Simulated annealing
    // 温度
    start_temperature = 1e10;
    end_temperature = 1e0;
    current_temperature = start_temperature;
    auto best_groups = groups;
    auto best_group_var = group_var;
    auto best_var = tmp_var;

    vector<int>weight(M);
    // vector<int>weight2(M);
    for(int i=0;i<M;i++){
       weight[i]=groups[i].size()*groups[i].size();
    //    weight2[i]=1/groups[i].size();
    }
    // 重みづけサンプリング
    discrete_distribution<> dist_g1(weight.begin(), weight.end());
    // discrete_distribution<> dist_g2(weight2.begin(), weight2.end());
    while(1) {
        // check time
        auto current_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(current_time - start_time);
        if (duration.count() > time_limit) {
            break;
        }

        int g1 = uniform_int_distribution<>(0, M - 1)(gen);
        // int g1 = dist_g1(gen);
        int g2 = uniform_int_distribution<>(0, M - 1)(gen);
        // int g2 = dist_g1(gen);
        // int g2 = dist_g2(gen);
        if (g1 == g2 || groups[g1].empty() || groups[g2].empty()) continue;
        
        int i1 = uniform_int_distribution<>(0, (int)groups[g1].size() - 1)(gen);
        int i2 = uniform_int_distribution<>(0, (int)groups[g2].size() - 1)(gen);

        int p1 = groups[g1][i1];
        int p2 = groups[g2][i2];
        // distanceが2500\sqrt(2)以上ならcontinue
        // double dist = sqrt(pow(get<0>(points_xy[p1]) - get<0>(points_xy[p2]), 2) +
                        //    pow(get<1>(points_xy[p1]) - get<1>(points_xy[p2]), 2));
        double dist = sqrt(pow(points_xy[p1].first - points_xy[p2].first, 2) +
                               pow(points_xy[p1].second - points_xy[p2].second, 2));
        if (dist > 10000) continue;
        // p1, p2をswapして分散を計算する
        swap(groups[g1][i1], groups[g2][i2]);
        double var1=calculate_variance(groups[g1]);
        double var2=calculate_variance(groups[g2]);

        // simulate annealing
        double delta= (var1 + var2) - (group_var[g1] + group_var[g2]);
        
        if (delta < 0 || exp(-delta / current_temperature) > uniform_real_distribution<>(0, 1)(gen)) {
            group_var[g1] = var1;
            group_var[g2] = var2;
            tmp_var += delta;
            // if (tmp_var < best_var) {
            //     best_var = tmp_var;
            //     best_groups = groups;
            // }
            
        } else {
            // 分散が大きくなったら元に戻す
            swap(groups[g1][i1], groups[g2][i2]);
        }
        double progress = (double) std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count() / (double) time_limit;
        current_temperature =std::pow(start_temperature, 1.0 - progress) * std::pow(end_temperature, progress);

    }
    // groups = best_groups;
    // Get edges from queries
    vector<set<pair<int, int>>> edges(M);
    atcoder::dsu uf(800);
    int num_queries = 0;
    vector<int> candidate_cities;
    // compute edges
    for (int k = 0; k < M; ++k) {
        vector<int> group = groups[k];
        edges[k]=compute_mst_edges(group);
    }
    map<int,int>id2group;
    for(int i=0;i<M;i++){
        for(auto a:groups[i]){
            id2group[a]=i;
        }
    }
    vector<set<int>> connection(N);
    rep(i,M){
        for(auto [a,b]:edges[i]){
            assert(a<=N);
            if(a>=N){
                cerr<<"a: " << a << endl;
            }
            connection[a].insert(b);
            
            assert(b<=N);
            if(b>=N){
                cerr<<"b: " << b << endl;
            }
            connection[b].insert(a);
         
        }
    }
    cerr<<"done"<<endl;
    // グループのサイズを基に重み付けサンプリング
  
    vector<int>nvisit_group(M,0);
    // auto group_id= uniform_int_distribution<>(0, M-1)(gen);
    std::vector<double> weights(M);
    for (int i = 0; i < M; ++i) {
        weights[i] = groups[i].size(); // グループのサイズを重みとする
        if(groups[i].size()<=2) weights[i]=0;
    }
    while(num_queries<Q){
        // check time
        auto current_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(current_time - start_time);
        if (duration.count() > time_limit_final) {
            break;
        }
        
    
        // 重みの合計を計算
        std::discrete_distribution<> dist(weights.begin(), weights.end());
        auto group_id = dist(gen); // 重み付けサンプリングで group_id を選択
        nvisit_group[group_id]++;
        auto group=groups[group_id];
        if(group.size()<=2) continue;
        auto [coordinates,used_edges]=generate_random_path(group, edges[group_id], L-1);
        // cerr<<"coordinates: " << coordinates.size() << endl;
        // for(auto a:coordinates){
        //     cerr<<a << " ";
        // }
        // cerr<<endl;
        for(auto [u,v]:used_edges){
            edges[group_id].erase({u,v});
        }
        auto ret=query(coordinates);
        for(auto [u,v]:ret){
            edges[group_id].insert({u,v});
        }

        num_queries++;
    }
    answer(groups, edges);
    cerr<<"Queries: " << num_queries << endl;
    return 0;
}