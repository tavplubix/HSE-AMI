#include <iostream>
#include <random>
#include <limits>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <functional>

#include <omp.h>

using arr = std::vector<unsigned>;

std::random_device rd;
std::mt19937 gen(rd());

arr gen_rand_arr(size_t size)
{
    std::uniform_int_distribution<unsigned> rand(0, size);
    arr res(size);
    for (auto & x : res)
        x = rand(gen);
    return res;
}

arr gen_reverse_arr(size_t size)
{
    arr res(size);
    for (size_t i = 0; i < size; ++i)
        res[i] = size - 1 - i;
    return res;
}

void print_arr(const arr & a)
{
    for (const auto & x : a)
        std::cout << x << ' ';
    std::cout << std::endl;
}

using iter = arr::iterator;

void quick_sort(size_t num_threads, iter beg, iter end, unsigned init_seed)
{
    size_t size = end - beg;
    if (size <= 1) return;
    unsigned r = rand_r(&init_seed);
    unsigned pivot = *(beg + r % size);
    auto mid = std::partition(beg, end, [=](auto x) { return x < pivot; });
    auto rmid = mid;
    while (rmid != end && *rmid == pivot)
        ++rmid;

    #pragma omp parallel for default(none) firstprivate(num_threads, beg, mid, rmid, end, r) if (num_threads > 1)
    for (size_t i = 0; i < 2; ++i)
    {
        //if (num_threads > 1)
        //    std::cerr << "S " << num_threads << " T " << omp_get_thread_num() << " i " << i << " s " << end - beg << std::endl;
        if (i)
            quick_sort(num_threads / 2, beg, mid, rand_r(&r));
        else
            quick_sort(num_threads / 2, rmid, end, rand_r(&r));

        //if (num_threads > 1)
        //    std::cerr << "F " << num_threads << " T " << omp_get_thread_num() << " i " << i << " s " << end - beg << std::endl;
    }
}

using sort_f = std::function<void(iter, iter)>;

void merge_sort(size_t num_threads, iter beg, iter end, const sort_f & other_sort = {})
{
    if (num_threads > 1 || !other_sort)
    {
        size_t size = end - beg;
        if (size <= 1) return;
        auto mid = beg + size / 2;

        #pragma omp parallel for default(none) firstprivate(num_threads, beg, mid, end, other_sort) if (num_threads > 1)
        for (size_t i = 0; i < 2; ++i)
        {
            if (i)
                merge_sort(num_threads / 2, beg, mid, other_sort);
            else
                merge_sort(num_threads / 2, mid, end, other_sort);
        }

        arr res(size);
        std::merge(beg, mid, mid, end, res.begin());
        std::copy(res.begin(), res.end(), beg);
    }
    else
    {
        other_sort(beg, end);
    }
}

void insertion_sort(iter beg, iter end)
{
    size_t size = end - beg;
    for (size_t i = 1; i < size; ++i)
    {
        unsigned key = *(beg + i);
        auto it = beg + i;
        while (it > beg && *(it - 1) > key)
        {
            *it = *(it - 1);
            --it;
        }
        *it = key;
    }
}

void selection_sort(iter beg, iter end)
{
    size_t size = end - beg;
    for (size_t i = 0; i + 1 < size; ++i)
    {
        auto min = beg + i;
        for (size_t j = i + 1; j < size; ++j)
            if (*(beg + j) < *min)
                min = beg + j;
        if (min != (beg + i))
            std::swap(*min, *(beg + i));
    }

}

void assert_sorted(const arr & a)
{
    for (size_t i = 1; i < a.size(); ++i)
        if (a[i] < a[i - 1])
            exit(2);
}

void on_error()
{
    std::cerr << "Usage:\nexport OMP_NESTED=TRUE\nhw2 <array_size> <quick|merge|insertion|selection> <random|reverse> <num_threads>\n";
    exit(1);
}

arr a;

int main(int argc, char ** argv)
{
    if (argc < 5)
        on_error();

    size_t N = std::strtoul(argv[1], nullptr, 10);
    size_t num_threads = std::strtoul(argv[4], nullptr, 10);

    if (strcmp(argv[3], "random") == 0)
        a = gen_rand_arr(N);
    else if (strcmp(argv[3], "reverse") == 0)
        a = gen_reverse_arr(N);
    else
        on_error();

    //print_arr(a);

    int seed = std::uniform_int_distribution<unsigned>(0, std::numeric_limits<int>::max())(gen);

    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

    if (strcmp(argv[2], "quick") == 0)
        quick_sort(num_threads, a.begin(), a.end(), seed);
    else if (strcmp(argv[2], "merge") == 0)
        merge_sort(num_threads, a.begin(), a.end(), [](iter beg, iter end) { merge_sort(1, beg, end); } );
    else if (strcmp(argv[2], "insertion") == 0)
        merge_sort(num_threads, a.begin(), a.end(), [](iter beg, iter end) { insertion_sort(beg, end); } );
        //insertion_sort(a.begin(), a.end());
    else if (strcmp(argv[2], "selection") == 0)
        merge_sort(num_threads, a.begin(), a.end(), [](iter beg, iter end) { selection_sort(beg, end); } );
        //selection_sort(a.begin(), a.end());
    else
        on_error();

    std::chrono::time_point<std::chrono::system_clock> finish = std::chrono::system_clock::now();

    //print_arr(a);
    assert_sorted(a);

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() << std::endl;
    return 0;
}
