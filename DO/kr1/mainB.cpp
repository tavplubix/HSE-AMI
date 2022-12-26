#include <iostream>
#include <vector>
#include <list>
#include <limits>
#include <queue>
#include <utility>
#include <cmath>
#include <algorithm>
#include <set>

size_t N;
std::vector<std::vector<double>> distances;
std::vector<std::pair<long long, long long>> points;
double T;

const size_t NO_VERTEX = std::numeric_limits<size_t>::max();
const double NO_EDGE = std::numeric_limits<double>::max();

std::vector<std::list<size_t>> mst;
std::vector<double> min_edge_weight_for_mst;
std::vector<size_t> min_edge_end;

std::vector<char> vertex_used_in_dfs;
std::vector<double> full_path_len;
std::vector<size_t> full_path;

void dfs(size_t vertex = 0, double dist = 0)
{
    if (vertex_used_in_dfs[vertex])
        return;
    vertex_used_in_dfs[vertex] = true;
    full_path.push_back(vertex);
    for (const auto & adjacent_vertex : mst[vertex])
        dfs(adjacent_vertex, distances[vertex][adjacent_vertex]);
}




void find_mst()
{
    min_edge_weight_for_mst[0] = 0;
    min_edge_end[0] = 0;
    for (size_t i = 0; i < N; ++i)
    {
        size_t vertex = NO_VERTEX;
        for (size_t j = 0; j < N; ++j)
        {
            if (!mst[j].empty())
                continue;
            if (vertex == NO_VERTEX)
                vertex = j;
            else if (min_edge_weight_for_mst[j] < min_edge_weight_for_mst[vertex])
                vertex = j;
        }


        size_t edge_end = min_edge_end[vertex];
        mst[edge_end].push_back(vertex);
        mst[vertex].push_back(edge_end);

        for (size_t j = 0; j < N; ++j)
        {
            if (distances[vertex][j] < min_edge_weight_for_mst[j])
            {
                min_edge_weight_for_mst[j] = distances[vertex][j];
                min_edge_end[j] = vertex;
            }
        }
    }
}

int main()
{
    std::cin.sync_with_stdio(false);
    std::cout.sync_with_stdio(false);

    std::cin >> N;
    ++N;
    points.resize(N);
    points[0] = std::make_pair(0ll, 0ll);
    for (size_t i = 1; i < N; ++i)
        std:: cin >> points[i].first >> points[i].second;
    std::cin >> T;

    distances.resize(N);
    for (size_t i = 0; i < N; ++i)
        distances[i].resize(N);
    for (size_t i = 0; i < N; ++i)
    {
        for (size_t j = 0; j < N; ++j)
        {
            auto dx = points[i].first - points[j].first;
            auto dy = points[i].second - points[j].second;
            double dist = sqrt(double(dx * dx + dy * dy));
            distances[i][j] = dist;
            distances[j][i] = dist;
        }
    }

    mst.resize(N);
    min_edge_weight_for_mst.resize(N, NO_EDGE);
    min_edge_end.resize(N, NO_VERTEX);

    find_mst();

    vertex_used_in_dfs.resize(N, false);
    dfs(0, 0);
    //full_path_len.push_back(full_path_len.back() + distances[full_path.back()][0]);
    full_path.push_back(0);
    double total_len = 0;
    size_t prev = 0;
    for (const auto & v : full_path)
    {
        full_path_len.push_back(total_len += distances[prev][v]);
        prev = v;
    }

    //for (auto v: full_path) std::cout << v << ' ';
    //std::cout <<  std::endl;
    //for (auto v: full_path_len) std::cout << v << ' ';
    //std::cout <<  std::endl;
    //full_path.erase(full_path.begin());
    //full_path_len.erase(full_path_len.begin());
    --N;

    if (full_path.size() != N + 2 || full_path_len.size() != full_path.size())
        throw 1;
    if (full_path.front() || full_path.back())
        throw 2;
    if (full_path_len.front() != 0 || full_path_len.back() > 2*T)
        throw 3;
    std::set<size_t> path;
    prev = 0;
    double eps = 1e-5;
    for (size_t i = 1; i <= N; ++i)
    {
        if (!(0 < full_path[i] && full_path[i] <= N))
            throw 4;
        if (path.find(full_path[i]) != path.end())
            throw 5;
        if (std::isnan(full_path_len[i]) || std::isinf(full_path_len[i]))
            throw 6;
        double diff = full_path_len[i] - full_path_len[i - 1];
        if (std::abs(diff - distances[prev][full_path[i]]) > eps)
            throw 7;
        path.insert(full_path[i]);
        prev = full_path[i];
    }

    size_t half_n = (N + 1) / 2;
    if (full_path_len[half_n] <= T)
    {
        for (size_t i = 1; i <= half_n; ++i)
            std::cout << full_path[i] << ' ';
        return 0;
    }

    if (N % 2 == 0)
        ++half_n;
    if (full_path_len.back() - full_path_len[half_n] <= T)
    {
        for (size_t i = N; i >= half_n; --i)
            std::cout << full_path[i] << ' ';
        return 0;
    }

    throw 0;
}
