#include <iostream>
#include <future>
#include <vector>
#include <thread>
#include <functional>
#include <exception>
#include <cstdint>
#include <cmath>


void
find_primes(uint64_t low, uint64_t high,
                                std::vector<std::promise<uint64_t>>& prom)
{
    size_t found = 0;
    uint64_t num = low;
    if (num < 2) {
        num = 2;
    }
    if (num % 2 == 0) {
        if (num == 2 && prom.size() && num <= high) {
            prom[found++].set_value(2);
        }
        ++num;
    }
    while(num <= high && found < prom.size()) {
        bool isprime = true;
        uint64_t sq = sqrt(num) + 1;
        for (uint64_t i = 2; i <= sq; ++i) {
            if (num % i == 0) {
                isprime = false;
                break;
            }
        }
        if (isprime) {
            prom[found++].set_value(num);
        }
        num += 2;
    }
    if (found < prom.size()) {
        try {
            throw high;
        } catch(...) {
            prom[found].set_exception(std::current_exception());
        }
    }
}

int
main()
{
    uint64_t low, high;
    int32_t count;
    std::cin >> low >> high >> count;
    std::vector<std::promise<uint64_t>> prom(count);
    std::vector<std::future<uint64_t>> fut(count);
    for (size_t i = 0; i < size_t(count); ++i) {
        fut[i] = prom[i].get_future();
    }
    std::thread thread(find_primes, low, high, std::ref(prom));
    thread.detach();
    try {
        for (size_t i = 0; i < fut.size(); ++i) {
            std::cout << fut[i].get() << '\n';
        }
    } catch (uint64_t i) {
        std::cout << i << std::endl;
    }
}

