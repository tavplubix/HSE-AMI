#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <tuple>

struct Item {
    int w;
    int c;
    size_t idx;
};

int W;
size_t N;

typedef std::vector<Item> Items;
typedef std::vector<int> Dpc;

std::vector<Item> items;

std::tuple<int, int, Dpc> dp_solve(const Items& items, size_t beg, size_t end, int W)
{
    size_t count = end - beg;
    Dpc old_max_cost(W + 1, 0);
    Dpc new_max_cost(W + 1);

    for (size_t i = beg; i < end; ++i) {
        for (size_t w = 0; w <= W; ++w) {
            if (items[i].w <= w)
                new_max_cost[w] = std::max(old_max_cost[w], old_max_cost[w - items[i].w] + items[i].c);
        }
        old_max_cost = new_max_cost;
    }

    int c = new_max_cost[W];
    while (W && new_max_cost[W - 1] == c) --W;

    return std::make_tuple(W, c, std::move(new_max_cost));
}

std::vector<size_t> dp_restore(const Items& items, size_t beg, size_t end, int W, int C)
{
    size_t count = end - beg;
    if (count == 0)
        return {};
    if (count == 1)
        return items[beg].w <= W ? std::vector<size_t>(1, beg) : std::vector<size_t>();


    size_t mid = (beg + end) / 2;
    size_t j = 0;
    int lcost, rcost;
    {
        auto left = dp_solve(items, beg, mid, W);
        auto right = dp_solve(items, mid, end, W);
        const Dpc & dpl = std::get<2>(left);
        const Dpc & dpr = std::get<2>(right);

        while (j <= W && dpl[j] + dpr[W - j] != C) ++j;
        lcost = dpl[j];
        rcost = dpr[W - j];
    }

    auto l = dp_restore(items, beg, mid, j, lcost);
    auto r = dp_restore(items, mid, end, W - j, rcost);

    l.insert(l.end(), r.begin(), r.end());
    return l;
}

const int Wdiff = 700000;

int main()
{
    std::cin.sync_with_stdio(false);
    std::cout.sync_with_stdio(false);
    std::cin >> N >> W;
    items.resize(N);
    int Wsum = 0;
    int Csum = 0;
    for (size_t i = 0; i < N; ++i)
    {
        std::cin >> items[i].c >> items[i].w;
        items[i].idx = i;
        Wsum += items[i].w;
        Csum += items[i].c;
    }
    std::sort(items.begin(), items.end(), [](const Item& a, const Item& b) {
        return float(a.c) / a.w > float(b.c) / b.w;
    });

    size_t greedy_count = 0;
    int greedy_c = 0;
    int greedy_w = 0;
    for (size_t i = 0; i < N; ++i)
    {
        if (W - Wdiff < greedy_w + items[greedy_count].w)
            break;
        greedy_w += items[greedy_count].w;
        greedy_c += items[greedy_count].c;
        ++greedy_count;
    }


    size_t greedy_count_end = greedy_count;
    int greedy_c_end = greedy_c;
    int greedy_w_end = greedy_w;
    for (size_t i = 0; i < N; ++i)
    {
        if (W + Wdiff < greedy_w_end + items[greedy_count_end].w)
            break;
        greedy_w_end += items[greedy_count_end].w;
        greedy_c_end += items[greedy_count_end].c;
        ++greedy_count_end;
    }

    auto s = dp_solve(items, greedy_count, greedy_count_end, W - greedy_w);
    auto sol = dp_restore(items, greedy_count, greedy_count_end, std::get<0>(s), std::get<1>(s));


    Wsum = 0;
    Csum = 0;
    std::cout << greedy_c + std::get<1>(s) << std::endl;
    for (size_t i = 0; i < greedy_count; ++i)
    {
        Wsum += items[i].w;
        Csum += items[i].c;
        std::cout << items[i].idx + 1 << ' ';
    }
    for (auto idx : sol)
    {
        Wsum += items[idx].w;
        Csum += items[idx].c;
        std::cout << items[idx].idx + 1 << ' ';
    }
    std::cout << std::endl;
    //std::cout << greedy_count << ' ' << greedy_w << ' ' << greedy_c << std::endl;
    //std::cout << sol.size() << ' ' << Wsum - greedy_w << ' ' << Csum - greedy_c << std::endl;
    //std::cout << greedy_count + sol.size() << ' ' << Wsum << ' ' << Csum << std::endl;


    return 0;
}