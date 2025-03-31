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
const int time_limit = 1850;
const int time_limit_init = 500;
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
    vector<pair<int, int>> points_xy(N); // use tuple<int,int> instead of tuple<int, int, int>
    for(int i =0; i<N; i++){
        points_xy[i] = make_pair(x[i],y[i]);
    }


    
    
    vector<double>group_var(M, 0);
    double tmp_var = 1e15;
    double tmp_dist =1e15;
    double start_temperature = 1e7;
    double end_temperature = 1e0;
    double current_temperature = start_temperature;
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
        vector<vector<int>> groups_t(M);
        vector<double> group_var_t(M, 0);
        vector<tuple<int, int, int>> remaining_points = points;
        for (int i = 0; i < M; ++i) {
            // Select a random point from points
            random_device rd;
            mt19937 gen(rd());
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
                        if (dist < min_dist or .05> uniform_real_distribution<>(0, 1)(gen)) {
                            min_dist = dist;
                            min_idx = (int)j; // cast j to int
                        }
                    }
                }
                dist_t += min_dist;
                groups_t[group_id].push_back(get<2>(remaining_points[min_idx]));
                remaining_points.erase(remaining_points.begin() + min_idx);
            }
            double var=calculate_variance(groups_t[group_id], points_xy);
            group_var_t[group_id] = var;
            var_t += var;
            double progress = (double) std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count() / (double) time_limit_init;
            current_temperature =std::pow(start_temperature, 1.0 - progress) * std::pow(end_temperature, progress);
        }
        // if(tmp_var > var_t){
        //     tmp_var = var_t;
        //     groups = groups_t;
        //     group_var = group_var_t;
        // }
        if(tmp_dist * tmp_var > dist_t*var_t){
            tmp_dist = dist_t;
            tmp_var = var_t;
            groups = groups_t;
            group_var = group_var_t;
        }
    }
    cerr<<"tmp_var: " << tmp_var << endl;
    random_device rd;
    mt19937 gen(rd());

    // Simulated annealing
    // 温度
    start_temperature = 1e7;
    end_temperature = 1e0;
    current_temperature = start_temperature;
    auto best_groups = groups;
    auto best_group_var = group_var;
    auto best_var = tmp_var;
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
        // double dist = sqrt(pow(get<0>(points_xy[p1]) - get<0>(points_xy[p2]), 2) +
                        //    pow(get<1>(points_xy[p1]) - get<1>(points_xy[p2]), 2));
        double dist = sqrt(pow(points_xy[p1].first - points_xy[p2].first, 2) +
                               pow(points_xy[p1].second - points_xy[p2].second, 2));
        if (dist > 2*W * sqrt(2)) continue;
        // p1, p2をswapして分散を計算する
        swap(groups[g1][i1], groups[g2][i2]);
        double var1=calculate_variance(groups[g1], points_xy);
        double var2=calculate_variance(groups[g2], points_xy);
        // 分散が小さくなったらswapを維持
        // simulate annealing
        double delta= (var1 + var2) - (group_var[g1] + group_var[g2]);
        
        if (delta < 0 || exp(-delta / current_temperature) > uniform_real_distribution<>(0, 1)(gen)) {
            group_var[g1] = var1;
            group_var[g2] = var2;
            tmp_var += delta;
            if (tmp_var < best_var) {
                best_var = tmp_var;
                best_groups = groups;
            }
        } else {
            // 分散が大きくなったら元に戻す
            swap(groups[g1][i1], groups[g2][i2]);
        }
        double progress = (double) std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count() / (double) time_limit;
        current_temperature =std::pow(start_temperature, 1.0 - progress) * std::pow(end_temperature, progress);

    }
    groups = best_groups;
    // Get edges from queries
    vector<set<pair<int, int>>> edges(M);
    atcoder::dsu uf(800);
    int num_queries = 0;
    vector<int> boundary_city;
    for (int k = 0; k < M; ++k) {
        vector<int> group = groups[k];
        // sort(group.begin(), group.end(), [&](int a, int b) {
        //     if (x[a] != x[b]) {
        //         return x[a] < x[b];
        //     }
        //     return y[a] < y[b];
        // });
        group = compute_mst_with_order(group, points_xy);
        
        int idx = 0;
        // L=min(L,15); 
        for (int i = 0; i <= (int)group.size() - L; i += L-1) {
            vector<int> sub_group(group.begin() + i, group.begin() + min((int)group.size(), i + L));
            boundary_city.push_back(sub_group.back());
            set<pair<int, int>> ret = query(sub_group);
            num_queries++;
            for (const auto& p : ret) {
                if(uf.same(p.first, p.second)) continue;
                edges[k].insert(p);
                uf.merge(p.first, p.second);
            }
            idx = i + L-1;
        }
        // 最後の余りの部分
        if ((int)group.size()-idx >2)
        {
            vector<int> sub_group(group.begin() + idx, group.end());
            set<pair<int, int>> ret = query(sub_group);
            num_queries++;
            for (const auto& p : ret) {
                if(uf.same(p.first, p.second)) continue;
                edges[k].insert(p);
                uf.merge(p.first, p.second);
            }
        }
        else if ((int)group.size() -idx ==2){
            int u=group[idx];
            int v=group[idx+1];
            if(u>v)swap(u,v);
            if(uf.same(u,v)) continue;
            num_queries++;
            uf.merge(u,v);
            edges[k].insert({u,v});
        }
        
    }
    map<int,int>id2group;
    for(int i=0;i<M;i++){
        for(auto a:groups[i]){
            id2group[a]=i;
        }
    }
    map<int,set<int>> connection;
    rep(i,M){
        
        for(auto [a,b]:edges[i]){
        //     connection[a].insert(b);
        //     connection[b].insert(a);
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
    set<vector<int>>visited;
    while(num_queries < Q){
        // check time
        auto current_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(current_time - start_time);
        if (duration.count() > time_limit+100) {
            break;
        }
        int a;
        if(boundary_city.size()){
            int aid= uniform_int_distribution<>(0, boundary_city.size()-1)(gen);
            a=boundary_city[aid];
        }else{
            a= uniform_int_distribution<>(0, N-1)(gen);
        }
        int agroup=id2group[a];
        if(connection[a].size() <=1) continue;
        int bid=0;
        int cid=0;
        while(bid==cid){
            bid= uniform_int_distribution<>(0,connection[a].size()-1)(gen);
            cid= uniform_int_distribution<>(0,connection[a].size()-1)(gen);
        }
        int b,c;
        b= *next(connection[a].begin(),bid);
        c= *next(connection[a].begin(),cid);
        vector<int>visit={a,b,c};
        sort(visit.begin(), visit.end());
        if(visited.count(visit)) continue;
        visited.insert(visit);
        pii edge1={a,b};
        pii edge2={a,c};
        if(a>b)swap(edge1.x,edge1.y);
        if(a>c)swap(edge2.x,edge2.y);
        if(edges[agroup].count(edge1)==1 and edges[agroup].count(edge2)==1){
            // cerr<<"ok"<<endl;
        }else{
            cerr<<"ng"<<endl;
            continue;
        }

        auto edges_new=query(vector<int>{a,b,c});
        edges[agroup].erase(edge1);
        edges[agroup].erase(edge2);
        connection[a].erase(b);
        connection[a].erase(c);
        connection[b].erase(a);
        connection[c].erase(a);
        for(auto [u,v]:edges_new){
            if(u>v)swap(u,v);
            edges[agroup].insert({u,v});
            connection[u].insert(v);
            connection[v].insert(u);
        }
        num_queries++;   

    }
    answer(groups, edges);
    cerr<<"Queries: " << num_queries << endl;
    return 0;
}