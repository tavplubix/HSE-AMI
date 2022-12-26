#include <iostream>
#include <thread>
#include <mutex>
#include <array>
#include <functional>
#include <utility>

#include <cstdio>

constexpr int N = 1000000;
constexpr size_t number_of_threads = 3;
std::mutex mut;

void func(size_t id, std::array<double, number_of_threads>& arr, std::mutex* m)
{
    std::lock_guard<std::mutex> lock(mut);
    std::cout << &m << std::endl;
    for (int i = 0; i < N; ++i) {
        arr[id] += 100 * (id + 1);
        arr[(id + 1) % number_of_threads] -= 100 * (id + 1) + 1;
    }
}

int main()
{
    std::array<double, number_of_threads> arr;
    arr.fill(0);
    std::array<std::thread, number_of_threads> threads;
    for (size_t id = 0; id < number_of_threads; ++id) {
        threads[id] = std::thread(func, id, std::ref(arr), &mut);
        //threads[id].join();
    }
    for (size_t id = 0; id < number_of_threads; ++id) {
        threads[id].join();
        printf("%.10g\n", arr[id]);
    }
}

