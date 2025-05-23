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
const int time_limit_init = 300;
int time_limit = 1500;
const int time_limit_final =1900;
bool LOCAL = false;

using ll = long long;
const int Nconst=800;
int W;
bool use_estimated_distance = false;
bool use_mst_distance = false;
vector<int>width(Nconst);
vector<int>height(Nconst);
vector<pair<int, int>> points_xy(Nconst);
vector<int> lx(Nconst), rx(Nconst), ly(Nconst), ry(Nconst);
// Function to calculate E[X^2] for X uniformly distributed in [a, b]
double expected_square(double a, double b) {
    // Avoid division by zero if a == b
    if (a == b) {
        return a * a;
    }
    // Use the formula (a^2 + ab + b^2) / 3 derived from (b^3 - a^3) / (3 * (b - a))
    return (a * a + a * b + b * b) / 3.0;
}

// Function to estimate E[dist(i, j)] using sqrt(E[d^2])
double estimate_expected_distance(int i, int j) {
    // E[xi], E[xj], E[yi], E[yj]
    double exi = (lx[i] + rx[i]) / 2.0;
    double eyi = (ly[i] + ry[i]) / 2.0;
    double exj = (lx[j] + rx[j]) / 2.0;
    double eyj = (ly[j] + ry[j]) / 2.0;

    // E[xi^2], E[xj^2], E[yi^2], E[yj^2]
    double exi2 = expected_square(lx[i], rx[i]);
    double eyi2 = expected_square(ly[i], ry[i]);
    double exj2 = expected_square(lx[j], rx[j]);
    double eyj2 = expected_square(ly[j], ry[j]);

    // E[(xi - xj)^2] = E[xi^2] - 2*E[xi]*E[xj] + E[xj^2]
    double expected_dx2 = exi2 - 2.0 * exi * exj + exj2;
    // E[(yi - yj)^2] = E[yi^2] - 2*E[yi]*E[yj] + E[yj^2]
    double expected_dy2 = eyi2 - 2.0 * eyi * eyj + eyj2;

    // E[d^2] = E[(xi - xj)^2] + E[(yi - yj)^2]
    // Ensure the value inside sqrt is non-negative due to potential floating point inaccuracies
    double expected_d2 = std::max(0.0, expected_dx2) + std::max(0.0, expected_dy2);

    // E[dist] ≈ sqrt(E[d^2])
    return std::sqrt(expected_d2);
}
// Function to perform a query
std::set<pii> query(const std::set<int>& c) {


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

// --- generate_query (edges を直接変更するバージョン) ---
pair<set<int>, vector<pair<int, int>>> generate_query(
    const vector<int>& group,
    set<pair<int, int>>& edges, // ★参照渡しで直接変更
    int L_limit) // pathLen -> L_limit に変更 (整合性のため)
{
    set<int> vertices_in_query;
    vector<pair<int, int>> erased; // この呼び出しで削除された辺のリスト
    random_device rd;
    mt19937 gen(rd());

    if (edges.empty() || group.size() < 2) {
        uniform_int_distribution<int> dist_group(0, group.size() - 1);
        while (vertices_in_query.size() < L_limit + 1 && vertices_in_query.size() < group.size()) {
            vertices_in_query.insert(group[dist_group(gen)]);
        }
        return {vertices_in_query, erased}; // 辺は削除されていない
    }

    // 1. 初期辺選択
    uniform_int_distribution<int> dist_edge(0, edges.size() - 1);
    auto it = next(edges.begin(), dist_edge(gen));
    int u_start = it->first;
    int v_start = it->second;
    pair<int, int> initial_edge = *it;

    vertices_in_query.insert(u_start);
    vertices_in_query.insert(v_start);
    edges.erase(it); // ★元の edges から直接削除
    erased.push_back(initial_edge);

    double sum_x = points_xy[u_start].first + points_xy[v_start].first;
    double sum_y = points_xy[u_start].second + points_xy[v_start].second;

    // 2. 辺を追加していくループ
    while(vertices_in_query.size() < L_limit + 1 && vertices_in_query.size() < group.size()) {
         if (edges.empty()) break;

         double current_mean_x = sum_x / vertices_in_query.size();
         double current_mean_y = sum_y / vertices_in_query.size();
         double min_dist_sq = 1e18;
         set<pii>::iterator best_edge_it = edges.end();

         for (auto edge_it = edges.begin(); edge_it != edges.end(); ++edge_it) {
             int u = edge_it->first;
             int v = edge_it->second;
             int needed = (vertices_in_query.count(u) == 0) + (vertices_in_query.count(v) == 0);
             if (vertices_in_query.size() + needed > L_limit + 1) continue;

             double edge_mid_x = (points_xy[u].first + points_xy[v].first) / 2.0;
             double edge_mid_y = (points_xy[u].second + points_xy[v].second) / 2.0;
             double dx = edge_mid_x - current_mean_x;
             double dy = edge_mid_y - current_mean_y;
             double dist_sq = dx * dx + dy * dy;
             double delta= dist_sq - min_dist_sq;
             if (dist_sq < min_dist_sq  or (uniform_real_distribution<double>(0, 1)(gen) < 0.05 and delta<2*W*sqrt(2))) {
                 min_dist_sq = dist_sq;
                 best_edge_it = edge_it;
             }
         }

         if (best_edge_it == edges.end()) break;

         int best_u = best_edge_it->first;
         int best_v = best_edge_it->second;
         pair<int, int> edge_to_remove = *best_edge_it;

         bool added_u = vertices_in_query.insert(best_u).second;
         bool added_v = vertices_in_query.insert(best_v).second;
         if (added_u) { sum_x += points_xy[best_u].first; sum_y += points_xy[best_u].second; }
         if (added_v) { sum_x += points_xy[best_v].first; sum_y += points_xy[best_v].second; }

         edges.erase(best_edge_it); // ★元の edges から直接削除
         erased.push_back(edge_to_remove);
    }

    return {vertices_in_query, erased};
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
    int iteration = 0;
    while (true) {
        // check time
        auto current_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(current_time - start_time);
        if (duration.count() > time_limit_final) {
            return {{}, {}};
        }
        if(iteration>30){
            pathLen = max(2, pathLen -1);
            iteration=0;
        }
  
        // ランダムな開始点
        uniform_int_distribution<int> dist(0, group.size() - 1);
        int start = group[dist(gen)];
        vector<int> path = {start};
        vector<pair<int, int>> used_edges;
        unordered_set<int> visited = {start};

        // ランダムウォーク
        while ((int)path.size() < pathLen + 1) {
       
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
        iteration++;
    }
}
// Function to compute the MST length
double compute_mst_distance(const std::vector<int>& group) {
    int n = group.size();
    std::vector<std::tuple<double, int, int>> edges;

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            int u = group[i], v = group[j];
            double dist;
            if(use_estimated_distance){
                dist = estimate_expected_distance(u, v);
            }else{
                dist = std::sqrt(std::pow(points_xy[u].first - points_xy[v].first, 2) +
                        std::pow(points_xy[u].second - points_xy[v].second, 2));
            }
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
            double dist;
            if(use_estimated_distance){
                dist = estimate_expected_distance(u, v);
            }else{
                dist = std::sqrt(std::pow(points_xy[u].first - points_xy[v].first, 2) +
                        std::pow(points_xy[u].second - points_xy[v].second, 2));
            }
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
            double dist;
            if(use_estimated_distance){
                dist = estimate_expected_distance(u, v);
            }else{
                dist = std::sqrt(std::pow(points_xy[u].first - points_xy[v].first, 2) +
                        std::pow(points_xy[u].second - points_xy[v].second, 2));
            }
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
// Function to calculate variance considering uncertainty (using fixed centroid)
double calculate_uncertainty_aware_variance(const std::vector<int>& group) {
    if (group.empty()) return 0.0;
    int group_size = group.size();

    // 1. Calculate centroid based on center points
    double sum_cx = 0, sum_cy = 0;
    for (int idx : group) {
        sum_cx += (lx[idx] + rx[idx]) / 2.0; // Use double for precision
        sum_cy += (ly[idx] + ry[idx]) / 2.0;
    }
    double centroid_cx = sum_cx / group_size;
    double centroid_cy = sum_cy / group_size;

    // 2. Calculate sum of expected squared distances to the fixed centroid
    double sum_expected_sq_dist = 0;
    for (int idx : group) {
        double lxi = lx[idx], rxi = rx[idx];
        double lyi = ly[idx], ryi = ry[idx];

        // E[xi] and E[yi] (center points)
        double exi = (lxi + rxi) / 2.0;
        double eyi = (lyi + ryi) / 2.0;

        // E[xi^2] and E[yi^2]
        double exi2 = expected_square(lxi, rxi);
        double eyi2 = expected_square(lyi, ryi);

        // E[(xi - centroid_cx)^2] = E[xi^2] - 2*centroid_cx*E[xi] + centroid_cx^2
        double expected_dx2 = exi2 - 2.0 * centroid_cx * exi + centroid_cx * centroid_cx;
        // E[(yi - centroid_cy)^2] = E[yi^2] - 2*centroid_cy*E[yi] + centroid_cy^2
        double expected_dy2 = eyi2 - 2.0 * centroid_cy * eyi + centroid_cy * centroid_cy;

        sum_expected_sq_dist += expected_dx2 + expected_dy2;
    }

    // 3. Return the average
    return sum_expected_sq_dist / group_size;
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
    double noise_correction = 0;
    for (int idx_g : group) {
        variance += std::pow(points_xy[idx_g].first - x_sum, 2) + std::pow(points_xy[idx_g].second - y_sum, 2);
        // variance += std::pow(width[idx_g], .5) + std::pow(height[idx_g], .5);
        // noise_correction += (width[idx_g] * width[idx_g] + height[idx_g] * height[idx_g]) / 12.0;

    }
    return (variance-noise_correction) / group.size();
}
double compute_all_path_length(const std::vector<int>& group) {
    double total_length = 0;
    for (size_t i = 0; i < group.size(); ++i) {
        for (size_t j = i + 1; j < group.size(); ++j) {
            int u = group[i], v = group[j];
            double dist;
            if(use_estimated_distance){
                dist = estimate_expected_distance(u, v);
            }else{
                dist = std::sqrt(std::pow(points_xy[u].first - points_xy[v].first, 2) +
                        std::pow(points_xy[u].second - points_xy[v].second, 2));
            }
            total_length += dist;
        }
    }
    return total_length;
}

int main() {
    ios::sync_with_stdio(false);
	cin.tie(nullptr);
    int N, M, Q, L;
    cin >> N >> M >> Q >> L >> W;
    // use_estimated_distance = true;
    if(L==3 or M>=380){
        use_estimated_distance = true;
        cerr<<"use_estimated_distance"<<endl;
        if(M>=380){
            use_mst_distance = true;
            cerr<<"use_mst_distance"<<endl;
        }
    }
    if(M>100){
        time_limit=1700;
        
    }else{
        time_limit=1200;
    }
    vector<pair<int,int>> G(M);
    for (int i = 0; i < M; ++i) {
        cin >> G[i].first;
        G[i].second = i;
    }
    sort(G.begin(), G.end(), greater<pair<int,int>>());
    
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
    for(int trial=0;trial<1000;trial++){
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
                        double dist;
                        if(use_estimated_distance){
                            dist = estimate_expected_distance(idx_g, get<2>(remaining_points[j]));
                        }else{
                            dist = std::sqrt(std::pow(points_xy[idx_g].first - get<0>(remaining_points[j]), 2) +
                                    std::pow(points_xy[idx_g].second - get<1>(remaining_points[j]), 2));
                        }
                        // auto delta= min_dist - dist;
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
            group_mst_dist_t[group_id] = compute_mst_distance(groups_t[group_id]);
            mst_dist_t += group_mst_dist_t[group_id];
            // double progress = (double) std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count() / (double) time_limit_init;
            // current_temperature =std::pow(start_temperature, 1.0 - progress) * std::pow(end_temperature, progress);
        }
        // variance
        // if(tmp_var > var_t){
        //     tmp_var = var_t;
        //     groups = groups_t;
        //     group_var = group_var_t;
        // }
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
        if(tmp_mst_dist> mst_dist_t){
            tmp_mst_dist = mst_dist_t;
            tmp_var = var_t;
            groups = groups_t;
            group_var = group_var_t;
            group_dist = group_dist_t;
            group_mst_dist = group_mst_dist_t;
        }

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
    auto best_mst_dist = tmp_mst_dist;
    const int annealing_stop_threshold_ms = 800; // 200ミリ秒改善がなければ停止
    auto last_update_time = chrono::high_resolution_clock::now();

    vector<int>weight(M);
    // vector<int>weight2(M);
    for(int i=0;i<M;i++){
       weight[i]=groups[i].size()*groups[i].size();
    //    weight2[i]=1/groups[i].size();
    }
    // 重みづけサンプリング
    discrete_distribution<> dist_g1(weight.begin(), weight.end());
    // discrete_distribution<> dist_g2(weight2.begin(), weight2.end());
    // --- 焼きなましループの前 ---
    const double accept_rate_threshold = 0.01; // 受理率1%未満で停止
    while(1) {
        // check time
        // trial_count++;
        auto current_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(current_time - start_time);
        if (duration.count() > time_limit) {
            break;
        }
        // 改善停滞チェック
        auto time_since_last_update = chrono::duration_cast<chrono::milliseconds>(current_time - last_update_time);
        if (time_since_last_update.count() > annealing_stop_threshold_ms) {
            cerr << "Annealing stopped due to stagnation at " << duration.count() << " ms" << endl;
            break; // 打ち切り
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
                        //    pow(get<1>(points_xy[p1]) - get<1>(points_xy[p2]), 2))
        
        double dist;
        if(use_estimated_distance){
            dist = estimate_expected_distance(p1, p2);
        }else{
            dist = std::sqrt(std::pow(points_xy[p1].first - points_xy[p2].first, 2) +
                        std::pow(points_xy[p1].second - points_xy[p2].second, 2));
        }
        if (dist > 5000) continue;
        // p1, p2をswapして分散を計算する
        swap(groups[g1][i1], groups[g2][i2]);
        double var1=calculate_variance(groups[g1]);
        double var2=calculate_variance(groups[g2]);
        double mst1, mst2;
        if(use_mst_distance){
            mst1=compute_mst_distance(groups[g1]);
            mst2=compute_mst_distance(groups[g2]);
        }


        // simulate annealing
        
        double delta= (var1 + var2) - (group_var[g1] + group_var[g2]);
        if(use_mst_distance){
            delta=mst1 + mst2 - (group_mst_dist[g1] + group_mst_dist[g2]);
        }
        
        
        if (delta < 0 || exp(-delta / current_temperature) > uniform_real_distribution<>(0, 1)(gen)) {
            group_var[g1] = var1;
            group_var[g2] = var2;
            tmp_var += delta;
            // accept_count++;
            if (tmp_var < best_var) {
                best_var = tmp_var;
                best_groups = groups;
                last_update_time = current_time; // 最後の更新時間を更新
            }
            if(use_mst_distance){
                group_mst_dist[g1] = mst1;
                group_mst_dist[g2] = mst2;
            }
            
            // tmp_mst_dist += delta;
            // if (tmp_mst_dist < best_var) {
            //     best_var = tmp_mst_dist;
            //     best_groups = groups;
            // }

            
        } else {
            // 分散が大きくなったら元に戻す
            swap(groups[g1][i1], groups[g2][i2]);
        }
        double progress = (double) std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count() / (double) time_limit;
        current_temperature =std::pow(start_temperature, 1.0 - progress) * std::pow(end_temperature, progress);
            // 温度閾値チェック
        const double temp_stop_threshold = 1e-5; // 例: 非常に低い温度
        if (current_temperature < temp_stop_threshold) {
            cerr << "Annealing stopped due to low temperature at " << duration.count() << " ms" << endl;
            break; // 打ち切り
        }
        // if (trial_count % check_interval == 0 && trial_count > 0) {
        //     double current_accept_rate = (double)accept_count / check_interval;
        //     if (current_accept_rate < accept_rate_threshold) {
        //         cerr << "Annealing stopped due to low acceptance rate at " << duration.count() << " ms" << endl;
        //         break; // 打ち切り
        //     }
        //     // カウンタリセット
        //     // accept_count = 0;
        //     // trial_count はリセットしない or するかは設計による
        // }

    }
    groups = best_groups;
    // Get edges from queries
    vector<set<pair<int, int>>> edges(M);
    // atcoder::dsu uf(800);
    int num_queries = 0;
    vector<int> candidate_cities;
    // compute edges
    for (int k = 0; k < M; ++k) {
        vector<int> group = groups[k];
        edges[k]=compute_mst_edges(group);
        // group_mst_dist[k] = compute_mst_distance(group);
    }
    // map<int,int>id2group;
    // for(int i=0;i<M;i++){
    //     for(auto a:groups[i]){
    //         id2group[a]=i;
    //     }
    // }
    // vector<set<int>> connection(N);
    // rep(i,M){
    //     for(auto [a,b]:edges[i]){
    //         assert(a<=N);
    //         if(a>=N){
    //             cerr<<"a: " << a << endl;
    //         }
    //         connection[a].insert(b);
            
    //         assert(b<=N);
    //         if(b>=N){
    //             cerr<<"b: " << b << endl;
    //         }
    //         connection[b].insert(a);
         
    //     }
    // }
    cerr<<"done"<<endl;
    // グループのサイズを基に重み付けサンプリング
  
    vector<int>nvisit(M,0);
    vector<int>fully_visited_group(M,0);
    vector<int>last_path_length(M,0);
    
    // map<set<int>, int> visited;
    map<set<int>, int> visited_coordinates;
    vector<pair<int,int>>last_failed(M,{1e9,0});
    // auto group_id= uniform_int_distribution<>(0, M-1)(gen);

    // int visited_streak=0;
    std::vector<double> weights(M);
    for (int i = 0; i < M; ++i) {
        weights[i] =  groups[i].size();
        if(groups[i].size()<=2) weights[i]=0;
    }
    discrete_distribution<> dist(weights.begin(), weights.end());
    while(num_queries<Q){
        // cerr<<"trial: " << num_queries << endl;
        // check time
        auto current_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(current_time - start_time);
        if (duration.count() > time_limit_final) {
            break;
        }

        
        auto group_id = dist(gen); // 重み付けサンプリングで group_id を選択
        if(fully_visited_group[group_id]>0){
            continue;
        }
        // cerr<<"group_id: " << group_id << endl;
        auto group=groups[group_id];
        if(group.size()<=2) continue;
        int pathLen = min(L-1, (int)group.size()-1);
        // if(last_failed[group_id].second>15)continue;
        auto [coordinates, erased]=generate_query(groups[group_id], edges[group_id], pathLen);
        // cerr<<"coordinates length: " << coordinates.size() << endl;
        // for(auto a:coordinates){
        //     cerr<<a<< " ";
        // }
        // cerr<<endl;
        if(coordinates.size()<=2) continue;
        if(coordinates.size()==group.size()){
            fully_visited_group[group_id]++;
        }
        if(visited_coordinates[coordinates]>0){
            // cerr<<"visited"<<" num_queries: " << num_queries << endl;
            
            for(auto [u,v]:erased){
                edges[group_id].insert({u,v});
            }
            last_failed[group_id].first=coordinates.size()-1;
            last_failed[group_id].second++;
            continue;
        }else{
            last_failed[group_id].second=0;
        }
        visited_coordinates[coordinates]++;
        auto new_edges=query(coordinates);

        atcoder::dsu uf(800);
        for(auto [u,v]:edges[group_id]){
            uf.merge(u,v);
        }
        for(auto [u,v]:new_edges){
            if(u>v) swap(u,v);
            if(uf.same(u,v)) continue;
            edges[group_id].insert({u,v});
        }
        num_queries++;

    }
    // while(num_queries<Q){
    //     // check time
    //     auto current_time = chrono::high_resolution_clock::now();
    //     auto duration = chrono::duration_cast<chrono::milliseconds>(current_time - start_time);
    //     if (duration.count() > time_limit_final) {
    //         break;
    //     }
        
    
    //     // 重みの合計を計算
    //     std::vector<double> weights(M);
    //     for (int i = 0; i < M; ++i) {
    //         weights[i] =  groups[i].size(); 
    //         if(groups[i].size()<=2) weights[i]=0;
    //     }
    //     std::discrete_distribution<> dist(weights.begin(), weights.end());
    //     auto group_id = dist(gen); // 重み付けサンプリングで group_id を選択
    //     if(fully_visited_group[group_id]>0){
    //         continue;
    //     }
    //     nvisit_group[group_id]++;
    //     auto group=groups[group_id];
    //     if(group.size()<=2) continue;
    //     auto [coordinates,used_edges]=generate_random_path(group, edges[group_id], L-1);
    //     if(coordinates.empty()){
    //         break;
    //     }

    //     sort(coordinates.begin(), coordinates.end());
    //     if(visited.count(coordinates)){
            
    //         continue;
    //     }

    //     visited.insert(coordinates);
    //     last_path_length[group_id]=coordinates.size()-1;
    //     if(coordinates.size()==group.size()){
    //         fully_visited_group[group_id]++;
    //     }
    //     for(auto [u,v]:used_edges){
    //         edges[group_id].erase({u,v});
    //     }
    //     auto ret=query(coordinates);
    //     for(auto [u,v]:ret){
    //         edges[group_id].insert({u,v});
    //     }

    //     num_queries++;
    // }
    answer(groups, edges);
    cerr<<"Queries: " << num_queries << endl;
    return 0;
}