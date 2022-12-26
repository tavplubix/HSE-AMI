#include <iostream>
#include <vector>
#include <list>
#include <limits>

size_t N, M, K, T;
std::vector<std::list<size_t>> first_part_to_second_part_edges;
std::vector<size_t> match_edges_second_to_first;
std::vector<char> vertex_used;
std::vector<char> first_vertex_in_match;

const size_t NO_VERTEX = std::numeric_limits<size_t>::max();

std::vector<size_t> path;

bool dfs(size_t from_vertex_first_part)
{
    if (vertex_used[from_vertex_first_part])
        return false;
    vertex_used[from_vertex_first_part] = true;
    for (const auto & adjacent_vertex : first_part_to_second_part_edges[from_vertex_first_part])
    {
        if (match_edges_second_to_first[adjacent_vertex] == NO_VERTEX)
        {
            path.push_back(adjacent_vertex);
            path.push_back(from_vertex_first_part);
            return true;
        }
        if (dfs(match_edges_second_to_first[adjacent_vertex]))
        {
            path.push_back(adjacent_vertex);
            path.push_back(from_vertex_first_part);
            return true;
        }
    }
    return false;
}

int main()
{
    std::cin.sync_with_stdio(false);
    std::cout.sync_with_stdio(false);

    std::cin >> N >> M >> K;
    first_part_to_second_part_edges.resize(N);
    for (size_t i = 0; i < K; ++i)
    {
        size_t first_part_vertex;
        size_t second_part_vertex;
        std::cin >> first_part_vertex >> second_part_vertex;
        first_part_to_second_part_edges[first_part_vertex - 1].push_back(second_part_vertex - 1);
    }
    std::cin >> T;
    match_edges_second_to_first.resize(M, NO_VERTEX);
    first_vertex_in_match.resize(N, false);
    for (size_t i = 0; i < T; ++i)
    {
        size_t first_part_vertex;
        size_t second_part_vertex;
        std::cin >> first_part_vertex >> second_part_vertex;
        match_edges_second_to_first[second_part_vertex - 1] = first_part_vertex - 1;
        first_vertex_in_match[first_part_vertex - 1] = true;
    }

    for (size_t i = 0; i < N; ++i)
    {
        vertex_used.resize(N, false);
        if (!first_vertex_in_match[i] && dfs(i))
            break;
    }
    std::cout << path.size() << '\n';
    for (size_t i = path.size(); i != 0; --i)
        std::cout << path[i - 1] + 1 << ' ';

    return 0;
}