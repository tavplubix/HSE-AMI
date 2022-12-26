#include <iostream>
#include <future>
#include <vector>
#include <utility>

#include <climits>
#include <cstdint>
#include <cstdlib>

std::pair<size_t, size_t>
monte_carlo(unsigned thread_id, size_t iterations)
{
    constexpr uint64_t radius = RAND_MAX < UINT32_MAX ? RAND_MAX : UINT32_MAX;
    size_t inside_circle = 0;
    for (size_t i = 0; i < iterations; ++i) {
        uint64_t x = rand_r(&thread_id) % radius;
        uint64_t y = rand_r(&thread_id) % radius;
        if (x * x + y * y <= radius * radius) {
            ++inside_circle;
        }
    }
    return std::make_pair(inside_circle, iterations);
}

int
main(int argc, char **argv)
{
    std::vector<std::future<std::pair<size_t, size_t>>> futures;
    unsigned number_of_threads = atol(argv[1]);
    size_t iterations = atol(argv[2]);
    for (unsigned i = 0; i < number_of_threads; ++i) {
        futures.emplace_back(std::async(monte_carlo, i, iterations));
    }
    size_t all_iterations = 0;
    size_t all_inside = 0;
    for (auto &i : futures) {
        auto res = i.get();
        all_inside += res.first;
        all_iterations += res.second;
    }

    std::cout.precision(10);
    std::cout << double(all_inside) / double(all_iterations) * 4 << std::endl;
}

