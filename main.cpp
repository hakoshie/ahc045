#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <algorithm>
#define _USE_MATH_DEFINES
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
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

#define rep(i, x) for (int i = 0; i < (int)(x); i++)
#define FOR(i, a, done) for (int i = (a); i < (done); ++i)
#define all(a) (a).begin(), (a).end()
#define x first
#define y second
#define debug(a) cout << (a) << endl
namespace mp = boost::multiprecision;
using cpp_int = mp::cpp_int;
using ll = long long;
using namespace std;
template <class T>
bool chmax(T &a, const T &b)
{
	if (a < b)
	{
		a = b;
		return 1;
	}
	return 0;
}
template <class T>
bool chmin(T &a, const T &b)
{
	if (a > b)
	{
		a = b;
		return 1;
	}
	return 0;
}

const int N = 800, Q = 400;
int M,L,W;
vector<pair<int, int>> city_rect[N]; // 各都市の長方形範囲 (xmin, xmax), (ymin, ymax)
vector<vector<tuple<int,int,int>>> mst_edges; // クエリで得たMSTの情報

// クエリを実行して MST を取得する (仮の実装)
vector<tuple<int, int, int>> query_mst(const vector<int>& cities) {
    cout << "? " << cities.size();
    for (int c : cities) cout << " " << c + 1;
    cout << endl;
    
    int num_edges;
    cin >> num_edges;
    vector<tuple<int, int, int>> mst;
    for (int i = 0; i < num_edges; i++) {
        int u, v, d;
        cin >> u >> v >> d;
        mst.emplace_back(u - 1, v - 1, d);
    }
    return mst;
}

// MST の情報を用いて都市の距離を推定
void estimate_distances() {
    for (int i = 0; i < Q; i++) {
        vector<int> sample;
        for (int j = 0; j < 10; j++) {
            sample.push_back(rand() % N);
        }
        auto mst = query_mst(sample);
        mst_edges.push_back(mst);
    }
}

// 都市を M グループに分割 (クラスタリング)
vector<vector<int>> cluster_cities() {
    vector<vector<int>> clusters(M);
    for (int i = 0; i < N; i++) {
        clusters[i % M].push_back(i);
    }
    return clusters;
}

// グループごとに MST を作る
vector<tuple<int, int>> construct_roads(const vector<vector<int>>& clusters) {
    vector<tuple<int, int>> roads;
    for (const auto& cluster : clusters) {
        auto mst = query_mst(cluster);
        for (auto [u, v, d] : mst) {
            roads.emplace_back(u, v);
        }
    }
    return roads;
}

// メイン処理
int main() {
    int tN, tQ,W;
    cin>>tN>>M>>tQ>>L>>W;
    vector<int>G(M);
    for (int i = 0; i < M; i++) {
        cin >> G[i];
    }
    for (int i = 0; i < N; i++) {
        int xmin, xmax, ymin, ymax;
        cin >> xmin >> xmax >> ymin >> ymax;
        city_rect[i] = {{xmin, xmax}, {ymin, ymax}};
    }
    
    estimate_distances();
    auto clusters = cluster_cities();
    auto roads = construct_roads(clusters);
    
    cout << "! " << roads.size() << endl;
    for (auto [u, v] : roads) {
        cout << u + 1 << " " << v + 1 << endl;
    }
    return 0;
}
