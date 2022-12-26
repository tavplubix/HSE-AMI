#include <iostream>
#include <thread>
#include <mutex>
#include <array>
#include <utility>

#include <cstdio>

constexpr int N = 1000000;
constexpr size_t number_of_threads = 3;


void func(size_t id, std::array<double, number_of_threads>& arr, std::mutex& m)
{
    for (int i = 0; i < N; ++i) {
        std::lock_guard<std::mutex> lock(m);
        arr[id] += 80 + 20 * id;
        arr[(id + 1) % number_of_threads] -= 90 + 20 * id;
    }
}

int main()
{
    std::array<double, number_of_threads> arr;
    arr.fill(0);
    std::array<std::thread, number_of_threads> threads;
    std::mutex m;
    for (size_t id = 0; id < number_of_threads; ++id) {
        threads[id] = std::thread(func, id, std::ref(arr), std::ref(m));
    }
    for (size_t id = 0; id < number_of_threads; ++id) {
        threads[id].join();
    }
    for (auto &i : arr) {
        printf("%.10g\n", i);
    }
}

