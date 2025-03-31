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
const int TIME_LIMIT = 1850;
const int INITIAL_GROUPING_TIME_LIMIT = 1500;
bool LOCAL = false;

using ll = long long;

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
    for (size_t i = 0; i < groups.size(); ++i) {
        for (int val : groups[i]) {
            std::cout << val << " ";
        }
        std::cout << std::endl;

        for (const auto& edge : edges[i]) {
            std::cout << edge.first << " " << edge.second << std::endl;
        }
    }
}

// Function to compute the MST length
double compute_mst_length(const std::vector<int>& group, const std::vector<pii>& points_xy) {
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
std::vector<int> compute_mst_with_order(const std::vector<int>& group, const std::vector<pii>& points_xy) {
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
double calculate_variance(const std::vector<int>& group, const std::vector<pii>& points_xy) {
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
    }
    return variance / group.size();
}
int main() {
    int N, M, Q, L, W;
    std::cin >> N >> M >> Q >> L >> W;

    std::vector<std::pair<int, int>> group_sizes(M);
    for (int i = 0; i < M; ++i) {
        std::cin >> group_sizes[i].first;
        group_sizes[i].second = i;
    }
    std::sort(group_sizes.begin(), group_sizes.end(), std::greater<std::pair<int, int>>());

    std::vector<int> lx(N), rx(N), ly(N), ry(N);
    for (int i = 0; i < N; ++i) {
        std::cin >> lx[i] >> rx[i] >> ly[i] >> ry[i];
    }

    std::vector<int> x(N), y(N);
    for (int i = 0; i < N; ++i) {
        x[i] = (lx[i] + rx[i]) / 2;
        y[i] = (ly[i] + ry[i]) / 2;
    }

    std::vector<pii> points_xy(N);
    for (int i = 0; i < N; ++i) {
        points_xy[i] = {x[i], y[i]};
    }

    std::vector<std::vector<int>> groups(M);
    std::vector<double> group_variance(M, 0);

    // Initial Grouping
    double best_mst_distance = 1e15;
    std::vector<std::vector<int>> best_groups;

    auto current_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = current_time - start_time;
    std::mt19937 gen(std::random_device{}());
    std::vector<int> remaining_points(N);
    std::iota(remaining_points.begin(), remaining_points.end(), 0);
    vector<int>start_points(M);
    rep(i,M){
        std::uniform_int_distribution<> distrib(0, (int)remaining_points.size() - 1);
        int index = distrib(gen);
        start_points[i] = remaining_points[index];
        remaining_points.erase(remaining_points.begin() + index);
    }
    double start_temperature1 = 1e9;
    double end_temperature1 = 1e0;
    double current_temperature1 = start_temperature1;
    double best_dist=1e18;
    while (duration.count() < INITIAL_GROUPING_TIME_LIMIT) {
        std::vector<std::vector<int>> temp_groups(M);
        std::uniform_int_distribution<> distrib(0, M-1);
        std::uniform_int_distribution<> distrib2(0, remaining_points.size() - 1);
        int group_id = distrib(gen);
        int chindex = distrib2(gen);
        swap(remaining_points[chindex], start_points[group_id]);
        auto tmp_remaining_points = remaining_points;
        double tmp_dist = 0;
        for (int i = 0; i < M; ++i) {
            int group_id = group_sizes[i].second;
            int start_index = start_points[i];
            temp_groups[group_id].push_back(start_index);    
            
            while (temp_groups[group_id].size() < (size_t)group_sizes[i].first) {
                double min_dist = 1e9;
                int min_index = -1;

                for (size_t j = 0; j < remaining_points.size(); ++j) {
                    for (int point_in_group : temp_groups[group_id]) {
                        double dist = std::sqrt(std::pow(points_xy[remaining_points[j]].first - points_xy[point_in_group].first, 2) +
                                                 std::pow(points_xy[remaining_points[j]].second - points_xy[point_in_group].second, 2));
                         if (dist < min_dist) {
                            min_dist = dist;
                            min_index = j;
                        }
                    }
                }
                tmp_dist+= min_dist;
                if (min_index != -1) {
                    temp_groups[group_id].push_back(remaining_points[min_index]);
                    remaining_points.erase(std::remove(remaining_points.begin(), remaining_points.end(), remaining_points[min_index]), remaining_points.end());
                }else{
                    if(!remaining_points.empty()){
                        distrib = std::uniform_int_distribution<>(0, (int)remaining_points.size() - 1);
                        start_index = remaining_points[distrib(gen)];
                        temp_groups[group_id].push_back(start_index);
                        remaining_points.erase(std::remove(remaining_points.begin(), remaining_points.end(), start_index), remaining_points.end());
                    }
                    else{
                        break;
                    }
                }
            }
        }

        // Calculate MST distance
        // double current_mst_distance = 0;
        // for (int i = 0; i < M; ++i) {
        //     current_mst_distance += compute_mst_length(temp_groups[i], points_xy);
        // }
        remaining_points = tmp_remaining_points;
        // if (current_mst_distance < best_mst_distance or exp(-(current_mst_distance - best_mst_distance) / current_temperature1 > std::uniform_real_distribution<>(0, 1)(gen)) ){
        //     best_mst_distance = current_mst_distance;
        //     best_groups = temp_groups;
        double delta= tmp_dist - best_dist;
        if (delta < 0 || std::exp(-delta / current_temperature1) > std::uniform_real_distribution<>(0, 1)(gen)) {
            best_dist = tmp_dist;
            best_groups = temp_groups;
        }else{
            swap(remaining_points[chindex], start_points[group_id]);
        }
        duration = std::chrono::high_resolution_clock::now() - start_time;
        double progress = (double)duration.count() / (double)TIME_LIMIT;
        current_temperature1 = std::pow(start_temperature1, 1.0 - progress) * std::pow(end_temperature1, progress);
        
        
    }

    groups = best_groups;

    // Simulated Annealing
    double start_temperature = 1e9;
    double end_temperature = 1e0;
    double current_temperature = start_temperature;
    double best_variance = 1e18;

    while (true) {
        current_time = std::chrono::high_resolution_clock::now();
        duration = current_time - start_time;
        if (duration.count() > TIME_LIMIT) break;

        int g1 = std::uniform_int_distribution<>(0, M - 1)(gen);
        int g2 = std::uniform_int_distribution<>(0, M - 1)(gen);
        if (g1 == g2 || groups[g1].empty() || groups[g2].empty()) continue;

        int i1 = std::uniform_int_distribution<>(0, (int)groups[g1].size() - 1)(gen);
        int i2 = std::uniform_int_distribution<>(0, (int)groups[g2].size() - 1)(gen);

        int p1 = groups[g1][i1];
        int p2 = groups[g2][i2];

        double dist = std::sqrt(std::pow(points_xy[p1].first - points_xy[p2].first, 2) +
                                 std::pow(points_xy[p1].second - points_xy[p2].second, 2));
        if (dist > 2 * W * std::sqrt(2)) continue;

        std::swap(groups[g1][i1], groups[g2][i2]);

        double var1 = calculate_variance(groups[g1], points_xy);
        double var2 = calculate_variance(groups[g2], points_xy);

        double delta = (var1 + var2) - (group_variance[g1] + group_variance[g2]);

        if (delta < 0 || std::exp(-delta / current_temperature) > std::uniform_real_distribution<>(0, 1)(gen)) {
            group_variance[g1] = var1;
            group_variance[g2] = var2;
            double current_variance = 0;
            for(auto v: group_variance){
                current_variance+=v;
            }
            if (current_variance < best_variance) {
                best_variance = current_variance;
                best_groups = groups;
            }
        } else {
            std::swap(groups[g1][i1], groups[g2][i2]);
        }

        double progress = (double)duration.count() / (double)TIME_LIMIT;
        current_temperature = std::pow(start_temperature, 1.0 - progress) * std::pow(end_temperature, progress);
    }

    groups = best_groups;
    std::vector<std::set<pii>> edges(M);
    int num_queries = 0;

    // Edge Queries
    for (int k = 0; k < M; ++k) {
        std::vector<int> group = groups[k];
        group = compute_mst_with_order(group, points_xy);

        int idx = 0;
        for (int i = 0; i <= (int)group.size() - L; i += L - 1) {
            std::vector<int> sub_group(group.begin() + i, group.begin() + std::min((int)group.size(), i + L));
            std::set<pii> ret = query(sub_group);
            num_queries++;
            for (const auto& p : ret) {
                edges[k].insert(p);
            }
            idx = i + L - 1;
        }
        if ((int)group.size() - idx > 2) {
            std::vector<int> sub_group(group.begin() + idx, group.end());
            std::set<pii> ret = query(sub_group);
            num_queries++;
            for (const auto& p : ret) {
                edges[k].insert(p);
            }
        } else if ((int)group.size() - idx == 2) {
            int u = group[idx];
            int v = group[idx + 1];
            if (u > v) std::swap(u, v);
            edges[k].insert({u, v});
        }
    }

    // Fixes
    std::map<int, int> id_to_group;
    for (int i = 0; i < M; i++) {
        for (auto a : groups[i]) {
            id_to_group[a] = i;
        }
    }
    std::map<int, std::set<int>> connections;
    rep(i, M) {
        for (auto [a, b] : edges[i]) {
            if (a >= N || b >= N) {
                continue;
            }
            connections[a].insert(b);
            connections[b].insert(a);
        }
    }

    // Additional Queries
    while (num_queries < Q) {
        current_time = std::chrono::high_resolution_clock::now();
        duration = current_time - start_time;
        if (duration.count() > TIME_LIMIT + 100) break;

        int a = std::uniform_int_distribution<>(0, N - 1)(gen);
        if (connections[a].size() <= 1) continue;
        int a_group = id_to_group[a];

        int bid = 0, cid = 0;
        while (bid == cid) {
            bid = std::uniform_int_distribution<>(0, connections[a].size() - 1)(gen);
            cid = std::uniform_int_distribution<>(0, connections[a].size() - 1)(gen);
        }

        int b = *std::next(connections[a].begin(), bid);
        int c = *std::next(connections[a].begin(), cid);
        pii edge1 = {a, b};
        pii edge2 = {a, c};
        if (a > b) std::swap(edge1.x, edge1.y);
        if (a > c) std::swap(edge2.x, edge2.y);
        if (edges[a_group].count(edge1) == 1 && edges[a_group].count(edge2) == 1) {
        } else {
            continue;
        }

        auto edges_new = query(std::vector<int>{a, b, c});
        edges[a_group].erase(edge1);
        edges[a_group].erase(edge2);
        connections[a].erase(b);
        connections[a].erase(c);
        connections[b].erase(a);
        connections[c].erase(a);

        for (auto [u, v] : edges_new) {
            if (u > v) std::swap(u, v);
            edges[a_group].insert({u, v});
            connections[u].insert(v);
            connections[v].insert(u);
        }
        num_queries++;
    }

    answer(groups, edges);
    std::cerr << "Queries: " << num_queries << std::endl;
    return 0;
}